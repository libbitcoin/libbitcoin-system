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
#ifndef LIBBITCOIN_SYSTEM_DATA_EXCLUSIVE_SLICE_HPP
#define LIBBITCOIN_SYSTEM_DATA_EXCLUSIVE_SLICE_HPP

#include <initializer_list>
#include <string>
#include <bitcoin/system/data/data_slice.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Exclusive slice hides data_array and data_chunk constructors.
/// This allows overloading with slice/array/chunk for performance optimization.
/// This has no member data, so should act merely as a compiler guide.
class exclusive_slice
  : public data_slice
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(exclusive_slice);

    constexpr exclusive_slice() NOEXCEPT
      : data_slice()
    {
    }

    template <size_type Size>
    constexpr exclusive_slice(const char(&text)[Size]) NOEXCEPT
      : data_slice(text)
    {
    }

    template <typename Iterator>
    constexpr exclusive_slice(const Iterator& begin, const Iterator& end) NOEXCEPT
      : data_slice(begin, end)
    {
    }

    template <typename Byte, if_one_byte<Byte> = true>
    constexpr exclusive_slice(const Byte* begin, const Byte* end) NOEXCEPT
      : data_slice(begin, end)
    {
    }

    SCONSTEXPR exclusive_slice(const std::string& text) NOEXCEPT
      : data_slice(text)
    {
    }

    SVCONSTEXPR exclusive_slice(std::initializer_list<value_type> bytes) NOEXCEPT
      : data_slice(bytes.begin(), bytes.end())
    {
    }
};

} // namespace system
} // namespace libbitcoin

#endif
