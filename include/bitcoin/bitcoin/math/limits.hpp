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
#ifndef LIBBITCOIN_LIMITS_HPP
#define LIBBITCOIN_LIMITS_HPP

#include <algorithm>
#include <limits>
#include <bitcoin/bitcoin/constants.hpp>

namespace libbitcoin {

template <typename Integer,
    typename = std::enable_if<std::is_unsigned<Integer>::value>>
Integer ceiling_add(Integer left, Integer right)
{
    static const auto ceiling = std::numeric_limits<Integer>::max();
    return left > ceiling - right ? ceiling : left + right;
}

template <typename Integer,
    typename = std::enable_if<std::is_unsigned<Integer>::value>>
Integer floor_subtract(Integer left, Integer right)
{
    static const auto floor = std::numeric_limits<Integer>::min();
    return right >= left ? floor : left - right;
}

} // namespace libbitcoin

#endif
