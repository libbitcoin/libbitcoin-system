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
#include <bitcoin/bitcoin/math/script_number.hpp>

#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>

namespace libbitcoin {

data_chunk script_number_serialize(const int64_t value)
{
    if (value == 0)
        return data_chunk();

    data_chunk result;
    const bool is_negative = value < 0;
    uint64_t abs_value = is_negative ? -value : value;

    while (abs_value)
    {
        result.push_back(abs_value & 0xff);
        abs_value >>= 8;
    }

    // - If the most significant byte is >= 0x80 and the value is positive,
    // push a new zero-byte to make the significant byte < 0x80 again.

    // - If the most significant byte is >= 0x80 and the value is negative,
    // push a new 0x80 byte that will be popped off when converting to
    // an integral.

    // - If the most significant byte is < 0x80 and the value is negative,
    // add 0x80 to it, since it will be subtracted and interpreted as
    // a negative when converting to an integral.

    if ((result.back() & 0x80) != 0)
        result.push_back(is_negative ? 0x80 : 0);
    else if (is_negative)
        result.back() |= 0x80;

    return result;
}

int64_t script_number_deserialize(const data_chunk& data)
{
    if (data.empty())
        return 0;

    int64_t result = 0;
    for (size_t i = 0; i != data.size(); ++i)
        result |= static_cast<int64_t>(data[i]) << 8 * i;

    // If the input vector's most significant byte is 0x80, remove it from
    // the result's msb and return a negative.
    if (data.back() & 0x80)
        return -(result & ~(0x80 << (8 * (data.size() - 1))));

    return result;
}

script_number::script_number(const int64_t value)
  : value_(value)
{
}
script_number::script_number()
{
    // You must call set_data() after.
}

bool script_number::set_data(const data_chunk& data, uint8_t max_size)
{
    if (data.size() > max_size)
        return false;

    value_ = script_number_deserialize(data);
    return true;
}
data_chunk script_number::data() const
{
    return script_number_serialize(value_);
}

int32_t script_number::int32() const
{
    if (value_ > max_int32)
        return max_int32;
    else if (value_ < min_int32)
        return min_int32;

    return static_cast<int32_t>(value_);
}

int64_t script_number::int64() const
{
    return value_;
}

bool script_number::operator==(const int64_t value) const
{
    return value_ == value;
}
bool script_number::operator!=(const int64_t value) const
{
    return value_ != value;
}
bool script_number::operator<=(const int64_t value) const
{
    return value_ <= value;
}
bool script_number::operator< (const int64_t value) const
{
    return value_ <  value;
}
bool script_number::operator>=(const int64_t value) const
{
    return value_ >= value;
}
bool script_number::operator> (const int64_t value) const
{
    return value_ >  value;
}

bool script_number::operator==(const script_number& other) const
{
    return operator==(other.value_);
}
bool script_number::operator!=(const script_number& other) const
{
    return operator!=(other.value_);
}
bool script_number::operator<=(const script_number& other) const
{
    return operator<=(other.value_);
}
bool script_number::operator<(const script_number& other) const
{
    return operator<(other.value_);
}
bool script_number::operator>=(const script_number& other) const
{
    return operator>=(other.value_);
}
bool script_number::operator>(const script_number& other) const
{
    return operator>(other.value_);
}

script_number script_number::operator+(const int64_t value) const
{
    return script_number(value_ + value);
}

script_number script_number::operator-(const int64_t value) const
{
    return script_number(value_ - value);
}

script_number script_number::operator+(const script_number& other) const
{
    return operator+(other.value_);
}

script_number script_number::operator-(const script_number& other) const
{
    return operator-(other.value_);
}

script_number script_number::operator-() const
{
    BITCOIN_ASSERT(value_ != std::numeric_limits<int64_t>::min());
    return script_number(-value_);
}

script_number& script_number::operator+=(const int64_t value)
{
    BITCOIN_ASSERT(value == 0 ||
        (value > 0 && value_ <= max_int64 - value) ||
        (value < 0 && value_ >= min_int64 - value));
    value_ += value;
    return *this;
}
script_number& script_number::operator-=(const int64_t value)
{
    BITCOIN_ASSERT(value == 0 ||
        (value > 0 && value_ >= min_int64 + value) ||
        (value < 0 && value_ <= max_int64 + value));
    value_ -= value;
    return *this;
}
script_number& script_number::operator+=(const script_number& other)
{
    return operator+=(other.value_);
}
script_number& script_number::operator-=(const script_number& other)
{
    return operator-=(other.value_);
}

} // namespace libbitcoin

