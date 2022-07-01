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

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <bitcoin/system/exceptions.hpp>

namespace libbitcoin {

/// signed_size_t
/// ---------------------------------------------------------------------------

using signed_size_t = std::make_signed_t<size_t>;

/// Size-based type selectors.
/// ---------------------------------------------------------------------------

/// Signed integral type selection by byte width and sign.
template <size_t Bytes = 0u,
    std::enable_if_t<!(Bytes > sizeof(int64_t)), bool> = true>
using signed_type =
    std::conditional_t<Bytes == 0u, signed_size_t,
        std::conditional_t<Bytes == sizeof(int8_t), int8_t,
            std::conditional_t<Bytes == sizeof(int16_t), int16_t,
                std::conditional_t<Bytes <= sizeof(int32_t), int32_t,
                    int64_t>>>>;

/// Unsigned integral type selection by byte width and sign.
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

////template <typename Type>
////using to_size_type = std::conditional_t<std::is_signed_v<Type>,
////    signed_size_t, size_t>;

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

/// uintx_t
/// ---------------------------------------------------------------------------

/// Template for constructing uintx types.
/// There is no dynamic memory allocation when minBits == maxBits.
template <uint32_t Bits> // <= uint32_t
using uintx_t = boost::multiprecision::number<
    boost::multiprecision::cpp_int_backend<Bits, Bits,
    boost::multiprecision::unsigned_magnitude,
    boost::multiprecision::unchecked, void>>;

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

/// Guard type assumptions within the codebase.
/// ---------------------------------------------------------------------------

// signed_size_t is not the same type, despite having the same size.
// In which case long and int are both 32 bit types, but are not the same type.
////static_assert(
////    is_same_type<signed_size_t, int32_t> ||
////    is_same_type<signed_size_t, int64_t>);

// This relies on the construction of signed_size_t using std::make_signed_t<size_t>,
// as this is how the to_signed_type/to_unsigned_type conversions also work.
static_assert(
    std::is_same_v<to_signed_type<size_t>, signed_size_t> &&
    std::is_same_v<to_unsigned_type<signed_size_t>, size_t>);

// This are design limitations, and not a matter of C++ specification.
static_assert(sizeof(char) == 1u);
static_assert(
    sizeof(size_t) == sizeof(uint32_t) ||
    sizeof(size_t) == sizeof(uint64_t));

// This tests that signed_size_t confirms to the design limitation.
static_assert(
    sizeof(signed_size_t) == sizeof(int32_t) ||
    sizeof(signed_size_t) == sizeof(int64_t));

// This is expected.
static_assert(sizeof(size_t) == sizeof(signed_size_t));

} // namespace libbitcoin

#endif
