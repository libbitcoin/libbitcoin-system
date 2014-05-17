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

#include <bitcoin/utility/external/uint256.h>
#include <bitcoin/types.hpp>

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
    BC_API void set_compact(uint32_t compact);
    BC_API uint32_t compact() const;
    BC_API void set_hash(const hash_digest& hash);

    // int64_t resolves to this in Satoshi's GetNextWorkRequired()
    BC_API void operator*=(uint32_t value);
    BC_API void operator/=(uint32_t value);
private:
    friend bool operator>(
        const hash_number& number_a, const hash_number& number_b);
    friend bool operator<=(
        const hash_number& number_a, const hash_number& number_b);

    uint256 hash_;
};

BC_API bool operator>(const hash_number& number_a, const hash_number& number_b);
BC_API bool operator<=(const hash_number& number_a, const hash_number& number_b);

} // namespace libbitcoin

#endif

