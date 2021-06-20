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
#ifndef LIBBITCOIN_SYSTEM_DATA_DATA_SLAB_HPP
#define LIBBITCOIN_SYSTEM_DATA_DATA_SLAB_HPP

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
   
class BC_API data_slab
{
public:
    typedef size_t size_type;
    typedef uint8_t value_type;
    typedef value_type* iterator;

    // Avoid conflict with data_chunk construction.
    data_slab(uint8_t* begin, uint8_t* end) noexcept;

    iterator begin() const noexcept;
    iterator end() const noexcept;
    size_type size() const noexcept;

private:
    iterator begin_;
    iterator end_;
};

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/data/data_slab.ipp>

#endif
