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

// Sponsored in part by Digital Contract Design, LLC

#include <bitcoin/system/messages/client_filter_headers.hpp>

#include <cstddef>
#include <cstdint>
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/messages/identifier.hpp>
#include <bitcoin/system/messages/message.hpp>
#include <bitcoin/system/messages/version.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace messages {
    
const std::string client_filter_headers::command = "cfheaders";
const identifier client_filter_headers::id = identifier::client_filter_headers;
const uint32_t client_filter_headers::version_minimum = version::level::bip157;
const uint32_t client_filter_headers::version_maximum = version::level::maximum;

// static
client_filter_headers client_filter_headers::deserialize(uint32_t version,
    reader& source)
{
    if (version < version_minimum || version > version_maximum)
        source.invalidate();

    const auto read_filter_hashes = [](reader& source)
    {
        hash_list filter_hashes;
        filter_hashes.reserve(source.read_size(chain::max_block_size));

        for (size_t header = 0; header < filter_hashes.capacity(); header++)
            filter_hashes.push_back(source.read_hash());

        return filter_hashes;
    };

    return
    {
        source.read_byte(),
        source.read_hash(),
        source.read_hash(),
        read_filter_hashes(source)
    };
}

void client_filter_headers::serialize(uint32_t DEBUG_ONLY(version),
    writer& sink) const
{
    DEBUG_ONLY(const auto bytes = size(version);)
    DEBUG_ONLY(const auto start = sink.get_position();)

    sink.write_byte(filter_type);
    sink.write_bytes(stop_hash);
    sink.write_bytes(previous_filter_header);
    sink.write_variable(filter_hashes.size());

    for (const auto& hash: filter_hashes)
        sink.write_bytes(hash);

    BITCOIN_ASSERT(sink && sink.get_position() - start == bytes);
}

size_t client_filter_headers::size(uint32_t) const
{
    return sizeof(uint8_t)
        + hash_size
        + hash_size
        + variable_size(filter_hashes.size()) +
            (filter_hashes.size() * hash_size);
}

} // namespace messages
} // namespace system
} // namespace libbitcoin
