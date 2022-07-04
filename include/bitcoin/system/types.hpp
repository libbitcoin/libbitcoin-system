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
#ifndef LIBBITCOIN_SYSTEM_TYPES_HPP
#define LIBBITCOIN_SYSTEM_TYPES_HPP

#include <array>
#include <type_traits>
#include <bitcoin/system/exceptions.hpp>

namespace libbitcoin {

/// signed_size_t
/// ---------------------------------------------------------------------------

using signed_size_t = std::make_signed_t<size_t>;

/// signed_type<Bytes>, unsigned_type<Bytes>
/// Integral integer type selection by byte width and sign.
/// ---------------------------------------------------------------------------

template <size_t Bytes = 0u,
    std::enable_if_t<!(Bytes > sizeof(int64_t)), bool> = true>
using signed_type =
    std::conditional_t<Bytes == 0u, signed_size_t,
        std::conditional_t<Bytes == sizeof(int8_t), int8_t,
            std::conditional_t<Bytes == sizeof(int16_t), int16_t,
                std::conditional_t<Bytes <= sizeof(int32_t), int32_t,
                    int64_t>>>>;


template <size_t Bytes = 0u,
    std::enable_if_t<!(Bytes > sizeof(uint64_t)), bool> = true>
using unsigned_type =
    std::conditional_t<Bytes == 0u, size_t,
        std::conditional_t<Bytes == sizeof(uint8_t), uint8_t,
            std::conditional_t<Bytes == sizeof(uint16_t), uint16_t,
                std::conditional_t<Bytes <= sizeof(uint32_t), uint32_t,
                    uint64_t>>>>;

/// Sign-based type selectors.
/// ---------------------------------------------------------------------------

template <typename Type>
using to_signed_type = std::make_signed_t<Type>;

template <typename Type>
using to_unsigned_type = std::make_unsigned_t<Type>;

template <typename Type>
using to_size_type = std::conditional_t<std::is_signed_v<Type>,
    signed_size_t, size_t>;

/// Promotion-based type selectors.
/// ---------------------------------------------------------------------------

/// Alias for -> decltype(Left [op] Right), resulting integral promotion type.
template <typename Left, typename Right>
using to_common_type = std::common_type_t<Left, Right>;

/// Not possible with std::common_type.
/// Alias for -> decltype([op] Unary), resulting integral promotion type.
/// This provides a type constraint for depromotion of native operator results.
template <typename Unary>
using to_common_sized_type = decltype(+Unary{});

/// Promotion to larger of two types, independent of sign.
/// This must prefer the Left over right, so that left controls explicit sign.
template <typename Left, typename Right>
using to_greater_type = std::conditional_t<
    (sizeof(Left) >= sizeof(Right)), Left, Right>;

/// uintx_t
/// ---------------------------------------------------------------------------

/// Boost changed the Bits type from unsigned to size_t in v1.79.
/// In some compilers the template binding fails if size_t is used but
/// boost <1.79 and build is 32 bit (unsigned vs. unsigned long).
/// So we derive type cpp_int_size_type and then hide it in uintx_t.
using cpp_int_size_type = decltype(
    boost::multiprecision::cpp_int_backend<8u, 8u,
    boost::multiprecision::unsigned_magnitude,
    boost::multiprecision::unchecked, void>{}.size());

/// Template for constructing uintx types (on Size_t).
/// There is no dynamic memory allocation when minBits == maxBits.
BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
template <size_t Bits>
using uintx_t = boost::multiprecision::number<
    boost::multiprecision::cpp_int_backend<
        static_cast<cpp_int_size_type>(Bits),
        static_cast<cpp_int_size_type>(Bits),
    boost::multiprecision::unsigned_magnitude,
    boost::multiprecision::unchecked, void>>;
BC_POP_WARNING()

/// Cannot generalize because no boost support for unsigned arbitrary precision.
/// Otherwise uintx_t<0> would suffice. uintx can construct from uintx_t types
/// but is not a base type. Use of signed types here would also not generalize
/// as boost uses a different allocator for arbitrary precision. So we are stuck
/// with this seam, requiring template specialization for uintx.
typedef boost::multiprecision::cpp_int uintx;

/// C++11: use std::integral_constant (up to primitives limit).
/// These are predefined due to use in the library, but any width is valid.
typedef uintx_t<5u> uint5_t;
typedef uintx_t<11u> uint11_t;
typedef uintx_t<48u> uint48_t;
typedef uintx_t<128u> uint128_t;
typedef uintx_t<160u> uint160_t;
typedef uintx_t<256u> uint256_t;
typedef uintx_t<512u> uint512_t;

/// No integral type rounding, all types exact byte size.
/// Prefers the exact integral type and falls back to uintx_t.
template <size_t Bytes>
using unsigned_exact_type =
    std::conditional_t<Bytes == 0u, size_t,
        std::conditional_t<Bytes == sizeof(uint8_t), uint8_t,
            std::conditional_t<Bytes == sizeof(uint16_t), uint16_t,
                std::conditional_t<Bytes == sizeof(uint32_t), uint32_t,
                    std::conditional_t<Bytes == sizeof(uint64_t), uint64_t,
                        uintx_t<Bytes * 8u>>>>>>;

/// Argument placeholders.
/// ---------------------------------------------------------------------------

struct place1 {};
struct place2 {};
struct place3 {};

} // namespace libbitcoin

#endif
