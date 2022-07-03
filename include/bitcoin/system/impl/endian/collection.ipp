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

// C++17: Parallel policy for std::transform.

// return value

template <size_t Size>
constexpr numbers<Size> from_big_endian(
    const numbers<Size>& in) NOEXCEPT
{
    numbers<Size> out{};
    from_big_endian(out, in);
    return out;
}

template <size_t Size>
constexpr numbers<Size> from_little_endian(
    const numbers<Size>& in) NOEXCEPT
{
    numbers<Size> out{};
    from_little_endian(out, in);
    return out;
}

template <size_t Size>
constexpr numbers<Size> to_big_endian(
    const numbers<Size>& in) NOEXCEPT
{
    numbers<Size> out{};
    to_big_endian(out, in);
    return out;
}

template <size_t Size>
constexpr numbers<Size> to_little_endian(
    const numbers<Size>& in) NOEXCEPT
{
    numbers<Size> out{};
    to_little_endian(out, in);
    return out;
}

// out parameter

template <size_t Size>
constexpr void from_big_endian(numbers<Size>& out,
    const numbers<Size>& in) NOEXCEPT
{
    std::transform(in.begin(), in.end(), out.begin(),
        [](const auto& integral) NOEXCEPT
        {
            return native_from_big_end(integral);
        });
}

template <size_t Size>
constexpr void from_little_endian(numbers<Size>& out,
    const numbers<Size>& in) NOEXCEPT
{
    std::transform(in.begin(), in.end(), out.begin(),
        [](const uint32_t& chunk) NOEXCEPT
        {
            return native_from_little_end(chunk);
        });
}

template <size_t Size>
constexpr void to_big_endian(numbers<Size>& out,
    const numbers<Size>& in) NOEXCEPT
{
    std::transform(in.begin(), in.end(), out.begin(),
        [](const auto& value) NOEXCEPT
        {
            return native_to_big_end(value);
        });
}

template <size_t Size>
constexpr void to_little_endian(numbers<Size>& out,
    const numbers<Size>& in) NOEXCEPT
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
