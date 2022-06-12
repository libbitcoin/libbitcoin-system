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

#include <algorithm>
#include <cstdint>
#include <cstddef>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/serial/serial.hpp>

//*****************************************************************************
// CONSENSUS: All operations except stack encoding exhibit native c++ behavior.
//*****************************************************************************

namespace libbitcoin {
namespace system {
namespace machine {

// TODO: This class can be reduced to:
// TODO: 'data_chunk to_data<Type>(Type value)'
// TODO: 'Type from_data<Type>(const data_chunk& data)'
// TODO: where Type is int64_t or narrower, or bool

using namespace system::chain;

constexpr uint8_t positive_sign_byte = 0x00;
constexpr uint8_t negative_sign_byte = bit_left<uint8_t>(0);

// A sign byte is necessary due to variable length integer encoding.
constexpr bool is_sign_byte(uint8_t byte) noexcept
{
    return byte == positive_sign_byte || byte == negative_sign_byte;
}

// This is an exception to number encoding (non-empty zeros, unlimited size).
bool number::is_stack_true(const data_chunk& bytes) noexcept
{
    // Empty is the standard number encoding for zero.
    if (bytes.empty())
        return false;

    // Byte encoding of integers is little-endian.
    // First byte (sign) of any zero-valued (false) number must be 0x00 or 0x01.
    // Negative zero is not emitted by number encoding, but is considered zero.
    if (!is_sign_byte(bytes.back()))
        return true;

    // C++17: Parallel policy for std::any_of.
    // Number encoding does not emit a sign byte on a zero, but it is
    // possible that a non-number will be 0x00[0x00, 0x00..., 0x00], and it is
    // possible that a non-number will be 0x80[0x00, 0x00..., 0x00].
    // First byte non-zero has already been evaluated by the sign check.
    return std::any_of(bytes.begin(), std::prev(bytes.end()),
        is_nonzero<uint8_t>);
}

number::number() noexcept
  : number(zero)
{
}

number::number(int64_t value) noexcept
  : value_(value)
{
}

// The data is interpreted as little-endian.
bool number::set_data(const data_chunk& data, size_t max_size) noexcept
{
    // Bitcoin defined max script number sizes are 4 and 5 bytes.
    // If max_size > sizeof(int64_t) bytes will be truncated by little endian.
    BC_ASSERT_MSG(max_size < sizeof(value_), "invalid number size");

    value_ = zero;

    // Empty is the standard number encoding for zero.
    if (data.empty())
        return true;

    if (data.size() > max_size)
        return false;

    // TODO: optimize endianness conversion.
    value_ = from_little_endian<int64_t>(data);
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
    // Empty is the standard number encoding for zero.
    if (is_zero(value_))
        return {};

    // TODO: optimize endianness conversion.
    auto data = to_little_endian_chunk(absolute(value_), one);
    const auto negative_bit_set = get_left(data.back());

    // Reserved one extra byte to preclude reallocation on possible push.
    BC_ASSERT_MSG(add1(data.size()) == data.capacity(), "unexpected capacity");

    // Push a 0x80 byte that will be discarded when converting to an integral.
    if (negative_bit_set && is_negative(value_))
        data.push_back(negative_sign_byte);

    // Push a 0x00 byte to make the most significant byte non-negative again.
    else if (negative_bit_set)
        data.push_back(positive_sign_byte);

    // Set the negative bit, since it will be subtracted and interpreted as
    // a negative when converting to an integral.
    else if (is_negative(value_))
        set_left_into(data.back());

    return data;
}

// Inherently limited by 'value_ = from_little_endian<int64_t>' above.
int64_t number::int64() const noexcept
{
    return value_;
}

} // namespace machine
} // namespace system
} // namespace libbitcoin
