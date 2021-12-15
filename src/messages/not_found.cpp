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
#include <bitcoin/system/messages/not_found.hpp>

#include <cstddef>
#include <cstdint>
#include <string>
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/messages/identifier.hpp>
#include <bitcoin/system/messages/message.hpp>
#include <bitcoin/system/messages/version.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace messages {
    
const std::string not_found::command = "notfound";
const identifier not_found::id = identifier::not_found;
const uint32_t not_found::version_minimum = version::level::bip37;
const uint32_t not_found::version_maximum = version::level::maximum;

// static
// Reimplements base class read to prevent a list move operation as well
// as the need to implement default, base move, and base copy constructors.
not_found not_found::deserialize(uint32_t version, reader& source)
{
    if (version < version_minimum || version > version_maximum)
        source.invalidate();

    not_found lost;
    lost.items.resize(source.read_size(max_inventory));

    for (size_t item = 0; item < lost.items.capacity(); ++item)
        lost.items.push_back(inventory_item::deserialize(version, source));

    return lost;
}

} // namespace messages
} // namespace system
} // namespace libbitcoin
