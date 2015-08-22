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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/message/get_address.hpp>

#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string message::get_address::satoshi_command = "getaddr";

get_address get_address::factory_from_data(const data_chunk& data)
{
    get_address instance;
    instance.from_data(data);
    return instance;
}

get_address get_address::factory_from_data(std::istream& stream)
{
    get_address instance;
    instance.from_data(stream);
    return instance;
}

get_address get_address::factory_from_data(reader& source)
{
    get_address instance;
    instance.from_data(source);
    return instance;
}

bool get_address::is_valid() const
{
    return true;
}

void get_address::reset()
{
}

bool get_address::from_data(const data_chunk& data)
{
    data_source istream(data);
    return from_data(istream);
}

bool get_address::from_data(std::istream& stream)
{
    istream_reader source(stream);
    return from_data(source);
}

bool get_address::from_data(reader& source)
{
    reset();
    return source;
}

data_chunk get_address::to_data() const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == satoshi_size());
    return data;
}

void get_address::to_data(std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(sink);
}

void get_address::to_data(writer& sink) const
{
}

uint64_t get_address::satoshi_size() const
{
    return get_address::satoshi_fixed_size();
}

uint64_t get_address::satoshi_fixed_size()
{
    return 0;
}

} // namspace message
} // namspace libbitcoin
