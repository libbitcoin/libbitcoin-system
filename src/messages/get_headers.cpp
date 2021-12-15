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
#include <bitcoin/system/messages/get_headers.hpp>

#include <cstddef>
#include <cstdint>
#include <bitcoin/system/messages/identifier.hpp>
#include <bitcoin/system/messages/message.hpp>
#include <bitcoin/system/messages/version.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace messages {
    
const std::string get_headers::command = "getheaders";
const identifier get_headers::id = identifier::get_headers;
const uint32_t get_headers::version_minimum = version::level::headers;
const uint32_t get_headers::version_maximum = version::level::maximum;

// static
// Reimplements base class read to prevent a list move operation as well
// as the need to implement default, base move, and base copy constructors.
get_headers get_headers::deserialize(uint32_t version, reader& source)
{
    if (version < version_minimum || version > version_maximum)
        source.invalidate();

    // Protocol version is stoopid (and unused).
    source.skip_bytes(sizeof(uint32_t));

    get_headers get;
    get.start_hashes.reserve(source.read_size(max_get_blocks));

    for (size_t hash = 0; hash < get.start_hashes.capacity(); ++hash)
        get.start_hashes.push_back(source.read_hash());

    get.stop_hash = source.read_hash();
    return get;
}

} // namespace messages
} // namespace system
} // namespace libbitcoin
