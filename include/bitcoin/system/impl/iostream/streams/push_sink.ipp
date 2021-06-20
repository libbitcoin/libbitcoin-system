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
#include <bitcoin/system/math/limits.hpp>

namespace libbitcoin {
namespace system {

template <typename Container>
push_sink<Container>::push_sink(Container& data) noexcept
  : base_sink(limit<size_type>(data.max_size() - data.size())),
    anchor_(), sink_(data), to_(data.begin())
{
}

template <typename Container>
push_sink<Container>::~push_sink() noexcept
{
}

template <typename Container>
void push_sink<Container>::do_write(const value_type* from,
    size_type size) noexcept
{
    // std::vector.insert returns iterator to first element inserted.
    sink_.insert(to_, from, std::next(from, size));
    to_ = std::next(to_, size);
}

} // namespace system
} // namespace libbitcoin

#endif
