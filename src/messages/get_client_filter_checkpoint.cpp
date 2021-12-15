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

#include <bitcoin/system/messages/get_client_filter_checkpoint.hpp>

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
    
const std::string get_client_filter_checkpoint::command = "getcfcheckpt";
const identifier get_client_filter_checkpoint::id = identifier::get_client_filter_checkpoint;
const uint32_t get_client_filter_checkpoint::version_minimum = version::level::minimum;
const uint32_t get_client_filter_checkpoint::version_maximum = version::level::maximum;

// static
size_t get_client_filter_checkpoint::size(uint32_t)
{
    return sizeof(uint8_t)
        + hash_size;
}

// static
get_client_filter_checkpoint get_client_filter_checkpoint::deserialize(
    uint32_t version, reader& source)
{
    if (version < version_minimum || version > version_maximum)
        source.invalidate();

    return
    {
        source.read_byte(),
        source.read_hash()
    };
}

void get_client_filter_checkpoint::serialize(uint32_t DEBUG_ONLY(version),
    writer& sink) const
{
    DEBUG_ONLY(const auto bytes = size(version);)
    DEBUG_ONLY(const auto start = sink.get_position();)

    sink.write_byte(filter_type);
    sink.write_bytes(stop_hash);

    BITCOIN_ASSERT(sink && sink.get_position() - start == bytes);
}

} // namespace messages
} // namespace system
} // namespace libbitcoin
