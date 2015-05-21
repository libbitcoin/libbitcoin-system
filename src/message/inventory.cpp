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
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream.hpp>
#include <bitcoin/bitcoin/utility/ostream.hpp>

namespace libbitcoin {
namespace message {

inventory inventory::factory_from_data(const data_chunk& data)
{
    inventory instance;
    instance.from_data(data);
    return instance;
}

inventory inventory::factory_from_data(std::istream& stream)
{
    inventory instance;
    instance.from_data(stream);
    return instance;
}

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
    boost::iostreams::stream<byte_source<data_chunk>> istream(data);
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
    data_chunk data;
    boost::iostreams::stream<byte_sink<data_chunk>> ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == satoshi_size());
    return data;
}

void inventory::to_data(std::ostream& stream) const
{
    write_variable_uint(stream, inventories.size());

    for (const inventory_vector inv : inventories)
        inv.to_data(stream);
}

uint64_t inventory::satoshi_size() const
{
    return variable_uint_size(inventories.size())
        + inventories.size() * inventory_vector::satoshi_fixed_size();
}

} // end message
} // end libbitcoin
