/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_IOSTREAM_STREAMS_BASE_SINK_IPP
#define LIBBITCOIN_SYSTEM_IOSTREAM_STREAMS_BASE_SINK_IPP

#include <algorithm>

namespace libbitcoin {
namespace system {

template <typename Container>
base_sink<Container>::base_sink(size_type size) noexcept
  : size_(size)
{
}

template <typename Container>
typename base_sink<Container>::size_type
base_sink<Container>::write(const char_type* buffer, size_type count) noexcept
{
    if (count < 1)
        return negative_one;

    const auto size = std::min(size_, count);
    do_write(reinterpret_cast<const value_type*>(buffer), size);
    size_ -= size;
    return size;
}

} // namespace system
} // namespace libbitcoin

#endif
