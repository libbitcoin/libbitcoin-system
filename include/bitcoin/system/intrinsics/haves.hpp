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
// sse41a (assembly) optimization is implemented without assembly.

#if defined(HAVE_SSE41)
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

/// Runtime checks for Intel SIMD and ARM SIMD (Neon) availability.
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
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse41_ecx_bit>(ecx)      // SSE4.1
        && (eax >= cpu7_0::leaf)
        && get_cpu(eax, ebx, ecx, edx, cpu7_0::leaf, cpu7_0::subleaf)
        && get_bit<cpu7_0::shani_ebx_bit>(ebx);     // SHA-NI
}

inline bool try_avx512() NOEXCEPT
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
        && get_bit<cpu7_0::avx2_ebx_bit>(ebx)       // AVX2 (implied?)
        && get_bit<cpu7_0::avx512bw_ebx_bit>(ebx);  // AVX512BW
}

inline bool try_avx2() NOEXCEPT
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

inline bool try_sse41() NOEXCEPT
{
    uint32_t eax{}, ebx{}, ecx{}, edx{};
    return get_cpu(eax, ebx, ecx, edx, cpu1_0::leaf, cpu1_0::subleaf)
        && get_bit<cpu1_0::sse41_ecx_bit>(ecx);     // SSE4.1
}

constexpr bool try_neon() NOEXCEPT
{
    // TODO: neon detection code.
    return false;
}

/// Type system helpers.
/// ---------------------------------------------------------------------------
/// xint types are always defined, though are mocked when not compiled.

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
    iif<is_one(capacity<uint8_t, Integral, Lanes>), uint8_t,
        iif<is_one(capacity<uint16_t, Integral, Lanes>), uint16_t,
            iif<is_one(capacity<uint32_t, Integral, Lanes>), uint32_t,
                iif<is_one(capacity<uint64_t, Integral, Lanes>), uint64_t,
                    iif<is_one(capacity<xint128_t, Integral, Lanes>), xint128_t,
                        iif<is_one(capacity<xint256_t, Integral, Lanes>), xint256_t,
                            xint512_t>>>>>>;

template <typename Extended, if_extended<Extended> = true>
constexpr bool have_() NOEXCEPT
{
    if constexpr (is_same_type<Extended, xint512_t>)
        return with_avx512;
    else if constexpr (is_same_type<Extended, xint256_t>)
        return with_avx2;
    else if constexpr (is_same_type<Extended, xint128_t>)
        return with_sse41;
    else
        return false;
}

/// Availability of extended integer intrinsics.
template <typename Extended, if_extended<Extended> = true>
constexpr bool have = have_<Extended>();

template <typename Integral, size_t Lanes,
    if_integral<Integral> = true>
constexpr bool have_lanes_() NOEXCEPT
{
    if constexpr (capacity<xint512_t, Integral> == Lanes)
        return with_avx512;
    else if constexpr (capacity<xint256_t, Integral> == Lanes)
        return with_avx2;
    else if constexpr (capacity<xint128_t, Integral> == Lanes)
        return with_sse41;
    else
        return false;
}

template <typename Integral, size_t Lanes,
    if_non_integral<Integral> = true>
constexpr bool have_lanes_() NOEXCEPT
{
    return false;
}

/// Availability of extended integer filled by Lanes Integrals.
template <typename Integral, size_t Lanes,
    if_integral<Integral> = true>
constexpr bool have_lanes = have_lanes_<Integral, Lanes>();

BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin

#endif
