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
#include <bitcoin/bitcoin/message/alert.hpp>

#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string message::alert::command = "alert";

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
    reset();

    auto size = source.read_variable_uint_little_endian();
    BITCOIN_ASSERT(size <= bc::max_size_t);
    const auto payload_size = static_cast<size_t>(size);
    size_t signature_size = 0;
    auto result = static_cast<bool>(source);

    if (result)
    {
        payload = source.read_data(payload_size);
        result = source && (payload.size() == payload_size);
    }

    if (result)
    {
        size = source.read_variable_uint_little_endian();
        BITCOIN_ASSERT(size <= bc::max_size_t);
        signature_size = static_cast<size_t>(size);
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
    BITCOIN_ASSERT(data.size() == serialized_size());
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

uint64_t alert::serialized_size() const
{
    return variable_uint_size(payload.size()) + payload.size() +
        variable_uint_size(signature.size()) + signature.size();
}

bool operator==(const alert& left, const alert& right)
{
    bool result = (left.payload.size() == right.payload.size()) &&
        (left.signature.size() == right.signature.size());

    for (data_chunk::size_type i = 0; i < left.payload.size() && result; i++)
        result = (left.payload[i] == right.payload[i]);

    for (data_chunk::size_type i = 0; i < left.signature.size() && result; i++)
        result = (left.signature[i] == right.signature[i]);

    return result;
}

bool operator!=(const alert& left, const alert& right)
{
    return !(left == right);
}

} // end message
} // end libbitcoin
