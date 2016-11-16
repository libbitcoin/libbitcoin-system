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

////#include <bitcoin/bitcoin/math/uint256.hpp>
////
////#include <string>
////#include <stdexcept>
////#include <string>
////#include <utility>
////#include <boost/program_options.hpp>
////#include <bitcoin/bitcoin/formats/base_16.hpp>
////#include <bitcoin/bitcoin/math/hash.hpp>
////#include <bitcoin/bitcoin/utility/assert.hpp>
////#include <bitcoin/bitcoin/utility/endian.hpp>
////
////namespace libbitcoin {
////
////// The number of bits in a word of the big number buffer.
////static const uint32_t word_bits = sizeof(uint32_t) * 8;
////
////// Constructors.
//////-----------------------------------------------------------------------------
////
////uint256_t::uint256_t()
////{
////    words_.fill(0);
////}
////
////uint256_t::uint256_t(uint256_t&& other)
////{
////    std::swap(words_, other.words_);
////}
////
////uint256_t::uint256_t(const uint256_t& other)
////{
////    words_ = other.words_;
////}
////
////uint256_t::uint256_t(const hash_digest& hash)
////{
////    const auto size = sizeof(uint32_t);
////    words_[0] = from_little_endian_unsafe<uint32_t>(&hash[0 * size]);
////    words_[1] = from_little_endian_unsafe<uint32_t>(&hash[1 * size]);
////    words_[2] = from_little_endian_unsafe<uint32_t>(&hash[2 * size]);
////    words_[3] = from_little_endian_unsafe<uint32_t>(&hash[3 * size]);
////    words_[4] = from_little_endian_unsafe<uint32_t>(&hash[4 * size]);
////    words_[5] = from_little_endian_unsafe<uint32_t>(&hash[5 * size]);
////    words_[6] = from_little_endian_unsafe<uint32_t>(&hash[6 * size]);
////    words_[7] = from_little_endian_unsafe<uint32_t>(&hash[7 * size]);
////}
////
////uint256_t::uint256_t(uint32_t value)
////{
////    words_.fill(0);
////    words_[0] = value;
////}
////
////// Properties.
//////-----------------------------------------------------------------------------
////
////inline size_t high_bit(uint32_t value)
////{
////    size_t bit = 0;
////    for (; value != 0 && bit < word_bits; ++bit, value >>= 1);
////    return bit;
////}
////
////// Determine the logical bit length of the big number [0..256].
////size_t uint256_t::bit_length() const
////{
////    // Reverse loop with zero lower bound requires signed index.
////    const auto last = static_cast<int8_t>(words_.size() - 1);
////
////    for (auto i = last; i >= 0; --i)
////        if (words_[i] != 0)
////            return i * word_bits + high_bit(words_[i]);
////
////    return 0;
////}
////
////// Determine the logical byte length of the big number [0..32].
////size_t uint256_t::byte_length() const
////{
////    return (bit_length() + 7) / 8;
////}
////
////hash_digest uint256_t::hash() const
////{
////    hash_digest hash;
////
////    /* safe to ignore */ build_array<hash_size>
////    (
////        hash,
////        {
////            to_little_endian(words_[0]),
////            to_little_endian(words_[1]),
////            to_little_endian(words_[2]),
////            to_little_endian(words_[3]),
////            to_little_endian(words_[4]),
////            to_little_endian(words_[5]),
////            to_little_endian(words_[6]),
////            to_little_endian(words_[7])
////        }
////    );
////
////    return hash;
////}
////
////// This is used to efficiently generate a compact number (otherwise shift).
////uint64_t uint256_t::operator[](size_t index) const
////{
////    BITCOIN_ASSERT_MSG(index < 4, "index out of range");
////
////    const auto offset = 2 * index;
////    return
////        (static_cast<uint64_t>(words_[offset + 0]) << 0 * word_bits) |
////        (static_cast<uint64_t>(words_[offset + 1]) << 1 * word_bits);
////}
////
////// Helpers.
//////-----------------------------------------------------------------------------
////
////bool uint256_t::equals(uint32_t value) const
////{
////    return
////        words_[0] == value &&
////        words_[1] == 0 &&
////        words_[2] == 0 &&
////        words_[3] == 0 &&
////        words_[4] == 0 &&
////        words_[5] == 0 &&
////        words_[6] == 0 &&
////        words_[7] == 0;
////}
////
////int uint256_t::compare(const uint256_t& other) const
////{
////    // Reverse loop with zero lower bound requires signed index.
////    const auto last = static_cast<int8_t>(words_.size() - 1);
////
////    for (auto i = last; i >= 0; i--)
////    {
////        if (words_[i] < other.words_[i])
////            return -1;
////
////        if (words_[i] > other.words_[i])
////            return 1;
////    }
////
////    return 0;
////}
////
////// Operators.
//////-----------------------------------------------------------------------------
////
////bool uint256_t::operator>(const uint256_t& other) const
////{
////    return compare(other) > 0;
////}
////
////bool uint256_t::operator<(const uint256_t& other) const
////{
////    return compare(other) < 0;
////}
////
////bool uint256_t::operator>=(const uint256_t& other) const
////{
////    return compare(other) >= 0;
////}
////
////bool uint256_t::operator<=(const uint256_t& other) const
////{
////    return compare(other) <= 0;
////}
////
////bool uint256_t::operator==(const uint256_t& other) const
////{
////    return compare(other) == 0;
////}
////
////bool uint256_t::operator!=(const uint256_t& other) const
////{
////    return compare(other) != 0;
////}
////
////uint256_t uint256_t::operator~() const
////{
////    uint256_t result;
////    result.words_[0] = ~words_[0];
////    result.words_[1] = ~words_[1];
////    result.words_[2] = ~words_[2];
////    result.words_[3] = ~words_[3];
////    result.words_[4] = ~words_[4];
////    result.words_[5] = ~words_[5];
////    result.words_[6] = ~words_[6];
////    result.words_[7] = ~words_[7];
////    return result;
////}
////
////uint256_t uint256_t::operator-() const
////{
////    // two's compliment
////    return ++(~(*this));
////}
////
////uint256_t uint256_t::operator>>(uint32_t shift) const
////{
////    return uint256_t(*this) >>= shift;
////}
////
////uint256_t uint256_t::operator+(const uint256_t& other) const
////{
////    uint256_t copy(*this);
////    copy += other;
////    return copy;
////}
////
////uint256_t uint256_t::operator/(const uint256_t& other) const
////{
////    uint256_t copy(*this);
////    copy /= other;
////    return copy;
////}
////
////uint256_t& uint256_t::operator++()
////{
////    for (auto& word: words_)
////        if (++word != 0)
////            break;
////
////    return *this;
////}
////
////uint256_t& uint256_t::operator=(uint32_t value)
////{
////    words_.fill(0);
////    words_[0] = value;
////    return *this;
////}
////
////uint256_t& uint256_t::operator>>=(uint32_t shift)
////{
////    const auto value = shift / word_bits;
////    const auto remainder = shift % word_bits;
////    uint256_t copy(*this);
////    words_.fill(0);
////
////    for (size_t i = 0; i < words_.size(); ++i)
////    {
////        if (i >= value + 1 && remainder != 0)
////            words_[i - value - 1] |= copy.words_[i] << (word_bits - remainder);
////
////        if (i >= value + 0)
////            words_[i - value - 0] |= copy.words_[i] >> remainder;
////    }
////
////    return *this;
////}
////
////uint256_t& uint256_t::operator<<=(uint32_t shift)
////{
////    const auto value = shift / word_bits;
////    const auto remainder = shift % word_bits;
////    uint256_t copy(*this);
////    words_.fill(0);
////
////    for (size_t i = 0; i < words_.size(); ++i)
////    {
////        if (i + value + 1 < words_.size() && remainder != 0)
////            words_[i + value + 1] |= copy.words_[i] >> (word_bits - remainder);
////
////        if (i + value + 0 < words_.size())
////            words_[i + value + 0] |= copy.words_[i] << remainder;
////    }
////
////    return *this;
////}
////
////uint256_t& uint256_t::operator*=(uint32_t value)
////{
////    uint64_t carry = 0;
////    for (auto i = 0; i < words_.size(); ++i)
////    {
////        auto product = carry + static_cast<uint64_t>(value) * words_[i];
////        words_[i] = static_cast<uint32_t>(product);
////        carry = product >> word_bits;
////    }
////
////    return *this;
////}
////
////uint256_t& uint256_t::operator/=(uint32_t value)
////{
////    return (*this) /= uint256_t(value);
////}
////
////uint256_t& uint256_t::operator+=(const uint256_t& other)
////{
////    uint64_t carry = 0;
////    for (auto i = 0; i < words_.size(); ++i)
////    {
////        auto sum = carry + words_[i] + other.words_[i];
////        words_[i] = static_cast<uint32_t>(sum);
////        carry = sum >> word_bits;
////    }
////
////    return *this;
////}
////
////uint256_t& uint256_t::operator-=(const uint256_t& other)
////{
////    *this += -other;
////    return *this;
////}
////
////uint256_t& uint256_t::operator/=(const uint256_t& other)
////{
////    // Make a copy, so we can shift.
////    uint256_t divisor(other);
////    auto divisor_bits = divisor.bit_length();
////
////    // Make a copy, so we can subtract.
////    uint256_t dividend(*this);
////    auto dividend_bits = dividend.bit_length();
////
////    // Initialize the quotient.
////    words_.fill(0);
////
////    if (divisor_bits == 0)
////        throw std::overflow_error("division by zero");
////
////    // The result is zero if the divisor exceeds the dividend.
////    if (divisor_bits > dividend_bits)
////        return *this;
////
////    // The number of bits to shift (both values are limited to 256).
////    const auto difference = static_cast<int32_t>(dividend_bits - divisor_bits);
////
////    // Shift so that divisor and dividend align.
////    divisor <<= difference;
////
////    // Reverse loop with zero lower bound requires signed index.
////    for (auto shift = difference; shift >= 0; --shift, divisor >>= 1)
////    {
////        if (dividend >= divisor)
////        {
////            // Set a bit of the result.
////            words_[shift / word_bits] |= (1 << (shift & (word_bits - 1)));
////
////            // Subtract out one instance of the divisor.
////            dividend -= divisor;
////        }
////    }
////
////    // The dividend contains the remainder.
////    return *this;
////}
////
////// Serialization.
//////-----------------------------------------------------------------------------
////
////std::istream& operator>>(std::istream& input, uint256_t& argument)
////{
////    std::string hexcode;
////    input >> hexcode;
////
////    hash_digest hash;
////
////    if (!decode_hash(hash, hexcode))
////    {
////        using namespace boost::program_options;
////        BOOST_THROW_EXCEPTION(invalid_option_value(hexcode));
////    }
////
////    return input;
////}
////
////std::ostream& operator<<(std::ostream& output, const uint256_t& argument)
////{
////    output << encode_hash(argument.hash());
////    return output;
////}
////
////} // namespace libbitcoin
