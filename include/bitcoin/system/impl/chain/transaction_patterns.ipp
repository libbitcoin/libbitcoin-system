/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_TRANSACTION_PATTERNS_IPP
#define LIBBITCOIN_SYSTEM_CHAIN_TRANSACTION_PATTERNS_IPP

#include <bitcoin/system/chain/enums/coverage.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace chain {
    
//*****************************************************************************
// CONSENSUS: Due to masking of bits 6/7 (8 is the anyone_can_pay flag), there
// possible 7 bit values that can set "single" and 4 others are 4 that can set 
// none, and yet all other values set "all".
//*****************************************************************************
constexpr coverage transaction::mask_sighash(uint8_t sighash_flags) NOEXCEPT
{
    switch (bit_and<uint8_t>(sighash_flags, coverage::mask))
    {
        case coverage::hash_single:
            return coverage::hash_single;
        case coverage::hash_none:
            return coverage::hash_none;
        default:
            return coverage::hash_all;
    }
}

constexpr bool transaction::is_anyone_can_pay(uint8_t sighash_flags) NOEXCEPT
{
    return get_right(sighash_flags, coverage::anyone_can_pay_bit);
}

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
