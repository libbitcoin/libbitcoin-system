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
#ifndef LIBBITCOIN_SYSTEM_IOSTREAM_SOURCES_SOURCE_IPP
#define LIBBITCOIN_SYSTEM_IOSTREAM_SOURCES_SOURCE_IPP

#include <algorithm>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/math/sign.hpp>

namespace libbitcoin {
namespace system {

template <typename Container>
base_source<Container>::base_source(size_type size) noexcept
  : size_(size)
{
}

template <typename Container>
typename base_source<Container>::size_type
base_source<Container>::read(char_type* buffer, size_type count) noexcept
{
    // Boost documents that -1 should be return to indicate eof, but this does
    // not allow for partial reads. So always return amount read if valid.
    if (is_null(buffer) || is_negative(count))
        return negative_one;

    const auto size = std::min(size_, count);
    do_read(reinterpret_cast<value_type*>(buffer), size);
    size_ -= size;

    // eof is indicated by the return of a value less than count.
    return size;
}

} // namespace system
} // namespace libbitcoin

#endif
