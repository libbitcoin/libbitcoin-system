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
#ifndef LIBBITCOIN_SYSTEM_DATA_BYTE_CAST_IPP
#define LIBBITCOIN_SYSTEM_DATA_BYTE_CAST_IPP

#include <array>
#include <bitcoin/system/data/data_array.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

// Byte casts.
// ----------------------------------------------------------------------------

template <typename Integral, if_integral_integer<Integral>>
constexpr data_array<sizeof(Integral)>&
byte_cast(Integral& value) NOEXCEPT
{
    return *pointer_cast<data_array<sizeof(Integral)>>(&value);
}

template <typename Integral, if_integral_integer<Integral>>
constexpr const data_array<sizeof(Integral)>&
byte_cast(const Integral& value) NOEXCEPT
{
    return *pointer_cast<const data_array<sizeof(Integral)>>(&value);
}

template <size_t Size, if_integral_size<Size>>
constexpr unsigned_type<Size>&
byte_cast(data_array<Size>& value) NOEXCEPT
{
    return *pointer_cast<unsigned_type<Size>>(&value);
}

template <size_t Size, if_integral_size<Size>>
constexpr const unsigned_type<Size>&
byte_cast(const data_array<Size>& value) NOEXCEPT
{
    return *pointer_cast<const unsigned_type<Size>>(&value);
}

// Array casts.
// ----------------------------------------------------------------------------

template <typename Left, typename Right,
    if_integral_integer<Left>,
    if_integral_integer<Right>>
constexpr bool is_proportional(size_t left_count) NOEXCEPT
{
    // TODO: replace (1) !with is_multiply_overflow when it supports signed.
    return (left_count <= (max_size_t / sizeof(Left)))
        && is_multiple(left_count * sizeof(Left), sizeof(Right));
}

template <typename Left, typename Right,
    if_integral_integer<Left>,
    if_integral_integer<Right>>
constexpr bool is_portional(size_t left_count, size_t right_count) NOEXCEPT
{
    // TODO: replace (2) with !is_multiply_overflow when it supports signed.
    return (left_count  <= (max_size_t / sizeof(Left)))
        && (right_count <= (max_size_t / sizeof(Right)))
        && (right_count * sizeof(Right) > left_count * sizeof(Left));
}

template <typename To, size_t Count, typename From,
    if_integral_integer<From>,
    if_integral_integer<To>>
constexpr std::array<To, proportion<Count, From, To>>&
array_cast(std::array<From, Count>& values) NOEXCEPT
{
    using to = std::array<To, proportion<Count, From, To>>;
    return *pointer_cast<to>(values.data());
}

template <typename To, size_t Count, typename From,
    if_integral_integer<From>,
    if_integral_integer<To>>
constexpr const std::array<To, proportion<Count, From, To>>&
array_cast(const std::array<From, Count>& values) NOEXCEPT
{
    using to = std::array<To, proportion<Count, From, To>>;
    return *pointer_cast<const to>(values.data());
}

template <typename To, size_t ToCount, typename From, size_t FromCount,
    if_integral_integer<From>,
    if_integral_integer<To>,
    if_portional<ToCount, To, FromCount, From>>
constexpr std::array<To, ToCount>&
narrowing_array_cast(std::array<From, FromCount>& values) NOEXCEPT
{
    using to = std::array<To, ToCount>;
    return *pointer_cast<to>(values.data());
}

template <typename To, size_t ToCount, typename From, size_t FromCount,
    if_integral_integer<From>,
    if_integral_integer<To>,
    if_portional<ToCount, To, FromCount, From>>
constexpr const std::array<To, ToCount>&
narrowing_array_cast(const std::array<From, FromCount>& values) NOEXCEPT
{
    using to = std::array<To, ToCount>;
    return *pointer_cast<const to>(values.data());
}

template <typename To, size_t Size, typename From,
    if_integral_integer<From>,
    if_integral_integer<To>>
constexpr std::array<To, Size>&
unsafe_array_cast(From* bytes) NOEXCEPT
{
    return *pointer_cast<std::array<To, Size>>(bytes);
}

// nullptr reinterpret_cast (at array size zero) is safe.
// &bytes[0] is undefined, so zero size is excluded.
template <typename To, size_t Size, typename From,
    if_integral_integer<From>,
    if_integral_integer<To>>
constexpr const std::array<To, Size>&
unsafe_array_cast(const From* bytes) NOEXCEPT
{
    return *pointer_cast<const std::array<To, Size>>(bytes);
}

} // namespace system
} // namespace libbitcoin

#endif
