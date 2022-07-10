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
#ifndef LIBBITCOIN_SYSTEM_DATA_ARRAY_CAST_HPP
#define LIBBITCOIN_SYSTEM_DATA_ARRAY_CAST_HPP

#include <array>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

/// Array casting utilities.
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

/// Array casts (require pointer_cast, not constexpr).
/// ---------------------------------------------------------------------------

/// Cast array& of integrals to same-sized array& of integrals.
template <typename To, size_t Count, typename From,
    if_integral_integer<From> = true,
    if_integral_integer<To> = true>
inline std::array<To, proportion<Count, From, To>>&
array_cast(std::array<From, Count>& values) NOEXCEPT;

/// Cast const array& of integrals to same-sized const array& of integrals.
template <typename To, size_t Count, typename From,
    if_integral_integer<From> = true,
    if_integral_integer<To> = true>
inline const std::array<To, proportion<Count, From, To>>&
array_cast(const std::array<From, Count>& values) NOEXCEPT;

/// Cast array&& of integrals to same-sized const array of integrals.
template <typename To, size_t Count, typename From,
    if_integral_integer<From> = true,
    if_integral_integer<To> = true>
inline std::array<To, proportion<Count, From, To>>
array_cast(std::array<From, Count>&& values) NOEXCEPT;

/// Cast array& of integrals to lesser-sized array& of integrals.
/// Use array_cast for non-narrowing array cast.
template <typename To, size_t ToCount = one, typename From, size_t FromCount,
    if_integral_integer<From> = true,
    if_integral_integer<To> = true,
    if_portional<ToCount, To, FromCount, From> = true>
inline std::array<To, ToCount>&
narrowing_array_cast(std::array<From, FromCount>& values) NOEXCEPT;

/// Cast const array& of integrals to lesser-sized const array& of integrals.
/// Use array_cast for non-narrowing array cast.
template <typename To, size_t ToCount = one, typename From, size_t FromCount,
    if_integral_integer<From> = true,
    if_integral_integer<To> = true,
    if_portional<ToCount, To, FromCount, From> = true>
inline const std::array<To, ToCount>&
narrowing_array_cast(const std::array<From, FromCount>& values) NOEXCEPT;

/// Cast array&& of integrals to lesser-sized array of integrals.
/// Use array_cast for non-narrowing array cast.
template <typename To, size_t ToCount = one, typename From, size_t FromCount,
    if_integral_integer<From> = true,
    if_integral_integer<To> = true,
    if_portional<ToCount, To, FromCount, From> = true>
inline std::array<To, ToCount>
narrowing_array_cast(std::array<From, FromCount>&& values) NOEXCEPT;

/// Cast contiguous integral buffer to array& of integrals.
/// Safe if byte count (size) is correct.
template <typename To, size_t Size = one, typename From,
    if_integral_integer<From> = true,
    if_integral_integer<To> = true>
inline std::array<To, Size>&
unsafe_array_cast(From* bytes) NOEXCEPT;

/// Cast contiguous const integral buffer to const array& of integrals.
/// Safe if byte count (size) is correct.
template <typename To, size_t Size = one, typename From,
    if_integral_integer<From> = true,
    if_integral_integer<To> = true>
inline const std::array<To, Size>&
unsafe_array_cast(const From* bytes) NOEXCEPT;

/// Cast contiguous integral buffer to array& of array of integrals.
/// Safe if byte count (size) is correct.
template <typename To, size_t Size = one, typename From,
    if_integral_integer<From> = true,
    if_integral_integer_std_array<To> = true>
inline std::array<To, Size>&
unsafe_array_cast(From* bytes) NOEXCEPT;

/// Cast contiguous const integral buffer to const array& of array of integrals.
/// Safe if byte count (size) is correct.
template <typename To, size_t Size = one, typename From,
    if_integral_integer<From> = true,
    if_integral_integer_std_array<To> = true>
inline const std::array<To, Size>&
unsafe_array_cast(const From* bytes) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/data/array_cast.ipp>

#endif
