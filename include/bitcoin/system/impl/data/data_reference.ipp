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
#ifndef LIBBITCOIN_SYSTEM_DATA_DATA_REFERENCE_IPP
#define LIBBITCOIN_SYSTEM_DATA_DATA_REFERENCE_IPP

#include <array>
#include <string>
#include <vector>
#include <bitcoin/system/data/data_array.hpp>
#include <bitcoin/system/data/data_slice.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

constexpr data_reference::data_reference() NOEXCEPT
  : data_slice()
{
}

constexpr data_reference::data_reference(
    const data_slice& data) NOEXCEPT
  : data_slice(data)
{
}

// data_slice(std::string) is SCONSTEXPR.
SCONSTEXPR data_reference::data_reference(
    const std::string& text) NOEXCEPT
  : data_slice(text)
{
}

// data_slice(std::vector) is VCONSTEXPR.
VCONSTEXPR data_reference::data_reference(
    const data_chunk& data) NOEXCEPT
  : data_slice(data)
{
}

// data_array constructor.
template <data_reference::size_type Size>
constexpr data_reference::data_reference(
    const data_array<Size>& data) NOEXCEPT
  : data_slice(data)
{
}

// Byte array constructor (casts Byte to uint8_t).
template <data_reference::size_type Size, typename Byte, if_one_byte<Byte>>
constexpr data_reference::data_reference(
    const std::array<Byte, Size>& data) NOEXCEPT
  : data_slice(data)
{
}

// data_slice(std::vector) is VCONSTEXPR.
// Byte vector constructor (casts Byte to uint8_t).
template <typename Byte, if_one_byte<Byte>>
VCONSTEXPR data_reference::data_reference(
    const std::vector<Byte>& data) NOEXCEPT
  : data_slice(data)
{
}

// Byte iterators constructor (casts to uint8_t).
template <typename Iterator>
constexpr data_reference::data_reference(const Iterator& begin,
    const Iterator& end) NOEXCEPT
  : data_slice(begin, end)
{
}

// Byte pointer to const constructor (casts Byte to uint8_t).
template <typename Byte, if_one_byte<Byte>>
constexpr data_reference::data_reference(const Byte* begin,
    const Byte* end) NOEXCEPT
  : data_slice(begin, end)
{
}

} // namespace system
} // namespace libbitcoin

#endif
