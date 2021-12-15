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
#include <bitcoin/system/messages/send_compact.hpp>

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
    
const std::string send_compact::command = "sendcmpct";
const identifier send_compact::id = identifier::send_compact;
const uint32_t send_compact::version_minimum = version::level::bip152;
const uint32_t send_compact::version_maximum = version::level::maximum;

// static
size_t send_compact::size(uint32_t)
{
    return sizeof(uint8_t)
        + sizeof(uint64_t);
}

// static
send_compact send_compact::deserialize(uint32_t version, reader& source)
{
    if (version < version_minimum || version > version_maximum)
        source.invalidate();

    const auto mode = source.read_byte();
    const auto protocol = source.read_8_bytes_little_endian();

    // bip152: high_bandwidth value is boolean and must be zero or one.
    if (mode > one)
        source.invalidate();

    // bip152: protocol version must currently be one.
    if (protocol != one)
        source.invalidate();

    return { to_bool(mode), protocol };
}

void send_compact::serialize(uint32_t DEBUG_ONLY(version), writer& sink) const
{
    DEBUG_ONLY(const auto bytes = size(version);)
    DEBUG_ONLY(const auto start = sink.get_position();)

    sink.write_byte(static_cast<uint8_t>(high_bandwidth));
    sink.write_8_bytes_little_endian(compact_version);

    BITCOIN_ASSERT(sink && sink.get_position() - start == bytes);
}

} // namespace messages
} // namespace system
} // namespace libbitcoin
