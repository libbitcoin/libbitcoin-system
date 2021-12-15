/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/messages/get_data.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <bitcoin/system/messages/identifier.hpp>
#include <bitcoin/system/messages/message.hpp>
#include <bitcoin/system/messages/version.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace messages {
    
// filtered_block flag allowed by bip37.
const std::string get_data::command = "getdata";
const identifier get_data::id = identifier::get_data;
const uint32_t get_data::version_minimum = version::level::minimum;
const uint32_t get_data::version_maximum = version::level::maximum;

// static
// Reimplements base class read to prevent a list move operation as well
// as the need to implement default, base move, and base copy constructors.
get_data get_data::deserialize(uint32_t version, reader& source)
{
    if (version < version_minimum || version > version_maximum)
        source.invalidate();

    get_data get;
    get.items.resize(source.read_size(max_inventory));

    for (size_t item = 0; item < get.items.capacity(); ++item)
        get.items.push_back(inventory_item::deserialize(version, source));

    return get;
}

// TODO: add inventory factory witness parameter (once node is ready).
// Requires a non-const instance for this in-place efficiency.
void get_data::to_witness()
{
    const auto convert = [](inventory_item& item)
    {
        item.to_witness();
    };

    std::for_each(items.begin(), items.end(), convert);
}

} // namespace messages
} // namespace system
} // namespace libbitcoin
