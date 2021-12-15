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
#include <bitcoin/system/messages/compact_block.hpp>

#include <cstddef>
#include <cstdint>
#include <numeric>
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/messages/compact_transaction.hpp>
#include <bitcoin/system/messages/identifier.hpp>
#include <bitcoin/system/messages/message.hpp>
#include <bitcoin/system/messages/version.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace messages {
    
const std::string compact_block::command = "cmpctblock";
const identifier compact_block::id = identifier::compact_block;
const uint32_t compact_block::version_minimum = version::level::bip152;
const uint32_t compact_block::version_maximum = version::level::maximum;

// static
compact_block compact_block::deserialize(uint32_t version, reader& source,
    bool witness)
{
    if (version < version_minimum || version > version_maximum)
        source.invalidate();

    const auto read_short_ids = [](reader& source)
    {
        short_id_list short_ids;
        short_ids.reserve(source.read_size(chain::max_block_size));

        for (size_t id = 0; id < short_ids.capacity(); ++id)
            short_ids.push_back(source.read_mini_hash());

        return short_ids;
    };

    const auto read_transactions = [=](reader& source)
    {
        compact_transaction::list transactions;
        transactions.reserve(source.read_size(chain::max_block_size));

        for (size_t id = 0; id < transactions.capacity(); ++id)
            transactions.push_back(
                compact_transaction::deserialize(version, source, witness));

        return transactions;
    };

    return 
    {
        chain::header(source),
        source.read_8_bytes_little_endian(),
        read_short_ids(source),
        read_transactions(source)
    };
}

void compact_block::serialize(uint32_t version, writer& sink,
    bool witness) const
{
    DEBUG_ONLY(const auto bytes = size(version, witness);)
    DEBUG_ONLY(const auto start = sink.get_position();)

    header.to_data(sink);
    sink.write_8_bytes_little_endian(nonce);
    sink.write_variable(short_ids.size());

    for (const auto& id: short_ids)
        sink.write_bytes(id);

    sink.write_variable(transactions.size());

    for (const auto& tx: transactions)
        tx.serialize(version, sink, witness);

    BITCOIN_ASSERT(sink && sink.get_position() - start == bytes);
}

size_t compact_block::size(uint32_t version, bool witness) const
{
    const auto txs_sizes = [=](size_t total, const compact_transaction& tx)
    {
        return total + tx.size(version, witness);
    };

    return chain::header::serialized_size()
        + sizeof(uint64_t)
        + variable_size(short_ids.size()) + (short_ids.size() * mini_hash_size)
        + variable_size(transactions.size()) + std::accumulate(
            transactions.begin(), transactions.end(), zero, txs_sizes);
}

} // namespace messages
} // namespace system
} // namespace libbitcoin
