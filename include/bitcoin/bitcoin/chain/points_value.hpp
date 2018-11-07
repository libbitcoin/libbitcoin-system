/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_CHAIN_POINTS_VALUE_HPP
#define LIBBITCOIN_CHAIN_POINTS_VALUE_HPP

#include <numeric>
#include <cstdint>
#include <bitcoin/bitcoin/chain/point_value.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/limits.hpp>

namespace libbitcoin {
namespace chain {

class BC_API points_value
{
public:
    /// A set of valued points.
    point_value::list points;

    /// Total value of the current set of points.
    uint64_t value() const;
};

} // namespace chain
} // namespace libbitcoin

#endif
