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
#include <bitcoin/bitcoin/message/header_message.hpp>

#include <cstdint>
#include <cstddef>
#include <istream>
#include <utility>
#include <bitcoin/bitcoin/chain/header.hpp>
#include <bitcoin/bitcoin/chain/transaction.hpp>
#include <bitcoin/bitcoin/message/version.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/reader.hpp>

namespace libbitcoin {
namespace message {

const std::string header_message::command = "headers";
const uint32_t header_message::version_minimum = version::level::minimum;
const uint32_t header_message::version_maximum = version::level::maximum;

header_message header_message::factory_from_data(const uint32_t version,
    const data_chunk& data, bool with_transaction_count)
{
    header_message instance;
    instance.from_data(version, data, with_transaction_count);
    return instance;
}

header_message header_message::factory_from_data(const uint32_t version,
    std::istream& stream, bool with_transaction_count)
{
    header_message instance;
    instance.from_data(version, stream, with_transaction_count);
    return instance;
}

header_message header_message::factory_from_data(const uint32_t version,
    reader& source, bool with_transaction_count)
{
    header_message instance;
    instance.from_data(version, source, with_transaction_count);
    return instance;
}

header_message::header_message()
  : header()
{
}

header_message::header_message(const header& other)
  : header(other)
{
}

header_message::header_message(const header_message& other)
: header_message(other.version, other.previous_block_hash, other.merkle,
      other.timestamp, other.bits, other.nonce, other.transaction_count)
{
}

header_message::header_message(uint32_t version,
    const hash_digest& previous_block_hash, const hash_digest& merkle,
    uint32_t timestamp, uint32_t bits, uint32_t nonce,
    uint64_t transaction_count)
  : header(version, previous_block_hash, merkle, timestamp, bits, nonce,
        transaction_count)
{
}

header_message::header_message(header&& other)
  : header(std::forward<header>(other))
{
}

header_message::header_message(header_message&& other)
  : header_message(other.version,
        std::forward<hash_digest>(other.previous_block_hash),
        std::forward<hash_digest>(other.merkle), other.timestamp, other.bits,
        other.nonce, other.transaction_count)
{
}

header_message::header_message(uint32_t version,
    hash_digest&& previous_block_hash, hash_digest&& merkle,
    uint32_t timestamp, uint32_t bits, uint32_t nonce,
    uint64_t transaction_count)
  : header(version, std::forward<hash_digest>(previous_block_hash),
        std::forward<hash_digest>(merkle), timestamp, bits, nonce,
        transaction_count)
{
}

header_message& header_message::operator=(header_message&& other)
{
    version = other.version;
    previous_block_hash = std::move(other.previous_block_hash);
    merkle = std::move(other.merkle);
    timestamp = other.timestamp;
    bits = other.bits;
    nonce = other.nonce;
    transaction_count = other.transaction_count;
    originator_ = other.originator_;
    return *this;
}

bool header_message::from_data(const uint32_t version, const data_chunk& data,
    bool with_transaction_count)
{
    originator_ = version;
    return header::from_data(data, with_transaction_count);
}

bool header_message::from_data(const uint32_t version, std::istream& stream,
    bool with_transaction_count)
{
    originator_ = version;
    return header::from_data(stream, with_transaction_count);
}

bool header_message::from_data(const uint32_t version, reader& source,
    bool with_transaction_count)
{
    originator_ = version;
    return header::from_data(source, with_transaction_count);
}

data_chunk header_message::to_data(const uint32_t version,
    bool with_transaction_count) const
{
    return header::to_data(with_transaction_count);
}

void header_message::to_data(const uint32_t version, std::ostream& stream,
    bool with_transaction_count) const
{
    header::to_data(stream, with_transaction_count);
}

void header_message::to_data(const uint32_t version, writer& sink,
    bool with_transaction_count) const
{
    header::to_data(sink, with_transaction_count);
}

uint64_t header_message::serialized_size(const uint32_t version,
    bool with_transaction_count) const
{
    return header::serialized_size(with_transaction_count);
}

} // namspace message
} // namspace libbitcoin
