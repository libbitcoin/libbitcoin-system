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
#include <bitcoin/system/messages/get_compact_transactions.hpp>

#include <cstddef>
#include <cstdint>
#include <numeric>
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/messages/identifier.hpp>
#include <bitcoin/system/messages/message.hpp>
#include <bitcoin/system/messages/version.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace messages {
    
const std::string get_compact_transactions::command = "getblocktxn";
const identifier get_compact_transactions::id = identifier::get_compact_transactions;
const uint32_t get_compact_transactions::version_minimum = version::level::bip152;
const uint32_t get_compact_transactions::version_maximum = version::level::bip152;

get_compact_transactions get_compact_transactions::deserialize(uint32_t version,
    reader& source)
{
    if (version < version_minimum || version > version_maximum)
        source.invalidate();

    const auto read_indexes = [](reader& source)
    {
        std::vector<uint64_t> indexes;
        indexes.reserve(source.read_size(chain::max_block_size));

        for (size_t index = 0; index < indexes.capacity(); ++index)
            indexes.push_back(source.read_size());

        return indexes;
    };

    return
    {
        source.read_hash(),
        read_indexes(source)
    };
}

void get_compact_transactions::serialize(uint32_t DEBUG_ONLY(version),
    writer& sink) const
{
    DEBUG_ONLY(const auto bytes = size(version);)
    DEBUG_ONLY(const auto start = sink.get_position();)

    sink.write_bytes(block_hash);
    sink.write_variable(indexes.size());

    for (const auto& index: indexes)
        sink.write_variable(index);

    BITCOIN_ASSERT(sink && sink.get_position() - start == bytes);
}

size_t get_compact_transactions::size(uint32_t) const
{
    auto size = hash_size + variable_size(indexes.size());

    const auto values = [](size_t total, uint64_t output)
    {
        return total + variable_size(output);
    };

    return hash_size +
        variable_size(indexes.size()) +
            std::accumulate(indexes.begin(), indexes.end(), zero, values);
}

} // namespace messages
} // namespace system
} // namespace libbitcoin
