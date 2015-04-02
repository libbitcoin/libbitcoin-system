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
#include <bitcoin/bitcoin/chain/point.hpp>

#include <sstream>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/formats/base16.hpp>

namespace libbitcoin {
namespace chain {

std::string point::to_string() const
{
    std::ostringstream ss;

    ss << "\thash = " << encode_hash(hash) << "\n" << "\tindex = " << index;

    return ss.str();
}

bool point::is_null() const
{
    return (index == max_index) && (hash == null_hash);
}

bool operator==(const point& a, const point& b)
{
    return a.hash == b.hash && a.index == b.index;
}
bool operator!=(const point& a, const point& b)
{
    return !(a == b);
}

} // end chain
} // end libbitcoin
