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
#include <bitcoin/system/messages/version.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/messages/identifier.hpp>
#include <bitcoin/system/messages/message.hpp>
#include <bitcoin/system/messages/version.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace messages {
    
// version.relay added by bip37.
const std::string version::command = "version";
const identifier version::id = identifier::version;
const uint32_t messages::version::version_minimum = level::minimum;
const uint32_t messages::version::version_maximum = level::maximum;

// Time stamps are always used in version messages.
constexpr auto with_timestamp = false;

// This is just a guess, required as memory guard.
constexpr size_t max_user_agent = max_uint8;

// static
version version::deserialize(uint32_t version, reader& source)
{
    const auto value = source.read_4_bytes_little_endian();

    // The relay field is optional at or above version 70001.
    // But the peer doesn't know our version when it sends its version.
    // This is a bug in the BIP37 design as it forces older peers to adapt to
    // the expansion of the version message, which is a clear compat break.
    // So relay is enabled if either peer is below 70001, it is not set, or
    // peers are at/above 70001 and the field is set.
    const auto read_relay = [=](reader& source)
    {
        const auto peer_bip37 = (value >= level::bip37);
        const auto self_bip37 = (version >= level::bip37);

        return
            (peer_bip37 != self_bip37) || source.is_exhausted() ||
            (self_bip37 && to_bool(source.read_byte()));
    };

    // HACK: disabled check due to inconsistent node implementation.
    // The protocol expects duplication of the sender's services.
    ////if (services != address_sender.services())
    ////    source.invalidate();

    return
    {
        value,
        source.read_8_bytes_little_endian(),
        source.read_8_bytes_little_endian(),
        address_item::deserialize(version, source, with_timestamp),
        address_item::deserialize(version, source, with_timestamp),
        source.read_8_bytes_little_endian(),
        source.read_string(max_user_agent),
        source.read_4_bytes_little_endian(),
        read_relay(source)
    };
}

void version::serialize(uint32_t version, writer& sink) const
{
    DEBUG_ONLY(const auto bytes = size(version);)
    DEBUG_ONLY(const auto start = sink.get_position();)

    sink.write_4_bytes_little_endian(value);
    sink.write_8_bytes_little_endian(services);
    sink.write_8_bytes_little_endian(timestamp);
    address_receiver.serialize(version, sink, with_timestamp);
    address_sender.serialize(version, sink, with_timestamp);
    sink.write_8_bytes_little_endian(nonce);
    sink.write_string(user_agent);
    sink.write_4_bytes_little_endian(start_height);

    if (std::min(version, value) >= level::bip37)
        sink.write_byte(to_int<uint8_t>(relay));

    BITCOIN_ASSERT(sink && sink.get_position() - start == bytes);
}

size_t version::size(uint32_t version) const
{
    return sizeof(uint32_t)
        + sizeof(uint64_t)
        + sizeof(uint64_t)
        + address_receiver.size(version, with_timestamp)
        + address_sender.size(version, with_timestamp)
        + sizeof(uint64_t)
        + variable_size(user_agent.length()) + user_agent.length()
        + sizeof(uint32_t)
        + (value < level::bip37) ? zero : sizeof(uint8_t);
}

} // namespace messages
} // namespace system
} // namespace libbitcoin
