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
#include "driver.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <span>
#include <bitcoin/system/crypto/secp256k1.hpp>
#include <bitcoin/system/crypto/secp256k1/algorithm.hpp>
#include <bitcoin/system/crypto/secp256k1/tables.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include "kernels.hpp"
#include "ptx.hpp"

#if defined(HAVE_CUDA)
    #include <boost/beast/zlib/inflate_stream.hpp>
    #if defined(HAVE_MSC)
        #include <windows.h>
    #else
        #include <dlfcn.h>
    #endif
#endif

namespace libbitcoin {
namespace system {
namespace secp256k1 {
namespace cuda {

#if defined(HAVE_CUDA)

BC_PUSH_WARNING(NO_REINTERPRET_CAST)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// Driver library.
// ----------------------------------------------------------------------------

#if defined(HAVE_MSC)
static void* load_library() NOEXCEPT
{
    return reinterpret_cast<void*>(::LoadLibraryW(L"nvcuda.dll"));
}

static void* load_symbol(void* library, const char* name) NOEXCEPT
{
    return reinterpret_cast<void*>(::GetProcAddress(
        static_cast<HMODULE>(library), name));
}
#else
static void* load_library() NOEXCEPT
{
    return ::dlopen("libcuda.so.1", RTLD_NOW);
}

static void* load_symbol(void* library, const char* name) NOEXCEPT
{
    return ::dlsym(library, name);
}
#endif

// Driver interface, declared here to avoid a toolkit dependency (cuda.h).
// ----------------------------------------------------------------------------

using result_t = int;
using device_t = int;
using pointer_t = unsigned long long;
using handle_t = void*;

constexpr result_t success = 0;
constexpr int compute_major = 75;
constexpr int compute_minor = 76;
constexpr int execution_timeout = 17;
constexpr int stack_limit = 0;
constexpr unsigned non_blocking = 1;
constexpr size_t stack_bytes = 8192;

struct functions
{
    result_t (*init)(unsigned int);
    result_t (*device_count)(int*);
    result_t (*device_get)(device_t*, int);
    result_t (*device_attribute)(int*, int, device_t);
    result_t (*context_retain)(handle_t*, device_t);
    result_t (*context_release)(device_t);
    result_t (*context_current)(handle_t);
    result_t (*context_limit)(int, size_t);
    result_t (*module_load)(handle_t*, const void*);
    result_t (*module_unload)(handle_t);
    result_t (*module_function)(handle_t*, handle_t, const char*);
    result_t (*module_global)(pointer_t*, size_t*, handle_t, const char*);
    result_t (*memory)(size_t*, size_t*);
    result_t (*allocate)(pointer_t*, size_t);
    result_t (*deallocate)(pointer_t);
    result_t (*copy_to)(pointer_t, const void*, size_t);
    result_t (*copy_to_async)(pointer_t, const void*, size_t, handle_t);
    result_t (*copy_from_async)(void*, pointer_t, size_t, handle_t);
    result_t (*stream_create)(handle_t*, unsigned);
    result_t (*stream_destroy)(handle_t);
    result_t (*stream_synchronize)(handle_t);
    result_t (*launch)(handle_t, unsigned, unsigned, unsigned, unsigned,
        unsigned, unsigned, unsigned, handle_t, void**, void**);
};

template <typename Function>
static bool resolve(Function& function, void* library,
    const char* name) NOEXCEPT
{
    function = reinterpret_cast<Function>(load_symbol(library, name));
    return !is_null(function);
}

static bool resolve(functions& out, void* library) NOEXCEPT
{
    return
        resolve(out.init, library, "cuInit") &&
        resolve(out.device_count, library, "cuDeviceGetCount") &&
        resolve(out.device_get, library, "cuDeviceGet") &&
        resolve(out.device_attribute, library, "cuDeviceGetAttribute") &&
        resolve(out.context_retain, library, "cuDevicePrimaryCtxRetain") &&
        resolve(out.context_release, library,
            "cuDevicePrimaryCtxRelease_v2") &&
        resolve(out.context_current, library, "cuCtxSetCurrent") &&
        resolve(out.context_limit, library, "cuCtxSetLimit") &&
        resolve(out.module_load, library, "cuModuleLoadData") &&
        resolve(out.module_unload, library, "cuModuleUnload") &&
        resolve(out.module_function, library, "cuModuleGetFunction") &&
        resolve(out.module_global, library, "cuModuleGetGlobal_v2") &&
        resolve(out.memory, library, "cuMemGetInfo_v2") &&
        resolve(out.allocate, library, "cuMemAlloc_v2") &&
        resolve(out.deallocate, library, "cuMemFree_v2") &&
        resolve(out.copy_to, library, "cuMemcpyHtoD_v2") &&
        resolve(out.copy_to_async, library, "cuMemcpyHtoDAsync_v2") &&
        resolve(out.copy_from_async, library, "cuMemcpyDtoHAsync_v2") &&
        resolve(out.stream_create, library, "cuStreamCreate") &&
        resolve(out.stream_destroy, library, "cuStreamDestroy_v2") &&
        resolve(out.stream_synchronize, library, "cuStreamSynchronize") &&
        resolve(out.launch, library, "cuLaunchKernel");
}

// Kernels.
// ----------------------------------------------------------------------------

static bool inflate(std::string& out) NOEXCEPT
{
    namespace zlib = boost::beast::zlib;
    const auto deflated = deflated_ptx();
    out.assign(ptx_size(), '\0');

    zlib::z_params params{};
    params.next_in = deflated.data();
    params.avail_in = deflated.size();
    params.next_out = out.data();
    params.avail_out = out.size();

    zlib::inflate_stream stream{};
    boost::system::error_code ec{};
    stream.write(params, zlib::Flush::finish, ec);
    return ec == zlib::error::end_of_stream && params.total_out == out.size();
}

struct shape
  : algorithm
{
    static constexpr auto slice_bytes = table_words * sizeof(uint64_t);
    static constexpr auto table_bytes = slice_count * slice_bytes;
};

/// Columns of a kernel's rows within the staging buffer.
template <typename Key>
struct layout
{
    using arguments = iif<is_same_type<Key, ec_compressed>, ecdsa_arguments,
        schnorr_arguments>;

