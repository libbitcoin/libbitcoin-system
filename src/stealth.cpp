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
#include <bitcoin/stealth.hpp>

#include <bitcoin/utility/assert.hpp>

namespace libbitcoin {

uint8_t bitfield_mask(uint8_t bitfield_byte, uint8_t number_bits)
{
    BITCOIN_ASSERT(number_bits < 8);
    // https://wiki.unsystem.net/index.php/DarkWallet/Stealth#Comparing_prefixes
    uint8_t mask = 0xff & ((1 << number_bits) - 1);
    return bitfield_byte & mask;
}

bool stealth_match(const stealth_prefix& prefix, uint8_t* raw_bitfield)
{
    BITCOIN_ASSERT(prefix.number_bits <= sizeof(prefix.bitfield) * 8);
    // Perform comparison of byte.
    const uint8_t* prefix_bitfield_byte =
        reinterpret_cast<const uint8_t*>(&prefix.bitfield);
    // Byte by byte comparison until we need to compare on the bit level.
    uint8_t compare_bits = prefix.number_bits;
    for (; compare_bits >= 8; compare_bits -= 8)
    {
        if (*prefix_bitfield_byte != *raw_bitfield)
            return false;
        ++prefix_bitfield_byte;
        ++raw_bitfield;
    }
    if (compare_bits == 0)
        return true;
    // Perform bit comparison for remaining bits.
    // Mask of the prefix provided.
    uint8_t masked_a = bitfield_mask(*prefix_bitfield_byte, compare_bits);
    // Mask of the value's bitfield.
    uint8_t masked_b = bitfield_mask(*raw_bitfield, compare_bits);
    if (masked_a != masked_b)
        return false;
    return true;
}

} // namespace libbitcoin

