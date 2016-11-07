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

static constexpr auto unsigned_max_int64 = static_cast<uint64_t>(max_int64);
static constexpr auto absolute_min_int64 = static_cast<uint64_t>(min_int64);

// The result is always LSB first.
static data_chunk number_serialize(int64_t value)
{
    if (value == 0)
        return{};

    data_chunk result;
    const auto negative = value < 0;

    // std::abs cannot handle min_int64, so we use a special case.
    auto absolute_value = value == min_int64 ? absolute_min_int64 :
        static_cast<uint64_t>(std::abs(value));

    while (absolute_value != 0)
    {
        static constexpr uint64_t byte_mask = 0xff;
        result.push_back(absolute_value & byte_mask);
        absolute_value >>= byte_bits;
    }

    auto negative_masked = (result.back() & number::negative_mask) != 0;

    // If the most significant byte is >= 0x80 and the value is negative,
    // push a new 0x80 byte that will be popped off when converting to
    // an integral.
    if (negative_masked && negative)
        result.push_back(number::negative_mask);

    // If the most significant byte is >= 0x80 and the value is positive,
    // push a new zero-byte to make the significant byte < 0x80 again.
    else if (negative_masked)
        result.push_back(0);

    // If the most significant byte is < 0x80 and the value is negative,
    // add 0x80 to it, since it will be subtracted and interpreted as
    // a negative when converting to an integral.
    else if (negative)
        result.back() |= number::negative_mask;

    return result;
}

// The parameter is assumed to be LSB first.
static int64_t number_deserialize(const data_chunk& data)
{
    if (data.empty())
        return 0;

    const auto consume_last_byte = data.back() != number::negative_mask;
    const auto value_size = consume_last_byte ? data.size() : data.size() - 1;

    // This is guarded by set_data().
    BITCOIN_ASSERT(value_size <= sizeof(uint64_t));

    const auto negative = data.back() & number::negative_mask;
    const auto mask_last_byte = negative && consume_last_byte;
    uint64_t absolute_value = 0;

    for (size_t byte = 0; byte < value_size; ++byte)
    {
        const auto shift = byte_bits * byte;
        const auto last_byte = byte + 1 == value_size;
        const auto value = mask_last_byte && last_byte ?
            data[byte] & ~number::negative_mask : data[byte];

        absolute_value |= static_cast<uint64_t>(value) << shift;
    }

    if ((!negative && absolute_value > unsigned_max_int64) ||
        (negative && absolute_value > absolute_min_int64))
        throw std::out_of_range("the data value is out of range");

    // Cannot assign absolute_min_int64 to int64, so we use a special case.
    if (negative && absolute_value == absolute_min_int64)
        return min_int64;

    // If the input vector's most significant byte is 0x80 return a negative.
    return negative ?
        static_cast<int64_t>(absolute_value) * -1 :
        static_cast<int64_t>(absolute_value);
}

number::number()
  : number(0)
{
}

number::number(int64_t value)
  : value_(value)
{
}

bool number::set_data(const data_chunk& data, size_t max_size)
{
    const auto size = data.size();

    if (size > max_size)
        return false;

    const auto negative_byte = !data.empty() && data.back() == negative_mask;
    const auto value_size = negative_byte ? size - 1 : size;

    if (value_size > sizeof(uint64_t))
        return false;

    value_ = number_deserialize(data);
    return true;
}

data_chunk number::data() const
{
    return number_serialize(value_);
}

int32_t number::int32() const
{
    return domain_constrain<int32_t>(value_);
}

int64_t number::int64() const
{
    return value_;
}

bool number::is_true() const
{
    return value_ != 0;
}

bool number::is_false() const
{
    return value_ == 0;
}

bool number::operator==(int64_t value) const
{
    return value_ == value;
}

bool number::operator!=(int64_t value) const
{
    return value_ != value;
}

bool number::operator<=(int64_t value) const
{
    return value_ <= value;
}

bool number::operator<(int64_t value) const
{
    return value_ < value;
}

bool number::operator>=(int64_t value) const
{
    return value_ >= value;
}

bool number::operator>(int64_t value) const
{
    return value_ > value;
}

bool number::operator==(const number& other) const
{
    return operator==(other.value_);
}

bool number::operator!=(const number& other) const
{
    return operator!=(other.value_);
}

bool number::operator<=(const number& other) const
{
    return operator<=(other.value_);
}

bool number::operator<(const number& other) const
{
    return operator<(other.value_);
}

bool number::operator>=(const number& other) const
{
    return operator>=(other.value_);
}

bool number::operator>(const number& other) const
{
    return operator>(other.value_);
}

number number::operator+(int64_t value) const
{
    if ((value > 0 && (value_ >= max_int64 - value)) ||
        (value < 0 && (value_ <= min_int64 - value)))
    {
        throw std::overflow_error("integer overflow");
    }

    return number(value_ + value);
}

number number::operator-(int64_t value) const
{
    if ((value > 0 && (value_ <= min_int64 + value)) ||
        (value < 0 && (value_ >= max_int64 + value)))
    {
        throw std::overflow_error("integer underflow");
    }

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
    if (value_ == min_int64)
        throw std::out_of_range("negation would be out of range");

    return number(-value_);
}

number& number::operator+=(int64_t value)
{
    if ((value > 0 && (value_ >= max_int64 - value)) ||
        (value < 0 && (value_ <= min_int64 - value)))
    {
        throw std::overflow_error("integer increment overflow");
    }

    value_ += value;
    return *this;
}

number& number::operator-=(int64_t value)
{
    if ((value > 0 && (value_ <= min_int64 + value)) ||
        (value < 0 && (value_ >= max_int64 + value)))
    {
        throw std::overflow_error("integer decrement underflow");
    }

    value_ -= value;
    return *this;
}

number& number::operator+=(const number& other)
{
    return operator+=(other.value_);
}

number& number::operator-=(const number& other)
{
    return operator-=(other.value_);
}

} // namespace chain
} // namespace libbitcoin
