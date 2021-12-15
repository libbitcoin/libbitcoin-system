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

#include <bitcoin/system/messages/client_filter_checkpoint.hpp>

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
    
const std::string client_filter_checkpoint::command = "cfcheckpt";
const identifier client_filter_checkpoint::id = identifier::client_filter_checkpoint;
const uint32_t client_filter_checkpoint::version_minimum = version::level::bip157;
const uint32_t client_filter_checkpoint::version_maximum = version::level::maximum;

// static
client_filter_checkpoint client_filter_checkpoint::deserialize(
    uint32_t version, reader& source)
{
    if (version < version_minimum || version > version_maximum)
        source.invalidate();

    const auto read_filter_headers = [](reader& source)
    {
        hash_list filter_headers;
        filter_headers.reserve(source.read_size(chain::max_block_size));

        for (size_t header = 0; header < filter_headers.capacity(); header++)
            filter_headers.push_back(source.read_hash());

        return filter_headers;
    };

    return
    {
        source.read_byte(),
        source.read_hash(),
        read_filter_headers(source)
    };
}

void client_filter_checkpoint::serialize(uint32_t, writer& sink) const
{
    sink.write_byte(filter_type);
    sink.write_bytes(stop_hash);
    sink.write_variable(filter_headers.size());

    for (const auto& header: filter_headers)
        sink.write_bytes(header);
}

size_t client_filter_checkpoint::size(uint32_t) const
{
    return sizeof(uint8_t)
        + hash_size
        + variable_size(filter_headers.size()) + 
            (filter_headers.size() * hash_size);
}

} // namespace messages
} // namespace system
} // namespace libbitcoin
