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
    const data_chunk& data)
{
    block_message instance;
    instance.from_data(version, data);
    return instance;
}

block_message block_message::factory_from_data(uint32_t version,
    std::istream& stream)
{
    block_message instance;
    instance.from_data(version, stream);
    return instance;
}

block_message block_message::factory_from_data(uint32_t version,
    reader& source)
{
    block_message instance;
    instance.from_data(version, source);
    return instance;
}

block_message::block_message()
  : block(), originator_(0)
{
}

block_message::block_message(const chain::header& header,
    const chain::transaction::list& transactions)
  : block(header, transactions), originator_(0)
{
}

block_message::block_message(chain::header&& header,
    chain::transaction::list&& transactions)
  : block(std::move(header), std::move(transactions)),
    originator_(0)
{
}

block_message::block_message(const block& other)
  : block(other), originator_(0)
{
}

block_message::block_message(block&& other)
  : block(std::move(other)), originator_(0)
{
}

block_message::block_message(const block_message& other)
  : block(other), originator_(other.originator_)
{
}

block_message::block_message(block_message&& other)
  : block(std::move(other)),
    originator_(other.originator_)
{
}

bool block_message::from_data(uint32_t, const data_chunk& data)
{
    return block::from_data(data);
}

bool block_message::from_data(uint32_t, std::istream& stream)
{
    return block::from_data(stream);
}

bool block_message::from_data(uint32_t, reader& source)
{
    return block::from_data(source);
}

data_chunk block_message::to_data(uint32_t) const
{
    return block::to_data();
}

void block_message::to_data(uint32_t, std::ostream& stream) const
{
    block::to_data(stream);
}

void block_message::to_data(uint32_t, writer& sink) const
{
    block::to_data(sink);
}

uint64_t block_message::serialized_size(uint32_t) const
{
    return block::serialized_size();
}

uint64_t block_message::originator() const
{
    return originator_;
}

void block_message::set_originator(uint64_t value) const
{
    originator_ = value;
}

block_message& block_message::operator=(chain::block&& other)
{
    reset();
    chain::block::operator=(std::move(other));
    return *this;
}

block_message& block_message::operator=(block_message&& other)
{
    originator_ = other.originator_;
    chain::block::operator=(std::move(other));
    return *this;
}

bool block_message::operator==(const chain::block& other) const
{
    return chain::block::operator==(other);
}

bool block_message::operator!=(const chain::block& other) const
{
    return chain::block::operator!=(other);
}

bool block_message::operator==(const block_message& other) const
{
    return chain::block::operator==(other);
}

bool block_message::operator!=(const block_message& other) const
{
    return !(*this == other);
}

} // namespace message
} // namespace libbitcoin
