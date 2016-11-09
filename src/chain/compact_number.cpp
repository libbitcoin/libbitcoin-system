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

#include <bitcoin/bitcoin/math/uint256.hpp>

namespace libbitcoin {
namespace chain {

// [exponent:8 | sign:1 | mantissa:31]
static constexpr uint32_t exp_byte = 0xff000000;
static constexpr uint32_t sign_bit = 0x00800000;
static constexpr uint32_t mantissa_max = ~(exp_byte | sign_bit);
static constexpr uint32_t mantissa_bits = (sizeof(uint32_t) - 1) * 8;

// overflow
static constexpr uint32_t two_byte = 0x0000ffff;
static constexpr uint32_t one_byte = 0x000000ff;
static constexpr uint32_t non_byte = 0x00000000;

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

inline bool is_overflow(uint32_t exponent, uint32_t shifted)
{
    return
        (shifted > two_byte && exponent > (32 + 0)) ||
        (shifted > one_byte && exponent > (32 + 1)) ||
        (shifted > non_byte && exponent > (32 + 2));
}

inline uint32_t round_up(uint32_t bits)
{
    return (bits + 7) / 8;
}

inline uint32_t shift_low(uint32_t exponent)
{
    BITCOIN_ASSERT(exponent <= 3);
    return  8 * (3u - exponent);
}

inline uint32_t shift_high(uint32_t exponent)
{
    BITCOIN_ASSERT(exponent > 3);
    return  8 * (exponent - 3);
}

////inline uint32_t sign(uint32_t mantissa, bool negative)
////{
////    return (negative && mantissa != 0) ? sign_bit : 0;
////}

inline uint64_t lower64(const uint256_t& big)
{
    return big.word(0) | (static_cast<uint64_t>(big.word(1)) << 32);
}

// Constructors
//-----------------------------------------------------------------------------

compact_number::compact_number(uint32_t compact)
{
    valid_ = from_compact(big_, compact);
    normal_ = to_compact(big_ /*, false*/);
}

compact_number::compact_number(const uint256_t& big)
  : valid_(true), big_(big)
{
    normal_ = to_compact(big_ /*, false*/);
}

////bool compact_number::is_negative() const
////{
////    return is_negated(compact_);
////}

bool compact_number::is_overflowed() const
{
    return !valid_;
}

uint32_t compact_number::normal() const
{
    return normal_;
}

compact_number::operator const uint256_t&() const
{
    return big_;
}

// Returns false on overflow, negative is converted to zero.
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

    // Shift off the mantissa (and dead sign bit) to get the exponent byte.
    const auto exponent = compact >> mantissa_bits;

    // Mask off the exponent byte and the sign bit to get the mantissa.
    auto mantissa = compact & mantissa_max;

    // Shift the mantissa into the big number.
    if (exponent <= 3)
    {
        mantissa >>= shift_low(exponent);
        out = mantissa;
    }
    else 
    {
        out = mantissa;
        out <<= shift_high(exponent);
    }

    // Note that the mantissa has been shifed in the low condition above.
    // There is no consensus expectation of a value in the case of overflow.
    if (is_overflow(exponent, mantissa))
    {
        out = 0;
        return false;
    }

    return true;
}

uint32_t compact_number::to_compact(const uint256_t& big /*, bool negative*/)
{
    // The bits() range is [0..256], round to the next byte boundary.
    auto exponent = round_up(big.bits());

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
        mantissa >>= 8;
        exponent++;
    }

    BITCOIN_ASSERT_MSG((exponent & first_byte_mask) == 0, "size exceess");
    BITCOIN_ASSERT_MSG((mantissa & mantissa_mask) == 0, "value exceess");

    // Assemble the compact notation.
    return (exponent << mantissa_bits) | 0 /*sign(mantissa, negative)*/ | mantissa;
}

} // namespace chain
} // namespace libbitcoin
