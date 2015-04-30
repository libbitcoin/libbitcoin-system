/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
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
class hash_number
{
public:
    BC_API hash_number();
    BC_API hash_number(const uint64_t value);
    // Returns false if negative or overflowed.
    BC_API bool set_compact(uint32_t compact);
    BC_API uint32_t compact() const;
    BC_API void set_hash(const hash_digest& hash);
    BC_API hash_digest hash() const;

    BC_API const hash_number operator~() const;

    // int64_t resolves to this in Satoshi's GetNextWorkRequired()
    BC_API hash_number& operator*=(uint32_t value);
    BC_API hash_number& operator/=(uint32_t value);
    BC_API hash_number& operator<<=(uint32_t shift);

    BC_API hash_number& operator/=(const hash_number& number_b);
    BC_API hash_number& operator+=(const hash_number& number_b);

private:
    friend bool operator>(
        const hash_number& number_a, const hash_number& number_b);
    friend bool operator<=(
        const hash_number& number_a, const hash_number& number_b);
    friend const hash_number operator<<(
        const hash_number& number_a, int shift);
    friend const hash_number operator/(
        const hash_number& number_a, const hash_number& number_b);
    friend const hash_number operator+(
        const hash_number& number_a, const hash_number& number_b);
    friend bool operator==(
        const hash_number& number, uint64_t value);

    uint256_t hash_;
};

BC_API bool operator>(
    const hash_number& number_a, const hash_number& number_b);
BC_API bool operator<=(
    const hash_number& number_a, const hash_number& number_b);
BC_API const hash_number operator<<(
    const hash_number& number_a, int shift);
BC_API const hash_number operator/(
    const hash_number& number_a, const hash_number& number_b);
BC_API const hash_number operator+(
    const hash_number& number_a, const hash_number& number_b);
BC_API bool operator==(
    const hash_number& number, uint64_t value);

} // namespace libbitcoin

#endif
