// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <bitcoin/bitcoin/math/uint256.hpp>

#include <cstring>
#include <stdexcept>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>

namespace libbitcoin {

compact_number::compact_number(uint32_t value)
  : value_(value)
{
}

uint32_t compact_number::value() const
{
return value_;
}

// Constructors.
//-----------------------------------------------------------------------------

uint256_t::uint256_t()
  : overflow_(false)
{
    std::memset(buffer_, 0, sizeof(buffer_));
}

uint256_t::uint256_t(uint256_t&& other)
  : overflow_(other.overflow_)
{
    std::memcpy(buffer_, other.buffer_, sizeof(buffer_));
}

uint256_t::uint256_t(const uint256_t& other)
  : overflow_(other.overflow_)
{
    std::memcpy(buffer_, other.buffer_, sizeof(buffer_));
}

// is_valid_proof_of_work
uint256_t::uint256_t(hash_digest&& hash)
  : overflow_(false)
{
    // BUGBUG: this appears to be an endianness bug.
    // TODO: take advantage of the move argument to populate the buffer.
    std::memcpy(buffer_, &hash[0], sizeof(buffer_));
}

uint256_t::uint256_t(const hash_digest& hash)
  : overflow_(false)
{
    // BUGBUG: this appears to be an endianness bug.
    std::memcpy(buffer_, &hash[0], sizeof(buffer_));
}

uint256_t::uint256_t(const compact_number& compact)
  : overflow_(!from_compact(*this, compact))
{
}

uint256_t::uint256_t(uint32_t value)
  : overflow_(false)
{
    std::memset(buffer_, 0, sizeof(buffer_));
    buffer_[0] = value;
}

// De/serialization.
//-----------------------------------------------------------------------------

// false if negative or overflow
bool uint256_t::from_compact(uint256_t& out, const compact_number& number)
{
    const uint32_t compact = number.value();
    int32_t size = compact >> 24;
    uint32_t word = compact & 0x007fffff;

    if (size <= 3)
    {
        word >>= 8 * (3 - size);
        out = word;
    }
    else 
    {
        out = word;
        out <<= 8 * (size - 3);
    }

    // zero
    if (word == 0)
        return true;

    // negative
    if ((compact & 0x00800000) != 0)
        return false;

    // overflow
    if ((word > 0x0000ffff && size > (word_bits_ + 0)) ||
        (word > 0x000000ff && size > (word_bits_ + 1)) ||
        (word > 0x00000000 && size > (word_bits_ + 2)))
        return false;

    // positive
    return true;
}

// non-negative only
compact_number uint256_t::to_compact() const
{
    uint32_t compact = 0;
    int32_t size = (bits() + 7) / 8;

    if (size <= 3)
    {
        auto double_word = (buffer_[0] | (uint64_t)buffer_[1] << word_bits_);
        compact = (uint32_t)(double_word << 8 * (3 - size));
    }
    else
    {
        uint256_t number = (*this >> 8 * (size - 3));
        compact = number.buffer_[0];
    }

    // The 0x00800000 bit denotes the sign. Thus, if it is already set, divide
    // the mantissa by 256 and increase the exponent.
    if (compact & 0x00800000)
    {
        compact >>= 8;
        size++;
    }

    BITCOIN_ASSERT((compact & ~0x007fffff) == 0);
    BITCOIN_ASSERT(size < full_bits_);

    compact |= size << 24;
    ////static const auto negative = false;
    ////compact |= (negative && (compact & 0x007fffff) ? 0x00800000 : 0);
    return{ compact };
}

// Properties.
//-----------------------------------------------------------------------------

uint32_t uint256_t::bits() const
{
    for (int32_t i = width_ - 1; i >= 0; i--)
    {
        if (buffer_[i] != 0)
        {
            for (int bits = word_bits_ - 1; bits > 0; bits--)
            {
                if ((buffer_[i] & 1 << bits) != 0)
                    return word_bits_ * i + bits + 1;
            }

            return word_bits_ * i + 1;
        }
    }

    return 0;
}

bool uint256_t::overflow() const
{
    return overflow_;
}

// Helpers.
//-----------------------------------------------------------------------------

bool uint256_t::equals(uint32_t value) const
{
    if (buffer_[0] != value)
        return false;

    const auto is_zero = [](uint32_t value) { return value == 0; };
    return std::all_of(&buffer_[1], &buffer_[width_], is_zero);
}

int uint256_t::compare(const uint256_t& other) const
{
    return std::memcmp(buffer_, other.buffer_, width_);
}

// Operators.
//-----------------------------------------------------------------------------

bool uint256_t::operator>(const uint256_t& other) const
{
    return compare(other) > 0;
}

bool uint256_t::operator<(const uint256_t& other) const
{
    return compare(other) < 0;
}

bool uint256_t::operator>=(const uint256_t& other) const
{
    return compare(other) >= 0;
}

bool uint256_t::operator<=(const uint256_t& other) const
{
    return compare(other) <= 0;
}

bool uint256_t::operator==(const uint256_t& other) const
{
    return compare(other) == 0;
}

const uint256_t uint256_t::operator~() const
{
    uint256_t result;

    for (int i = 0; i < width_; ++i)
        result.buffer_[i] = ~buffer_[i];

    return result;
}

const uint256_t uint256_t::operator-() const
{
    uint256_t result;

    for (int i = 0; i < width_; ++i)
        result.buffer_[i] = ~buffer_[i];

    return ++result;
}

const uint256_t uint256_t::operator>>(int shift) const
{
    return uint256_t(*this) >>= shift;
}

const uint256_t uint256_t::operator+(const uint256_t& other) const
{
    uint256_t result;
    result += other;
    return result;
}

const uint256_t uint256_t::operator/(const uint256_t& other) const
{
    uint256_t result;
    result /= other;
    return result;
}

uint256_t& uint256_t::operator++()
{
    for (int32_t i = 0; ++buffer_[i] == 0 && i < width_ - 1; ++i);
    return *this;
}

uint256_t& uint256_t::operator<<=(int32_t shift)
{
    std::memset(buffer_, 0, sizeof(buffer_));
    const int32_t quotient = shift / word_bits_;
    const int32_t remainder = shift % word_bits_;
    uint256_t copy(*this);

    for (int32_t i = 0; i < width_; ++i)
    {
        if (i + quotient + 1 < width_ && remainder != 0)
            buffer_[i + quotient + 1] |= 
                (copy.buffer_[i] >> (word_bits_ - remainder));

        if (i + quotient < width_)
            buffer_[i + quotient] |= 
                (copy.buffer_[i] << remainder);
    }

    return *this;
}

uint256_t& uint256_t::operator>>=(int32_t shift)
{
    std::memset(buffer_, 0, sizeof(buffer_));

    uint256_t copy(*this);
    const int32_t quotient = shift / word_bits_;
    const int32_t remainder = shift % word_bits_;

    for (int32_t i = 0; i < width_; ++i)
    {
        if (i - quotient - 1 >= 0 && remainder != 0)
            buffer_[i - quotient - 1] |=
                (copy.buffer_[i] << (word_bits_ - remainder));

        if (i - quotient >= 0)
            buffer_[i - quotient] |=
                (copy.buffer_[i] >> remainder);
    }

    return *this;
}

uint256_t& uint256_t::operator=(uint32_t value)
{
    std::memset(buffer_, 0, sizeof(buffer_));
    buffer_[0] = value;
    return *this;
}

uint256_t& uint256_t::operator*=(uint32_t value)
{
    uint64_t carry = 0;

    for (int32_t i = 0; i < width_; ++i)
    {
        uint64_t product = carry + (uint64_t)value * buffer_[i];
        buffer_[i] = (uint32_t)(product & 0xffffffff);
        carry = product >> word_bits_;
    }

    return *this;
}

uint256_t& uint256_t::operator/=(uint32_t value)
{
    // This results in an extra construction.
    return (*this) /= uint256_t(value);
}

uint256_t& uint256_t::operator+=(const uint256_t& other)
{
    uint64_t carry = 0;

    for (int i = 0; i < width_; ++i)
    {
        uint64_t sum = carry + buffer_[i] + other.buffer_[i];
        buffer_[i] = (uint32_t)(sum & 0xffffffff);
        carry = sum >> word_bits_;
    }

    return *this;
}

uint256_t& uint256_t::operator-=(const uint256_t& other)
{
    *this += -other;
    return *this;
}

uint256_t& uint256_t::operator/=(const uint256_t& other)
{
    // The quotient.
    std::memset(buffer_, 0, sizeof(buffer_));

    // Make a copy, so we can shift.
    uint256_t divisor(other);
    uint32_t divisor_bits = divisor.bits();

    // Make a copy, so we can subtract.
    uint256_t dividend(*this);
    uint32_t dividend_bits = dividend.bits();

    if (divisor_bits == 0)
        throw std::overflow_error("division by zero");

    // The result is certainly 0.
    if (divisor_bits > dividend_bits)
        return *this;

    int32_t shift = dividend_bits - divisor_bits;

    // Shift so that divisor and dividend align.
    divisor <<= shift;

    while (shift >= 0)
    {
        if (dividend >= divisor)
        {
            dividend -= divisor;

            // Set a bit of the result.
            buffer_[shift / word_bits_] |= (1 << (shift & (word_bits_ - 1)));
        }

        // Shift back.
        divisor >>= 1;
        shift--;
    }

    // The dividend now contains the remainder of the division.
    return *this;
}

} // namespace libbitcoin
