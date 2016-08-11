/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/message/block_message.hpp>

#include <cstdint>
#include <cstddef>
#include <istream>
#include <utility>
#include <bitcoin/bitcoin/message/version.hpp>
#include <bitcoin/bitcoin/chain/header.hpp>
#include <bitcoin/bitcoin/chain/transaction.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>

namespace libbitcoin {
namespace message {

const std::string block_message::command = "block";
const uint32_t block_message::version_minimum = version::level::minimum;
const uint32_t block_message::version_maximum = version::level::maximum;

block_message block_message::factory_from_data(uint32_t version,
    const data_chunk& data, bool with_transaction_count)
{
    block_message instance;
    instance.from_data(version, data, with_transaction_count);
    return instance;
}

block_message block_message::factory_from_data(uint32_t version,
    std::istream& stream, bool with_transaction_count)
{
    block_message instance;
    instance.from_data(version, stream, with_transaction_count);
    return instance;
}

block_message block_message::factory_from_data(uint32_t version,
    reader& source, bool with_transaction_count)
{
    block_message instance;
    instance.from_data(version, source, with_transaction_count);
    return instance;
}

block_message::block_message()
  : block(), originator_(0)
{
}

block_message::block_message(const block& other)
  : block(other)
{
}

block_message::block_message(const block_message& other)
  : block_message(other.header, other.transactions)
{
}

block_message::block_message(const chain::header& header,
    const chain::transaction::list& transactions)
  : block(header, transactions), originator_(0)
{
}

block_message::block_message(block&& other)
  : block(std::forward<block>(other))
{
}

block_message::block_message(block_message&& other)
  : block_message(std::forward<chain::header>(other.header),
        std::forward<chain::transaction::list>(other.transactions))
{
}

block_message::block_message(chain::header&& header,
    chain::transaction::list&& transactions)
  : block(std::forward<chain::header>(header),
        std::forward<chain::transaction::list>(transactions)),
    originator_(0)
{
}

block_message& block_message::operator=(block_message&& other)
{
    header = std::move(other.header);
    transactions = std::move(other.transactions);
    originator_ = other.originator_;
    return *this;
}

bool block_message::from_data(uint32_t version, const data_chunk& data,
    bool with_transaction_count)
{
    originator_ = version;
    return block::from_data(data, with_transaction_count);
}

bool block_message::from_data(uint32_t version, std::istream& stream,
    bool with_transaction_count)
{
    originator_ = version;
    return block::from_data(stream, with_transaction_count);
}

bool block_message::from_data(uint32_t version, reader& source,
    bool with_transaction_count)
{
    originator_ = version;
    return block::from_data(source, with_transaction_count);
}

data_chunk block_message::to_data(uint32_t version,
    bool with_transaction_count) const
{
    return block::to_data(with_transaction_count);
}

void block_message::to_data(uint32_t version, std::ostream& stream,
    bool with_transaction_count) const
{
    block::to_data(stream, with_transaction_count);
}

void block_message::to_data(uint32_t version, writer& sink,
    bool with_transaction_count) const
{
    block::to_data(sink, with_transaction_count);
}

uint64_t block_message::serialized_size(uint32_t version,
    bool with_transaction_count) const
{
    return block::serialized_size(with_transaction_count);
}

uint64_t block_message::originator() const
{
    return originator_;
}

void block_message::set_originator(uint64_t value)
{
    originator_ = value;
}

} // namspace message
} // namspace libbitcoin
