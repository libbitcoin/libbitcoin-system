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
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

// return value
    
template <typename Integral, size_t Count>
constexpr std::array<Integral, Count> from_big_endian(
    const std::array<Integral, Count>& values) NOEXCEPT
{
    std::array<Integral, Count> out{};
    from_big_endian(out, values);
    return out;
}

template <typename Integral, size_t Count>
constexpr std::array<Integral, Count> from_little_endian(
    const std::array<Integral, Count>& values) NOEXCEPT
{
    std::array<Integral, Count> out{};
    from_little_endian(out, values);
    return out;
}

template <typename Integral, size_t Count>
constexpr std::array<Integral, Count> to_big_endian(
    const std::array<Integral, Count>& values) NOEXCEPT
{
    std::array<Integral, Count> out{};
    to_big_endian(out, values);
    return out;
}

template <typename Integral, size_t Count>
constexpr std::array<Integral, Count> to_little_endian(
    const std::array<Integral, Count>& values) NOEXCEPT
{
    std::array<Integral, Count> out{};
    to_little_endian(out, values);
    return out;
}

// out parameter

// C++17: Parallel policy for std::transform.

template <typename Integral, size_t Count>
constexpr void from_big_endian(std::array<Integral, Count>& out,
    const std::array<Integral, Count>& in) NOEXCEPT
{
    std::transform(in.begin(), in.end(), out.begin(),
        [](const auto& integral) NOEXCEPT
        {
            return native_from_big_end(integral);
        });
}

template <typename Integral, size_t Count>
constexpr void from_little_endian(std::array<Integral, Count>& out,
    const std::array<Integral, Count>& in) NOEXCEPT
{
    std::transform(in.begin(), in.end(), out.begin(),
        [](const uint32_t& chunk) NOEXCEPT
        {
            return native_from_little_end(chunk);
        });
}

template <typename Integral, size_t Count>
constexpr void to_big_endian(std::array<Integral, Count>& out,
    const std::array<Integral, Count>& in) NOEXCEPT
{
    std::transform(in.begin(), in.end(), out.begin(),
        [](const auto& value) NOEXCEPT
        {
            return native_to_big_end(value);
        });
}

template <typename Integral, size_t Count>
constexpr void to_little_endian(std::array<Integral, Count>& out,
    const std::array<Integral, Count>& in) NOEXCEPT
{
    std::transform(in.begin(), in.end(), out.begin(),
        [](const auto& value) NOEXCEPT
        {
            return native_to_little_end(value);
        });
}

} // namespace system
} // namespace libbitcoin

#endif
