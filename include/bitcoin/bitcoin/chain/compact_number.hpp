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
#ifndef LIBBITCOIN_COMPACT_NUMBER_HPP
#define LIBBITCOIN_COMPACT_NUMBER_HPP

#include <cstdint>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/uint256.hpp>

namespace libbitcoin {
namespace chain {

/// A signed but zero-floored scientific notation in 32 bits.
class BC_API compact_number
{
public:
    /// Construct a normal form compact number from a 32 bit compact number.
    explicit compact_number(uint32_t compact);

    /// Construct a normal form compact number from a 256 bit number
    explicit compact_number(const uint256_t& big);

    /// Move constructor.
    compact_number(compact_number&& other);

    /// Copy constructor.
    compact_number(const compact_number& other);

    /// True if construction overflowed.
    bool is_overflowed() const;

    /// Consensus-normalized compact number value.
    /// This is derived from the construction parameter.
    uint32_t normal() const;

    /// Big number that the compact number represents.
    /// This is either saved or generated from the construction parameter.
    operator const uint256_t&() const;

private:
    static bool from_compact(uint256_t& out, uint32_t compact);
    static uint32_t from_big(const uint256_t& big);

    uint256_t big_;
    uint32_t normal_;
    bool overflowed_;
};

} // namespace chain
} // namespace libbitcoin

#endif
