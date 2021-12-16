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
#include <bitcoin/system/messages/headers.hpp>

#include <cstddef>
#include <cstdint>
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/messages/identifier.hpp>
#include <bitcoin/system/messages/inventory.hpp>
#include <bitcoin/system/messages/inventory_item.hpp>
#include <bitcoin/system/messages/message.hpp>
#include <bitcoin/system/messages/version.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace messages {
    
const std::string headers::command = "headers";
const identifier headers::id = identifier::headers;
const uint32_t headers::version_minimum = version::level::headers;
const uint32_t headers::version_maximum = version::level::maximum;

// Each header must trail a zero byte (yes, it's stoopid).
constexpr uint8_t trail = 0x00;

// static
headers headers::deserialize(uint32_t version, reader& source)
{
    if (version < version_minimum || version > version_maximum)
        source.invalidate();

    chain::header_ptrs headers;
    headers.reserve(source.read_size(max_get_headers));

    for (size_t header = 0; header < headers.capacity(); ++header)
    {
        headers.emplace_back(new chain::header{ source });

        if (source.read_byte() != trail)
            source.invalidate();
    }

    return { headers };
}

void headers::serialize(uint32_t DEBUG_ONLY(version), writer& sink) const
{
    DEBUG_ONLY(const auto bytes = size(version);)
    DEBUG_ONLY(const auto start = sink.get_position();)

    sink.write_variable(headers.size());

    for (const auto& header: headers)
    {
        header->to_data(sink);
        sink.write_byte(trail);
    }

    BITCOIN_ASSERT(sink&& sink.get_position() - start == bytes);
}

size_t headers::size(uint32_t version) const
{
    return variable_size(headers.size()) +
        (headers.size() * chain::header::serialized_size() + sizeof(trail));
}

// TODO: This would benefit from block hash store/return as pointer.
bool headers::is_sequential() const
{
    if (headers.empty())
        return true;

    auto previous = headers.front()->hash();

    for (auto it = std::next(headers.begin()); it != headers.end(); ++it)
    {
        if ((*it)->previous_block_hash() != previous)
            return false;

        previous = (*it)->hash();
    }

    return true;
}

// TODO: This would benefit from hash_list as list of pointers.
hash_list headers::to_hashes() const
{
    hash_list out;
    out.reserve(headers.size());

    for (const auto& header: headers)
        out.push_back(header->hash());

    return out;
}

// TODO: This would benefit from inventory_item hash pointers.
inventory_item::list headers::to_inventory(inventory::type_id type) const
{
    inventory_item::list out;
    out.reserve(headers.size());

    // msvc: emplace_back(type, header->hash()) does not compile.
    for (const auto& header: headers)
        out.push_back({ type, header->hash() });

    return out;
}

} // namespace messages
} // namespace system
} // namespace libbitcoin
