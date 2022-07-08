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
#ifndef LIBBITCOIN_SYSTEM_DATA_ARRAY_CAST_IPP
#define LIBBITCOIN_SYSTEM_DATA_ARRAY_CAST_IPP

#include <array>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

template <typename Left, typename Right,
    if_integral_integer<Left>,
    if_integral_integer<Right>>
constexpr bool is_proportional(size_t left_count) NOEXCEPT
{
    return !is_multiply_overflow(left_count, sizeof(Left))
        && is_multiple(left_count * sizeof(Left), sizeof(Right));
}

template <typename Left, typename Right,
    if_integral_integer<Left>,
    if_integral_integer<Right>>
constexpr bool is_portional(size_t left_count, size_t right_count) NOEXCEPT
{
    return !is_multiply_overflow(left_count, sizeof(Left))
        && !is_multiply_overflow(right_count, sizeof(Right))
        && (right_count * sizeof(Right) > left_count * sizeof(Left));
}

template <typename To, size_t Count, typename From,
    if_integral_integer<From>,
    if_integral_integer<To>>
inline std::array<To, proportion<Count, From, To>>&
array_cast(std::array<From, Count>& values) NOEXCEPT
{
    using to = std::array<To, proportion<Count, From, To>>;
    return *pointer_cast<to>(values.data());
}

template <typename To, size_t Count, typename From,
    if_integral_integer<From>,
    if_integral_integer<To>>
inline const std::array<To, proportion<Count, From, To>>&
array_cast(const std::array<From, Count>& values) NOEXCEPT
{
    using to = std::array<To, proportion<Count, From, To>>;
    return *pointer_cast<const to>(values.data());
}

// Avoids cast of rvalue to reference, which would dangle.
template <typename To, size_t Count, typename From,
    if_integral_integer<From>,
    if_integral_integer<To>>
inline std::array<To, proportion<Count, From, To>>
array_cast(std::array<From, Count>&& values) NOEXCEPT
{
    return array_cast<To>(unmove(values));
}

template <typename To, size_t ToCount, typename From, size_t FromCount,
    if_integral_integer<From>,
    if_integral_integer<To>,
    if_portional<ToCount, To, FromCount, From>>
inline std::array<To, ToCount>&
narrowing_array_cast(std::array<From, FromCount>& values) NOEXCEPT
{
    using to = std::array<To, ToCount>;
    return *pointer_cast<to>(values.data());
}

template <typename To, size_t ToCount, typename From, size_t FromCount,
    if_integral_integer<From>,
    if_integral_integer<To>,
    if_portional<ToCount, To, FromCount, From>>
inline const std::array<To, ToCount>&
narrowing_array_cast(const std::array<From, FromCount>& values) NOEXCEPT
{
    using to = std::array<To, ToCount>;
    return *pointer_cast<const to>(values.data());
}

// Avoids cast of rvalue to reference, which would dangle.
template <typename To, size_t ToCount, typename From, size_t FromCount,
    if_integral_integer<From>,
    if_integral_integer<To>,
    if_portional<ToCount, To, FromCount, From>>
inline std::array<To, ToCount>
narrowing_array_cast(std::array<From, FromCount>&& values) NOEXCEPT
{
    return narrowing_array_cast<To, ToCount>(unmove(values));
}

template <typename To, size_t Size, typename From,
    if_integral_integer<From>,
    if_integral_integer<To>>
inline std::array<To, Size>&
unsafe_array_cast(From* bytes) NOEXCEPT
{
    return *pointer_cast<std::array<To, Size>>(bytes);
}

template <typename To, size_t Size, typename From,
    if_integral_integer<From>,
    if_integral_integer<To>>
inline const std::array<To, Size>&
unsafe_array_cast(const From* bytes) NOEXCEPT
{
    return *pointer_cast<const std::array<To, Size>>(bytes);
}

} // namespace system
} // namespace libbitcoin

#endif
