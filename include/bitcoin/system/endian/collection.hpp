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
#ifndef LIBBITCOIN_SYSTEM_ENDIAN_COLLECTION_HPP
#define LIBBITCOIN_SYSTEM_ENDIAN_COLLECTION_HPP

#include <array>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Hack: pointer_cast fools transform into accepting a C-style array. This is
/// safe because std::array must have only the C-style array non-static member.
template <size_t Size, typename Integral = uint32_t,
    if_integral_integer<Integral> = true>
using numbers = std::array<Integral, Size>;

// return value

template <size_t Size>
constexpr numbers<Size> from_big_endian(
    const numbers<Size>& in) NOEXCEPT;

template <size_t Size>
constexpr numbers<Size> from_little_endian(
    const numbers<Size>& in) NOEXCEPT;

template <size_t Size>
constexpr numbers<Size> to_big_endian(
    const numbers<Size>& in) NOEXCEPT;

template <size_t Size>
constexpr numbers<Size> to_little_endian(
    const numbers<Size>& in) NOEXCEPT;

// out parameter

template <size_t Size>
constexpr void from_big_endian(numbers<Size>& out,
    const numbers<Size>& in) NOEXCEPT;

template <size_t Size>
constexpr void from_little_endian(numbers<Size>& out,
    const numbers<Size>& in) NOEXCEPT;

template <size_t Size>
constexpr void to_big_endian(numbers<Size>& out,
    const numbers<Size>& in) NOEXCEPT;

template <size_t Size>
constexpr void to_little_endian(numbers<Size>& out,
    const numbers<Size>& in) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/endian/collection.ipp>

#endif
