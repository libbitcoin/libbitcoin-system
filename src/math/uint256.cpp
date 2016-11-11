// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <bitcoin/bitcoin/math/uint256.hpp>

#include <cstring>
#include <stdexcept>
#include <utility>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/endian.hpp>

namespace libbitcoin {

// The number of bits in a word of the big number buffer.
static const uint32_t word_bits = sizeof(uint32_t) * 8;

// Constructors.
//-----------------------------------------------------------------------------

uint256_t::uint256_t()
{
    words_.fill(0);
}

uint256_t::uint256_t(uint256_t&& other)
{
    std::swap(words_, other.words_);
}

uint256_t::uint256_t(const uint256_t& other)
{
    words_ = other.words_;
}

uint256_t::uint256_t(const hash_digest& hash)
{
    auto offset = hash.begin();
    for (auto& word: words_)
    {
        word = from_little_endian_unsafe<uint32_t>(offset);
        offset += sizeof(uint32_t);
    }
}

uint256_t::uint256_t(uint32_t value)
{
    words_.fill(0);
    words_[0] = value;
}

// Properties.
//-----------------------------------------------------------------------------

// Determine the logical bit length of the big number [0..256].
size_t uint256_t::bit_length() const
{
    auto word = static_cast<size_t>(words_.size() - 1);
    for (auto it = words_.rbegin(); it != words_.rend(); ++it, --word)
    {
        if (*it != 0)
        {
            size_t bit = word_bits - 1;
            for (; (bit != 0) && (*it & (1 << bit)) == 0; --bit);
            return word_bits * word + bit + 1;
        }
    }

    return 0;
}

// Determine the logical byte length of the big number [0..32].
size_t uint256_t::byte_length() const
{
    return (bit_length() + 7) / 8;
}

// This is used to efficiently generate a compact number (otherwise shift).
uint64_t uint256_t::operator[](size_t index) const
{
    BITCOIN_ASSERT_MSG(index < 4, "index out of range");

    return
        (static_cast<uint64_t>(words_[index + 0]) << 0 * word_bits) |
        (static_cast<uint64_t>(words_[index + 1]) << 1 * word_bits);
}

// Helpers.
//-----------------------------------------------------------------------------

bool uint256_t::equals(uint32_t value) const
{
    if (words_.front() != value)
        return false;

    const auto is_zero = [](uint32_t word)
    {
        return word == 0;
    };

    return std::all_of(words_.begin() + 1, words_.end(), is_zero);
}

int uint256_t::compare(const uint256_t& other) const
{
    // Reverse loop with zero lower bound requires signed index.
    const auto last = static_cast<int32_t>(words_.size() - 1);

    for (auto i = last; i >= 0; i--)
    {
        if (words_[i] < other.words_[i])
            return -1;

        if (words_[i] > other.words_[i])
            return 1;
    }

    return 0;
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

bool uint256_t::operator!=(const uint256_t& other) const
{
    return compare(other) != 0;
}

uint256_t uint256_t::operator~() const
{
    uint256_t result;
    for (auto i = 0; i < words_.size(); ++i)
        result.words_[i] = ~words_[i];

    return result;
}

// overflows
uint256_t uint256_t::operator-() const
{
    uint256_t result;
    for (auto i = 0; i < words_.size(); ++i)
        result.words_[i] = ~words_[i];

    return ++result;
}

uint256_t uint256_t::operator>>(uint32_t shift) const
{
    return uint256_t(*this) >>= shift;
}

// overflows
uint256_t uint256_t::operator+(const uint256_t& other) const
{
    uint256_t result(*this);
    result += other;
    return result;
}

uint256_t uint256_t::operator/(const uint256_t& other) const
{
    uint256_t result(*this);
    result /= other;
    return result;
}

// overflows
uint256_t& uint256_t::operator++()
{
    for (auto it = words_.begin(); it != words_.end() - 1; ++it)
        if (++(*it) != 0)
            break;

    return *this;
}

uint256_t& uint256_t::operator=(uint32_t value)
{
    words_.fill(0);
    words_[0] = value;
    return *this;
}

uint256_t& uint256_t::operator>>=(uint32_t shift)
{
    const auto value = shift / word_bits;
    const auto remainder = shift % word_bits;
    uint256_t copy(*this);
    words_.fill(0);

    for (size_t i = 0; i < words_.size(); ++i)
    {
        if (i >= value + 1 && remainder != 0)
            words_[i - value - 1] |= copy.words_[i] << (word_bits - remainder);

        if (i >= value + 0)
            words_[i - value - 0] |= copy.words_[i] >> remainder;
    }

    return *this;
}

uint256_t& uint256_t::operator<<=(uint32_t shift)
{
    const auto value = shift / word_bits;
    const auto remainder = shift % word_bits;
    uint256_t copy(*this);
    words_.fill(0);

    for (size_t i = 0; i < words_.size(); ++i)
    {
        if (i + value + 1 < words_.size() && remainder != 0)
            words_[i + value + 1] |= copy.words_[i] >> (word_bits - remainder);

        if (i + value + 0 < words_.size())
            words_[i + value + 0] |= copy.words_[i] << remainder;
    }

    return *this;
}

// overflows
uint256_t& uint256_t::operator*=(uint32_t value)
{
    uint64_t carry = 0;
    for (auto i = 0; i < words_.size(); ++i)
    {
        // The reason we use 32 bit words is so that we can take advantage of
        // integral addition and multiplication while capturing the overflow.
        auto product = carry + (uint64_t)value * words_[i];
        words_[i] = static_cast<uint32_t>(product);
        carry = product >> word_bits;
    }

    // carry is the overflow.
    return *this;
}

uint256_t& uint256_t::operator/=(uint32_t value)
{
    // This results in an extra copy but simplifies reuse.
    return (*this) /= uint256_t(value);
}

// overflows
uint256_t& uint256_t::operator+=(const uint256_t& other)
{
    uint64_t carry = 0;
    for (auto i = 0; i < words_.size(); ++i)
    {
        // The reason we use 32 bit words is so that we can take advantage of
        // integral addition and multiplication while capturing the overflow.
        auto sum = carry + (uint64_t)words_[i] + other.words_[i];
        words_[i] = static_cast<uint32_t>(sum);
        carry = sum >> word_bits;
    }

    // carry is the overflow.
    return *this;
}

// underflows
uint256_t& uint256_t::operator-=(const uint256_t& other)
{
    *this += -other;
    return *this;
}

uint256_t& uint256_t::operator/=(const uint256_t& other)
{
    // Make a copy, so we can shift.
    uint256_t divisor(other);
    auto divisor_bits = divisor.bit_length();

    // Make a copy, so we can subtract.
    uint256_t dividend(*this);
    auto dividend_bits = dividend.bit_length();

    // Initialize the quotient.
    words_.fill(0);

    if (divisor_bits == 0)
        throw std::overflow_error("division by zero");

    // The result is zero if the divisor exceeds the dividend.
    if (divisor_bits > dividend_bits)
        return *this;

    // The number of bits to shift (both values are limited to 256).
    const auto difference = static_cast<int32_t>(dividend_bits - divisor_bits);

    // Shift so that divisor and dividend align.
    divisor <<= difference;

    // Reverse loop with zero lower bound requires signed index.
    for (auto shift = difference; shift >= 0; --shift, divisor >>= 1)
    {
        if (dividend >= divisor)
        {
            // Set a bit of the result.
            words_[shift / word_bits] |= (1 << (shift & (word_bits - 1)));

            // Subtract out one instance of the divisor.
            dividend -= divisor;
        }
    }

    // The dividend contains the remainder.
    return *this;
}

} // namespace libbitcoin
