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
#include <bitcoin/bitcoin/message/get_address.hpp>

namespace libbitcoin {
namespace message {

get_address::get_address()
{
}

get_address::get_address(std::istream& stream)
{
    if (stream.fail() || !(stream.peek() == std::istream::traits_type::eof()))
        throw std::ios_base::failure("get_address");
}

data_chunk get_address::to_data() const
{
    data_chunk result(satoshi_size());
    return result;
}

size_t get_address::satoshi_size() const
{
    return get_address::satoshi_fixed_size();
}

size_t get_address::satoshi_fixed_size()
{
    return 0;
}

} // end message
} // end libbitcoin
