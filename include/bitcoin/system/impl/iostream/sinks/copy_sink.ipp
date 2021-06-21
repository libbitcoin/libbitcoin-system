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
#ifndef LIBBITCOIN_SYSTEM_IOSTREAM_SINKS_COPY_SINK_IPP
#define LIBBITCOIN_SYSTEM_IOSTREAM_SINKS_COPY_SINK_IPP

#include <algorithm>
#include <bitcoin/system/math/limits.hpp>

namespace libbitcoin {
namespace system {
    
template <typename Container>
copy_sink<Container>::copy_sink(Container& data) noexcept
  : base_sink(limit<size_type>(data.size())),
    to_(data.begin())
{
}

template <typename Container>
void copy_sink<Container>::do_write(const value_type* from,
    size_type size) noexcept
{
    // std::copy_n returns iterator past last element copied to.
    to_ = std::copy_n(from, size, to_);
}

} // namespace system
} // namespace libbitcoin

#endif
