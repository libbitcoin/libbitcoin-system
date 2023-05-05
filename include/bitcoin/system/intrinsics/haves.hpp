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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_HAVES_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_HAVES_HPP

#include <limits>
#include <type_traits>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/xcpu/cpuid.hpp>
#include <bitcoin/system/intrinsics/xcpu/functional_128.hpp>
#include <bitcoin/system/intrinsics/xcpu/functional_256.hpp>
#include <bitcoin/system/intrinsics/xcpu/functional_512.hpp>

namespace libbitcoin {
namespace system {

// Functions may only be constexpr conditionally.
BC_PUSH_WARNING(USE_CONSTEXPR_FOR_FUNCTION)

/// Constant symbols for compiled intrinsics interfaces.
/// ---------------------------------------------------------------------------

// TODO: unclear if there is a necessary x64 limitation.
#if defined(HAVE_X64) && defined(HAVE_XASSEMBLY)
    constexpr auto with_sse41a = true;
#else
    constexpr auto with_sse41a = false;
#endif
#if defined(HAVE_SSE4)
    constexpr auto with_sse41 = true;
#else
    constexpr auto with_sse41 = false;
#endif
#if defined(HAVE_AVX2)
    constexpr auto with_avx2 = true;
#else
    constexpr auto with_avx2 = false;
#endif
#if defined(HAVE_AVX512)
    constexpr auto with_avx512 = true;
#else
    constexpr auto with_avx512 = false;
#endif
#if defined(HAVE_SHANI)
    constexpr auto with_shani = true;
#else
    constexpr auto with_shani = false;
#endif
#if defined(HAVE_NEON)
    constexpr auto with_neon = true;
#else
    constexpr auto with_neon = false;
#endif

/// Runtime checks for Intel SIMD and ARM Neon availability.
/// ---------------------------------------------------------------------------

namespace cpu1_0
{
    constexpr auto leaf = 1;
    constexpr auto subleaf = 0;
    constexpr auto sse41_ecx_bit = 19;
    constexpr auto xsave_ecx_bit = 27;
    constexpr auto avx_ecx_bit = 28;
}

// wikichip.org/wiki/x86/avx-512
namespace cpu7_0
{
    constexpr auto leaf = 7;
    constexpr auto subleaf = 0;
    constexpr auto avx2_ebx_bit = 5;
    ////constexpr auto avx512f_ebx_bit = 16;
    constexpr auto avx512bw_ebx_bit = 30;
    constexpr auto shani_ebx_bit = 29;
}

namespace xcr0
{
    constexpr auto feature = 0;
    constexpr auto sse_bit = 1;
    constexpr auto avx_bit = 2;
}

// Local util because no dependency on /math.
template <size_t Bit, typename Value>
constexpr bool get_bit(Value value) NOEXCEPT
{
    constexpr auto mask = (Value{ 1 } << Bit);
    return !is_zero(value & mask);
}

inline bool try_shani() NOEXCEPT
{
    if constexpr (with_shani)
    {
        uint32_t eax{}, ebx{}, ecx{}, edx{};
        return get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
            && get_bit<cpu1_0::sse41_ecx_bit>(ecx)      // SSE4.1
            && (eax >= cpu7_0::leaf)
            && get_cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
            && get_bit<cpu7_0::shani_ebx_bit>(ebx);     // SHA-NI
    }
    else
        return false;
}

inline bool try_avx512() NOEXCEPT
{
    if constexpr (with_avx512)
    {
        uint64_t extended{};
        uint32_t eax{}, ebx{}, ecx{}, edx{};
        return get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
            && get_bit<cpu1_0::sse41_ecx_bit>(ecx)      // SSE4.1
            && get_bit<cpu1_0::xsave_ecx_bit>(ecx)      // XSAVE
            && get_bit<cpu1_0::avx_ecx_bit>(ecx)        // AVX
            && get_xcr(extended, xcr0::feature)
            && get_bit<xcr0::sse_bit>(extended)
            && get_bit<xcr0::avx_bit>(extended)
            && get_cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
            && get_bit<cpu7_0::avx512bw_ebx_bit>(ebx);  // AVX512BW
    }
    else
        return false;
}

inline bool try_avx2() NOEXCEPT
{
    if constexpr (with_avx2)
    {
        uint64_t extended{};
        uint32_t eax{}, ebx{}, ecx{}, edx{};
        return get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
            && get_bit<cpu1_0::sse41_ecx_bit>(ecx)      // SSE4.1
            && get_bit<cpu1_0::xsave_ecx_bit>(ecx)      // XSAVE
            && get_bit<cpu1_0::avx_ecx_bit>(ecx)        // AVX
            && get_xcr(extended, xcr0::feature)
            && get_bit<xcr0::sse_bit>(extended)
            && get_bit<xcr0::avx_bit>(extended)
            && get_cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
            && get_bit<cpu7_0::avx2_ebx_bit>(ebx);      // AVX2
    }
    else
        return false;
}

inline bool try_sse41() NOEXCEPT
{
    if constexpr (with_sse41)
    {
        uint32_t eax{}, ebx{}, ecx{}, edx{};
        return get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
            && get_bit<cpu1_0::sse41_ecx_bit>(ecx);     // SSE4.1
    }
    else
        return false;
}

inline bool try_sse41a() NOEXCEPT
{
    // SSE41a implies assembly, with no need for SSE41 intrinsics.
    if constexpr (with_sse41a)
    {
        uint32_t eax{}, ebx{}, ecx{}, edx{};
        return get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
            && get_bit<cpu1_0::sse41_ecx_bit>(ecx);     // SSE4.1
    }
    else
        return false;
}

constexpr bool try_neon() NOEXCEPT
{
    if constexpr (with_neon)
    {
        // TODO
        return false;
    }
    else
        return false;
}

/// Runtime tests for Intel SIMD, and ARM SIMD (Neon) availability.
/// ---------------------------------------------------------------------------
/// Thread local statics introduce a guard, not ideal for excessive iteration.
/// But no performance impact measured in sha except minor impact to merkle.
/// These keep binary portable, otherwise can reply on "with" symbols.

inline bool have_shani() NOEXCEPT
{
    static auto enable = try_shani();
    return enable;
}

inline bool have_avx512() NOEXCEPT
{
    static auto enable = try_avx512();
    return enable;
}

inline bool have_avx2() NOEXCEPT
{
    static auto enable = try_avx2();
    return enable;
}

inline bool have_sse41() NOEXCEPT
{
    static auto enable = try_sse41();
    return enable;
}

inline bool have_sse41a() NOEXCEPT
{
    static auto enable = try_sse41a();
    return enable;
}

inline bool have_neon() NOEXCEPT
{
    static auto enable = try_neon();
    return enable;
}

/// ---------------------------------------------------------------------------
/// xint types are always defined, though are mocked when not compiled.
/// Use with_ constants to check for compiled option and have_ functions to
/// check for runtime API availability. This enables mostly unconditional code.

template <typename Type>
constexpr auto is_extended =
     is_same_type<Type, xint128_t> ||
     is_same_type<Type, xint256_t> ||
     is_same_type<Type, xint512_t>;

template <typename Type>
using if_extended = bool_if<is_extended<Type>>;

/// Define lane-expanded 32/64 bit types.
template <typename Integral, size_t Lanes,
    if_not_greater<safe_multiply(sizeof(Integral), Lanes),
        sizeof(xint512_t)> = true>
using to_extended =
    iif<capacity<uint8_t, Integral, Lanes> == one, uint8_t,
        iif<capacity<uint16_t, Integral, Lanes> == one, uint16_t,
            iif<capacity<uint32_t, Integral, Lanes> == one, uint32_t,
                iif<capacity<uint64_t, Integral, Lanes> == one, uint64_t,
                    iif<capacity<xint128_t, Integral, Lanes> == one, xint128_t,
                        iif<capacity<xint256_t, Integral, Lanes> == one, xint256_t,
                            xint512_t>>>>>>;

/// Runtime time availability of extended integer intrinsics, as a template
/// function of the extended integer type.
template <typename Extended, if_extended<Extended> = true>
inline bool have() NOEXCEPT
{
    if constexpr (is_same_type<Extended, xint512_t>)
        return have_avx512();
    else if constexpr (is_same_type<Extended, xint256_t>)
        return have_avx2();
    else if constexpr (is_same_type<Extended, xint128_t>)
        return have_sse41();
    else return false;
}

/// Runtime time availability of extended integer filled by Lanes Integrals.
template <typename Integral, size_t Lanes,
    if_integral_integer<Integral> = true>
inline bool have_lanes() NOEXCEPT
{
    if constexpr (capacity<xint512_t, Integral> == Lanes)
        return have_avx512();
    else if constexpr (capacity<xint256_t, Integral> == Lanes)
        return have_avx2();
    else if constexpr (capacity<xint128_t, Integral> == Lanes)
        return have_sse41();
    else return false;
}

BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin

#endif
