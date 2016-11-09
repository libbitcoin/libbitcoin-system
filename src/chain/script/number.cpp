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
#include <bitcoin/bitcoin/chain/script/number.hpp>

#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/math/limits.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>

namespace libbitcoin {
namespace chain {
    
// members
const uint8_t number::negative_1 = negative_mask | positive_1;
const uint8_t number::negative_0 = negative_mask | positive_0;
const uint8_t number::positive_0 = 0;
const uint8_t number::positive_1 = 1;
const uint8_t number::positive_2 = 2;
const uint8_t number::positive_3 = 3;
const uint8_t number::positive_4 = 4;
const uint8_t number::positive_5 = 5;
const uint8_t number::positive_6 = 6;
const uint8_t number::positive_7 = 7;
const uint8_t number::positive_8 = 8;
const uint8_t number::positive_9 = 9;
const uint8_t number::positive_10 = 10;
const uint8_t number::positive_11 = 11;
const uint8_t number::positive_12 = 12;
const uint8_t number::positive_13 = 13;
const uint8_t number::positive_14 = 14;
const uint8_t number::positive_15 = 15;
const uint8_t number::positive_16 = 16;
const uint8_t number::negative_mask = 0x80;

// locals
static constexpr uint64_t negative_bit = number::negative_mask;
static constexpr uint64_t unsigned_max_int64 = bc::max_int64;
static constexpr uint64_t absolute_min_int64 = bc::min_int64;

inline bool is_negative(const data_chunk& data)
{
    return (data.back() & number::negative_mask) != 0;
}

number::number()
  : number(0)
{
}

number::number(int64_t value)
  : value_(value)
{
}

// Properties
//-----------------------------------------------------------------------------

// The parameter is assumed to be LSB first.
bool number::set_data(const data_chunk& data, size_t max_size)
{
    if (data.size() > max_size)
        return false;

    value_ = 0;

    if (data.empty())
        return true;

    for (size_t i = 0; i != data.size(); ++i)
        value_ |= static_cast<int64_t>(data[i]) << 8 * i;

    if (is_negative(data))
    {
        const auto last_shift = 8 * (data.size() - 1);
        const auto mask = ~(negative_bit << last_shift);
        value_ = -1 * (static_cast<int64_t>(value_ & mask));
    }

    return true;
}

// The result is always LSB first.
data_chunk number::data() const
{
    if (value_ == 0)
        return{};

    data_chunk data;
    const bool negative = value_ < 0;
    uint64_t absolute = negative ? -value_ : value_;

    while (absolute != 0)
    {
        data.push_back(static_cast<uint8_t>(absolute));
        absolute >>= 8;
    }

    const auto negative_bit_set = is_negative(data);

    // If the most significant byte is >= 0x80 and the value is negative,
    // push a new 0x80 byte that will be popped off when converting to
    // an integral.
    if (negative_bit_set && negative)
        data.push_back(number::negative_mask);

    // If the most significant byte is >= 0x80 and the value is positive,
    // push a new zero-byte to make the significant byte < 0x80 again.
    else if (negative_bit_set)
        data.push_back(0);

    // If the most significant byte is < 0x80 and the value is negative,
    // add 0x80 to it, since it will be subtracted and interpreted as
    // a negative when converting to an integral.
    else if (negative)
        data.back() |= number::negative_mask;

    return data;
}

int32_t number::int32() const
{
    return domain_constrain<int32_t>(value_);
}

int64_t number::int64() const
{
    return value_;
}

// Stack Helpers
//-----------------------------------------------------------------------------

bool number::is_true() const
{
    return value_ != 0;
}

bool number::is_false() const
{
    return value_ == 0;
}

// Operators
//-----------------------------------------------------------------------------

bool number::operator>(int64_t value) const
{
    return value_ > value;
}

bool number::operator<(int64_t value) const
{
    return value_ < value;
}

bool number::operator>=(int64_t value) const
{
    return value_ >= value;
}

bool number::operator<=(int64_t value) const
{
    return value_ <= value;
}

bool number::operator==(int64_t value) const
{
    return value_ == value;
}

bool number::operator!=(int64_t value) const
{
    return value_ != value;
}

bool number::operator>(const number& other) const
{
    return operator>(other.value_);
}

bool number::operator<(const number& other) const
{
    return operator<(other.value_);
}

bool number::operator>=(const number& other) const
{
    return operator>=(other.value_);
}

bool number::operator<=(const number& other) const
{
    return operator<=(other.value_);
}

bool number::operator==(const number& other) const
{
    return operator==(other.value_);
}

bool number::operator!=(const number& other) const
{
    return operator!=(other.value_);
}

number number::operator+(int64_t value) const
{
    BITCOIN_ASSERT_MSG(value == 0 ||
        (value > 0 && value_ <= max_int64 - value) ||
        (value < 0 && value_ >= min_int64 - value), "overflow");

    return number(value_ + value);
}

number number::operator-(int64_t value) const
{
    BITCOIN_ASSERT_MSG(value == 0 ||
        (value > 0 && value_ >= min_int64 + value) ||
        (value < 0 && value_ <= max_int64 + value), "underflow");

    return number(value_ - value);
}

number number::operator+(const number& other) const
{
    return operator+(other.value_);
}

number number::operator-(const number& other) const
{
    return operator-(other.value_);
}

number number::operator+() const
{
    return *this;
}

number number::operator-() const
{
    BITCOIN_ASSERT_MSG(value_ != min_int64, "out of range");

    return number(-value_);
}

number& number::operator+=(const number& other)
{
    return operator+=(other.value_);
}

number& number::operator-=(const number& other)
{
    return operator-=(other.value_);
}

number& number::operator+=(int64_t value)
{
    BITCOIN_ASSERT_MSG(value == 0 ||
        (value > 0 && value_ <= max_int64 - value) ||
        (value < 0 && value_ >= min_int64 - value), "overflow");

    value_ += value;
    return *this;
}

number& number::operator-=(int64_t value)
{
    BITCOIN_ASSERT_MSG(value == 0 ||
        (value > 0 && value_ >= min_int64 + value) ||
        (value < 0 && value_ <= max_int64 + value), "underflow");

    value_ -= value;
    return *this;
}

} // namespace chain
} // namespace libbitcoin
