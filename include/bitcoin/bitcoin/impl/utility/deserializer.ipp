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
#ifndef LIBBITCOIN_DESERIALIZER_IPP
#define LIBBITCOIN_DESERIALIZER_IPP

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/endian.hpp>

namespace libbitcoin {

template <typename Iterator, bool CheckSafe>
deserializer<Iterator, CheckSafe>::deserializer(const Iterator begin,
    const Iterator end)
  : iterator_(begin), end_(end), valid_(true)
{
}

// Context.
//-----------------------------------------------------------------------------

template <typename Iterator, bool CheckSafe>
deserializer<Iterator, CheckSafe>::operator bool() const
{
    return valid_;
}

template <typename Iterator, bool CheckSafe>
bool deserializer<Iterator, CheckSafe>::operator!() const
{
    return !valid_;
}

template <typename Iterator, bool CheckSafe>
bool deserializer<Iterator, CheckSafe>::is_exhausted() const
{
    return remaining() == 0;
}

template <typename Iterator, bool CheckSafe>
void deserializer<Iterator, CheckSafe>::invalidate()
{
    valid_ = false;
}

// Hashes.
//-----------------------------------------------------------------------------

template <typename Iterator, bool CheckSafe>
hash_digest deserializer<Iterator, CheckSafe>::read_hash()
{
    return read_forward<hash_size>();
}

template <typename Iterator, bool CheckSafe>
short_hash deserializer<Iterator, CheckSafe>::read_short_hash()
{
    return read_forward<short_hash_size>();
}

template <typename Iterator, bool CheckSafe>
mini_hash deserializer<Iterator, CheckSafe>::read_mini_hash()
{
    return read_forward<mini_hash_size>();
}

// Big Endian Integers.
//-----------------------------------------------------------------------------

template <typename Iterator, bool CheckSafe>
uint16_t deserializer<Iterator, CheckSafe>::read_2_bytes_big_endian()
{
    return read_big_endian<uint16_t>();
}

template <typename Iterator, bool CheckSafe>
uint32_t deserializer<Iterator, CheckSafe>::read_4_bytes_big_endian()
{
    return read_big_endian<uint32_t>();
}

template <typename Iterator, bool CheckSafe>
uint64_t deserializer<Iterator, CheckSafe>::read_8_bytes_big_endian()
{
    return read_big_endian<uint64_t>();
}

template <typename Iterator, bool CheckSafe>
uint64_t deserializer<Iterator, CheckSafe>::read_variable_big_endian()
{
    const auto value = read_byte();

    switch (value)
    {
        case eight_bytes:
            return read_8_bytes_big_endian();
        case four_bytes:
            return read_4_bytes_big_endian();
        case two_bytes:
            return read_2_bytes_big_endian();
        default:
            return value;
    }
}

template <typename Iterator, bool CheckSafe>
size_t deserializer<Iterator, CheckSafe>::read_size_big_endian()
{
    auto size = read_variable_big_endian();

    // This facilitates safely passing the size into a follow-on reader.
    // Return zero allows follow-on use before testing reader state.
    if (size > max_size_t)
    {
        invalidate();
        size = 0;
    }

    return static_cast<size_t>(size);
}

// Little Endian Integers.
//-----------------------------------------------------------------------------

template <typename Iterator, bool CheckSafe>
code deserializer<Iterator, CheckSafe>::read_error_code()
{
    const auto value = read_little_endian<uint32_t>();
    return code(static_cast<error::error_code_t>(value));
}

template <typename Iterator, bool CheckSafe>
uint16_t deserializer<Iterator, CheckSafe>::read_2_bytes_little_endian()
{
    return read_little_endian<uint16_t>();
}

template <typename Iterator, bool CheckSafe>
uint32_t deserializer<Iterator, CheckSafe>::read_4_bytes_little_endian()
{
    return read_little_endian<uint32_t>();
}

template <typename Iterator, bool CheckSafe>
uint64_t deserializer<Iterator, CheckSafe>::read_8_bytes_little_endian()
{
    return read_little_endian<uint64_t>();
}

template <typename Iterator, bool CheckSafe>
uint64_t deserializer<Iterator, CheckSafe>::read_variable_little_endian()
{
    const auto value = read_byte();

    switch (value)
    {
        case eight_bytes:
            return read_8_bytes_little_endian();
        case four_bytes:
            return read_4_bytes_little_endian();
        case two_bytes:
            return read_2_bytes_little_endian();
        default:
            return value;
    }
}

template <typename Iterator, bool CheckSafe>
size_t deserializer<Iterator, CheckSafe>::read_size_little_endian()
{
    auto size = read_variable_little_endian();

    // This facilitates safely passing the size into a follow-on reader.
    // Return zero allows follow-on use before testing reader state.
    if (size > max_size_t)
    {
        invalidate();
        size = 0;
    }

    return static_cast<size_t>(size);
}

// Bytes.
//-----------------------------------------------------------------------------

template <typename Iterator, bool CheckSafe>
uint8_t deserializer<Iterator, CheckSafe>::read_byte()
{
    return read_forward<1>()[0];
}

template <typename Iterator, bool CheckSafe>
data_chunk deserializer<Iterator, CheckSafe>::read_bytes()
{
    // This read is always safe.
    return read_bytes(remaining());
}

// Return size is guaraneteed.
template <typename Iterator, bool CheckSafe>
data_chunk deserializer<Iterator, CheckSafe>::read_bytes(size_t size)
{
    data_chunk out(size);
    if (!safe(size))

        invalidate();

    if (!valid_)
        return out;

    const auto begin = iterator_;
    skip(size);
    std::copy(begin, iterator_, out.begin());
    return out;
}

template <typename Iterator, bool CheckSafe>
std::string deserializer<Iterator, CheckSafe>::read_string()
{
    return read_string(read_size_little_endian());
}

// Removes trailing zeros, required for bitcoin string comparisons.
template <typename Iterator, bool CheckSafe>
std::string deserializer<Iterator, CheckSafe>::read_string(size_t size)
{
    if (!safe(size))
        invalidate();

    if (!valid_)
        return{};

    std::string out;
    out.reserve(size);

    // Read up to size characters, stopping at the first null (may be many).
    for (auto index = 0; index < size; ++index)
    {
        const auto character = iterator_[index];

        if (character == terminator)
            break;

        out.push_back(character);
    }

    // Consume all size characters from the buffer.
    skip(size);

    // Reduce the allocation to the number of characters pushed.
    out.shrink_to_fit();
    return out;
}

template <typename Iterator, bool CheckSafe>
void deserializer<Iterator, CheckSafe>::skip(size_t size)
{
    if (!safe(size))
        invalidate();

    if (!valid_)
        return;

    iterator_ += size;
}

template <typename Iterator, bool CheckSafe>
template <unsigned Size>
byte_array<Size> deserializer<Iterator, CheckSafe>::read_forward()
{
    if (!safe(Size))
        invalidate();

    if (!valid_)
        return{};

    byte_array<Size> out;
    const auto begin = iterator_;
    skip(Size);
    std::copy(begin, iterator_, out.begin());
    return out;
}

template <typename Iterator, bool CheckSafe>
template <unsigned Size>
byte_array<Size> deserializer<Iterator, CheckSafe>::read_reverse()
{
    if (!safe(Size))
        invalidate();

    if (!valid_)
        return{};

    byte_array<Size> out;
    const auto begin = iterator_;
    skip(Size);
    std::reverse_copy(begin, iterator_, out.begin());
    return out;
}

template <typename Iterator, bool CheckSafe>
template <typename Integer>
Integer deserializer<Iterator, CheckSafe>::read_big_endian()
{
    if (!safe(sizeof(Integer)))
        invalidate();

    if (!valid_)
        return{};

    const auto begin = iterator_;
    skip(sizeof(Integer));
    return from_big_endian_unsafe<Integer>(begin);
}

template <typename Iterator, bool CheckSafe>
template <typename Integer>
Integer deserializer<Iterator, CheckSafe>::read_little_endian()
{
    if (!safe(sizeof(Integer)))
        invalidate();

    if (!valid_)
        return{};

    const auto begin = iterator_;
    skip(sizeof(Integer));
    return from_little_endian_unsafe<Integer>(begin);
}

// private

template <typename Iterator, bool CheckSafe>
bool deserializer<Iterator, CheckSafe>::safe(size_t size) const
{
    // Bounds checking is disabled for unsafe deserializers.
    if (!CheckSafe)
        return true;

    return size <= remaining();
}

template <typename Iterator, bool CheckSafe>
size_t deserializer<Iterator, CheckSafe>::remaining() const
{
    return std::distance(iterator_, end_);
}

// Factories.
//-----------------------------------------------------------------------------

template <typename Iterator>
deserializer<Iterator, true> make_safe_deserializer(const Iterator begin,
    const Iterator end)
{
    return deserializer<Iterator, true>(begin, end);
}

template <typename Iterator>
deserializer<Iterator, false> make_unsafe_deserializer(const Iterator begin)
{
    // The end_ member will only be used by read_bytes() method.
    return deserializer<Iterator, false>(begin, Iterator.end());
}

} // namespace libbitcoin

#endif
