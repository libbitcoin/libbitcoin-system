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
#include <bitcoin/system/messages/inventory_item.hpp>

#include <cstdint>
#include <string>
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace messages {

// static
uint32_t inventory_item::to_number(type_id inventory_type)
{
    return static_cast<uint32_t>(inventory_type);
}

// static
inventory_item::type_id inventory_item::to_type(uint32_t value)
{
    return static_cast<type_id>(value);
}

// static
std::string inventory_item::to_string(type_id inventory_type)
{
    switch (inventory_type)
    {
        case type_id::transaction:
            return "transaction";
        case type_id::block:
            return "block";
        case type_id::filtered_block:
            return "filtered_block";
        case type_id::compact_block:
            return "compact_block";
        case type_id::witness_transaction:
            return "witness_transaction";
        case type_id::witness_block:
            return "witness_block";
        case type_id::reserved:
            return "reserved";
        case type_id::error:
        default:
            return "error";
    }
}

// static
size_t inventory_item::size(uint32_t)
{
    return hash_size
        + sizeof(uint32_t);
}

inventory_item inventory_item::deserialize(uint32_t, reader& source)
{
    return
    {
        to_type(source.read_4_bytes_little_endian()),
        source.read_hash()
    };
}

void inventory_item::serialize(uint32_t DEBUG_ONLY(version),
    writer& sink) const
{
    DEBUG_ONLY(const auto bytes = size(version);)
    DEBUG_ONLY(const auto start = sink.get_position();)

    sink.write_4_bytes_little_endian(to_number(type));
    sink.write_bytes(hash);

    BITCOIN_ASSERT(sink && sink.get_position() - start == bytes);
}

bool inventory_item::is_block_type() const
{
    return type == type_id::witness_block
        || type == type_id::block
        || type == type_id::compact_block
        || type == type_id::filtered_block;
}

bool inventory_item::is_transaction_type() const
{
    return type == type_id::witness_transaction
        || type == type_id::transaction;
}

bool inventory_item::is_witnessable_type() const
{
    return type == type_id::block
        || type == type_id::transaction;
}

// Requires a non-const instance for this in-place efficiency.
void inventory_item::to_witness()
{
    if (is_witnessable_type())
        type = to_type(to_number(type) | to_number(type_id::witness));
}

} // namespace messages
} // namespace system
} // namespace libbitcoin
