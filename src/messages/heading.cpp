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
#include <bitcoin/system/messages/heading.hpp>

#include <map>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/messages/address.hpp>
#include <bitcoin/system/messages/alert.hpp>
#include <bitcoin/system/messages/block.hpp>
#include <bitcoin/system/messages/bloom_filter_add.hpp>
#include <bitcoin/system/messages/bloom_filter_clear.hpp>
#include <bitcoin/system/messages/bloom_filter_load.hpp>
#include <bitcoin/system/messages/client_filter.hpp>
#include <bitcoin/system/messages/client_filter_checkpoint.hpp>
#include <bitcoin/system/messages/client_filter_headers.hpp>
#include <bitcoin/system/messages/compact_block.hpp>
#include <bitcoin/system/messages/compact_transactions.hpp>
#include <bitcoin/system/messages/fee_filter.hpp>
#include <bitcoin/system/messages/get_address.hpp>
#include <bitcoin/system/messages/get_blocks.hpp>
#include <bitcoin/system/messages/get_client_filter_checkpoint.hpp>
#include <bitcoin/system/messages/get_client_filter_headers.hpp>
#include <bitcoin/system/messages/get_client_filters.hpp>
#include <bitcoin/system/messages/get_compact_transactions.hpp>
#include <bitcoin/system/messages/get_data.hpp>
#include <bitcoin/system/messages/get_headers.hpp>
#include <bitcoin/system/messages/headers.hpp>
#include <bitcoin/system/messages/identifier.hpp>
#include <bitcoin/system/messages/inventory.hpp>
#include <bitcoin/system/messages/message.hpp>
#include <bitcoin/system/messages/memory_pool.hpp>
#include <bitcoin/system/messages/merkle_block.hpp>
#include <bitcoin/system/messages/not_found.hpp>
#include <bitcoin/system/messages/ping.hpp>
#include <bitcoin/system/messages/pong.hpp>
#include <bitcoin/system/messages/reject.hpp>
#include <bitcoin/system/messages/send_compact.hpp>
#include <bitcoin/system/messages/send_headers.hpp>
#include <bitcoin/system/messages/transaction.hpp>
#include <bitcoin/system/messages/version_acknowledge.hpp>
#include <bitcoin/system/messages/version.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace messages {

// Pre-Witness:
// A maximal inventory is 50,000 entries, the largest valid message.
// Inventory with 50,000 entries is 3 + 36 * 50,000 bytes (1,800,003).
// The variable integer portion is maximum 3 bytes (with a count of 50,000).
// According to protocol documentation get_blocks is limited only by the
// general maximum payload size of 0x02000000 (33,554,432). But this is an
// absurd limit for a message that is properly [10 + log2(height) + 1]. Since
// protocol limits height to 2^32 this is 43. Even with expansion to 2^64
// this is limited to 75. So limit payloads to the max inventory payload size.
// Post-Witness:
// The maximum block size inclusive of witness is greater than 1,800,003, so
// with witness-enabled block size (4,000,000).
// This calculation should be revisited given any protocol change.
size_t heading::maximum_payload_size(uint32_t, bool witness)
{
    static constexpr size_t vector = sizeof(uint32_t) + hash_size;
    static constexpr size_t maximum = 3u + vector * max_inventory;
    return witness ? chain::max_block_weight : maximum;
}

// static
heading heading::factory(uint32_t magic, const std::string& command,
    const data_slice& payload)
{
    const auto size = payload.size();
    const auto payload_size = size > max_uint32 ? 0u : size;
    const auto payload_trimmed = is_zero(payload_size) ? data_slice{} : payload;

    return
    {
        magic,
        command,
        static_cast<uint32_t>(payload_size),
        network_checksum(payload_trimmed)
    };
}

// static
size_t heading::size()
{
    return
        sizeof(uint32_t) +
        command_size +
        sizeof(uint32_t) +
        sizeof(uint32_t);
}

// static
heading heading::deserialize(reader& source)
{
    return
    {
        source.read_4_bytes_little_endian(),
        source.read_string_buffer(command_size),
        source.read_4_bytes_little_endian(),
        source.read_4_bytes_little_endian()
    };
}

void heading::serialize(writer& sink) const
{
    sink.write_4_bytes_little_endian(magic);
    sink.write_string_buffer(command, command_size);
    sink.write_4_bytes_little_endian(payload_size);
    sink.write_4_bytes_little_endian(checksum);
}

#define COMMAND_ID(name) { name::command, name::id }

identifier heading::id() const
{
    // Internal to function avoids static initialization race.
    static const std::map<std::string, identifier> identifiers
    {
        COMMAND_ID(address),
        COMMAND_ID(alert),
        COMMAND_ID(block),
        COMMAND_ID(bloom_filter_add),
        COMMAND_ID(bloom_filter_clear),
        COMMAND_ID(bloom_filter_load),
        COMMAND_ID(client_filter),
        COMMAND_ID(client_filter_checkpoint),
        COMMAND_ID(client_filter_headers),
        COMMAND_ID(compact_block),
        COMMAND_ID(compact_transactions),
        COMMAND_ID(fee_filter),
        COMMAND_ID(get_address),
        COMMAND_ID(get_blocks),
        COMMAND_ID(get_client_filter_checkpoint),
        COMMAND_ID(get_client_filter_headers),
        COMMAND_ID(get_client_filters),
        COMMAND_ID(get_compact_transactions),
        COMMAND_ID(get_data),
        COMMAND_ID(get_headers),
        COMMAND_ID(headers),
        COMMAND_ID(inventory),
        COMMAND_ID(memory_pool),
        COMMAND_ID(merkle_block),
        COMMAND_ID(not_found),
        COMMAND_ID(ping),
        COMMAND_ID(pong),
        COMMAND_ID(reject),
        COMMAND_ID(send_compact),
        COMMAND_ID(send_headers),
        COMMAND_ID(transaction),
        COMMAND_ID(version_acknowledge),
        COMMAND_ID(version)
    };

    const auto it = identifiers.find(command);
    return (it == identifiers.end() ? identifier::unknown : it->second);
}

#undef COMMAND_ID

bool heading::verify_checksum(const data_slice& body) const
{
    return network_checksum(body) == checksum;
}

} // namespace messages
} // namespace system
} // namespace libbitcoin
