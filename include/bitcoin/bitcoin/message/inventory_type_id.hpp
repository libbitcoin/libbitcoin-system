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
#ifndef LIBBITCOIN_MESSAGE_INVENTORY_TYPE_ID_HPP
#define LIBBITCOIN_MESSAGE_INVENTORY_TYPE_ID_HPP

#include <cstdint>
#include <bitcoin/bitcoin/define.hpp>

namespace libbitcoin {
namespace message {

enum class inventory_type_id
{
    error,
    transaction,
    block,
    none
};

BC_API uint32_t inventory_type_to_number(inventory_type_id inv_type);

BC_API inventory_type_id inventory_type_from_number(uint32_t raw_type);

} // end message
} // end libbitcoin

#endif
