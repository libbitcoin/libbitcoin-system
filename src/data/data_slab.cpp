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
#include <bitcoin/system/data/data_slab.hpp>

#include <cstdint>
#include <iterator>

namespace libbitcoin {
namespace system {

data_slab::data_slab(uint8_t* begin, uint8_t* end) noexcept
  : begin_(begin), end_(end)
{
}

data_slab::iterator data_slab::begin() const noexcept
{
    return begin_;
}

data_slab::iterator data_slab::end() const noexcept
{
    return end_;
}

data_slab::size_type data_slab::size() const noexcept
{
    return static_cast<size_type>(std::distance(begin_, end_));
}

} // namespace system
} // namespace libbitcoin
