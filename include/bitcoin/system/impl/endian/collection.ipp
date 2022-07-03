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
#ifndef LIBBITCOIN_SYSTEM_ENDIAN_COLLECTION_IPP
#define LIBBITCOIN_SYSTEM_ENDIAN_COLLECTION_IPP

#include <algorithm>
#include <iterator>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/nominal.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

// TODO: change to std::array& args and move to /math/bytes/.
// By moving the pointer_cast to intrinsics this can become constexpr.
// TODO: change parameterization for in-place update.
// C++17: Parallel policy for std::transform.

// void (data[] <=> integral[])
/// ---------------------------------------------------------------------------
// Hack: pointer_cast fools transform into accepting a C-style array. This is
// safe because std::array must have only the C-style array non-static member.

template <size_t Count, typename Integral,
    if_integral_integer<Integral>>
constexpr void from_big_endian(Integral to[Count],
    const uint8_t from[Count * sizeof(Integral)]) NOEXCEPT
{
    typedef data_array<sizeof(Integral)> data;
    const auto in = pointer_cast<const data>(from);
    std::transform(in, std::next(in, Count), to, [](const data& value) NOEXCEPT
    {
        return from_big_endian(value);
    });
}

template <size_t Count, typename Integral,
    if_integral_integer<Integral>>
constexpr void from_little_endian(Integral to[Count],
    const uint8_t from[Count * sizeof(Integral)]) NOEXCEPT
{
    typedef data_array<sizeof(Integral)> data;
    const auto in = pointer_cast<const data>(from);
    std::transform(in, std::next(in, Count), to, [](const data& value) NOEXCEPT
    {
        return from_little_endian<Integral>(value);
    });
}

template <size_t Count, typename Integral,
    if_integral_integer<Integral>>
constexpr void to_big_endian(uint8_t to[Count * sizeof(Integral)],
    const Integral from[Count]) NOEXCEPT
{
    typedef data_array<sizeof(Integral)> data;
    const auto out = pointer_cast<data>(to);
    std::transform(from, std::next(from, Count), out, [](Integral value) NOEXCEPT
    {
        return to_big_endian<Integral>(value);
    });
}

template <size_t Count, typename Integral,
    if_integral_integer<Integral>>
constexpr void to_little_endian(uint8_t to[Count * sizeof(Integral)],
    const Integral from[Count]) NOEXCEPT
{
    typedef data_array<sizeof(Integral)> data;
    const auto out = pointer_cast<data>(to);
    std::transform(from, std::next(from, Count), out, [](Integral value) NOEXCEPT
    {
        return to_little_endian<Integral>(value);
    });
}

} // namespace system
} // namespace libbitcoin

#endif
