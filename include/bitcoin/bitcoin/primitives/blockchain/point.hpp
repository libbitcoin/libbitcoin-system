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
#ifndef LIBBITCOIN_BLOCKCHAIN_PRIMITIVE_POINT_HPP
#define LIBBITCOIN_BLOCKCHAIN_PRIMITIVE_POINT_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>

namespace libbitcoin {

class BC_API point_type
{
public:

    hash_digest hash;
    uint32_t index;

    std::string to_string() const;
};

typedef point_type input_point;
typedef std::vector<input_point> input_point_list;

typedef point_type output_point;
typedef std::vector<output_point> output_point_list;

BC_API bool operator==(const output_point& output_a,
    const output_point& output_b);

BC_API bool operator!=(const output_point& output_a,
    const output_point& output_b);

}

#endif
