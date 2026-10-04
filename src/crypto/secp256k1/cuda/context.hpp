/**
 * Copyright (c) 2011-2026 libbitcoin developers
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_SYSTEM_SRC_CRYPTO_SECP256K1_CUDA_CONTEXT_HPP
#define LIBBITCOIN_SYSTEM_SRC_CRYPTO_SECP256K1_CUDA_CONTEXT_HPP

#include <atomic>
#include <chrono>
#include <span>
#include <bitcoin/system/crypto/secp256k1.hpp>
#include <bitcoin/system/crypto/secp256k1/algorithm.hpp>
#include <bitcoin/system/crypto/secp256k1/tables.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include "kernels.hpp"
#include "layout.hpp"
#include "library.hpp"
#include "turns.hpp"

namespace libbitcoin {
namespace system {
namespace secp256k1 {
namespace cuda {

BC_PUSH_WARNING(NO_REINTERPRET_CAST)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// Context.
// ----------------------------------------------------------------------------

/// The device and its resources, released with the instance.
class context
{
public:
    /// Threads per block.
    static constexpr size_t threads = 128;

    /// Staging is at most a quarter of the device's free memory.
    static constexpr size_t memory_share = 4;

    /// Staging is at most this many bytes.
    static constexpr size_t maximum_bytes = power2(29_size);

    /// Rows timed to measure a kernel's rate.
    static constexpr size_t calibration_rows = power2(16_size);

    /// A chunk is at most this many rows, staged while the prior computes.
    static constexpr size_t chunk_rows = power2(16_size);

    /// A launch is sized to run about this long where the device has a limit.
    static constexpr auto launch_time = std::chrono::milliseconds{ 50 };

    DELETE_COPY_MOVE(context);

    static context& instance() NOEXCEPT
    {
        static context singleton{};
        return singleton;
    }

    ~context() NOEXCEPT
    {
        for (const auto stream: streams_)
            if (!is_null(stream))
                call_.stream_destroy(stream);

        if (!is_zero(staging_))
            call_.deallocate(staging_);

        if (!is_null(module_))
            call_.module_unload(module_);

        if (retained_)
            call_.context_release(device_);
    }

    bool available() const NOEXCEPT
    {
        return loaded_ && !failed_.load();
    }

    /// Results of the rows, empty if all valid or canceled, false on failure.
    template <typename Key>
    bool verify(data_chunk& out, const stopper& cancel,
        const std::span<const hash_digest>& digests,
        const std::span<const Key>& keys,
        const std::span<const ec_signature>& signatures, bool urgent) NOEXCEPT
    {
        const auto count = keys.size();
        const auto rows = std::min(launch_rows<Key>(), chunk_rows);
        out.resize(count);
        if (!pipeline(count, rows, urgent, cancel,
            [&](size_t half, size_t offset, size_t size) NOEXCEPT
            {
                return stage(half, digests.subspan(offset, size),
                    keys.subspan(offset, size),
                    signatures.subspan(offset, size));
            },
            [&](size_t half, size_t offset, size_t size) NOEXCEPT
            {
                return collect<Key>(half, std::span{ &out[offset], size });
            }))
            return false;

        if (cancel || std::ranges::all_of(out, [](uint8_t value) NOEXCEPT
            {
                return is_one(value);
            }))
            out.clear();

        return true;
    }

    /// Output key prefixes of the receiver for each summary, false on failure.
    bool scan(std::vector<prefix>& prefixes, data_chunk& valid,
        const stopper& cancel, const std::span<const ec_compressed>& summaries,
        const silent_arguments& keys) NOEXCEPT
    {
        const auto count = summaries.size();
        const auto stride = add1(size_t{ keys.label_count });
        const auto fit = silent_layout::rows(half_bytes_, stride);
        const auto rows = std::min({ silent_rows_, chunk_rows, fit });

        prefixes.resize(count * stride);
        valid.resize(count);
        return pipeline(count, rows, false, cancel,
            [&](size_t half, size_t offset, size_t size) NOEXCEPT
            {
                return stage(half, summaries.subspan(offset, size), keys);
            },
            [&](size_t half, size_t offset, size_t size) NOEXCEPT
            {
                const auto first = offset * stride;
                const std::span out{ &prefixes[first], size * stride };
                return collect(half, out, std::span{ &valid[offset], size });
            });
    }

private:
    context() NOEXCEPT
      : loaded_(load())
    {
    }

    template <typename Key>
    size_t launch_rows() const NOEXCEPT
    {
        if constexpr (is_same_type<Key, ec_compressed>)
            return ecdsa_rows_;
        else
            return schnorr_rows_;
    }

    template <typename Key>
    handle_t kernel() const NOEXCEPT
    {
        if constexpr (is_same_type<Key, ec_compressed>)
            return ecdsa_;
        else
            return schnorr_;
    }

    // Stage the rows in a half and queue the kernel over them on its stream.
    template <typename Key>
    result_t stage(size_t half, const std::span<const hash_digest>& digests,
        const std::span<const Key>& keys,
        const std::span<const ec_signature>& signatures) NOEXCEPT
    {
        const auto size = keys.size();
        const auto base = staging_ + half * half_bytes_;
        const auto stream = streams_[half];
        const layout<Key> columns{ size };
        typename layout<Key>::arguments arguments
        {
            reinterpret_cast<const hash_digest*>(base),
            reinterpret_cast<const Key*>(base + columns.keys),
            reinterpret_cast<const ec_signature*>(base + columns.signatures),
            reinterpret_cast<uint8_t*>(base + columns.results),
            possible_narrow_cast<uint32_t>(size)
        };

        void* parameters[]{ &arguments };
        const auto blocks = possible_narrow_cast<unsigned>(
            ceilinged_divide(size, threads));

        auto result = call_.copy_to_async(base, digests.data(),
            digests.size_bytes(), stream);
        if (result == success)
            result = call_.copy_to_async(base + columns.keys, keys.data(),
                keys.size_bytes(), stream);
        if (result == success)
            result = call_.copy_to_async(base + columns.signatures,
                signatures.data(), signatures.size_bytes(), stream);
        if (result == success)
            result = call_.launch(kernel<Key>(), blocks, 1, 1,
                possible_narrow_cast<unsigned>(threads), 1, 1, 0, stream,
                parameters, nullptr);

        return result;
    }

    // Return the results of the rows staged in a half, once computed.
    template <typename Key>
    result_t collect(size_t half, const std::span<uint8_t>& results) NOEXCEPT
    {
        const auto base = staging_ + half * half_bytes_;
        const layout<Key> columns{ results.size() };
        auto result = call_.copy_from_async(results.data(),
            base + columns.results, results.size(), streams_[half]);
        if (result == success)
            result = call_.stream_synchronize(streams_[half]);

        return result;
    }

    // Stage the rows, run the kernel over them, and return their results.
    template <typename Key>
    result_t launch(const std::span<uint8_t>& results,
        const std::span<const hash_digest>& digests,
        const std::span<const Key>& keys,
        const std::span<const ec_signature>& signatures) NOEXCEPT
    {
        auto result = call_.context_current(context_);
        if (result == success)
            result = stage(zero, digests, keys, signatures);
        if (result == success)
            result = collect<Key>(zero, results);

        return result;
    }

    // Stage each chunk in a half while the prior computes in the other,
    // collecting the prior before staging the next, and yielding the device
    // between chunks to a caller that would be served ahead.
    template <typename Stage, typename Collect>
    bool pipeline(size_t count, size_t rows, bool urgent,
        const stopper& cancel, Stage&& stage, Collect&& collect) NOEXCEPT
    {
        turn turn{ turns_, urgent };
        if (failed_.load() || is_zero(rows))
            return false;

        auto result = call_.context_current(context_);
        size_t prior{}, prior_size{}, prior_half{}, chunk{};
        for (size_t offset{}; result == success && offset < count;
            offset += rows, ++chunk)
        {
            if (cancel)
                break;

            if (turn.contended())
            {
                if (!is_zero(prior_size))
                    result = collect(prior_half, prior, prior_size);

                prior_size = zero;
                if (result != success)
                    break;

                turn.yield();
                if (failed_.load())
                    return false;

                result = call_.context_current(context_);
                if (result != success)
                    break;
            }

            const auto size = std::min(rows, count - offset);
            const auto half = chunk % two;
            result = stage(half, offset, size);
            if (result == success && !is_zero(prior_size))
                result = collect(prior_half, prior, prior_size);

            prior = offset;
            prior_size = size;
            prior_half = half;
        }

        if (result == success && !cancel && !is_zero(prior_size))
            result = collect(prior_half, prior, prior_size);
        if (result == success)
            result = call_.stream_synchronize(streams_.front());
        if (result == success)
            result = call_.stream_synchronize(streams_.back());

        if (result != success)
        {
            failed_.store(true);
            return false;
        }

        return true;
    }

    // Stage the summaries in a half and queue the scan over them.
    result_t stage(size_t half,
        const std::span<const ec_compressed>& summaries,
        silent_arguments arguments) NOEXCEPT
    {
        const auto size = summaries.size();
        const auto stride = add1(size_t{ arguments.label_count });
        const auto base = staging_ + half * half_bytes_;
        const auto stream = streams_[half];
        const silent_layout columns{ size, stride };
        arguments.summaries = reinterpret_cast<const ec_compressed*>(base);
        arguments.prefixes = reinterpret_cast<prefix*>(base + columns.prefixes);
        arguments.valid = reinterpret_cast<uint8_t*>(base + columns.valid);
        arguments.count = possible_narrow_cast<uint32_t>(size);

        void* parameters[]{ &arguments };
        const auto blocks = possible_narrow_cast<unsigned>(
            ceilinged_divide(size, threads));

        auto result = call_.copy_to_async(base, summaries.data(),
            summaries.size_bytes(), stream);
        if (result == success)
            result = call_.launch(silent_, blocks, 1, 1,
                possible_narrow_cast<unsigned>(threads), 1, 1, 0, stream,
                parameters, nullptr);

        return result;
    }

    // Return the prefixes and validity of the summaries staged in a half.
    result_t collect(size_t half, const std::span<prefix>& prefixes,
        const std::span<uint8_t>& valid) NOEXCEPT
    {
        const auto stride = prefixes.size() / valid.size();
        const auto base = staging_ + half * half_bytes_;
        const silent_layout columns{ valid.size(), stride };
        auto result = call_.copy_from_async(prefixes.data(),
            base + columns.prefixes, prefixes.size_bytes(), streams_[half]);
        if (result == success)
            result = call_.copy_from_async(valid.data(), base + columns.valid,
                valid.size(), streams_[half]);
        if (result == success)
            result = call_.stream_synchronize(streams_[half]);

        return result;
    }

    bool load() NOEXCEPT
    {
        const auto library = load_library();
        if (is_null(library) || !resolve(call_, library) ||
            call_.init(0) != success)
            return false;

        int count{};
        if (call_.device_count(&count) != success)
            return false;

        for (int ordinal{}; ordinal < count; ++ordinal)
        {
            int major{}, minor{};
            if (call_.device_get(&device_, ordinal) == success
                && call_.device_attribute(&major, compute_major, device_) == success
                && call_.device_attribute(&minor, compute_minor, device_) == success
                && (major > 7 || (major == 7 && minor >= 5)))
                return open();
        }

        return false;
    }

    bool open() NOEXCEPT
    {
        if (call_.context_retain(&context_, device_) != success)
            return false;

        retained_ = true;
        std::string ptx{};
        pointer_t table{}, comb{};
        size_t size{}, comb_bytes{};
        if (!inflate(ptx)
            || call_.context_current(context_) != success
            || call_.context_limit(stack_limit, stack_bytes) != success
            || call_.module_load(&module_, ptx.c_str()) != success
            || call_.module_function(&ecdsa_, module_, "verify_ecdsa") != success
            || call_.module_function(&schnorr_, module_, "verify_schnorr") != success
            || call_.module_function(&silent_, module_, "scan_silent") != success
            || call_.module_global(&table, &size, module_, "generator_table") != success
            || size != shape::table_bytes
            || call_.module_global(&comb, &comb_bytes, module_, "comb_table") != success
            || comb_bytes != shape::comb_bytes)
            return false;

        for (size_t slice{}; slice < generator_slices.size(); ++slice)
            if (call_.copy_to(table + slice * shape::slice_bytes,
                generator_slices[slice], shape::slice_bytes) != success)
                return false;

        for (size_t part{}; part < comb_parts.size(); ++part)
            if (call_.copy_to(comb + part * shape::part_bytes,
                comb_parts[part], shape::part_bytes) != success)
                return false;

        size_t free{}, total{};
        if (call_.memory(&free, &total) != success)
            return false;

        const auto bytes = std::min(maximum_bytes, free / memory_share);
        half_bytes_ = bytes / two;
        if (layout<ec_compressed>::rows(half_bytes_) < calibration_rows ||
            call_.allocate(&staging_, bytes) != success ||
            call_.stream_create(&streams_.front(), non_blocking) != success ||
            call_.stream_create(&streams_.back(), non_blocking) != success)
            return false;

        int limited{};
        if (call_.device_attribute(&limited, execution_timeout, device_) !=
            success)
            return false;

        return
            calibrate_ecdsa(half_bytes_, is_nonzero(limited)) &&
            calibrate_schnorr(half_bytes_, is_nonzero(limited)) &&
            calibrate_silent(half_bytes_, is_nonzero(limited));
    }

    // Launch rows from capacity, bounded by launch time where limited.
    template <typename Key>
    size_t calibrate(const hash_digest& digest, const hash_digest& wrong,
        const Key& key, const ec_signature& signature, size_t bytes,
        bool limited) NOEXCEPT
    {
        const std::vector<hash_digest> digests(calibration_rows, digest);
        const std::vector<Key> keys(calibration_rows, key);
        const std::vector<ec_signature> signatures(calibration_rows,
            signature);

        using namespace std::chrono;
        data_chunk results(calibration_rows);
        const auto start = steady_clock::now();
        if (launch<Key>(results, digests, keys, signatures) != success)
            return zero;

        const auto elapsed = steady_clock::now() - start;
        if (!std::ranges::all_of(results, [](uint8_t value) NOEXCEPT
            {
                return is_one(value);
            }))
            return zero;

        uint8_t result{ 1 };
        if (launch<Key>(std::span{ &result, one }, std::span{ &wrong, one },
            std::span{ keys }.first(one),
            std::span{ signatures }.first(one)) != success ||
            !is_zero(result))
            return zero;

        return limit_rows(layout<Key>::rows(bytes), limited, elapsed);
    }

    // Rows of capacity, bounded by calibrated launch time where limited.
    static size_t limit_rows(size_t capacity, bool limited,
        const std::chrono::steady_clock::duration& elapsed) NOEXCEPT
    {
        using namespace std::chrono;
        if (!limited)
            return capacity;

        const auto measured = std::max<int64_t>(one,
            duration_cast<nanoseconds>(elapsed).count());
        const auto target = duration_cast<nanoseconds>(launch_time).count();
        const auto rows = calibration_rows *
            possible_sign_cast<size_t>(target) /
            possible_sign_cast<size_t>(measured);
        return std::clamp(rows, threads, capacity);
    }

    bool calibrate_ecdsa(size_t bytes, bool limited) NOEXCEPT
    {
        constexpr ec_secret secret{ 0x01 };
        constexpr hash_digest digest{ 0x02 };
        constexpr hash_digest wrong{ 0x03 };
        ec_compressed key{};
        ec_signature signature{};
        if (!secret_to_public(key, secret) ||
            !ecdsa::sign(signature, secret, digest))
            return false;

        ecdsa_rows_ = calibrate(digest, wrong, key, signature, bytes,
            limited);
        return !is_zero(ecdsa_rows_);
    }

    bool calibrate_schnorr(size_t bytes, bool limited) NOEXCEPT
    {
        constexpr ec_secret secret{ 0x01 };
        constexpr hash_digest message{ 0x02 };
        constexpr hash_digest wrong{ 0x03 };
        ec_compressed point{};
        ec_xonly key{};
        ec_signature signature{};
        if (!secret_to_public(point, secret) ||
            !schnorr::sign(signature, secret, message, null_hash))
            return false;

        std::copy_n(std::next(point.begin()), key.size(), key.begin());
        schnorr_rows_ = calibrate(message, wrong, key, signature, bytes,
            limited);
        return !is_zero(schnorr_rows_);
    }

    // BIP352 receiving vector "Simple send: two inputs", whose summary pays
    // the receiver's unlabeled output key 3e9fce73d4e77a48...
    bool calibrate_silent(size_t bytes, bool limited) NOEXCEPT
    {
        constexpr ec_compressed summary = base16_array(
            "024ac253c216532e961988e2a8ce266a447c894c781e52ef6cee902361db960004");
        constexpr ec_secret secret = base16_array(
            "9d6ad855ce3417ef84e836892e5a56392bfba05fa5d97ccea30e266f540e08b3");
        constexpr prefix expected = base16_array("3e9fce73d4e77a48");

        silent_arguments keys{};
        keys.scan = base16_array(
            "0f694e068028a717f8af6b9411f9a133dd3565258714cc226594b34db90c1f2c");
        if (!secret_to_public(keys.spend, secret))
            return false;

        using namespace std::chrono;
        const std::vector<ec_compressed> summaries(calibration_rows, summary);
        std::vector<prefix> prefixes(calibration_rows);
        data_chunk valid(calibration_rows);
        const auto start = steady_clock::now();
        if (call_.context_current(context_) != success ||
            stage(zero, summaries, keys) != success ||
            collect(zero, prefixes, valid) != success)
            return false;

        const auto elapsed = steady_clock::now() - start;
        if (!std::ranges::all_of(valid, [](uint8_t value) NOEXCEPT
            {
                return is_one(value);
            }) ||
            !std::ranges::all_of(prefixes, [&](const prefix& value) NOEXCEPT
            {
                return value == expected;
            }))
            return false;

        const auto capacity = silent_layout::rows(bytes, one);
        silent_rows_ = limit_rows(capacity, limited, elapsed);
        return true;
    }

    functions call_{};
    device_t device_{};
    handle_t context_{};
    handle_t module_{};
    handle_t ecdsa_{};
    handle_t schnorr_{};
    handle_t silent_{};
    pointer_t staging_{};
    size_t half_bytes_{};
    std_array<handle_t, two> streams_{};
    size_t ecdsa_rows_{};
    size_t schnorr_rows_{};
    size_t silent_rows_{};
    bool retained_{};
    turns turns_{};
    std::atomic_bool failed_{};
    const bool loaded_;
};

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace cuda
} // namespace secp256k1
} // namespace system
} // namespace libbitcoin

#endif
