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
#include <bitcoin/bitcoin/message/compact_block.hpp>

#include <initializer_list>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/math/limits.hpp>
#include <bitcoin/bitcoin/message/version.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string compact_block::command = "cmpctblock";
const uint32_t compact_block::version_minimum = version::level::bip152;
const uint32_t compact_block::version_maximum = version::level::bip152;

compact_block compact_block::factory_from_data(uint32_t version,
    const data_chunk& data)
{
    compact_block instance;
    instance.from_data(version, data);
    return instance;
}

compact_block compact_block::factory_from_data(uint32_t version,
    std::istream& stream)
{
    compact_block instance;
    instance.from_data(version, stream);
    return instance;
}

compact_block compact_block::factory_from_data(uint32_t version,
    reader& source)
{
    compact_block instance;
    instance.from_data(version, source);
    return instance;
}

compact_block::compact_block()
  : header_(), nonce_(0), short_ids_(), transactions_()
{
}

compact_block::compact_block(const chain::header& header, uint64_t nonce,
    const short_id_list& short_ids,
    const prefilled_transaction::list& transactions)
  : header_(header), nonce_(nonce), short_ids_(short_ids),
    transactions_(transactions)
{
}

compact_block::compact_block(chain::header&& header, uint64_t nonce,
    short_id_list&& short_ids, prefilled_transaction::list&& transactions)
  : header_(std::move(header)), nonce_(nonce), short_ids_(std::move(short_ids)),
    transactions_(std::move(transactions))
{
}

compact_block::compact_block(const compact_block& other)
  : compact_block(other.header_, other.nonce_, other.short_ids_,
      other.transactions_)
{
}

compact_block::compact_block(compact_block&& other)
  : compact_block(std::move(other.header_), other.nonce_,
      std::move(other.short_ids_), std::move(other.transactions_))
{
}

bool compact_block::is_valid() const
{
    return header_.is_valid() && !short_ids_.empty() && !transactions_.empty();
}

void compact_block::reset()
{
    header_.reset();
    nonce_ = 0;
    short_ids_.clear();
    short_ids_.shrink_to_fit();
    transactions_.clear();
    transactions_.shrink_to_fit();
}

bool compact_block::from_data(uint32_t version, const data_chunk& data)
{
    data_source istream(data);
    return from_data(version, istream);
}

bool compact_block::from_data(uint32_t version, std::istream& stream)
{
    istream_reader source(stream);
    return from_data(version, source);
}

bool compact_block::from_data(uint32_t version, reader& source)
{
    reset();

    auto insufficient_version = (version < compact_block::version_minimum);
    auto result = header_.from_data(source, false);
    nonce_ = source.read_8_bytes_little_endian();
    const auto short_ids_count = source.read_variable_uint_little_endian();
    result &= static_cast<bool>(source);

    if (result)
        short_ids_.reserve(safe_unsigned<size_t>(short_ids_count));

    for (uint64_t i = 0; (i < short_ids_count) && result; ++i)
    {
        short_ids_.push_back(source.read_mini_hash());
        result = static_cast<bool>(source);
    }

    const auto transaction_count = source.read_variable_uint_little_endian();
    result &= static_cast<bool>(source);

    if (result)
    {
        transactions_.resize(safe_unsigned<size_t>(transaction_count));

        for (auto& transaction: transactions_)
        {
            result = transaction.from_data(version, source);

            if (!result)
                break;
        }
    }

    if (!result || insufficient_version)
        reset();

    return result && !insufficient_version;
}

data_chunk compact_block::to_data(uint32_t version) const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(version, ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size(version));
    return data;
}

void compact_block::to_data(uint32_t version, std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(version, sink);
}

void compact_block::to_data(uint32_t version, writer& sink) const
{
    header_.to_data(sink, false);
    sink.write_8_bytes_little_endian(nonce_);
    sink.write_variable_uint_little_endian(short_ids_.size());
    for (const auto& element: short_ids_)
        sink.write_mini_hash(element);

    sink.write_variable_uint_little_endian(transactions_.size());
    for (const auto& element: transactions_)
        element.to_data(version, sink);
}

uint64_t compact_block::serialized_size(uint32_t version) const
{
    uint64_t size = chain::header::satoshi_fixed_size_without_transaction_count() +
        variable_uint_size(short_ids_.size()) + (short_ids_.size() * 6) +
        variable_uint_size(transactions_.size()) + 8;

    for (const auto& tx: transactions_)
        size += tx.serialized_size(version);

    return size;
}

chain::header& compact_block::header()
{
    return header_;
}

const chain::header& compact_block::header() const
{
    return header_;
}

void compact_block::set_header(const chain::header& value)
{
    header_ = value;
}

void compact_block::set_header(chain::header&& value)
{
    header_ = std::move(value);
}

uint64_t compact_block::nonce() const
{
    return nonce_;
}

void compact_block::set_nonce(uint64_t value)
{
    nonce_ = value;
}

compact_block::short_id_list& compact_block::short_ids()
{
    return short_ids_;
}

const compact_block::short_id_list& compact_block::short_ids() const
{
    return short_ids_;
}

void compact_block::set_short_ids(const short_id_list& value)
{
    short_ids_ = value;
}

void compact_block::set_short_ids(short_id_list&& value)
{
    short_ids_ = std::move(value);
}

prefilled_transaction::list& compact_block::transactions()
{
    return transactions_;
}

const prefilled_transaction::list& compact_block::transactions() const
{
    return transactions_;
}

void compact_block::set_transactions(const prefilled_transaction::list& value)
{
    transactions_ = value;
}

void compact_block::set_transactions(prefilled_transaction::list&& value)
{
    transactions_ = std::move(value);
}

compact_block& compact_block::operator=(compact_block&& other)
{
    header_ = std::move(other.header_);
    nonce_ = other.nonce_;
    short_ids_ = std::move(other.short_ids_);
    transactions_ = std::move(other.transactions_);
    return *this;
}

bool compact_block::operator==(const compact_block& other) const
{
    return (header_ == other.header_) && (nonce_ == other.nonce_)
        && (short_ids_ == other.short_ids_)
        && (transactions_ == other.transactions_);
}

bool compact_block::operator!=(const compact_block& other) const
{
    return !(*this == other);
}

} // namespace message
} // namespace libbitcoin
