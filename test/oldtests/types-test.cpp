/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/types.hpp>
#include <algorithm>
#include <iostream>
using namespace libbitcoin;

int main()
{
    data_chunk bs;
    // 0 f2 5 2a 1 0 0 0
    bs.push_back(0x00);
    bs.push_back(0xf2);
    bs.push_back(0x05);
    bs.push_back(0x2a);
    bs.push_back(0x01);
    bs.push_back(0x00);
    bs.push_back(0x00);
    bs.push_back(0x00);

    uint64_t val = cast_chunk<uint64_t>(bs);
    std::cout << val << "\n";
    std::cout << std::hex << val << "\n";

    std::reverse(bs.begin(), bs.end());
    val = 0;
    for (size_t i = 0; i < bs.size(); i++)
    {
        val += static_cast<uint64_t>(bs[i]) << (8*i);
    }

    std::cout << std::dec << val << "\n";
    std::cout << std::hex << val << "\n";
    return 0;
}

