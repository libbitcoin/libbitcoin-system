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
#include <bitcoin/system/crypto/secp256k1/batch/device.hpp>

#if defined(HAVE_LINUX)
    #include <dlfcn.h>
#endif
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/unicode/unicode.hpp>

namespace libbitcoin {
namespace system {

#if (defined(HAVE_WINDOWS) || defined(HAVE_LINUX)) && \
    (defined(HAVE_X64) || defined(HAVE_ARM64))

// The cuda runtime installs with the device driver, so its device count
// answers for the backend that batch acceleration requires when compiled.
using cu_init_t = int32_t(*)(uint32_t);
using cu_count_t = int32_t(*)(int32_t*);
using cu_get_t = int32_t(*)(int32_t*, int32_t);
using cu_attribute_t = int32_t(*)(int32_t*, int32_t, int32_t);
using cu_bus_t = int32_t(*)(char*, int32_t, int32_t);
using nvml_init_t = int32_t(*)();
using nvml_shutdown_t = int32_t(*)();
using nvml_handle_t = int32_t(*)(const char*, void**);
using nvml_ecc_t = int32_t(*)(void*, int32_t*, int32_t*);

// CU_DEVICE_ATTRIBUTE_COMPUTE_CAPABILITY_MAJOR/MINOR, and the floor of the
// compiled cuda kernels (turing).
constexpr int32_t cu_major = 75;
constexpr int32_t cu_minor = 76;
constexpr int32_t cuda_major = 7;
constexpr int32_t cuda_minor = 5;

// CU_DEVICE_ATTRIBUTE_ECC_ENABLED.
constexpr int32_t cu_ecc = 32;

// NVML_ERROR_NOT_SUPPORTED, returned for a device without ecc memory.
constexpr int32_t nvml_unsupported = 3;

// NVML_DEVICE_PCI_BUS_ID_BUFFER_SIZE.
constexpr int32_t bus_size = 32;

#if defined(HAVE_WINDOWS)

constexpr auto cuda_library = "nvcuda.dll";
constexpr auto nvml_library = "nvml.dll";
using library_t = HMODULE;

static library_t load_library(const std::string& name) NOEXCEPT
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    return ::LoadLibraryW(to_utf16(name).c_str());
    BC_POP_WARNING()
}

static void* load_symbol(library_t library, const char* name) NOEXCEPT
{
    BC_PUSH_WARNING(NO_REINTERPRET_CAST)
    return reinterpret_cast<void*>(::GetProcAddress(library, name));
    BC_POP_WARNING()
}

static void free_library(library_t library) NOEXCEPT
{
    ::FreeLibrary(library);
}

#else

constexpr auto cuda_library = "libcuda.so.1";
constexpr auto nvml_library = "libnvidia-ml.so.1";
using library_t = void*;

static library_t load_library(const std::string& name) NOEXCEPT
{
    return ::dlopen(name.c_str(), RTLD_LAZY);
}

static void* load_symbol(library_t library, const char* name) NOEXCEPT
{
    return ::dlsym(library, name);
}

static void free_library(library_t library) NOEXCEPT
{
    ::dlclose(library);
}

#endif

static bool find_cuda(bool& ecc, std::string& bus) NOEXCEPT
{
    const auto library = load_library(cuda_library);
    if (is_null(library))
        return false;

    LCOV_EXCL_START("Requires a cuda driver.")
    BC_PUSH_WARNING(NO_REINTERPRET_CAST)
    const auto initialize = reinterpret_cast<cu_init_t>(
        load_symbol(library, "cuInit"));
    const auto count = reinterpret_cast<cu_count_t>(
        load_symbol(library, "cuDeviceGetCount"));
    const auto get = reinterpret_cast<cu_get_t>(
        load_symbol(library, "cuDeviceGet"));
    const auto attribute = reinterpret_cast<cu_attribute_t>(
        load_symbol(library, "cuDeviceGetAttribute"));
    const auto bus_id = reinterpret_cast<cu_bus_t>(
        load_symbol(library, "cuDeviceGetPCIBusId"));
    BC_POP_WARNING()

    auto found = false;
    int32_t devices{};
    if (!is_null(initialize) && !is_null(count) && !is_null(get) &&
        !is_null(attribute) && is_zero(initialize(0)) &&
        is_zero(count(&devices)))
    {
        for (int32_t ordinal{}; ordinal < devices; ++ordinal)
        {
            int32_t device{}, major{}, minor{};
            if (is_zero(get(&device, ordinal)) &&
                is_zero(attribute(&major, cu_major, device)) &&
                is_zero(attribute(&minor, cu_minor, device)) &&
                ((major > cuda_major) ||
                    (major == cuda_major && minor >= cuda_minor)))
            {
                int32_t enabled{};
                ecc = is_zero(attribute(&enabled, cu_ecc, device)) &&
                    !is_zero(enabled);

                char id[bus_size]{};
                if (!is_null(bus_id) && is_zero(bus_id(id, bus_size, device)))
                    bus = id;

                found = true;
                break;
            }
        }
    }

    free_library(library);
    return found;
    LCOV_EXCL_STOP()
}

// Ecc support of the device at the bus, false if not determinable.
static bool nvml_ecc(bool& supported, const std::string& bus) NOEXCEPT
{
    const auto library = load_library(nvml_library);
    if (is_null(library))
        return false;

    LCOV_EXCL_START("Requires an nvidia driver.")
    BC_PUSH_WARNING(NO_REINTERPRET_CAST)
    const auto initialize = reinterpret_cast<nvml_init_t>(
        load_symbol(library, "nvmlInit_v2"));
    const auto shutdown = reinterpret_cast<nvml_shutdown_t>(
        load_symbol(library, "nvmlShutdown"));
    const auto handle = reinterpret_cast<nvml_handle_t>(
        load_symbol(library, "nvmlDeviceGetHandleByPciBusId_v2"));
    const auto mode = reinterpret_cast<nvml_ecc_t>(
        load_symbol(library, "nvmlDeviceGetEccMode"));
    BC_POP_WARNING()

    auto known = false;
    if (!bus.empty() && !is_null(initialize) && !is_null(shutdown) &&
        !is_null(handle) && !is_null(mode) && is_zero(initialize()))
    {
        void* device{};
        int32_t current{}, pending{};
        if (is_zero(handle(bus.c_str(), &device)))
        {
            const auto result = mode(device, &current, &pending);
            known = is_zero(result) || result == nvml_unsupported;
            supported = is_zero(result);
        }

        shutdown();
    }

    free_library(library);
    return known;
    LCOV_EXCL_STOP()
}

bool cuda_device() NOEXCEPT
{
    bool enabled{};
    std::string bus{};
    return find_cuda(enabled, bus);
}

// Enabled implies supported where the management library is unavailable.
bool cuda_ecc() NOEXCEPT
{
    bool enabled{}, supported{};
    std::string bus{};
    return find_cuda(enabled, bus) &&
        (nvml_ecc(supported, bus) ? supported : enabled);
}

bool cuda_ecc_enabled() NOEXCEPT
{
    bool enabled{};
    std::string bus{};
    return find_cuda(enabled, bus) && enabled;
}

#else

bool cuda_device() NOEXCEPT
{
    return false;
}

bool cuda_ecc() NOEXCEPT
{
    return false;
}

bool cuda_ecc_enabled() NOEXCEPT
{
    return false;
}

#endif

} // namespace system
} // namespace libbitcoin
