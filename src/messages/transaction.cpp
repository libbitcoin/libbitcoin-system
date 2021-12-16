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
#include <bitcoin/system/messages/transaction.hpp>

#include <cstddef>
#include <cstdint>
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/messages/identifier.hpp>
#include <bitcoin/system/messages/version.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace messages {
    
const std::string transaction::command = "tx";
const identifier transaction::id = identifier::transaction;
const uint32_t transaction::version_minimum = version::level::minimum;
const uint32_t transaction::version_maximum = version::level::maximum;

// static
transaction transaction::deserialize(uint32_t version, reader& source,
    bool witness)
{
    if (version < version_minimum || version > version_maximum)
        source.invalidate();

    return { to_shared(new chain::transaction{ source, witness }) };
}

void transaction::serialize(uint32_t DEBUG_ONLY(version), writer& sink,
    bool witness) const
{
    DEBUG_ONLY(const auto bytes = size(version, witness);)
    DEBUG_ONLY(const auto start = sink.get_position();)

    if (transaction)
        transaction->to_data(sink, witness);

    BITCOIN_ASSERT(sink && sink.get_position() - start == bytes);
}

size_t transaction::size(uint32_t, bool witness) const
{
    return transaction ? transaction->serialized_size(witness) : zero;
}

} // namespace messages
} // namespace system
} // namespace libbitcoin
