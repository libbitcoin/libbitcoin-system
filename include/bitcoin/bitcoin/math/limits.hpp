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
#include <bitcoin/bitcoin/constants.hpp>

namespace libbitcoin {

// This is a big ugly but it avoids exposing numeric_limits to headers.

inline uint64_t ceiling_add(uint64_t left, uint64_t right)
{
    static const auto ceiling = max_uint64;
    return left > ceiling - right ? ceiling : left + right;
}

inline uint64_t ceiling_add(uint32_t left, uint32_t right)
{
    static const auto ceiling = max_uint32;
    return left > ceiling - right ? ceiling : left + right;
}

template <typename Integer,
    typename = std::enable_if<std::is_unsigned<Integer>::value>>
Integer floor_subtract(Integer left, Integer right)
{
    return right >= left ? 0 : left - right;
}

} // namespace libbitcoin

#endif
