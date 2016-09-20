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
#ifndef LIBBITCOIN_CHAIN_OUTPUT_POINT_HPP
#define LIBBITCOIN_CHAIN_OUTPUT_POINT_HPP

#include <cstddef>
#include <cstdint>
#include <vector>
#include <bitcoin/bitcoin/chain/output.hpp>
#include <bitcoin/bitcoin/chain/point.hpp>
#include <bitcoin/bitcoin/define.hpp>

namespace libbitcoin {
namespace chain {

class BC_API output_point
  : public point
{
public:
    /// This is a sentinel used in .height to indicate not coinbase.
    static const size_t not_coinbase;

    output_point();
    output_point(const output_point& other);
    output_point(const chain::point& value);
    output_point(const hash_digest& hash, uint32_t index);

    // TODO: implement move argument constructors.
    ////output_point(output_point&& other);
    ////output_point(chain::point&& point);
    ////output_point(hash_digest&& hash, uint32_t index);

    bool operator==(const output_point& other) const;
    bool operator!=(const output_point& other) const;

    void reset();

    /// Using .height and the given target height, determine spend maturity.
    bool is_mature(size_t target_height) const;

    // These fields do not participate in serialization or comparison.

    /// An output is spent if a valid transaction has a valid claim on it.
    /// When validating blocks only long chain blocks can have a claim.
    /// When validating a memory pool tx another mempool tx can have a claim.
    bool spent;

    /// A spend is confirmed if the spender is long chain (not memory pool).
    bool confirmed;

    /// The height of a coinbase output is necessary in determining maturity.
    /// This should be set to not_coinbase if the output is not coinbase.
    size_t height;

    /// The output cache contains the output referenced by the input point.
    /// If the cache.value is not_found then the output has not been found.
    chain::output cache;
};

struct BC_API points_info
{
    output_point::list points;
    uint64_t change;
};

struct BC_API output_info
{
    typedef std::vector<output_info> list;

    output_point point;
    uint64_t value;
};

} // namespace chain
} // namespace libbitcoin

#endif
