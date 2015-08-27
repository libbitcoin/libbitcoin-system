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
#include <bitcoin/bitcoin/message/filter_load.hpp>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string message::filter_load::satoshi_command = "filterload";

filter_load filter_load::factory_from_data(const data_chunk& data)
{
    filter_load instance;
    instance.from_data(data);
    return instance;
}

filter_load filter_load::factory_from_data(std::istream& stream)
{
    filter_load instance;
    instance.from_data(stream);
    return instance;
}

filter_load filter_load::factory_from_data(reader& source)
{
    filter_load instance;
    instance.from_data(source);
    return instance;
}

bool filter_load::is_valid() const
{
    return !filter.empty()
        || (hash_functions != 0)
        || (tweak != 0)
        || (flags != 0x00);
}

void filter_load::reset()
{
    filter.clear();
    hash_functions = 0;
    tweak = 0;
    flags = 0x00;
}

bool filter_load::from_data(const data_chunk& data)
{
    boost::iostreams::stream<byte_source<data_chunk>> istream(data);
    return from_data(istream);
}

bool filter_load::from_data(std::istream& stream)
{
    istream_reader source(stream);
    return from_data(source);
}

bool filter_load::from_data(reader& source)
{
    bool result = false;

    reset();

    uint64_t filter_size = source.read_variable_uint_little_endian();
    result = source;

    if (result)
    {
        filter = source.read_data(filter_size);
        hash_functions = source.read_4_bytes_little_endian();
        tweak = source.read_4_bytes_little_endian();
        flags = source.read_byte();
        result = source && (filter.size() == filter_size);
    }

    if (!result)
        reset();

    return result;
}

data_chunk filter_load::to_data() const
{
    data_chunk data;
    boost::iostreams::stream<byte_sink<data_chunk>> ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == satoshi_size());
    return data;
}

void filter_load::to_data(std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(sink);
}

void filter_load::to_data(writer& sink) const
{
    sink.write_variable_uint_little_endian(filter.size());
    sink.write_data(filter);
    sink.write_4_bytes_little_endian(hash_functions);
    sink.write_4_bytes_little_endian(tweak);
    sink.write_byte(flags);
}

uint64_t filter_load::satoshi_size() const
{
    return 1 + 4 + 4 + variable_uint_size(filter.size()) + filter.size();
}

bool operator==(const filter_load& left,
    const filter_load& right)
{
    bool result = (left.filter.size() == right.filter.size()) &&
        (left.hash_functions == right.hash_functions) &&
        (left.tweak == right.tweak) &&
        (left.flags == right.flags);

    for (data_chunk::size_type i = 0; i < left.filter.size() && result; i++)
        result = (left.filter[i] == right.filter[i]);

    return result;
}

bool operator!=(const filter_load& left,
    const filter_load& right)
{
    return !(left == right);
}

} // end message
} // end libbitcoin