    static constexpr size_t align = 256;
    static constexpr size_t row_bytes = sizeof(hash_digest) + sizeof(Key) +
        sizeof(ec_signature) + one;

    static constexpr size_t round(size_t bytes) NOEXCEPT
    {
        return ceilinged_divide(bytes, align) * align;
    }

    /// Rows of the kernel that fit in the given bytes.
    static constexpr size_t rows(size_t bytes) NOEXCEPT
    {
        constexpr auto slack = 3 * align;
        return bytes < slack ? zero : (bytes - slack) / row_bytes;
    }

    explicit constexpr layout(size_t rows) NOEXCEPT
      : keys(round(rows * sizeof(hash_digest))),
        signatures(round(keys + rows * sizeof(Key))),
        results(round(signatures + rows * sizeof(ec_signature)))
    {
    }

    const size_t keys;
    const size_t signatures;
    const size_t results;
};

// Turns.
// ----------------------------------------------------------------------------

/// Device turns, by chunk, urgent callers ahead of queued callers.
class turns
{
public:
    void acquire(bool urgent) NOEXCEPT
    {
        std::unique_lock lock(mutex_);
        if (urgent)
        {
            ++urgent_;
            ready_.wait(lock, [&]() NOEXCEPT { return !busy_; });
            --urgent_;
        }
        else
        {
            const auto ticket = next_++;
            ready_.wait(lock, [&]() NOEXCEPT
            {
                return !busy_ && is_zero(urgent_) && serving_ == ticket;
            });

            ++serving_;
        }

        busy_ = true;
    }

    void release() NOEXCEPT
    {
        {
            std::lock_guard lock(mutex_);
            busy_ = false;
        }

        ready_.notify_all();
    }

    /// A caller is waiting that would be served ahead of the holder.
    bool contended(bool urgent) const NOEXCEPT
    {
        std::lock_guard lock(mutex_);
        return !is_zero(urgent_) || (!urgent && next_ != serving_);
    }

private:
    mutable std::mutex mutex_{};
    std::condition_variable ready_{};
    size_t urgent_{};
    uint64_t next_{};
    uint64_t serving_{};
    bool busy_{};
};

class turn
{
public:
    DELETE_COPY_MOVE(turn);

    turn(turns& turns, bool urgent) NOEXCEPT
      : turns_(turns), urgent_(urgent)
    {
        turns_.acquire(urgent_);
    }

    ~turn() NOEXCEPT
    {
        turns_.release();
    }

    bool contended() const NOEXCEPT
    {
        return turns_.contended(urgent_);
    }

