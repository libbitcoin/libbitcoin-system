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
#ifndef LIBBITCOIN_SYSTEM_TYPES_HPP
#define LIBBITCOIN_SYSTEM_TYPES_HPP

#include <array>
#include <tuple>
#include <type_traits>
#include <vector>
#include <bitcoin/system/allocator.hpp>

namespace libbitcoin {

/// Aliases for std types.
/// ---------------------------------------------------------------------------
/// is_same_type (is_same_v) and is_signed (is_signed_v) aliased in typelets.

/// std_array aliased because MSVS highlights `array` as a reserved word.
template <typename Type, size_t Size>
using std_array = std::array<Type, Size>;

/// std_vector aliased to allow seamless conditional allocator usage.
/// The default area is applied by default, so remains globally relocatable.
/// In some cases library code requires default allocator vectors, so these
/// are retained but are thus not usable with utilities that accept std_vector.
/// Since string and other containers are not widely used in high performance
/// scenarios requiring performant memory allocation, they are not specialized.
template <typename Type>
using std_vector = std::vector<Type, allocator<Type>>;

/// Const and non-const reference wrappers.
template <typename Type>
using ref = std::reference_wrapper<Type>;
template <typename Type>
using cref = std::reference_wrapper<const Type>;

/// Immediate if (iif) type selector.
template <bool Condition, typename IfTrue, typename IfFalse>
using iif = std::conditional_t<Condition, IfTrue, IfFalse>;

/// Extract type argument by position in parameter pack.
template <size_t Position, typename ...Args>
using argument = std::tuple_element_t<Position, std::tuple<Args...>>;

/// Define signed_size_t
/// ---------------------------------------------------------------------------

using signed_size_t = std::make_signed_t<size_t>;

/// signed_type<Bytes>, unsigned_type<Bytes>
/// Integral integer type selection by byte width and sign.
/// ---------------------------------------------------------------------------

template <size_t Bytes = 0u,
    std::enable_if_t<!(Bytes > sizeof(int64_t)), bool> = true>
using signed_type =
    iif<Bytes == 0u, signed_size_t,
        iif<Bytes == sizeof(int8_t), int8_t,
            iif<Bytes == sizeof(int16_t), int16_t,
                iif<Bytes <= sizeof(int32_t), int32_t,
                    int64_t>>>>;

template <size_t Bytes = 0u,
    std::enable_if_t<!(Bytes > sizeof(uint64_t)), bool> = true>
using unsigned_type =
    iif<Bytes == 0u, size_t,
        iif<Bytes == sizeof(uint8_t), uint8_t,
            iif<Bytes == sizeof(uint16_t), uint16_t,
                iif<Bytes <= sizeof(uint32_t), uint32_t,
                    uint64_t>>>>;

/// Sign-based type selectors.
/// ---------------------------------------------------------------------------

template <typename Type>
using to_signed_type = std::make_signed_t<Type>;

template <typename Type>
using to_unsigned_type = std::make_unsigned_t<Type>;

template <typename Type>
using to_size_type = iif<std::is_signed_v<Type>,
    signed_size_t, size_t>;

/// This creates suboptimality in x86 vs. x64 builds.
template <typename Type>
using to_promoted_type =
    iif<!std::is_integral_v<Type>, Type,
        iif<std::is_signed_v<Type>, int64_t, uint64_t>>;

/// rvalue-to-lvalue type convertor.
/// ---------------------------------------------------------------------------

template<class Type>
constexpr Type& unmove(Type&& value) noexcept
{
    return value;
}

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
using to_greater_type = iif<
    (sizeof(Left) >= sizeof(Right)), Left, Right>;

/// uintx_t
/// ---------------------------------------------------------------------------

/// Boost changed the Bits type from unsigned to size_t in v1.79.
using uintx_size_t = decltype(
    boost::multiprecision::cpp_int_backend<8u, 8u,
    boost::multiprecision::unsigned_magnitude,
    boost::multiprecision::unchecked, void>{}.size());

/// Template for constructing uintx types.
/// There is no dynamic memory allocation when minBits == maxBits.
template <uintx_size_t Bits> // <== uintx_size_t
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
    iif<Bytes == 0u, size_t,
        iif<Bytes == sizeof(uint8_t), uint8_t,
            iif<Bytes == sizeof(uint16_t), uint16_t,
                iif<Bytes == sizeof(uint32_t), uint32_t,
                    iif<Bytes == sizeof(uint64_t), uint64_t,
                        uintx_t<Bytes * 8u>>>>>>;

/// Argument placeholders.
/// ---------------------------------------------------------------------------

struct place1 {};
struct place2 {};
struct place3 {};

} // namespace libbitcoin

#endif
