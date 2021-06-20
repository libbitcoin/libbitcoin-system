/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_MACHINE_NUMBER_IPP
#define LIBBITCOIN_SYSTEM_MACHINE_NUMBER_IPP

#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/math/limits.hpp>

namespace libbitcoin {
namespace system {
namespace machine {

static const uint64_t unsigned_max_int64 = max_int64;
static const uint64_t absolute_min_int64 = min_int64;
static const uint64_t negative_bit = number::negative_sign;

inline bool is_negative(const data_chunk& data)
{
    return (data.back() & number::negative_sign) != 0;
}

inline number::number()
  : number(0)
{
}

inline number::number(int64_t value)
  : value_(value)
{
}

// Properties
//-----------------------------------------------------------------------------

// The data is interpreted as little-endian.
inline bool number::set_data(const data_chunk& data, size_t max_size)
{
    if (data.size() > max_size)
        return false;

    value_ = 0;

    if (data.empty())
        return true;

    // This is "from little endian" with a variable buffer.
    for (size_t i = 0; i != data.size(); ++i)
        value_ |= static_cast<int64_t>(data[i]) << (8 * i);

    if (is_negative(data))
    {
        const auto last_shift = 8 * (data.size() - 1);
        const auto mask = ~(negative_bit << last_shift);
        value_ = -1 * (static_cast<int64_t>(value_ & mask));
    }

    return true;
}

// The result is little-endian.
inline data_chunk number::data() const
{
    if (value_ == 0)
        return {};

    data_chunk data;
    const auto set_negative = value_ < 0;
    uint64_t absolute = set_negative ? -value_ : value_;

    // This is "to little endian" with a minimal buffer.
    while (absolute != 0)
    {
        data.push_back(static_cast<uint8_t>(absolute));
        absolute >>= 8;
    }

    const auto negative_bit_set = is_negative(data);

    // If the most significant byte is >= 0x80 and the value is negative,
    // push a new 0x80 byte that will be popped off when converting to
    // an integral.
    if (negative_bit_set && set_negative)
        data.push_back(number::negative_sign);

    // If the most significant byte is >= 0x80 and the value is positive,
    // push a new zero-byte to make the significant byte < 0x80 again.
    else if (negative_bit_set)
        data.push_back(0);

    // If the most significant byte is < 0x80 and the value is negative,
    // add 0x80 to it, since it will be subtracted and interpreted as
    // a negative when converting to an integral.
    else if (set_negative)
        data.back() |= number::negative_sign;

    return data;
}

inline int32_t number::int32() const
{
    return limit<int32_t>(value_);
}

inline int64_t number::int64() const
{
    return value_;
}

// Stack Helpers
//-----------------------------------------------------------------------------

inline bool number::is_true() const
{
    return value_ != 0;
}

inline bool number::is_false() const
{
    return value_ == 0;
}

// Operators
//-----------------------------------------------------------------------------

inline bool number::operator>(int64_t value) const
{
    return value_ > value;
}

inline bool number::operator<(int64_t value) const
{
    return value_ < value;
}

inline bool number::operator>=(int64_t value) const
{
    return value_ >= value;
}

inline bool number::operator<=(int64_t value) const
{
    return value_ <= value;
}

inline bool number::operator==(int64_t value) const
{
    return value_ == value;
}

inline bool number::operator!=(int64_t value) const
{
    return value_ != value;
}

inline bool number::operator>(const number& other) const
{
    return operator>(other.value_);
}

inline bool number::operator<(const number& other) const
{
    return operator<(other.value_);
}

inline bool number::operator>=(const number& other) const
{
    return operator>=(other.value_);
}

inline bool number::operator<=(const number& other) const
{
    return operator<=(other.value_);
}

inline bool number::operator==(const number& other) const
{
    return operator==(other.value_);
}

inline bool number::operator!=(const number& other) const
{
    return operator!=(other.value_);
}

inline number number::operator+(int64_t value) const
{
    BITCOIN_ASSERT_MSG(value == 0 ||
        (value > 0 && value_ <= max_int64 - value) ||
        (value < 0 && value_ >= min_int64 - value), "overflow");

    return number(value_ + value);
}

inline number number::operator-(int64_t value) const
{
    BITCOIN_ASSERT_MSG(value == 0 ||
        (value > 0 && value_ >= min_int64 + value) ||
        (value < 0 && value_ <= max_int64 + value), "underflow");

    return number(value_ - value);
}

inline number number::operator+(const number& other) const
{
    return operator+(other.value_);
}

inline number number::operator-(const number& other) const
{
    return operator-(other.value_);
}

inline number number::operator+() const
{
    return *this;
}

inline number number::operator-() const
{
    BITCOIN_ASSERT_MSG(value_ != min_int64, "out of range");

    return number(-value_);
}

inline number& number::operator+=(const number& other)
{
    return operator+=(other.value_);
}

inline number& number::operator-=(const number& other)
{
    return operator-=(other.value_);
}

inline number& number::operator+=(int64_t value)
{
    BITCOIN_ASSERT_MSG(value == 0 ||
        (value > 0 && value_ <= max_int64 - value) ||
        (value < 0 && value_ >= min_int64 - value), "overflow");

    value_ += value;
    return *this;
}

inline number& number::operator-=(int64_t value)
{
    BITCOIN_ASSERT_MSG(value == 0 ||
        (value > 0 && value_ >= min_int64 + value) ||
        (value < 0 && value_ <= max_int64 + value), "underflow");

    value_ -= value;
    return *this;
}

} // namespace machine
} // namespace system
} // namespace libbitcoin

#endif
