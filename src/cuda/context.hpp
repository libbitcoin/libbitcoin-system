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
#ifndef LIBBITCOIN_SYSTEM_SRC_CUDA_CONTEXT_HPP
#define LIBBITCOIN_SYSTEM_SRC_CUDA_CONTEXT_HPP

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
    bool verify(const stopper& cancel, data_chunk& out,
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

    /// Output key prefixes of the receiver for each point, false on failure.
    bool scan(const stopper& cancel, std::vector<prefix>& prefixes,
        data_chunk& valid, const std::span<const ec_compressed>& points,
        const scan::batch::receiver& keys) NOEXCEPT
    {
        scan_arguments arguments{};
        if (!scan_receiver::decode(arguments, keys))
            return false;

        const auto count = points.size();
        const auto stride = add1(size_t{ arguments.label_count });
        const auto fit = scan_layout::rows(half_bytes_, stride);
        const auto rows = std::min({ scan_rows_, chunk_rows, fit });

        prefixes.resize(count * stride);
        valid.resize(count);
        return pipeline(count, rows, false, cancel,
            [&](size_t half, size_t offset, size_t size) NOEXCEPT
            {
                return stage(half, points.subspan(offset, size),
                    arguments);
            },
            [&](size_t half, size_t offset, size_t size) NOEXCEPT
            {
                const auto first = offset * stride;
                const std::span out{ &prefixes[first], size * stride };
                return collect(half, out, std::span{ &valid[offset], size });
            });
    }

    /// Point (hash * sum) of each row, false on failure.
    bool compute(const stopper& cancel, const std::span<ec_compressed>& out,
        const std::span<uint8_t>& valid,
        const std::span<const ec_compressed>& sums,
        const std::span<const ec_secret>& hashes) NOEXCEPT
    {
        const auto count = sums.size();
        const auto fit = silent_layout::rows(half_bytes_);
        const auto rows = std::min({ silent_rows_, chunk_rows, fit });

        return pipeline(count, rows, false, cancel,
            [&](size_t half, size_t offset, size_t size) NOEXCEPT
            {
                return stage(half, sums.subspan(offset, size),
                    hashes.subspan(offset, size));
            },
            [&](size_t half, size_t offset, size_t size) NOEXCEPT
            {
                return collect(half, out.subspan(offset, size),
                    valid.subspan(offset, size));
            });
    }

    /// Label the pool cells in place, false if not available or on failure.
    bool label(pool::report_t& out, const std::span<uint32_t>& cells,
        const std::span<const pool_layout::edges_t>& edges,
        const std::span<const uint32_t>& rows,
        const std::span<const uint32_t>& rivals) NOEXCEPT
    {
        return scan(out, label_, cells, edges, rows, rivals);
    }

    /// Settle the pool cells in place, false if not available or on failure.
    bool settle(pool::report_t& out, const std::span<uint32_t>& cells,
        const std::span<const pool_layout::edges_t>& edges,
        const std::span<const uint32_t>& rows,
        const std::span<const uint32_t>& rivals) NOEXCEPT
    {
        return scan(out, settle_, cells, edges, rows, rivals);
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

    // Stage each chunk in one half while prior computes in other, collecting
    // prior before staging next, yielding device between chunks to caller that
    // would be served ahead.
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

    // Stage the points in a half and queue the scan over them.
    result_t stage(size_t half,
        const std::span<const ec_compressed>& points,
        scan_arguments arguments) NOEXCEPT
    {
        const auto size = points.size();
        const auto stride = add1(size_t{ arguments.label_count });
        const auto base = staging_ + half * half_bytes_;
        const auto stream = streams_[half];
        const scan_layout columns{ size, stride };
        arguments.points = reinterpret_cast<const ec_compressed*>(base);
        arguments.prefixes = reinterpret_cast<prefix*>(base + columns.prefixes);
        arguments.valid = reinterpret_cast<uint8_t*>(base + columns.valid);
        arguments.count = possible_narrow_cast<uint32_t>(size);

        void* parameters[]{ &arguments };
        const auto blocks = possible_narrow_cast<unsigned>(
            ceilinged_divide(size, threads));

        auto result = call_.copy_to_async(base, points.data(),
            points.size_bytes(), stream);
        if (result == success)
            result = call_.launch(scan_, blocks, 1, 1,
                possible_narrow_cast<unsigned>(threads), 1, 1, 0, stream,
                parameters, nullptr);

        return result;
    }

    // Stage the sums and hashes in a half and queue the points over them.
    result_t stage(size_t half, const std::span<const ec_compressed>& sums,
        const std::span<const ec_secret>& hashes) NOEXCEPT
    {
        const auto size = sums.size();
        const auto base = staging_ + half * half_bytes_;
        const auto stream = streams_[half];
        const silent_layout columns{ size };
        silent_arguments arguments
        {
            reinterpret_cast<const ec_compressed*>(base),
            reinterpret_cast<const ec_secret*>(base + columns.hashes),
            reinterpret_cast<ec_compressed*>(base + columns.points),
            reinterpret_cast<uint8_t*>(base + columns.valid),
            possible_narrow_cast<uint32_t>(size)
        };

        void* parameters[]{ &arguments };
        const auto blocks = possible_narrow_cast<unsigned>(
            ceilinged_divide(size, threads));

        auto result = call_.copy_to_async(base, sums.data(),
            sums.size_bytes(), stream);
        if (result == success)
            result = call_.copy_to_async(base + columns.hashes,
                hashes.data(), hashes.size_bytes(), stream);
        if (result == success)
            result = call_.launch(silent_, blocks, 1, 1,
                possible_narrow_cast<unsigned>(threads), 1, 1, 0, stream,
                parameters, nullptr);

        return result;
    }

    // Return the points and validity of the rows staged in a half.
    result_t collect(size_t half, const std::span<ec_compressed>& points,
        const std::span<uint8_t>& valid) NOEXCEPT
    {
        const auto base = staging_ + half * half_bytes_;
        const silent_layout columns{ valid.size() };
        auto result = call_.copy_from_async(points.data(),
            base + columns.points, points.size_bytes(), streams_[half]);
        if (result == success)
            result = call_.copy_from_async(valid.data(), base + columns.valid,
                valid.size(), streams_[half]);
        if (result == success)
            result = call_.stream_synchronize(streams_[half]);

        return result;
    }

    // Return the prefixes and validity of the points staged in a half.
    result_t collect(size_t half, const std::span<prefix>& prefixes,
        const std::span<uint8_t>& valid) NOEXCEPT
    {
        const auto stride = prefixes.size() / valid.size();
        const auto base = staging_ + half * half_bytes_;
        const scan_layout columns{ valid.size(), stride };
        auto result = call_.copy_from_async(prefixes.data(),
            base + columns.prefixes, prefixes.size_bytes(), streams_[half]);
        if (result == success)
            result = call_.copy_from_async(valid.data(), base + columns.valid,
                valid.size(), streams_[half]);
        if (result == success)
            result = call_.stream_synchronize(streams_[half]);

        return result;
    }

    // Queue a kernel over the count of elements on the stream.
    result_t launch(handle_t kernel, size_t count, handle_t stream,
        void** parameters) NOEXCEPT
    {
        const auto blocks = possible_narrow_cast<unsigned>(
            ceilinged_divide(count, threads));

        return call_.launch(kernel, blocks, 1, 1,
            possible_narrow_cast<unsigned>(threads), 1, 1, 0, stream,
            parameters, nullptr);
    }

    // Kernels of pool scan: once over rows, once over inputs,
    // and over inputs to fixed point.
    struct pool_kernels
    {
        handle_t rows;
        handle_t inputs;
        handle_t pass;
    };

    // Run staged scan over allocation of bytes, passes and transfer reported.
    template <typename Scan>
    bool staged(pool::report_t& out, size_t bytes, const Scan& scan) NOEXCEPT
    {
        turn turn{ turns_, false };
        if (failed_.load() || !pooled_)
            return false;

        using clock = std::chrono::steady_clock;
        const auto start = clock::now();
        pointer_t base{};
        if (call_.context_current(context_) != success ||
            call_.allocate(&base, bytes) != success)
            return false;

        size_t passes{};
        pool::report_t::duration transfer{ clock::now() - start };
        const auto result = scan(passes, transfer, base);

        const auto finish = clock::now();
        call_.deallocate(base);
        transfer += clock::now() - finish;
        if (result != success)
        {
            failed_.store(true);
            return false;
        }

        out.passes = passes;
        out.transfer = transfer;
        return true;
    }

    // Scan the pool cells in place, with passes and transfer reported.
    bool scan(pool::report_t& out, const pool_kernels& kernels,
        const std::span<uint32_t>& cells,
        const std::span<const pool_layout::edges_t>& edges,
        const std::span<const uint32_t>& rows,
        const std::span<const uint32_t>& rivals) NOEXCEPT
    {
        if (cells.empty() || rows.empty())
            return false;

        const pool_layout columns{ cells.size(), rows.size() };
        return staged(out, columns.bytes,
            [&](size_t& passes, pool::report_t::duration& transfer,
                pointer_t base) NOEXCEPT
            {
                return scan(passes, transfer, base, columns, kernels, cells,
                    edges, rows, rivals);
            });
    }

    // Stage the pool columns, scan to the fixed point and return the cells.
    result_t scan(size_t& passes, pool::report_t::duration& transfer,
        pointer_t base, const pool_layout& columns,
        const pool_kernels& kernels, const std::span<uint32_t>& cells,
        const std::span<const pool_layout::edges_t>& edges,
        const std::span<const uint32_t>& rows,
        const std::span<const uint32_t>& rivals) NOEXCEPT
    {
        const auto stream = streams_.front();
        const auto flag = base + columns.changed;
        using edges_t = pool_layout::edges_t;
        pool_arguments arguments
        {
            reinterpret_cast<uint32_t*>(base),
            reinterpret_cast<const edges_t*>(base + columns.edges),
            reinterpret_cast<const uint32_t*>(base + columns.rows),
            reinterpret_cast<const uint32_t*>(base + columns.rivals),
            reinterpret_cast<uint32_t*>(base + columns.owners),
            reinterpret_cast<uint32_t*>(flag),
            possible_narrow_cast<uint32_t>(cells.size()),
            possible_narrow_cast<uint32_t>(rows.size())
        };

        using clock = std::chrono::steady_clock;
        void* parameters[]{ &arguments };
        const auto upload = clock::now();
        auto result = call_.copy_to_async(base, cells.data(),
            cells.size_bytes(), stream);
        if (result == success)
            result = call_.copy_to_async(base + columns.edges, edges.data(),
                edges.size_bytes(), stream);
        if (result == success)
            result = call_.copy_to_async(base + columns.rows, rows.data(),
                rows.size_bytes(), stream);
        if (result == success)
            result = call_.copy_to_async(base + columns.rivals, rivals.data(),
                rivals.size_bytes(), stream);
        if (result == success)
            result = call_.stream_synchronize(stream);

        transfer += clock::now() - upload;
        if (result == success)
            result = launch(kernels.rows, cells.size(), stream, parameters);
        if (result == success)
            result = launch(kernels.inputs, rows.size(), stream, parameters);

        constexpr uint32_t clear{};
        uint32_t changed{ 1 };
        while (result == success && is_nonzero(changed))
        {
            ++passes;
            result = call_.copy_to_async(flag, &clear, sizeof(clear), stream);
            if (result == success)
                result = launch(kernels.pass, rows.size(), stream,
                    parameters);
            if (result == success)
                result = call_.copy_from_async(&changed, flag,
                    sizeof(changed), stream);
            if (result == success)
                result = call_.stream_synchronize(stream);
        }

        const auto download = clock::now();
        if (result == success)
            result = call_.copy_from_async(cells.data(), base,
                cells.size_bytes(), stream);
        if (result == success)
            result = call_.stream_synchronize(stream);

        transfer += clock::now() - download;
        return result;
    }

    bool load() NOEXCEPT
    {
        const auto library = load_library();
        if (is_null(library) || !resolve(call_, library)
            || call_.init(0) != success)
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
            || call_.module_function(&scan_, module_, "scan_silent") != success
            || call_.module_function(&silent_, module_, "compute_silent") != success
            || call_.module_global(&table, &size, module_, "generator_table") != success
            || size != shape::table_bytes
            || call_.module_global(&comb, &comb_bytes, module_, "comb_table") != success
            || comb_bytes != shape::comb_bytes)
            return false;

        const auto& slices = secp256k1::generator_slices;
        for (size_t slice{}; slice < slices.size(); ++slice)
            if (call_.copy_to(table + slice * shape::slice_bytes,
                slices[slice], shape::slice_bytes) != success)
                return false;

        const auto& parts = secp256k1::comb_parts;
        for (size_t part{}; part < parts.size(); ++part)
            if (call_.copy_to(comb + part * shape::part_bytes,
                parts[part], shape::part_bytes) != success)
                return false;

        size_t free{}, total{};
        if (call_.memory(&free, &total) != success)
            return false;

        // The embedded kernels may lack the pool kernels.
        const auto found = [&](handle_t& out, const char* name) NOEXCEPT
        {
            return call_.module_function(&out, module_, name) == success;
        };

        pooled_ = found(label_.rows, "prepare_pool")
            && found(label_.inputs, "seed_pool")
            && found(label_.pass, "label_pool")
            && found(settle_.rows, "reduce_pool")
            && found(settle_.inputs, "contest_pool")
            && found(settle_.pass, "descend_pool");

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
            calibrate_scan(half_bytes_, is_nonzero(limited)) &&
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

        const auto count = duration_cast<nanoseconds>(elapsed).count();
        const auto measured = greater<uint64_t>(one, count);
        const auto target = duration_cast<nanoseconds>(launch_time).count();
        const auto targeted = possible_sign_cast<uint64_t>(target);
        const auto rows = calibration_rows * targeted / measured;
        return limit<size_t>(rows, threads, capacity);
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

        std::copy_n(std::next(point.cbegin()), key.size(), key.begin());
        schnorr_rows_ = calibrate(message, wrong, key, signature, bytes,
            limited);
        return !is_zero(schnorr_rows_);
    }

    // BIP352 receiving vector "Simple send: two inputs", whose point pays
    // receiver's unlabeled output key 3e9fce73d4e77a48...
    bool calibrate_scan(size_t bytes, bool limited) NOEXCEPT
    {
        constexpr ec_compressed point = base16_array(
            "024ac253c216532e961988e2a8ce266a447c894c781e52ef6cee902361db960004");
        constexpr ec_secret secret = base16_array(
            "9d6ad855ce3417ef84e836892e5a56392bfba05fa5d97ccea30e266f540e08b3");
        constexpr prefix expected = base16_array("3e9fce73d4e77a48");

        scan_arguments arguments{};
        scan::batch::receiver keys{};
        keys.scan = base16_array(
            "0f694e068028a717f8af6b9411f9a133dd3565258714cc226594b34db90c1f2c");
        if (!secret_to_public(keys.spend, secret) ||
            !scan_receiver::decode(arguments, keys))
            return false;

        using namespace std::chrono;
        const std::vector<ec_compressed> points(calibration_rows, point);
        std::vector<prefix> prefixes(calibration_rows);
        data_chunk valid(calibration_rows);
        const auto start = steady_clock::now();
        if (call_.context_current(context_) != success ||
            stage(zero, points, arguments) != success ||
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

        const auto capacity = scan_layout::rows(bytes, one);
        scan_rows_ = limit_rows(capacity, limited, elapsed);
        return true;
    }

    bool calibrate_silent(size_t bytes, bool limited) NOEXCEPT
    {
        constexpr ec_secret secret{ 0x01 };
        constexpr ec_secret hash = base16_array(
            "9d6ad855ce3417ef84e836892e5a56392bfba05fa5d97ccea30e266f540e08b3");
        ec_compressed sum{};
        if (!secret_to_public(sum, secret))
            return false;

        auto expected = sum;
        if (!ec_multiply(expected, hash))
            return false;

        using namespace std::chrono;
        const std::vector<ec_compressed> sums(calibration_rows, sum);
        const std::vector<ec_secret> hashes(calibration_rows, hash);
        std::vector<ec_compressed> points(calibration_rows);
        data_chunk valid(calibration_rows);
        const auto start = steady_clock::now();
        if (call_.context_current(context_) != success ||
            stage(zero, sums, hashes) != success ||
            collect(zero, points, valid) != success)
            return false;

        const auto elapsed = steady_clock::now() - start;
        constexpr auto rows = to_signed(calibration_rows);
        if (std::ranges::count(valid, one) != rows ||
            std::ranges::count(points, expected) != rows)
            return false;

        const auto capacity = silent_layout::rows(bytes);
        silent_rows_ = limit_rows(capacity, limited, elapsed);
        return true;
    }

    functions call_{};
    device_t device_{};
    handle_t context_{};
    handle_t module_{};
    handle_t ecdsa_{};
    handle_t schnorr_{};
    handle_t scan_{};
    handle_t silent_{};
    pool_kernels label_{};
    pool_kernels settle_{};
    pointer_t staging_{};
    size_t half_bytes_{};
    std_array<handle_t, two> streams_{};
    size_t ecdsa_rows_{};
    size_t schnorr_rows_{};
    size_t scan_rows_{};
    size_t silent_rows_{};
    bool retained_{};
    bool pooled_{};
    turns turns_{};
    std::atomic_bool failed_{};
    const bool loaded_;
};

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace cuda
} // namespace system
} // namespace libbitcoin

#endif
