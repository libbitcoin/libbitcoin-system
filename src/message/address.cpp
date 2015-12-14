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
#include <bitcoin/bitcoin/message/address.hpp>

#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string message::address::command = "addr";

address address::factory_from_data(const data_chunk& data)
{
    address instance;
    instance.from_data(data);
    return instance;
}

address address::factory_from_data(std::istream& stream)
{
    address instance;
    instance.from_data(stream);
    return instance;
}

address address::factory_from_data(reader& source)
{
    address instance;
    instance.from_data(source);
    return instance;
}

bool address::is_valid() const
{
    return !addresses.empty();
}

void address::reset()
{
    addresses.clear();
}

bool address::from_data(const data_chunk& data)
{
    data_source istream(data);
    return from_data(istream);
}

bool address::from_data(std::istream& stream)
{
    istream_reader source(stream);
    return from_data(source);
}

bool address::from_data(reader& source)
{
    reset();

    uint64_t count = source.read_variable_uint_little_endian();
    auto result = static_cast<bool>(source);

    for (uint64_t i = 0; (i < count) && result; ++i)
    {
        addresses.emplace_back();
        result = addresses.back().from_data(source, true);
    }

    if (!result)
        reset();

    return result;
}

data_chunk address::to_data() const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size());
    return data;
}

void address::to_data(std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(sink);
}

void address::to_data(writer& sink) const
{
    sink.write_variable_uint_little_endian(addresses.size());
    for (const network_address& net_address : addresses)
        net_address.to_data(sink, true);
}

uint64_t address::serialized_size() const
{
    return variable_uint_size(addresses.size()) + 
        (addresses.size() * network_address::satoshi_fixed_size(true));
}

} // namspace message
} // namspace libbitcoin
