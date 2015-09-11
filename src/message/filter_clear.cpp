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
#include <bitcoin/bitcoin/message/filter_clear.hpp>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string message::filter_clear::command = "filterclear";

filter_clear filter_clear::factory_from_data(const data_chunk& data)
{
    filter_clear instance;
    instance.from_data(data);
    return instance;
}

filter_clear filter_clear::factory_from_data(std::istream& stream)
{
    filter_clear instance;
    instance.from_data(stream);
    return instance;
}

filter_clear filter_clear::factory_from_data(reader& source)
{
    filter_clear instance;
    instance.from_data(source);
    return instance;
}

bool filter_clear::is_valid() const
{
    return true;
}

void filter_clear::reset()
{
}

bool filter_clear::from_data(const data_chunk& data)
{
    boost::iostreams::stream<byte_source<data_chunk>> istream(data);
    return from_data(istream);
}

bool filter_clear::from_data(std::istream& stream)
{
    istream_reader source(stream);
    return from_data(source);
}

bool filter_clear::from_data(reader& source)
{
    reset();
    return source;
}

data_chunk filter_clear::to_data() const
{
    data_chunk data;
    boost::iostreams::stream<byte_sink<data_chunk>> ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size());
    return data;
}

void filter_clear::to_data(std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(sink);
}

void filter_clear::to_data(writer& sink) const
{
}

uint64_t filter_clear::serialized_size() const
{
    return filter_clear::satoshi_fixed_size();
}

uint64_t filter_clear::satoshi_fixed_size()
{
    return 0;
}

} // end message
} // end libbitcoin
