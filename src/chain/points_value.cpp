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
#include <numeric>
#include <cstdint>
#include <bitcoin/bitcoin/chain/point_value.hpp>
#include <bitcoin/bitcoin/chain/points_value.hpp>
#include <bitcoin/bitcoin/math/limits.hpp>

namespace libbitcoin {
namespace chain {

uint64_t points_value::value() const
{
    const auto sum = [](uint64_t total, const point_value& point)
    {
        return safe_add(total, point.value());
    };

    return std::accumulate(points.begin(), points.end(), uint64_t(0), sum);
}

} // namespace chain
} // namespace libbitcoin
