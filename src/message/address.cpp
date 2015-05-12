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
#include <bitcoin/bitcoin/utility/data_stream_host.hpp>

namespace libbitcoin {
namespace message {

network_address_list& address::addresses()
{
    return addresses_;
}

const network_address_list& address::addresses() const
{
    return addresses_;
}

void address::reset()
{
    addresses_.clear();
}

bool address::from_data(const data_chunk& data)
{
    data_chunk_stream_host host(data);
    return from_data(host.stream);
}

bool address::from_data(std::istream& stream)
{
    bool result = true;

    reset();

    uint64_t count = read_variable_uint(stream);
    result = !stream.fail();

    for (size_t i = 0; (i < count) && result; ++i)
    {
        network_address addr;
        result = addr.from_data(stream, true);
        addresses_.push_back(addr);
    }

    if (!result)
        reset();

    return result;
}

data_chunk address::to_data() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());

    serial.write_variable_uint(addresses_.size());

    for (const network_address& net_address: addresses_)
    {
        serial.write_4_bytes(net_address.timestamp());
        serial.write_data(net_address.to_data());
    }

    return result;
}

size_t address::satoshi_size() const
{
    return variable_uint_size(addresses_.size())
        + addresses_.size() * network_address::satoshi_fixed_size();
}

} // end message
} // end libbitcoin
