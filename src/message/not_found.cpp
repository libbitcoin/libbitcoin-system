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
#include <bitcoin/bitcoin/message/not_found.hpp>

#include <initializer_list>
#include <bitcoin/bitcoin/math/hash.hpp>

namespace libbitcoin {
namespace message {

const std::string message::not_found::command = "notfound";

not_found not_found::factory_from_data(const uint32_t version,
    const data_chunk& data)
{
    not_found instance;
    instance.from_data(version, data);
    return instance;
}

not_found not_found::factory_from_data(const uint32_t version,
    std::istream& stream)
{
    not_found instance;
    instance.from_data(version, stream);
    return instance;
}

not_found not_found::factory_from_data(const uint32_t version,
    reader& source)
{
    not_found instance;
    instance.from_data(version, source);
    return instance;
}

not_found::not_found()
  : inventory()
{
}

not_found::not_found(const inventory_vector::list& values)
  : inventory(values)
{
}

not_found::not_found(const hash_list& hashes, inventory_type_id type_id)
  : inventory(hashes, type_id)
{
}

not_found::not_found(const std::initializer_list<inventory_vector>& values)
  : inventory(values)
{
}

} // namspace message
} // namspace libbitcoin
