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
#include <bitcoin/bitcoin/primitives/blockchain/point.hpp>
#include <sstream>
#include <bitcoin/bitcoin/formats/base16.hpp>

namespace libbitcoin {

std::string point_type::to_string() const
{
    std::ostringstream ss;

    ss << "\thash = " << encode_hash(hash) << "\n" << "\tindex = " << index;

    return ss.str();
}

bool operator==(const output_point& output_a, const output_point& output_b)
{
    return output_a.hash == output_b.hash && output_a.index == output_b.index;
}
bool operator!=(const output_point& output_a, const output_point& output_b)
{
    return !(output_a == output_b);
}

}
