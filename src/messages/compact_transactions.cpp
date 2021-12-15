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
#include <bitcoin/system/messages/compact_transactions.hpp>

#include <cstddef>
#include <cstdint>
#include <numeric>
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/messages/identifier.hpp>
#include <bitcoin/system/messages/message.hpp>
#include <bitcoin/system/messages/version.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace messages {
    
const std::string compact_transactions::command = "blocktxn";
const identifier compact_transactions::id = identifier::compact_transactions;
const uint32_t compact_transactions::version_minimum = version::level::bip152;
const uint32_t compact_transactions::version_maximum = version::level::maximum;

// static
compact_transactions compact_transactions::deserialize(uint32_t version,
    reader& source, bool witness)
{
    if (version < version_minimum || version > version_maximum)
        source.invalidate();

    const auto read_transactions = [=](reader& source)
    {
        chain::transactions transactions;
        transactions.reserve(source.read_size(chain::max_block_size));

        for (size_t tx = 0; tx < transactions.capacity(); ++tx)
            transactions.emplace_back(source, witness);

        return transactions;
    };

    return
    {
        source.read_hash(),
        read_transactions(source)
    };
}

void compact_transactions::serialize(uint32_t DEBUG_ONLY(version),
    writer& sink, bool witness) const
{
    DEBUG_ONLY(const auto bytes = size(version, witness);)
    DEBUG_ONLY(const auto start = sink.get_position();)

    sink.write_bytes(block_hash);
    sink.write_variable(transactions.size());

    for (const auto& tx: transactions)
        tx.to_data(sink, witness);

    BITCOIN_ASSERT(sink && sink.get_position() - start == bytes);
}

size_t compact_transactions::size(uint32_t, bool witness) const
{
    const auto sizes = [=](size_t total, const chain::transaction& tx)
    {
        return total + tx.serialized_size(witness);
    };

    return hash_size
        + variable_size(transactions.size()) + std::accumulate(
            transactions.begin(), transactions.end(), zero, sizes);
}

} // namespace messages
} // namespace system
} // namespace libbitcoin
