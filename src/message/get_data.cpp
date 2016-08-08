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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/message/get_data.hpp>

#include <initializer_list>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>

namespace libbitcoin {
namespace message {

const std::string message::get_data::command = "getdata";
const uint32_t message::get_data::version_minimum = peer_minimum_version;
const uint32_t message::get_data::version_maximum = protocol_version;

get_data get_data::factory_from_data(const uint32_t version,
    const data_chunk& data)
{
    get_data instance;
    instance.from_data(version,data);
    return instance;
}

get_data get_data::factory_from_data(const uint32_t version,
    std::istream& stream)
{
    get_data instance;
    instance.from_data(version, stream);
    return instance;
}

get_data get_data::factory_from_data(const uint32_t version,
    reader& source)
{
    get_data instance;
    instance.from_data(version, source);
    return instance;
}

get_data::get_data()
  : inventory()
{
}

get_data::get_data(const inventory_vector::list& elements)
  : inventory(elements)
{
}

get_data::get_data(const hash_list& hashes, inventory_type_id type_id)
  : inventory(hashes, type_id)
{
}

get_data::get_data(const std::initializer_list<inventory_vector>& elements)
  : inventory(elements)
{
}

bool get_data::from_data(const uint32_t version, const data_chunk& data)
{
    return inventory::from_data(version, data);
}

bool get_data::from_data(const uint32_t version, std::istream& stream)
{
    return inventory::from_data(version, stream);
}

bool get_data::from_data(const uint32_t version, reader& source)
{
    bool result = !(version < get_data::version_minimum);

    if (result)
        result = inventory::from_data(version, source);

    if (!result)
        reset();

    return result;
}

} // namspace message
} // namspace libbitcoin
