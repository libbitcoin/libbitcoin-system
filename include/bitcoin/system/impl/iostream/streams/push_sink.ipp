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
#ifndef LIBBITCOIN_SYSTEM_IOSTREAM_STREAMS_PUSH_SINK_IPP
#define LIBBITCOIN_SYSTEM_IOSTREAM_STREAMS_PUSH_SINK_IPP

#include <iterator>
#include <limits>
#include <bitcoin/system/constants.hpp>

namespace libbitcoin {
namespace system {

template <typename Container>
push_sink<Container>::push_sink(Container& data) noexcept
  : sink_(data),
    to_(data.begin()),
    size_(limit<size_type>(data.max_size() - data.size()))
{
}

template <typename Container>
typename push_sink<Container>::size_type
push_sink<Container>::do_write(const value_type* from, size_type size) noexcept
{
    // std::vector.insert returns iterator to first element inserted.
    sink_.insert(to_, size, from);
    to_ = std::next(to_, size);
}

} // namespace system
} // namespace libbitcoin

#endif
