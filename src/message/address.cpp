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
#include <bitcoin/bitcoin/message/address.hpp>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/utility/data_source.hpp>

namespace libbitcoin {
namespace message {

void address::reset()
{
    addresses.clear();
}

bool address::from_data(const data_chunk& data)
{
    byte_source<data_chunk> source(data);
    boost::iostreams::stream<byte_source<data_chunk>> istream(source);
    return from_data(istream);
}

bool address::from_data(std::istream& stream)
{
    bool result = true;

    reset();

    uint64_t count = read_variable_uint(stream);
    result = stream;

    for (uint64_t i = 0; (i < count) && result; ++i)
    {
        addresses.emplace_back();
        result = addresses.back().from_data(stream, true);
    }

    if (!result)
        reset();

    return result;
}

data_chunk address::to_data() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());

    serial.write_variable_uint(addresses.size());

    for (const network_address& net_address: addresses)
    {
        serial.write_data(net_address.to_data(true));
    }

    return result;
}

uint64_t address::satoshi_size() const
{
    return variable_uint_size(addresses.size())
        + (addresses.size() * network_address::satoshi_fixed_size(true));
}

} // end message
} // end libbitcoin
