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

#include <cstddef>
#include <cstdint>
#include <utility>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/chain/point.hpp>

namespace libbitcoin {
namespace chain {

const size_t output_point::validation::not_specified = max_size_t;

// The validation properties are not configured for initializer syntax.

output_point::output_point()
{
}

output_point::output_point(const output_point& other)
  : point(other),
    validation(other.validation)
{
}

output_point::output_point(const point& value)
  : point(value)
{
}

output_point::output_point(const hash_digest& hash, uint32_t index)
  : point({ hash, index })
{
}

output_point::output_point(output_point&& other)
  : point(std::forward<point>(other)),
    validation(other.validation)
{
}

output_point::output_point(point&& value)
  : point(std::forward<chain::point>(value))
{
}

output_point::output_point(hash_digest&& hash, uint32_t index)
  : point({ hash, index })
{
}

output_point& output_point::operator=(output_point&& other)
{
    static_cast<point>(*this) = std::forward<point>(other);
    validation = other.validation;
    return *this;
}

output_point& output_point::operator=(const output_point& other)
{
    static_cast<point>(*this) = static_cast<point>(other);
    validation = other.validation;
    return *this;
}

void output_point::reset()
{
    point::reset();
}

// For tx pool validation target_height is that of the *next* block.
// For block validation target_height is that for which block is considered.
bool output_point::is_mature(size_t target_height) const
{
    if (validation.height == validation::not_specified)
        return true;

    // The (non-coinbase) outpoint refers to a coinbase output, measure depth.
    return ((target_height - 1) - validation.height) >= coinbase_maturity;
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
