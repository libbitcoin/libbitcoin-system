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
#include <bitcoin/system/messages/reject.hpp>

#include <cstddef>
#include <cstdint>
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/messages/identifier.hpp>
#include <bitcoin/system/messages/message.hpp>
#include <bitcoin/system/messages/version.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace messages {
    
const std::string reject::command = "reject";
const identifier reject::id = identifier::reject;
const uint32_t reject::version_minimum = version::level::bip61;
const uint32_t reject::version_maximum = version::level::maximum;

// This is just a guess, required as memory guard.
constexpr size_t max_message = max_uint16;

// static
bool reject::is_chain(const std::string& message)
{
    return (message == block::command) || (message == transaction::command);
}

// static
uint8_t reject::reason_to_byte(reason_code value)
{
    return static_cast<uint8_t>(value);
}

// static
reject::reason_code reject::byte_to_reason(uint8_t byte)
{
    switch (byte)
    {
        case 0x01:
            return reason_code::malformed;
        case 0x10:
            return reason_code::invalid;
        case 0x11:
            return reason_code::obsolete;
        case 0x12:
            return reason_code::duplicate;
        case 0x40:
            return reason_code::nonstandard;
        case 0x41:
            return reason_code::dust;
        case 0x42:
            return reason_code::insufficient_fee;
        case 0x43:
            return reason_code::checkpoint;
        default:
            return reason_code::undefined;
    }
}

// static
reject reject::deserialize(uint32_t version, reader& source)
{
    auto message = source.read_string(max_message);
    const auto chain = is_chain(message);

    return
    {
        std::move(message),
        byte_to_reason(source.read_byte()),
        source.read_string(max_message),

        // Some nodes do not follow the documented convention of supplying hash
        // for tx and block rejects. Use this to prevent error by ensuring only
        // and all provided bytes are read. to_array will pad/truncate.
        chain ? to_array<hash_size>(source.read_bytes()) : null_hash
    };
}

void reject::serialize(uint32_t DEBUG_ONLY(version), writer& sink) const
{
    DEBUG_ONLY(const auto bytes = size(version);)
    DEBUG_ONLY(const auto start = sink.get_position();)

    sink.write_string(message);
    sink.write_byte(reason_to_byte(code));
    sink.write_string(reason);

    if (is_chain(message))
        sink.write_bytes(hash);

    BITCOIN_ASSERT(sink && sink.get_position() - start == bytes);
}

size_t reject::size(uint32_t) const
{
    return variable_size(message.length()) + message.length()
        + sizeof(uint8_t)
        + variable_size(reason.length()) + reason.length()
        + is_chain(message) ? hash_size : zero;
}

} // namespace messages
} // namespace system
} // namespace libbitcoin
