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

#include <bitcoin/system/messages/client_filter.hpp>

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
    
const std::string client_filter::command = "cfilter";
const identifier client_filter::id = identifier::client_filter;
const uint32_t client_filter::version_minimum = version::level::bip157;
const uint32_t client_filter::version_maximum = version::level::maximum;

// static
client_filter client_filter::deserialize(uint32_t version, reader& source)
{
    if (version < version_minimum || version > version_maximum)
        source.invalidate();

    return
    {
        source.read_byte(),
        source.read_hash(),
        source.read_bytes(source.read_size(chain::max_block_size))
    };
}

void client_filter::serialize(uint32_t DEBUG_ONLY(version), writer& sink) const
{
    DEBUG_ONLY(const auto bytes = size(version);)
    DEBUG_ONLY(const auto start = sink.get_position();)

    sink.write_byte(filter_type);
    sink.write_bytes(block_hash);
    sink.write_variable(filter.size());
    sink.write_bytes(filter);

    BITCOIN_ASSERT(sink && sink.get_position() - start == bytes);
}

size_t client_filter::size(uint32_t) const
{
    return sizeof(uint8_t)
        + hash_size
        + variable_size(filter.size()) + filter.size();
}

} // namespace messages
} // namespace system
} // namespace libbitcoin
