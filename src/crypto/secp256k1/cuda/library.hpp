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
#ifndef LIBBITCOIN_SYSTEM_SRC_CRYPTO_SECP256K1_CUDA_LIBRARY_HPP
#define LIBBITCOIN_SYSTEM_SRC_CRYPTO_SECP256K1_CUDA_LIBRARY_HPP

#include <boost/beast/zlib/inflate_stream.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include "ptx.hpp"

#if !defined(HAVE_MSC)
    #include <dlfcn.h>
#endif

namespace libbitcoin {
namespace system {
namespace secp256k1 {
namespace cuda {

BC_PUSH_WARNING(NO_REINTERPRET_CAST)

// Driver library.
// ----------------------------------------------------------------------------

#if defined(HAVE_MSC)
inline void* load_library() NOEXCEPT
{
    return reinterpret_cast<void*>(::LoadLibraryW(L"nvcuda.dll"));
}

inline void* load_symbol(void* library, const char* name) NOEXCEPT
{
    return reinterpret_cast<void*>(::GetProcAddress(
        static_cast<HMODULE>(library), name));
}
#else
inline void* load_library() NOEXCEPT
{
    return ::dlopen("libcuda.so.1", RTLD_NOW);
}

inline void* load_symbol(void* library, const char* name) NOEXCEPT
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
inline bool resolve(Function& function, void* library,
    const char* name) NOEXCEPT
{
    function = reinterpret_cast<Function>(load_symbol(library, name));
    return !is_null(function);
}

inline bool resolve(functions& out, void* library) NOEXCEPT
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

inline bool inflate(std::string& out) NOEXCEPT
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

    // The stream may report need_buffers in place of end_of_stream when the
    // final block ends in the last input byte, so completion is by sizes.
    return is_zero(params.avail_in) && params.total_out == out.size() &&
        (ec == zlib::error::end_of_stream || ec == zlib::error::need_buffers);
}

BC_POP_WARNING()

} // namespace cuda
} // namespace secp256k1
} // namespace system
} // namespace libbitcoin

#endif
