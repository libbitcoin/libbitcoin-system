/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_ASSEMBLY_HPP
#define LIBBITCOIN_SYSTEM_ASSEMBLY_HPP

#include <bitcoin/system/literals.hpp>

#if defined(HAVE_XCPU)

#include <cstdint>
#include <immintrin.h>

/// Common CPU instructions used to locate CPU features.

// MSC inline assembly (via __asm, no __asm__ support) does not support ARM
// and x64. Since we cross compile to x64 we consider this lack of support
// prohibitive for __asm. github.com/MicrosoftDocs/cpp-docs/blob/main/docs/
// assembler/inline/inline-assembler.md

namespace libbitcoin {

namespace cpu1_0
{
    constexpr auto leaf = 1;
    constexpr auto subleaf = 0;
    constexpr auto sse4_ecx_bit = 19;
    constexpr auto xsave_ecx_bit = 27;
    constexpr auto avx_ecx_bit = 28;
}

namespace cpu7_0
{
    constexpr auto leaf = 7;
    constexpr auto subleaf = 0;
    constexpr auto avx2_ebx_bit = 5;
    constexpr auto shani_ebx_bit = 29;
}

namespace xcr0
{
    constexpr auto feature = 0;
    constexpr auto sse_bit = 1;
    constexpr auto avx_bit = 2;
}

template <size_t Bit, typename Value>
constexpr bool get_bit(Value value) noexcept
{
    constexpr auto mask = (Value{ 1 } << Bit);
    return (value & mask) != 0;
}

inline bool xgetbv(uint64_t& value, uint32_t index) noexcept
{
#if defined(HAVE_XGETBV)
    value = _xgetbv(index);
    return true;
#elif defined(HAVE_XASSEMBLY)
    // Compile error: built-in _xgetbv requires target feature xsave.
    // But xsave can only be determined at run time.
    uint32_t a{}, d{};
    __asm__("xgetbv" : "=a"(a), "=d"(d) : "c"(index));
    value = (static_cast<uint64_t>(d) << 32) | a;
    return true;
#else
    return false;
#endif
}

inline bool cpuid_count(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d,
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
    __cpuid_count(leaf, subleaf, a, b, c, d);
    return true;
#elif defined(HAVE_XASSEMBLY)
    // __cpuid_count commonly undefined.
    __asm__("cpuid" : "=a"(a), "=b"(b), "=c"(c), "=d"(d) : "0"(leaf), "2"(subleaf));
    return true;
#else
    return false;
#endif
}

} // namespace libbitcoin

#endif // HAVE_XCPU
#endif
