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
#include <bitcoin/system/messages/alert_payload.hpp>

#include <cstddef>
#include <cstdint>
#include <numeric>
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/messages/message.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace messages {

constexpr size_t max_message = max_uint8;
constexpr size_t max_messages = 42;

// Libbitcoin doesn't use this.
const ec_uncompressed alert_payload::satoshi_public_key
{
    {
        0x04, 0xfc, 0x97, 0x02, 0x84, 0x78, 0x40, 0xaa, 0xf1, 0x95, 0xde,
        0x84, 0x42, 0xeb, 0xec, 0xed, 0xf5, 0xb0, 0x95, 0xcd, 0xbb, 0x9b,
        0xc7, 0x16, 0xbd, 0xa9, 0x11, 0x09, 0x71, 0xb2, 0x8a, 0x49, 0xe0,
        0xea, 0xd8, 0x56, 0x4f, 0xf0, 0xdb, 0x22, 0x20, 0x9e, 0x03, 0x74,
        0x78, 0x2c, 0x09, 0x3b, 0xb8, 0x99, 0x69, 0x2d, 0x52, 0x4e, 0x9d,
        0x6a, 0x69, 0x56, 0xe7, 0xc5, 0xec, 0xbc, 0xd6, 0x82, 0x84
    }
};

alert_payload alert_payload::deserialize(uint32_t, reader& source)
{
    const auto read_cans = [](reader& source)
    {
        cancels cans;
        cans.reserve(source.read_size(max_messages));

        for (size_t can = 0; can < cans.capacity(); can++)
            cans.push_back(source.read_4_bytes_little_endian());

        return cans;
    };

    const auto read_subs = [](reader& source)
    {
        sub_versions subs;
        subs.reserve(source.read_size(max_messages));

        for (size_t sub = 0; sub < subs.capacity(); sub++)
            subs.push_back(source.read_string(max_message));

        return subs;
    };

    return
    {
        source.read_4_bytes_little_endian(),
        source.read_8_bytes_little_endian(),
        source.read_8_bytes_little_endian(),
        source.read_4_bytes_little_endian(),
        source.read_4_bytes_little_endian(),
        read_cans(source),
        source.read_4_bytes_little_endian(),
        source.read_4_bytes_little_endian(),
        read_subs(source),
        source.read_4_bytes_little_endian(),
        source.read_string(max_message),
        source.read_string(max_message),
        source.read_string(max_message)
    };
}

void alert_payload::serialize(uint32_t DEBUG_ONLY(version), writer& sink) const
{
    DEBUG_ONLY(const auto bytes = size(version);)
    DEBUG_ONLY(const auto start = sink.get_position();)

    sink.write_4_bytes_little_endian(version);
    sink.write_8_bytes_little_endian(relay_until);
    sink.write_8_bytes_little_endian(expiration);
    sink.write_4_bytes_little_endian(id);
    sink.write_4_bytes_little_endian(cancel);
    sink.write_variable(set_cancel.size());

    for (const auto& entry: set_cancel)
        sink.write_4_bytes_little_endian(entry);

    sink.write_4_bytes_little_endian(min_version);
    sink.write_4_bytes_little_endian(max_version);
    sink.write_variable(set_sub_version.size());

    for (const auto& entry: set_sub_version)
        sink.write_string(entry);

    sink.write_4_bytes_little_endian(priority);
    sink.write_string(comment);
    sink.write_string(status_bar);
    sink.write_string(reserved);

    BITCOIN_ASSERT(sink && sink.get_position() - start == bytes);
}

size_t alert_payload::size(uint32_t) const
{
    const auto subs = [](size_t total, const std::string& sub)
    {
        return total + variable_size(sub.length()) + sub.length();
    };

    return sizeof(uint32_t)
        + sizeof(uint64_t)
        + sizeof(uint64_t)
        + sizeof(uint32_t)
        + sizeof(uint32_t)
        + variable_size(set_cancel.size()) + 
            (set_cancel.size() * sizeof(uint32_t))
        + sizeof(uint32_t)
        + sizeof(uint32_t)
        + variable_size(set_sub_version.size()) + std::accumulate(
            set_sub_version.begin(), set_sub_version.end(), zero, subs)
        + sizeof(uint32_t)
        + variable_size(comment.length()) + comment.length()
        + variable_size(status_bar.length()) + status_bar.length()
        + variable_size(reserved.length()) + reserved.length();
}

} // namespace messages
} // namespace system
} // namespace libbitcoin
