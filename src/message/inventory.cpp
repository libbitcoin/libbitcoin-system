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
#include <bitcoin/bitcoin/utility/data_stream_host.hpp>
#include <bitcoin/bitcoin/utility/istream.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace message {

inventory_list& inventory::inventories()
{
    return inventories_;
}

const inventory_list& inventory::inventories() const
{
    return inventories_;
}

void inventory::reset()
{
    inventories_.clear();
}

bool inventory::from_data(const data_chunk& data)
{
    data_chunk_stream_host host(data);
    return from_data(host.stream);
}

bool inventory::from_data(std::istream& stream)
{
    bool result = true;

    reset();

    uint64_t count = read_variable_uint(stream);
    result = !stream.fail();

    for (size_t i = 0; (i < count) && result; ++i)
    {
        inventory_vector inv;
        result = inv.from_data(stream);
        inventories_.push_back(inv);
    }

    if (!result)
        reset();

    return result;
}

data_chunk inventory::to_data() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());
    serial.write_variable_uint(inventories_.size());

    for (const inventory_vector inv: inventories_)
        serial.write_data(inv.to_data());

    return result;
}

size_t inventory::satoshi_size() const
{
    return variable_uint_size(inventories_.size())
        + inventories_.size() * inventory_vector::satoshi_fixed_size();
}

} // end message
} // end libbitcoin
