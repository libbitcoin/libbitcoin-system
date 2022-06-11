/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/machine/number.hpp>

#include <cstdint>
#include <cstdlib>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/serial/serial.hpp>

namespace libbitcoin {
namespace system {
namespace machine {

using namespace system::chain;

number::number() noexcept
  : number(0)
{
}

number::number(int_fast64_t value) noexcept
  : value_(value)
{
}

// Properties
// ----------------------------------------------------------------------------
// TODO: This class can be reduced to:
// TODO: 'data_chunk to_data<Type>(Type value)'
// TODO: 'Type from_data<Type>(const data_chunk& data)'
// TODO: where Type is int_fast64_t or narrower, or bool

// The data is interpreted as little-endian.
bool number::set_data(const data_chunk& data, size_t max_size) noexcept
{
    // Bitcoin defined max script number sizes are 4 and 5 bytes.
    BC_ASSERT_MSG(max_size < sizeof(value_), "invalid number size");

    if (data.size() > max_size)
        return false;

    value_ = 0;

    if (data.empty())
        return true;

    value_ = from_little_endian<int_fast64_t>(data);
    const auto negative_bit_set = get_left(data.back());

    // Clear the negative bit and negate the value.
    // The bit is the leftmost of the bits decoded (offset from right).
    if (negative_bit_set)
        value_ = -set_right(value_, sub1(to_bits(data.size())), false);

    return true;
}

// The result is little-endian, with negative sign bit.
data_chunk number::data() const noexcept
{
    if (is_false())
        return {};

    auto data = to_little_endian_chunk(absolute(value_), one);
    const auto negative_bit_set = get_left(data.back());

    // Reserved one extra byte to preclude reallocation on possible push.
    BC_ASSERT_MSG(add1(data.size()) == data.capacity(), "unexpected capacity");

    // Push a 0x80 byte that will be discarded when converting to an integral.
    if (negative_bit_set && is_negative())
        data.push_back(negative_sign_byte());

    // Push a 0x00 byte to make the most significant byte non-negative again.
    else if (negative_bit_set)
        data.push_back(positive_sign_byte());

    // Set the negative bit, since it will be subtracted and interpreted as
    // a negative when converting to an integral.
    else if (is_negative())
        set_left_into(data.back());

    return data;
}

// int32_t: -2**31+1 to 2**31-1
// int40_t: -2**39+1 to 2**39-1
// int64_t: -2**63+1 to 2**63-1

int_fast32_t number::to_int32() const noexcept
{
    // This can be safely cast if the data was 4 byte limited.
    // narrow_cast<int_fast32_t> fails to match integral type (clang).
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<int_fast32_t>(value_);
    BC_POP_WARNING()
}

number::int_fast40_t number::to_int40() const noexcept
{
    static_assert(width<int_fast64_t>() - to_bits(5) == 24u, "");

    // This can be safely masked/cast if the data was 5 byte limited.
    // We are using a type alias of uint64_t for int40_t, so just mask return.
    return mask_left(value_, width<int_fast64_t>() - to_bits(5));
}

// deprecated
int_fast32_t number::int32() const noexcept
{
    // TODO: this can be simply cast if was 4 byte limited set_data.
    // TODO: however number tests have this unnecessary limit constraint.
    return limit<int_fast32_t>(value_);
}

// deprecated
int_fast64_t number::int64() const noexcept
{
    // Inherently limited by 'value_ = from_little_endian<int_fast64_t>' above.
    return value_;
}

// Stack Helpers
// ----------------------------------------------------------------------------
// These are nothing more than c++ native semantics.

bool number::is_true() const noexcept
{
    return !is_zero(value_);
}

bool number::is_false() const noexcept
{
    return is_zero(value_);
}

bool number::is_negative() const noexcept
{
    return system::is_negative(value_);
}

// Operators
// ----------------------------------------------------------------------------
// These never overflow in script (32 bit add/subtract in 64 bit space).
// Other than assertion this is nothing more than c++ native operators.

bool number::operator>(int_fast64_t value) const noexcept
{
    return value_ > value;
}

bool number::operator<(int_fast64_t value) const noexcept
{
    return value_ < value;
}

bool number::operator>=(int_fast64_t value) const noexcept
{
    return value_ >= value;
}

bool number::operator<=(int_fast64_t value) const noexcept
{
    return value_ <= value;
}

bool number::operator==(int_fast64_t value) const noexcept
{
    return value_ == value;
}

bool number::operator!=(int_fast64_t value) const noexcept
{
    return value_ != value;
}

bool number::operator>(const number& other) const noexcept
{
    return operator>(other.value_);
}

bool number::operator<(const number& other) const noexcept
{
    return operator<(other.value_);
}

bool number::operator>=(const number& other) const noexcept
{
    return operator>=(other.value_);
}

bool number::operator<=(const number& other) const noexcept
{
    return operator<=(other.value_);
}

bool number::operator==(const number& other) const noexcept
{
    return operator==(other.value_);
}

bool number::operator!=(const number& other) const noexcept
{
    return operator!=(other.value_);
}

number number::operator+(int_fast64_t value) const noexcept
{
    BC_ASSERT(!overflows(value_, value));
    return number(value_ + value);
}

number number::operator-(int_fast64_t value) const noexcept
{
    BC_ASSERT(!underflows(value_, value));
    return number(value_ - value);
}

number number::operator+(const number& other) const noexcept
{
    return operator+(other.value_);
}

number number::operator-(const number& other) const noexcept
{
    return operator-(other.value_);
}

number number::operator+() const noexcept
{
    return *this;
}

number number::operator-() const noexcept
{
    BC_ASSERT_MSG(value_ != min_int64, "out of range");
    return number(-value_);
}

number& number::operator+=(const number& other) noexcept
{
    return operator+=(other.value_);
}

number& number::operator-=(const number& other) noexcept
{
    return operator-=(other.value_);
}

number& number::operator+=(int_fast64_t value) noexcept
{
    BC_ASSERT(!overflows(value_, value));
    value_ += value;
    return *this;
}

number& number::operator-=(int_fast64_t value) noexcept
{
    BC_ASSERT(!underflows(value_, value));
    value_ -= value;
    return *this;
}

} // namespace machine
} // namespace system
} // namespace libbitcoin
