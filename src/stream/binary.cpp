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
#include <bitcoin/system/stream/binary.hpp>

#include <iostream>
#include <string>
#include <utility>
/// DELETEMENOW
#include <bitcoin/system/data/data_chunk.hpp>
#include <bitcoin/system/data/data_slice.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {

constexpr uint8_t pad = 0x00;

constexpr bool is_binary(char character) NOEXCEPT
{
    return character == '0' || character == '1';
}

bool binary::is_base2(const std::string& text) NOEXCEPT
{
    return std::all_of(text.begin(), text.end(), is_binary);
}

// constructors
// ----------------------------------------------------------------------------

binary::binary() NOEXCEPT
  : bits_(0), bytes_()
{
}

binary::binary(const std::string& bits) NOEXCEPT
  : binary(from_string(bits))
{
}

binary::binary(size_t bits, const data_slice& data) NOEXCEPT
  : binary(from_data(bits, data.to_chunk()))
{
}

// private
binary::binary(data_chunk&& bytes, size_t bits) NOEXCEPT
  : bits_(bits), bytes_(bytes)
{
}

// factories
// ----------------------------------------------------------------------------

binary binary::from_data(size_t bits, data_chunk&& data) NOEXCEPT
{
    data.resize(ceilinged_divide(bits, byte_bits), pad);

    // absolute cannot overflow as magnitude is limited to sub1(byte_bits).
    if (!data.empty())
        mask_right_into(data.back(),
            absolute(ceilinged_modulo(bits, byte_bits)));

    return { std::move(data), bits };
}

binary binary::from_string(const std::string bits) NOEXCEPT
{
    if (!binary::is_base2(bits))
        return {};

    auto length = bits.length();
    data_chunk data(ceilinged_divide(length, byte_bits), pad);
    write::bits::copy writer(data);

    for (const auto bit: bits)
        writer.write_bit(bit == '1');

    writer.flush();
    return { std::move(data), length };
}

// methods
// ----------------------------------------------------------------------------

std::string binary::encoded() const NOEXCEPT
{
    std::string text(to_bits(bytes_.size()), pad);
    write::bytes::copy writer(text);
    read::bits::copy reader(bytes_);

    while (!reader.is_exhausted())
        writer.write_byte(reader.read_bit() ? '1' : '0');

    // trim up to 7 characters of padding.
    text.resize(bits_);
    text.shrink_to_fit();
    return text;
}

const data_chunk& binary::data() const NOEXCEPT
{
    return bytes_;
}

size_t binary::bytes() const NOEXCEPT
{
    return bytes_.size();
}

size_t binary::bits() const NOEXCEPT
{
    return bits_;
}

// operators
// ----------------------------------------------------------------------------

binary::operator const data_chunk&() const NOEXCEPT
{
    return bytes_;
}

bool binary::operator[](size_t index) const NOEXCEPT
{
    const auto byte = index / byte_bits;
    return (byte < bits_) && get_left(bytes_[byte], index % byte_bits);
}

bool binary::operator<(const binary& other) const NOEXCEPT
{
    return encoded() < other.encoded();
}

bool operator==(const binary& left, const binary& right) NOEXCEPT
{
    return left.encoded() == right.encoded();
}

bool operator!=(const binary& left, const binary& right) NOEXCEPT
{
    return !(left == right);
}

std::istream& operator>>(std::istream& in, binary& to) NOEXCEPT
{
    std::string text;
    in >> text;
    to = binary(text);
    return in;
}

std::ostream& operator<<(std::ostream& out, const binary& of) NOEXCEPT
{
    out << of.encoded();
    return out;
}

} // namespace system
} // namespace libbitcoin
