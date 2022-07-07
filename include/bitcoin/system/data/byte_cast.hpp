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
#ifndef LIBBITCOIN_SYSTEM_DATA_BYTE_CAST_HPP
#define LIBBITCOIN_SYSTEM_DATA_BYTE_CAST_HPP

#include <array>
#include <bitcoin/system/data/data_array.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

/// Byte casts (require pointer_cast, not constexpr).
/// ---------------------------------------------------------------------------
/// C++20: std::bit_cast is a copy, not a cast, these are true casts.

/// Cast integral& to a data array& with byte length of the integral.
template <typename Integral, if_integral_integer<Integral> = true>
constexpr data_array<sizeof(Integral)>&
byte_cast(Integral& value) NOEXCEPT;

/// Cast const integral& to a const data array& with byte length of the integral.
template <typename Integral, if_integral_integer<Integral> = true>
constexpr const data_array<sizeof(Integral)>&
byte_cast(const Integral& value) NOEXCEPT;

/// Cast data array& to unsigned integral& of same byte length.
template <size_t Size, if_integral_size<Size> = true>
constexpr unsigned_type<Size>&
byte_cast(data_array<Size>& value) NOEXCEPT;

/// Cast const data array& to const unsigned integral& of same byte length.
template <size_t Size, if_integral_size<Size> = true>
constexpr const unsigned_type<Size>&
byte_cast(const data_array<Size>& value) NOEXCEPT;

/// Array casts (require pointer_cast, not constexpr).
/// ---------------------------------------------------------------------------

template <typename Left, typename Right,
    if_integral_integer<Left> = true,
    if_integral_integer<Right> = true>
constexpr bool is_proportional(size_t left_count) NOEXCEPT;

template <size_t LeftCount, typename Left, typename Right>
using if_proportional = bool_if<is_proportional<Left, Right>(LeftCount)>;

template <size_t LeftCount, typename Left, typename Right,
    if_integral_integer<Left> = true,
    if_integral_integer<Right> = true,
    if_proportional<LeftCount, Left, Right> = true>
constexpr size_t proportion = (LeftCount * sizeof(Left)) / sizeof(Right);

template <typename Left, typename Right,
    if_integral_integer<Left> = true,
    if_integral_integer<Right> = true>
constexpr bool is_portional(size_t left_count, size_t right_count) NOEXCEPT;

template <size_t LeftCount, typename Left, size_t RightCount, typename Right>
using if_portional = bool_if<is_portional<Left, Right>(LeftCount, RightCount)>;

/// Cast array& of integrals to same-sized array& of integrals.
template <typename To, size_t Count, typename From,
    if_integral_integer<From> = true,
    if_integral_integer<To> = true>
constexpr std::array<To, proportion<Count, From, To>>&
array_cast(std::array<From, Count>& values) NOEXCEPT;

/// Cast const array& of integrals to same-sized const array& of integrals.
template <typename To, size_t Count, typename From,
    if_integral_integer<From> = true,
    if_integral_integer<To> = true>
constexpr const std::array<To, proportion<Count, From, To>>&
array_cast(const std::array<From, Count>& values) NOEXCEPT;

/// Cast array& of integrals to lesser-sized array& of integrals.
/// Use array_cast for non-narrowing array cast.
template <typename To, size_t ToCount, typename From, size_t FromCount,
    if_integral_integer<From> = true,
    if_integral_integer<To> = true,
    if_portional<ToCount, To, FromCount, From> = true>
constexpr std::array<To, ToCount>&
narrowing_array_cast(std::array<From, FromCount>& values) NOEXCEPT;

/// Cast const array& of integrals to lesser-sized const array& of integrals.
/// Use array_cast for non-narrowing array cast.
template <typename To, size_t ToCount, typename From, size_t FromCount,
    if_integral_integer<From> = true,
    if_integral_integer<To> = true,
    if_portional<ToCount, To, FromCount, From> = true>
constexpr const std::array<To, ToCount>&
narrowing_array_cast(const std::array<From, FromCount>& values) NOEXCEPT;

/// Cast contiguous integral buffer to array& of integrals.
/// Safe if byte count (size) is correct.
template <typename To, size_t Size, typename From,
    if_integral_integer<From> = true,
    if_integral_integer<To> = true>
constexpr std::array<To, Size>&
unsafe_array_cast(From* bytes) NOEXCEPT;

/// Cast contiguous const integral buffer to const array& of integrals.
/// Safe if byte count (size) is correct.
template <typename To, size_t Size, typename From,
    if_integral_integer<From> = true,
    if_integral_integer<To> = true>
constexpr const std::array<To, Size>&
unsafe_array_cast(const From* bytes) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/data/byte_cast.ipp>

#endif
