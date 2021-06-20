/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/data/binary.hpp>

#include <algorithm>
#include <iostream>
#include <string>
#include <utility>
#include <boost/multiprecision/cpp_int.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/data/uintx.hpp>
#include <bitcoin/system/math/addition.hpp>
#include <bitcoin/system/math/division.hpp>
#include <bitcoin/system/serialization/endian.hpp>

// See std::bitset (fixed size), std::vector<bool>, and boost::dynamic_bitset
// for other options. boost::dynamic_bitset supports serialization and is the
// most comprehensive. We use uintx as the contained bit vector because of our
// endian support. dynamic_bitset would give us serialization but we would have
// to implement endianed byte conversion (using block append iteration). Uintx
// provides all potentially-necessary operations and reduces boost dependency.

namespace libbitcoin {
namespace system {

using namespace boost::multiprecision;
constexpr auto capacity = std::numeric_limits<binary::size_type>::max();

constexpr bool is_binary(char character) noexcept
{
    return character == '0' || character == '1';
}

bool binary::is_base2(const std::string& text) noexcept
{
    return std::all_of(text.begin(), text.end(), is_binary);
}

// constructors
// ----------------------------------------------------------------------------

binary::binary() noexcept
  : bits_(0), size_(0)
{
}

binary::binary(binary&& other) noexcept
  : bits_(std::move(other.bits_)), size_(other.size_)
{
}

binary::binary(const binary& other) noexcept
  : bits_(other.bits_), size_(other.size_)
{
}

binary::binary(const std::string& bits) noexcept
  : binary(from_string(bits))
{
}

// Size parameter controls the extent of serialization.
binary::binary(size_type size, const data_slice& data) noexcept
  : binary(from_data(size, data))
{
}

// private
binary::binary(size_type size, const uintx& number) noexcept
  : bits_(number), size_(size)
{
}

// factories
// ----------------------------------------------------------------------------

// TODO: endianness.
binary binary::from_data(size_type size, const data_slice& data) noexcept
{
    if (data.size() > ceilinged_divide(capacity, byte_bits))
        return {};

    // If data bits provided is greater than size, shift the excess out.
    const auto bits = static_cast<size_type>(to_bits(data.size()));
    auto number = from_little_endian(data);
    number >>= floored_subtract(bits, size);
    return { size, number };
}

// TODO: endianness.
binary binary::from_string(const std::string bits) noexcept
{
    if ((bits.length() > capacity) || !binary::is_base2(bits))
        return {};

    uintx number;
    for (size_type bit = 0; bit < bits.length(); ++bit)
        if (bits[bit] == '1') bit_set(number, bit);

    return { static_cast<size_type>(bits.length()), number };
}

// methods
// ----------------------------------------------------------------------------

// TODO: endianness.
std::string binary::encoded() const noexcept
{
    std::string bits(size_, '\0');
    auto character = bits.begin();
    auto self = *this;

    for (size_type bit = 0; bit < bits.length(); ++bit)
        *character++ = self[bit] ? '1' : '0';

    return bits;
}

// TODO: endianness.
data_chunk binary::data() const noexcept
{
    // If converted byte size is less than size, pad little-endian.
    auto buffer = to_little_endian(bits_);
    buffer.resize(bytes(), 0x00);
    return buffer;
}

binary::size_type binary::bytes() const noexcept
{
    return ceilinged_divide(size_, static_cast<size_type>(byte_bits));
}

binary::size_type binary::bits() const noexcept
{
    return size_;
}

// operators
// ----------------------------------------------------------------------------
// For information on implementing a reference operator[] against a proxy, see:
// en.cppreference.com/w/cpp/utility/bitset/reference

bool binary::operator[](size_type index) const noexcept
{
    return bit_test(bits_, index);
}

bool binary::operator<(const binary& other) const noexcept
{
    return encoded() < other.encoded();
}

binary& binary::operator=(binary&& other) noexcept
{
    bits_ = std::move(other.bits_);
    size_ = other.size_;
    return *this;
}

binary& binary::operator=(const binary& other) noexcept
{
    bits_ = other.bits_;
    size_ = other.size_;
    return *this;
}

bool operator==(const binary& left, const binary& right) noexcept
{
    return left.encoded() == right.encoded();
}

bool operator!=(const binary& left, const binary& right) noexcept
{
    return !(left == right);
}

std::istream& operator>>(std::istream& in, binary& to) noexcept
{
    std::string text;
    in >> text;
    to = binary(text);
    return in;
}

std::ostream& operator<<(std::ostream& out, const binary& of) noexcept
{
    out << of.encoded();
    return out;
}

} // namespace system
} // namespace libbitcoin
