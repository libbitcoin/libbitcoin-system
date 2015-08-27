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
#include <bitcoin/bitcoin/message/alert.hpp>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string message::alert::satoshi_command = "alert";

alert alert::factory_from_data(const data_chunk& data)
{
    alert instance;
    instance.from_data(data);
    return instance;
}

alert alert::factory_from_data(std::istream& stream)
{
    alert instance;
    instance.from_data(stream);
    return instance;
}

alert alert::factory_from_data(reader& source)
{
    alert instance;
    instance.from_data(source);
    return instance;
}

bool alert::is_valid() const
{
    return !payload.empty() || !signature.empty();
}

void alert::reset()
{
    payload.clear();
    signature.clear();
}

bool alert::from_data(const data_chunk& data)
{
    boost::iostreams::stream<byte_source<data_chunk>> istream(data);
    return from_data(istream);
}

bool alert::from_data(std::istream& stream)
{
    istream_reader source(stream);
    return from_data(source);
}

bool alert::from_data(reader& source)
{
    bool result = false;

    reset();

    uint64_t payload_size = source.read_variable_uint_little_endian();
    uint64_t signature_size = 0;
    result = source;

    if (result)
    {
        payload = source.read_data(payload_size);
        result = source && (payload.size() == payload_size );
    }

    if (result)
    {
        signature_size = source.read_variable_uint_little_endian();
        result = source;
    }

    if (result)
    {
        signature = source.read_data(signature_size);
        result = source && (signature.size() == signature_size);
    }

    if (!result)
        reset();

    return result;
}

data_chunk alert::to_data() const
{
    data_chunk data;
    boost::iostreams::stream<byte_sink<data_chunk>> ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == satoshi_size());
    return data;
}

void alert::to_data(std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(sink);
}

void alert::to_data(writer& sink) const
{
    sink.write_variable_uint_little_endian(payload.size());
    sink.write_data(payload);
    sink.write_variable_uint_little_endian(signature.size());
    sink.write_data(signature);
}

uint64_t alert::satoshi_size() const
{
    return variable_uint_size(payload.size()) + payload.size() +
        variable_uint_size(signature.size()) + signature.size();
}

bool operator==(const alert& msg_a, const alert& msg_b)
{
    bool result = (msg_a.payload.size() == msg_b.payload.size()) &&
        (msg_a.signature.size() == msg_b.signature.size());

    for (data_chunk::size_type i = 0; i < msg_a.payload.size() && result; i++)
        result = (msg_a.payload[i] == msg_b.payload[i]);

    for (data_chunk::size_type i = 0; i < msg_a.signature.size() && result; i++)
        result = (msg_a.signature[i] == msg_b.signature[i]);

    return result;
}

bool operator!=(const alert& msg_a, const alert& msg_b)
{
    return !(msg_a == msg_b);
}

} // end message
} // end libbitcoin
