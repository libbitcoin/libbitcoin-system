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

// The metadata properties are not configured for initializer syntax.

output_point::output_point()
  : cache({ not_found, {} }),
    spent(false),
    confirmed(false)
{
}

output_point::output_point(const output_point& other)
  : point(other),
    cache(other.cache),
    spent(other.spent),
    confirmed(other.confirmed)
{
}

output_point::output_point(const chain::point& value)
  : point(value),
    cache({ not_found, {} }),
    spent(false),
    confirmed(false)
{
}

output_point::output_point(const hash_digest& hash, uint32_t index)
  : point({ hash, index }),
    cache({ not_found, {} }),
    spent(false),
    confirmed(false)
{
}

void output_point::reset()
{
    spent = false;
    confirmed = false;
    cache.value = not_found;
    cache.script.reset();
    static_cast<point>(*this).reset();
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
