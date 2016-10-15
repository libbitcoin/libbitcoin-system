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
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string header_message::command = "headers";
const uint32_t header_message::version_minimum = version::level::minimum;
const uint32_t header_message::version_maximum = version::level::maximum;

header_message header_message::factory_from_data(const uint32_t version,
    const data_chunk& data)
{
    header_message instance;
    instance.from_data(version, data);
    return instance;
}

header_message header_message::factory_from_data(const uint32_t version,
    std::istream& stream)
{
    header_message instance;
    instance.from_data(version, stream);
    return instance;
}

header_message header_message::factory_from_data(const uint32_t version,
    reader& source)
{
    header_message instance;
    instance.from_data(version, source);
    return instance;
}

uint64_t header_message::satoshi_fixed_size(const uint32_t version)
{
    return chain::header::satoshi_fixed_size() + variable_uint_size(0);
}

header_message::header_message()
  : header(), originator_(0u)
{
}

header_message::header_message(uint32_t version,
    const hash_digest& previous_block_hash, const hash_digest& merkle,
    uint32_t timestamp, uint32_t bits, uint32_t nonce, uint64_t originator)
  : header(version, previous_block_hash, merkle, timestamp, bits, nonce),
    originator_(originator)
{
}

header_message::header_message(uint32_t version,
    hash_digest&& previous_block_hash, hash_digest&& merkle,
    uint32_t timestamp, uint32_t bits, uint32_t nonce, uint64_t originator)
  : header(version, std::move(previous_block_hash), std::move(merkle),
      timestamp, bits, nonce), originator_(originator)
{
}

header_message::header_message(const chain::header& other)
  : header(other), originator_(0u)
{
}

header_message::header_message(chain::header&& other)
  : header(std::move(other)), originator_(0u)
{
}

header_message::header_message(const header_message& other)
: header(other), originator_(other.originator_)
{
}

header_message::header_message(header_message&& other)
  : header(other), originator_(other.originator_)
{
}

bool header_message::from_data(const uint32_t version, const data_chunk& data)
{
    data_source istream(data);
    return from_data(version, istream);
}

bool header_message::from_data(const uint32_t version, std::istream& stream)
{
    istream_reader source(stream);
    return from_data(version, source);
}

bool header_message::from_data(const uint32_t version, reader& source)
{
    if (!header::from_data(source))
        return false;

    // The header message must trail a zero byte (yes, it's stoopid).
    // bitcoin.org/en/developer-reference#headers
    if (source.read_byte() != 0x00)
        source.invalidate();

    if (!source)
        reset();

    return source;
}

data_chunk header_message::to_data(const uint32_t version) const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(version, ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size(version));
    return data;
}

void header_message::to_data(const uint32_t version, std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(version, sink);
}

void header_message::to_data(const uint32_t version, writer& sink) const
{
    header::to_data(sink);
    sink.write_variable_little_endian(0);
}

void header_message::reset()
{
    header::reset();
    originator_ = 0u;
}

uint64_t header_message::serialized_size(const uint32_t version) const
{
    return satoshi_fixed_size(version);
}

uint64_t header_message::originator() const
{
    return originator_;
}

void header_message::set_originator(uint64_t value) const
{
    originator_ = value;
}

header_message& header_message::operator=(chain::header&& other)
{
    chain::header::operator=(std::move(other));
    return *this;
}

header_message& header_message::operator=(header_message&& other)
{
    originator_ = other.originator_;
    chain::header::operator=(std::move(other));
    return *this;
}

header_message& header_message::operator=(const header_message& other)
{
    originator_ = other.originator_;
    chain::header::operator=(other);
    return *this;
}

bool header_message::operator==(const chain::header& other) const
{
    return chain::header::operator==(other);
}

bool header_message::operator!=(const chain::header& other) const
{
    return chain::header::operator!=(other);
}

bool header_message::operator==(const header_message& other) const
{
    return chain::header::operator==(other);
}

bool header_message::operator!=(const header_message& other) const
{
    return chain::header::operator!=(other);
}

} // namespace message
} // namespace libbitcoin
