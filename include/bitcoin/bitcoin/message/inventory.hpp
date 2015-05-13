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
#ifndef LIBBITCOIN_MESSAGE_INVENTORY_HPP
#define LIBBITCOIN_MESSAGE_INVENTORY_HPP

#include <istream>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/message/inventory_vector.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace message {

class BC_API inventory
{
public:

    static const std::string satoshi_command;

    inventory_list& inventories();

    const inventory_list& inventories() const;

    bool from_data(const data_chunk& data);

    bool from_data(std::istream& stream);

    data_chunk to_data() const;

    void reset();

    uint64_t satoshi_size() const;

private:

    inventory_list inventories_;
};

} // end message
} // end libbitcoin

#endif
