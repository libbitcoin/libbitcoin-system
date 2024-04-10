/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_XCPU_CPUID_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_XCPU_CPUID_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/xcpu/defines.hpp>

/// Common CPU instructions used to locate CPU features.

namespace libbitcoin {
namespace system {

#if defined(HAVE_XCPU)

inline bool get_xcr(uint64_t& value, uint32_t index) noexcept
{
#if defined(HAVE_XGETBV)
    value = _xgetbv(index);
    return true;
#elif defined(HAVE_XASSEMBLY)
    uint32_t a{}, d{};
    __asm__("xgetbv" : "=a"(a), "=d"(d) : "c"(index));
    value = (static_cast<uint64_t>(d) << 32) | a;
    return true;
#else
    return false;
#endif
}

inline bool get_cpu(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d,
    uint32_t leaf, uint32_t subleaf) noexcept
{
#if defined(HAVE_XCPUIDEX)
    int out[4]{};
    __cpuidex(&out[0], leaf, subleaf);
    a = out[0];
    b = out[1];
    c = out[2];
    d = out[3];
    return true;
#elif defined(HAVE_XCPUID_COUNT)
    __cpuid_count(a, b, c, d, leaf, subleaf);
    return true;
#elif defined(HAVE_XASSEMBLY)
    __asm__("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "0"(leaf), "2"(subleaf));
    return true;
#else
    return false;
#endif
}

#else // HAVE_XCPU

inline bool get_xcr(uint64_t&, uint32_t) noexcept
{
    return false;
}
inline bool get_cpu(uint32_t&, uint32_t&, uint32_t&, uint32_t&, uint32_t,
    uint32_t) noexcept
{
    return false;
}

#endif // HAVE_XCPU

} // namespace system
} // namespace libbitcoin

#endif
