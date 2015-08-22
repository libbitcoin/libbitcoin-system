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
#include <bitcoin/bitcoin/message/inventory_type_id.hpp>

namespace libbitcoin {
namespace message {

uint32_t inventory_type_to_number(inventory_type_id inv_type)
{
    switch (inv_type)
    {
        case inventory_type_id::error:
        case inventory_type_id::none:
        default:
            return 0;

        case inventory_type_id::transaction:
            return 1;

        case inventory_type_id::block:
            return 2;
    }
}

inventory_type_id inventory_type_from_number(uint32_t raw_type)
{
    switch (raw_type)
    {
        case 0:
            return inventory_type_id::error;
        case 1:
            return inventory_type_id::transaction;
        case 2:
            return inventory_type_id::block;
        default:
            return inventory_type_id::none;
    }
}

} // namspace message
} // namspace libbitcoin
