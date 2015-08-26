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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/message/reject.hpp>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/chain/block.hpp>
#include <bitcoin/bitcoin/chain/transaction.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string message::reject::satoshi_command = "reject";

reject reject::factory_from_data(const data_chunk& data)
{
    reject instance;
    instance.from_data(data);
    return instance;
}

reject reject::factory_from_data(std::istream& stream)
{
    reject instance;
    instance.from_data(stream);
    return instance;
}

reject reject::factory_from_data(reader& source)
{
    reject instance;
    instance.from_data(source);
    return instance;
}

bool reject::is_valid() const
{
    return !message.empty()
        || (code != error_code::undefined)
        || !reason.empty()
        || (data != null_hash);
}

void reject::reset()
{
    message.clear();
    code = error_code::undefined;
    reason.clear();
    data.fill(0);
}

bool reject::from_data(const data_chunk& data)
{
    boost::iostreams::stream<byte_source<data_chunk>> istream(data);
    return from_data(istream);
}

bool reject::from_data(std::istream& stream)
{
    istream_reader source(stream);
    return from_data(source);
}

bool reject::from_data(reader& source)
{
    bool result = 0;

    reset();

    message = source.read_string();
    code = error_code_from_byte(source.read_byte());
    reason = source.read_string();

    if ((message == chain::block::satoshi_command) ||
        (message == chain::transaction::satoshi_command))
    {
        data = source.read_hash();
    }

    result = source;

    if (!result)
        reset();

    return result;
}

data_chunk reject::to_data() const
{
    data_chunk data;
    boost::iostreams::stream<byte_sink<data_chunk>> ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == satoshi_size());
    return data;
}

void reject::to_data(std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(sink);
}

void reject::to_data(writer& sink) const
{
    sink.write_string(message);
    sink.write_byte(error_code_to_byte(code));
    sink.write_string(reason);

    if ((message == chain::block::satoshi_command) ||
        (message == chain::transaction::satoshi_command))
    {
        sink.write_hash(data);
    }
}

uint64_t reject::satoshi_size() const
{
    uint64_t size = 1 + variable_uint_size(message.size()) + message.size() +
        variable_uint_size(reason.size()) + reason.size();

    if ((message == chain::block::satoshi_command) ||
        (message == chain::transaction::satoshi_command))
    {
        size += hash_size;
    }

    return size;
}

reject::error_code reject::error_code_from_byte(const uint8_t byte)
{
    error_code result = error_code::undefined;

    switch (byte)
    {
        case 0x01:
            result = error_code::malformed;
            break;

        case 0x10:
            result = error_code::invalid;
            break;

        case 0x11:
            result = error_code::obsolete;
            break;

        case 0x12:
            result = error_code::duplicate;
            break;

        case 0x40:
            result = error_code::nonstandard;
            break;

        case 0x41:
            result = error_code::dust;
            break;

        case 0x42:
            result = error_code::insufficient_fee;
            break;

        case 0x43:
            result = error_code::checkpoint;
            break;

        default:
            result = error_code::undefined;
            break;
    }

    return result;
}

uint8_t reject::error_code_to_byte(const error_code code)
{
    uint8_t result = 0x00;

    switch (code)
    {
        case error_code::malformed:
            result = 0x01;
            break;

        case error_code::invalid:
            result = 0x10;
            break;

        case error_code::obsolete:
            result = 0x11;
            break;

        case error_code::duplicate:
            result = 0x12;
            break;

        case error_code::nonstandard:
            result = 0x40;
            break;

        case error_code::dust:
            result = 0x41;
            break;

        case error_code::insufficient_fee:
            result = 0x42;
            break;

        case error_code::checkpoint:
            result = 0x43;
            break;

        default:
            result = 0x00;
            break;
    }

    return result;
}

} // end message
} // end libbitcoin
