/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_TYPES_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_TYPES_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

// Each sve typed is identical but unique identity carries lane expectation.
#define SVE_TYPE(bits) \
    struct _xint##bits##_t \
    { \
        svuint32_t value; \
        explicit _xint##bits##_t(svuint32_t a) : value(a) {} \
        _xint##bits##_t() = default; \
    }

// xint types are always defined, though must be mocked when not aliased.
#define MOCK_TYPE(bits) std_array<uint8_t, bytes<bits>>

// Specialiations are facilitated by direct use of bc namespace.
namespace libbitcoin {

/// Always define xint128_t, xint256_t, xint512_t. Avoids conditional compile.
/// ---------------------------------------------------------------------------

#if defined(HAVE_SVE)
    #include <arm_sve.h>
    using xint128_t = SVE_TYPE(128);
    using xint256_t = SVE_TYPE(256);
    using xint512_t = SVE_TYPE(512);
#endif
#if defined(HAVE_NEON)
    #include <arm_neon.h>
    using xint128_t = uint32x4_t;
    using xint256_t = MOCK_TYPE(256);
    using xint512_t = MOCK_TYPE(512);
#endif
#if defined(HAVE_XCPU)
    #include <immintrin.h>
    #if defined(HAVE_SSE4)
        using xint128_t = __m128i;
    #else
        using xint128_t = MOCK_TYPE(128);
    #endif
    #if defined(HAVE_AVX2)
        using xint256_t = __m256i;
    #else
        using xint256_t = MOCK_TYPE(256);
    #endif
    #if defined(HAVE_AVX512)
        using xint512_t = __m512i;
    #else
        using xint512_t = MOCK_TYPE(512);
    #endif
#endif
#if !defined(HAVE_SVE) && !defined(HAVE_NEON) && !defined(HAVE_XCPU)
    using xint128_t = MOCK_TYPE(128);
    using xint256_t = MOCK_TYPE(256);
    using xint512_t = MOCK_TYPE(512);
#endif

/// Lane selectors.
/// ---------------------------------------------------------------------------

/// Specializations of size_of (used by capacity) for SVE types.
template<>
constexpr size_t size_of<xint128_t>() { return bytes<128>; }
template<>
constexpr size_t size_of<xint256_t>() { return bytes<256>; }
template<>
constexpr size_t size_of<xint512_t>() { return bytes<512>; }

/// Define lane-expanded 32/64 bit types.
template <typename Integral, size_t Lanes,
    if_integral<Integral> = true,
    if_not_greater<system::safe_multiply(sizeof(Integral), Lanes),
        size_of<xint512_t>()> = true>
using to_extended =
    iif<is_one(capacity<uint8_t, Integral, Lanes>), uint8_t,
        iif<is_one(capacity<uint16_t, Integral, Lanes>), uint16_t,
            iif<is_one(capacity<uint32_t, Integral, Lanes>), uint32_t,
                iif<is_one(capacity<uint64_t, Integral, Lanes>), uint64_t,
                    iif<is_one(capacity<xint128_t, Integral, Lanes>), xint128_t,
                        iif<is_one(capacity<xint256_t, Integral, Lanes>), xint256_t,
                            xint512_t>>>>>>;

template <typename Type>
constexpr auto is_extended =
     is_same_type<Type, xint128_t> ||
     is_same_type<Type, xint256_t> ||
     is_same_type<Type, xint512_t>;

template <typename Type>
using if_extended = bool_if<is_extended<Type>>;

template <typename Extended, if_extended<Extended> = true>
constexpr bool have_() NOEXCEPT
{
    if constexpr (is_same_type<Extended, xint512_t>)
        return have_512;
    else if constexpr (is_same_type<Extended, xint256_t>)
        return have_256;
    else if constexpr (is_same_type<Extended, xint128_t>)
        return have_128;
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
        return have_512;
    else if constexpr (capacity<xint256_t, Integral> == Lanes)
        return have_256;
    else if constexpr (capacity<xint128_t, Integral> == Lanes)
        return have_128;
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

} // namespace libbitcoin

#undef SVE_TYPE
#undef MOCK_TYPE

#endif
