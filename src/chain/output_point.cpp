/**
 * Copyright (c) 2011-2016 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/chain/output_point.hpp>

#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/chain/point.hpp>

namespace libbitcoin {
namespace chain {

const uint64_t output_point::not_found = max_uint64;

output_point::output_point()
  : cache({ not_found, {} }),
    spent_(false)
{
}

output_point::output_point(const output_point& other)
  : point(other),
    cache(other.cache),
    spent_(other.spent_)
{
}

output_point::output_point(const chain::point& value)
  : output_point(value, {})
{
}

output_point::output_point(const hash_digest& hash, uint32_t index)
  : output_point({ hash, index }, {})
{
}

output_point::output_point(const chain::point& value,
    const chain::output& cache)
  : point(value),
    cache(cache),
    spent_(false)
{
}

output_point::output_point(const hash_digest& hash, uint32_t index,
    const chain::output& cache)
  : output_point({ hash, index }, cache)
{
}

void output_point::reset()
{
    spent_ = false;
    cache.value = not_found;
    cache.script.reset();
    static_cast<point>(*this).reset();
}

bool output_point::spent()
{
    return spent_;
}

void output_point::set_spent()
{
    // Must reset to revert to "unspent".
    spent_ = true;
}

bool output_point::operator==(const output_point& other) const
{
    // Cash is not considered in comparisons.
    return static_cast<point>(*this) == static_cast<point>(other);
}

bool output_point::operator!=(const output_point& other) const
{
    // Cash is not considered in comparisons.
    return !(*this == other);
}

} // namespace chain
} // namespace libbitcoin
