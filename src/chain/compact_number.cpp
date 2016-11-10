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
#include <bitcoin/bitcoin/chain/compact_number.hpp>

#include <cstdint>
#include <bitcoin/bitcoin/math/uint256.hpp>

namespace libbitcoin {
namespace chain {

// Bitcoin compact for represents a value in base 256 notation as follows:
// value = (-1^sign) * mantissa * 256^(exponent-3)

// [exponent:8 | sign:1 | mantissa:31]
static constexpr uint32_t exp_byte = 0xff000000;
static constexpr uint32_t sign_bit = 0x00800000;
static constexpr uint32_t mantissa_max = ~(exp_byte | sign_bit);
static constexpr uint32_t mantissa_bits = (sizeof(uint32_t) - 1) * 8;

// assertions
static constexpr uint32_t mantissa_mask = ~mantissa_max;
static constexpr uint32_t first_byte_mask = 0xffffff00;

// Inlines
//-----------------------------------------------------------------------------

inline bool is_negated(uint32_t compact)
{
    return (compact & sign_bit) != 0;
}

inline bool is_nonzero(uint32_t compact)
{
    return (compact & mantissa_max) != 0;
}

inline uint8_t log_256(uint32_t mantissa)
{
    BITCOIN_ASSERT_MSG(mantissa <= 0x00ffffff, "mantissa log256 is 4");

    return
        (mantissa > 0x0000ffff ? 3 :
        (mantissa > 0x000000ff ? 2 : 
        (mantissa > 0x00000000 ? 1 : 0)));
}

inline bool is_overflow(uint8_t exponent, uint32_t mantissa)
{
    // Overflow if exponent would shift the mantissa more than 32 bytes.
    return (mantissa > 0) && (exponent > 32 + 3 - log_256(mantissa));
}

inline uint32_t bits_to_exponent(uint32_t bits)
{
    // Round up to the nearest multiple of eight, because 256/8=32.
    return (bits + 7) / 8;
}

inline uint32_t shift_low(uint32_t exponent)
{
    BITCOIN_ASSERT(exponent <= 3);
    return  8 * (3 - exponent);
}

inline uint32_t shift_high(uint32_t exponent)
{
    BITCOIN_ASSERT(exponent > 3);
    return  8 * (exponent - 3);
}

inline uint64_t lower64(const uint256_t& big)
{
    const auto words = big.words();
    return words[0] | (static_cast<uint64_t>(words[1]) << 32);
}

// Constructors
//-----------------------------------------------------------------------------

compact_number::compact_number(uint32_t compact)
{
    overflowed_ = from_compact(big_, compact);
    normal_ = from_big(big_);
}

compact_number::compact_number(const uint256_t& value)
  : big_(value), overflowed_(true)
{
    normal_ = from_big(big_);
}

bool compact_number::is_overflowed() const
{
    return !overflowed_;
}

uint32_t compact_number::normal() const
{
    return normal_;
}

compact_number::operator const uint256_t&() const
{
    return big_;
}

// Returns false on overflow, negatives are converted to zero.
bool compact_number::from_compact(uint256_t& out, uint32_t compact)
{
    //*************************************************************************
    // CONSENSUS: The sign bit is not honored and is instead produces zero.
    // This results from having used a signed data structure for unsigned data.
    //*************************************************************************
    if (is_negated(compact))
    {
        out = 0;
        return true;
    }

    // Mask off the exponent byte and sign to get the mantissa.
    auto mantissa = compact & mantissa_max;

    // Shift off the mantissa and sign to get the exponent byte.
    const auto exponent = static_cast<uint8_t>(compact >> mantissa_bits);

    // Shift the mantissa into the big number.
    if (exponent <= 3)
    {
        mantissa >>= shift_low(exponent);
        out = mantissa;
    }
    else 
    {
        if (is_overflow(exponent, mantissa))
            return false;

        out = mantissa;
        out <<= shift_high(exponent);
    }

    return true;
}

uint32_t compact_number::from_big(const uint256_t& big)
{
    uint32_t exponent = bits_to_exponent(big.bits());
    uint32_t mantissa = 0;

    // Shift the big number significant digits into the mantissa.
    if (exponent <= 3)
        mantissa = static_cast<uint32_t>(lower64(big) << shift_low(exponent));
    else
        mantissa = static_cast<uint32_t>(lower64(big >> shift_high(exponent)));

    //*************************************************************************
    // CONSENSUS: Satoshi used a signed implementation to represent unsigned.
    // If the sign bit is set drop it from the mantissa into the exponent.
    // This is intended to prevent the appearance of a negative sign bit.
    //*************************************************************************
    if (is_negated(mantissa))
    {
        BITCOIN_ASSERT(exponent < 0xff);
        exponent++;
        mantissa >>= 8;
    }

    BITCOIN_ASSERT_MSG((exponent & first_byte_mask) == 0, "size exceess");
    BITCOIN_ASSERT_MSG((mantissa & mantissa_mask) == 0, "value exceess");

    // Assemble the compact notation.
    return (exponent << mantissa_bits) | mantissa;
}

} // namespace chain
} // namespace libbitcoin
