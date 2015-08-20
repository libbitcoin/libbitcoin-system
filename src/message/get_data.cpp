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
#include <bitcoin/bitcoin/message/get_data.hpp>

namespace libbitcoin {
namespace message {

get_data get_data::factory_from_data(const data_chunk& data)
{
    get_data instance;
    instance.from_data(data);
    return instance;
}

get_data get_data::factory_from_data(std::istream& stream)
{
    get_data instance;
    instance.from_data(stream);
    return instance;
}

get_data get_data::factory_from_data(reader& source)
{
    get_data instance;
    instance.from_data(source);
    return instance;
}

get_data::get_data()
{
}

get_data::get_data(const inventory_vector::list& elements)
{
    inventories = elements;
}

} // end message
} // end libbitcoin
