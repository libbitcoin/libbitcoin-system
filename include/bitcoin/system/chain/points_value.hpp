/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_POINTS_VALUE_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_POINTS_VALUE_HPP

#include <numeric>
#include <cstdint>
#include <bitcoin/system/chain/point_value.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/limits.hpp>

namespace libbitcoin {
namespace system {
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
} // namespace system
} // namespace libbitcoin

#endif
