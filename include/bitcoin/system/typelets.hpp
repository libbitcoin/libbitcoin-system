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
#ifndef LIBBITCOIN_SYSTEM_TYPELETS_HPP
#define LIBBITCOIN_SYSTEM_TYPELETS_HPP

#include <limits>
#include <type_traits>
#include <bitcoin/system/funclets.hpp>

namespace libbitcoin {

/// Simple functions over type argument(s).
/// ---------------------------------------------------------------------------

/// Same size and signedness, independent of const and volatility.
template <typename Left, typename Right>
constexpr bool is_same_type = std::is_same_v<Left, Right>;

/// bool is unsigned: bool(-1) < bool(0). w/char sign unspecified.
/// w/charxx_t types are unsigned. iostream relies on w/char.
template <typename Type>
constexpr bool is_signed = std::is_signed_v<Type>;

/// sizeof(Left) == sizeof(Right).
template <typename Left, typename Right>
constexpr bool is_same_size = (sizeof(Left) == sizeof(Right));

/// sizeof(Left)< sizeof(Right).
template <typename Left, typename Right>
constexpr bool is_lesser_size = (sizeof(Left) < sizeof(Right));

/// sizeof(Left) > sizeof(Right).
template <typename Left, typename Right>
constexpr bool is_greater_size = (sizeof(Left) > sizeof(Right));

/// Type is the possible result of a common operation (see std::common_type)
template <typename Type>
constexpr bool is_common_size = (sizeof(Type) >= sizeof(int));

/// Future-proofing against larger integrals or language features that
/// promote 3, 5, 6, 7 byte-sized types to integral (see std::is_integral).
template <typename Type>
constexpr bool is_integral_size = is_integral_sized(sizeof(Type));

/// bool is a c++ integral, but excluded here.
/// Type is s/u int8, int16, int32, int64 (including all aliases).
template <typename Type>
constexpr bool is_integral = std::is_integral_v<Type> &&
    is_integral_size<Type> && !is_same_type<Type, bool>;

/// numeric_limits may be specialized by non-integrals (such as uintx).
template <typename Type>
constexpr bool is_integer = std::numeric_limits<Type>::is_integer &&
    !is_same_type<Type, bool>;

/// Excludes non-integral integers (such as uintx).
template <typename Type>
constexpr bool is_integral_integer = is_integral<Type> && is_integer<Type>;

/// Constrained to is_integral types.
template <typename Type, std::enable_if_t<is_integral_size<Type>, bool> = true>
constexpr size_t bits = to_bits(sizeof(Type));

/// Limited to is_nonzero(Bits) && is_zero(Bits % 8).
/// Use to_ceilinged_bytes/to_floored_bytes for non-aligned conversions.
template <size_t Bits, std::enable_if_t<is_byte_sized(Bits), bool> = true>
constexpr size_t bytes = Bits / byte_bits;

/// std::array.
/// ---------------------------------------------------------------------------

template<typename>
struct is_std_array_t : std::false_type {};
template<typename Type, size_t Size>
struct is_std_array_t<std_array<Type, Size>> : std::true_type {};
template<typename Type>
constexpr bool is_std_array = is_std_array_t<Type>::value;

template <typename Type, std::enable_if_t<is_std_array<Type>, bool> = true>
constexpr size_t array_count = Type{}.size();

template <typename Type, std::enable_if_t<is_std_array<Type>, bool> = true>
using array_element = typename Type::value_type;

template <typename Type, std::enable_if_t<!is_std_array<Type>, bool> = true>
constexpr size_t size_of() noexcept { return sizeof(Type); }

template <typename Type, std::enable_if_t<is_std_array<Type>, bool> = true>
constexpr size_t size_of() noexcept(false)
{
    // Recurse array to integral type.
    constexpr auto size = size_of<typename Type::value_type>();
    constexpr auto count = array_count<Type>;

    // Type constraint fails to match as throw is not constexpr.
    if (count > (std::numeric_limits<size_t>::max() / size))
        throw overflow_exception("type contraint violated");

    return size * count;
}

/// std::reference_wrapper.
/// ---------------------------------------------------------------------------

template <typename Type>
using ref = std::reference_wrapper<Type>;

template <typename Type>
using cref = std::reference_wrapper<const Type>;

} // namespace libbitcoin

#endif
