/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/message/prefilled_transaction.hpp>

#include <bitcoin/bitcoin/chain/transaction.hpp>
#include <bitcoin/bitcoin/message/messages.hpp>
#include <bitcoin/bitcoin/message/version.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

prefilled_transaction prefilled_transaction::factory(
    uint32_t version, const data_chunk& data)
{
    prefilled_transaction instance;
    instance.from_data(version, data);
    return instance;
}

prefilled_transaction prefilled_transaction::factory(
    uint32_t version, std::istream& stream)
{
    prefilled_transaction instance;
    instance.from_data(version, stream);
    return instance;
}

prefilled_transaction prefilled_transaction::factory(
    uint32_t version, reader& source)
{
    prefilled_transaction instance;
    instance.from_data(version, source);
    return instance;
}

prefilled_transaction::prefilled_transaction()
  : index_(max_uint16), transaction_()
{
}

prefilled_transaction::prefilled_transaction(uint64_t index,
    const chain::transaction& tx)
  : index_(index), transaction_(tx)
{
}

prefilled_transaction::prefilled_transaction(uint64_t index,
    chain::transaction&& tx)
  : index_(index), transaction_(std::move(tx))
{
}

prefilled_transaction::prefilled_transaction(
    const prefilled_transaction& other)
  : prefilled_transaction(other.index_, other.transaction_)
{
}

prefilled_transaction::prefilled_transaction(prefilled_transaction&& other)
  : prefilled_transaction(other.index_, std::move(other.transaction_))
{
}

bool prefilled_transaction::is_valid() const
{
    return (index_ < max_uint16) && transaction_.is_valid();
}

void prefilled_transaction::reset()
{
    index_ = max_uint16;
    transaction_ = chain::transaction{};
}

bool prefilled_transaction::from_data(uint32_t version,
    const data_chunk& data)
{
    data_source istream(data);
    return from_data(version, istream);
}

bool prefilled_transaction::from_data(uint32_t version,
    std::istream& stream)
{
    istream_reader source(stream);
    return from_data(version, source);
}

bool prefilled_transaction::from_data(uint32_t ,
    reader& source)
{
    reset();

    index_ = source.read_variable_little_endian();
    transaction_.from_data(source, true);

    if (!source)
        reset();

    return source;
}

data_chunk prefilled_transaction::to_data(uint32_t version) const
{
    data_chunk data;
    const auto size = serialized_size(version);
    data.reserve(size);
    data_sink ostream(data);
    to_data(version, ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == size);
    return data;
}

void prefilled_transaction::to_data(uint32_t version,
    std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(version, sink);
}

void prefilled_transaction::to_data(uint32_t ,
    writer& sink) const
{
    sink.write_variable_little_endian(index_);
    transaction_.to_data(sink);
}

size_t prefilled_transaction::serialized_size(uint32_t) const
{
    return variable_uint_size(index_) + transaction_.serialized_size(true);
}

uint64_t prefilled_transaction::index() const
{
    return index_;
}

void prefilled_transaction::set_index(uint64_t value)
{
    index_ = value;
}

chain::transaction& prefilled_transaction::transaction()
{
    return transaction_;
}

const chain::transaction& prefilled_transaction::transaction() const
{
    return transaction_;
}

void prefilled_transaction::set_transaction(const chain::transaction& tx)
{
    transaction_ = tx;
}

void prefilled_transaction::set_transaction(chain::transaction&& tx)
{
    transaction_ = std::move(tx);
}

prefilled_transaction& prefilled_transaction::operator=(prefilled_transaction&& other)
{
    index_ = other.index_;
    transaction_ = std::move(other.transaction_);
    return *this;
}

prefilled_transaction& prefilled_transaction::operator=(const prefilled_transaction& other)
{
    index_ = other.index_;
    transaction_ = other.transaction_;
    return *this;
}

bool prefilled_transaction::operator==(const prefilled_transaction& other) const
{
    return (index_ == other.index_) && (transaction_ == other.transaction_);
}

bool prefilled_transaction::operator!=(const prefilled_transaction& other) const
{
    return !(*this == other);
}

} // namespace message
} // namespace libbitcoin
