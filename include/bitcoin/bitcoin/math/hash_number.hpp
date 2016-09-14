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
#ifndef LIBBITCOIN_HASH_NUMBER_HPP
#define LIBBITCOIN_HASH_NUMBER_HPP

#include <cstddef>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/math/uint256.hpp>

namespace libbitcoin {

/**
 * Represents a target hash or proof of work sum.
 * Used for block proof of works to calculate whether they reach
 * a certain target or which chain is longest.
 */
class BC_API hash_number
{
public:
    /// Constructors.
    hash_number();
    hash_number(uint64_t value);
    hash_number(const hash_digest& value);

    /// Properties.
    hash_digest hash() const;
    uint32_t compact() const;
    bool set_compact(uint32_t value);

    /// Copy Operators.
    hash_number operator~() const;
    hash_number operator<<(uint32_t value);
    hash_number operator/(const hash_number& value);
    hash_number operator+(const hash_number& value);

    /// Change Operators.
    hash_number& operator<<=(uint32_t value);
    hash_number& operator*=(uint32_t value);
    hash_number& operator/=(uint32_t value);
    hash_number& operator/=(const hash_number& value);
    hash_number& operator+=(const hash_number& value);

    /// Comparison Operators.
    bool operator==(uint64_t value) const;
    bool operator!=(uint64_t value) const;
    bool operator>(const hash_number& value) const;
    bool operator<(const hash_number& value) const;
    bool operator>=(const hash_number& value) const;
    bool operator<=(const hash_number& value) const;

private:
    uint256_t hash_;
};

} // namespace libbitcoin

#endif
