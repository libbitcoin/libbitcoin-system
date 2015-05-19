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
#include <bitcoin/bitcoin/message/inventory.hpp>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/utility/data_source.hpp>
#include <bitcoin/bitcoin/utility/istream.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace message {

bool inventory::is_valid() const
{
    return !inventories.empty();
}

void inventory::reset()
{
    inventories.clear();
}

bool inventory::from_data(const data_chunk& data)
{
    byte_source<data_chunk> source(data);
    boost::iostreams::stream<byte_source<data_chunk>> istream(source);
    return from_data(istream);
}

bool inventory::from_data(std::istream& stream)
{
    bool result = true;

    reset();

    uint64_t count = read_variable_uint(stream);
    result = stream;

    for (uint64_t i = 0; (i < count) && result; ++i)
    {
        inventory_vector inv;
        result = inv.from_data(stream);
        inventories.push_back(inv);
    }

    if (!result)
        reset();

    return result;
}

data_chunk inventory::to_data() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());
    serial.write_variable_uint(inventories.size());

    for (const inventory_vector inv: inventories)
        serial.write_data(inv.to_data());

    return result;
}

uint64_t inventory::satoshi_size() const
{
    return variable_uint_size(inventories.size())
        + inventories.size() * inventory_vector::satoshi_fixed_size();
}

} // end message
} // end libbitcoin
