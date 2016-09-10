/*
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
#include <bitcoin/bitcoin/message/reject.hpp>

#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/message/version.hpp>
#include <bitcoin/bitcoin/message/block_message.hpp>
#include <bitcoin/bitcoin/message/transaction_message.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string reject::command = "reject";
const uint32_t reject::version_minimum = version::level::bip61;
const uint32_t reject::version_maximum = version::level::maximum;

reject reject::factory_from_data(uint32_t version,
    const data_chunk& data)
{
    reject instance;
    instance.from_data(version, data);
    return instance;
}

reject reject::factory_from_data(uint32_t version,
    std::istream& stream)
{
    reject instance;
    instance.from_data(version, stream);
    return instance;
}

reject reject::factory_from_data(uint32_t version,
    reader& source)
{
    reject instance;
    instance.from_data(version, source);
    return instance;
}

bool reject::is_valid() const
{
    return !message.empty()
        || (code != reason_code::undefined)
        || !reason.empty()
        || (data != null_hash);
}

void reject::reset()
{
    message.clear();
    message.shrink_to_fit();
    code = reason_code::undefined;
    reason.clear();
    reason.shrink_to_fit();
    data.fill(0);
}

bool reject::from_data(uint32_t version, const data_chunk& data)
{
    boost::iostreams::stream<byte_source<data_chunk>> istream(data);
    return from_data(version, istream);
}

bool reject::from_data(uint32_t version, std::istream& stream)
{
    istream_reader source(stream);
    return from_data(version, source);
}

bool reject::from_data(uint32_t version, reader& source)
{
    const auto insufficient_version = (version < reject::version_minimum);

    reset();

    message = source.read_string();
    code = reason_from_byte(source.read_byte());
    reason = source.read_string();

    if ((message == block_message::command) ||
        (message == transaction_message::command))
    {
        data = source.read_hash();
    }


    const auto result = source && !insufficient_version;

    if (!result)
        reset();

    return result;
}

data_chunk reject::to_data(uint32_t version) const
{
    data_chunk data;
    boost::iostreams::stream<byte_sink<data_chunk>> ostream(data);
    to_data(version, ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size(version));
    return data;
}

void reject::to_data(uint32_t version, std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(version, sink);
}

void reject::to_data(uint32_t version, writer& sink) const
{
    sink.write_string(message);
    sink.write_byte(reason_to_byte(code));
    sink.write_string(reason);

    if ((message == block_message::command) ||
        (message == transaction_message::command))
    {
        sink.write_hash(data);
    }
}

uint64_t reject::serialized_size(uint32_t version) const
{
    uint64_t size = 1 + variable_uint_size(message.size()) + message.size() +
        variable_uint_size(reason.size()) + reason.size();

    if ((message == block_message::command) ||
        (message == transaction_message::command))
    {
        size += hash_size;
    }

    return size;
}

reject::reason_code reject::reason_from_byte(uint8_t byte)
{
    switch (byte)
    {
        case 0x01:
            return reason_code::malformed;
        case 0x10:
            return reason_code::invalid;
        case 0x11:
            return reason_code::obsolete;
        case 0x12:
            return reason_code::duplicate;
        case 0x40:
            return reason_code::nonstandard;
        case 0x41:
            return reason_code::dust;
        case 0x42:
            return reason_code::insufficient_fee;
        case 0x43:
            return reason_code::checkpoint;
        default:
            return reason_code::undefined;
    }
}

uint8_t reject::reason_to_byte(reason_code value)
{
    switch (value)
    {
        case reason_code::malformed:
            return 0x01;
        case reason_code::invalid:
            return 0x10;
        case reason_code::obsolete:
            return 0x11;
        case reason_code::duplicate:
            return 0x12;
        case reason_code::nonstandard:
            return 0x40;
        case reason_code::dust:
            return 0x41;
        case reason_code::insufficient_fee:
            return 0x42;
        case reason_code::checkpoint:
            return 0x43;
        default:
            return 0x00;
    }
}

} // end message
} // end libbitcoin