    void yield() NOEXCEPT
    {
        turns_.release();
        turns_.acquire(urgent_);
    }

private:
    turns& turns_;
    const bool urgent_;
};

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
        std::span<const hash_digest> digests, std::span<const Key> keys,
        std::span<const ec_signature> signatures, bool urgent) NOEXCEPT
    {
        const auto count = keys.size();
        const auto rows = std::min(launch_rows<Key>(), chunk_rows);
        out.resize(count);

        turn turn{ turns_, urgent };
        if (failed_.load())
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
                    result = collect<Key>(prior_half,
                        std::span{ &out[prior], prior_size });

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
            result = stage(half, digests.subspan(offset, size),
                keys.subspan(offset, size), signatures.subspan(offset, size));

            if (result == success && !is_zero(prior_size))
                result = collect<Key>(prior_half,
                    std::span{ &out[prior], prior_size });

            prior = offset;
            prior_size = size;
            prior_half = half;
        }

        if (result == success && !cancel && !is_zero(prior_size))
            result = collect<Key>(prior_half,
                std::span{ &out[prior], prior_size });
        if (result == success)
            result = call_.stream_synchronize(streams_.front());
        if (result == success)
            result = call_.stream_synchronize(streams_.back());

        if (result != success)
        {
            failed_.store(true);
            return false;
        }

        if (cancel || std::ranges::all_of(out, [](uint8_t value) NOEXCEPT
            {
                return is_one(value);
            }))
            out.clear();

        return true;
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
    result_t stage(size_t half, std::span<const hash_digest> digests,
        std::span<const Key> keys,
        std::span<const ec_signature> signatures) NOEXCEPT
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
    result_t collect(size_t half, std::span<uint8_t> results) NOEXCEPT
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
    result_t launch(std::span<uint8_t> results,
        std::span<const hash_digest> digests, std::span<const Key> keys,
        std::span<const ec_signature> signatures) NOEXCEPT
    {
        auto result = call_.context_current(context_);
        if (result == success)
            result = stage(zero, digests, keys, signatures);
        if (result == success)
            result = collect<Key>(zero, results);

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
            if (call_.device_get(&device_, ordinal) == success &&
                call_.device_attribute(&major, compute_major, device_) ==
                    success &&
                call_.device_attribute(&minor, compute_minor, device_) ==
                    success &&
                (major > 7 || (major == 7 && minor >= 5)))
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
        pointer_t table{};
        size_t size{};
        if (!inflate(ptx) ||
            call_.context_current(context_) != success ||
            call_.context_limit(stack_limit, stack_bytes) != success ||
            call_.module_load(&module_, ptx.c_str()) != success ||
            call_.module_function(&ecdsa_, module_, "verify_ecdsa") !=
                success ||
            call_.module_function(&schnorr_, module_, "verify_schnorr") !=
                success ||
            call_.module_global(&table, &size, module_, "generator_table") !=
                success || size != shape::table_bytes)
            return false;

        for (size_t slice{}; slice < generator_slices.size(); ++slice)
            if (call_.copy_to(table + slice * shape::slice_bytes,
                generator_slices[slice], shape::slice_bytes) != success)
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
            calibrate_schnorr(half_bytes_, is_nonzero(limited));
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

        const auto capacity = layout<Key>::rows(bytes);
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

    functions call_{};
    device_t device_{};
    handle_t context_{};
    handle_t module_{};
    handle_t ecdsa_{};
    handle_t schnorr_{};
    pointer_t staging_{};
    size_t half_bytes_{};
    std_array<handle_t, two> streams_{};
    size_t ecdsa_rows_{};
    size_t schnorr_rows_{};
    bool retained_{};
    turns turns_{};
    std::atomic_bool failed_{};
    const bool loaded_;
};

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

// Interface.
// ----------------------------------------------------------------------------

bool compiled() NOEXCEPT
{
    return true;
}

bool available() NOEXCEPT
{
    return context::instance().available();
}

bool verify(data_chunk& out, const stopper& cancel,
    const ecdsa::batch& batch) NOEXCEPT
{
    return context::instance().verify(out, cancel, batch.digests,
        batch.points, batch.signatures, true);
}

bool verify(data_chunk& out, const stopper& cancel,
    const schnorr::batch& batch) NOEXCEPT
{
    return context::instance().verify(out, cancel, batch.digests,
        batch.points, batch.signatures, true);
}

#else

bool compiled() NOEXCEPT
{
    return false;
}

bool available() NOEXCEPT
{
    return false;
}

LCOV_EXCL_START("Not called where the device is not available.")
bool verify(data_chunk&, const stopper&, const ecdsa::batch&) NOEXCEPT
{
    return false;
}

bool verify(data_chunk&, const stopper&, const schnorr::batch&) NOEXCEPT
{
    return false;
}
LCOV_EXCL_STOP()

#endif

} // namespace cuda
} // namespace secp256k1
} // namespace system
} // namespace libbitcoin
