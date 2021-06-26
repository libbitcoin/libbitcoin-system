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
#ifndef LIBBITCOIN_SYSTEM_STREAM_DEVICE_IPP
#define LIBBITCOIN_SYSTEM_STREAM_DEVICE_IPP

#include <algorithm>
#include <iterator>
#include <locale>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/math/sign.hpp>

namespace libbitcoin {
namespace system {

////// Device default is 4096.
////// Allocated by the iostreams library when a filter or device is attached to a
////// stream_buffer or stream, or when it is added to a chain.
////constexpr auto buffer_size = BOOST_IOSTREAMS_DEFAULT_DEVICE_BUFFER_SIZE;

// protected
template <typename Container>
device<Container>::device(size_type remaining) noexcept
  : remaining_(remaining)
{
}

// public, device interface
// ----------------------------------------------------------------------------

////template <typename Container>
////void device<Container>::imbue(const std::locale& loc) noexcept
////{
////    // Required for std_io_tag but a no-op as all text is treated as UTF8.
////}

////template <typename Container>
////typename device<Container>::size_type
////device<Container>::optimal_buffer_size() const noexcept
////{
////    return buffer_size;
////}

////template <typename Container>
////bool device<Container>::putback(char_type) noexcept
////{
////    if (from_ == container_.begin())
////        return false;
////
////    // The character value is ignored, as it is presumed to still exist.
////    // This precludes putting back a different character and will not detect an
////    // error of attempting to do so, assuming that is even considered an error.
////    --size_;
////    std::advance(from_, negative_one);
////    return true;
////}

template <typename Container>
typename device<Container>::sequence
device<Container>::input_sequence() noexcept
{
    // Required to match output_sequence for read/write devices.
    return do_sequence();
}

template <typename Container>
typename device<Container>::sequence
device<Container>::output_sequence() noexcept
{
    // Required to match input_sequence for read/write devices.
    return do_sequence();
}

template <typename Container>
typename device<Container>::size_type
device<Container>::read(char_type* buffer, size_type count) noexcept
{
    if (is_null(buffer) || is_negative(count))
        return negative_one;

    const auto size = std::min(remaining_, count);
    do_read(reinterpret_cast<value_type*>(buffer), size);
    remaining_ -= size;

    // eof is indicated by the return of a value less than count.
    return size;
}

template <typename Container>
typename device<Container>::size_type
device<Container>::write(const char_type* buffer, size_type count) noexcept
{
    if (is_null(buffer) || is_negative(count))
        return negative_one;

    const auto size = std::min(remaining_, count);
    do_write(reinterpret_cast<const value_type*>(buffer), size);
    remaining_ -= size;

    // eof is indicated by the return of a value less than count.
    return size;
}

////template <typename Container>
////typename device<Container>::char_type device<Container>::get() noexcept
////{
////    char_type character;
////    do_read(&character, one);
////    return character;
////}

////template <typename Container>
////void device<Container>::put(Sink value, char_type character) noexcept
////{
////    do_write(&character, one);
////}

// protected virtual
// ----------------------------------------------------------------------------

template <typename Container>
typename device<Container>::sequence
device<Container>::do_sequence() noexcept
{
    // not implemented, override.
    return { nullptr, nullptr };
}

// protected virtual
template <typename Container>
void device<Container>::do_read(value_type* buffer, size_type size) noexcept
{
    // not implemented, override.
    std::fill(buffer, std::next(buffer, size), 0x00);
}

// protected virtual
template <typename Container>
void device<Container>::do_write(const value_type*, size_type) noexcept
{
    // not implemented, override.
}

} // namespace system
} // namespace libbitcoin

#endif

// unfinished.
////template <typename Container>
////typename copy_source<Container>::position
////copy_source<Container>::seek(offset count, direction way) noexcept
////{
////    const auto scope = container_.size();
////    const auto expended = scope - size_;
////    const auto remaining = size_;
////
////    // Boost example code has over overflows depending on types in use.
////    if (way == std::ios_base::beg)
////    {
////        if (!is_negative(count) && is_lesser(count, scope))
////        {
////            size_ = container_.size() - count;
////            from_ = std::next(container_.begin(), count);
////            return count;
////        }
////    }
////    else if (way == std::ios_base::end)
////    {
////        // zero specifies the last byte, not the end.
////        if (is_negative(count) && is_lesser(absolute(count), scope))
////        {
////            size_ = container_.size() + count;
////            from_ = std::next(container_.begin(), count);
////            return count;
////        }
////    }
////    else if (way == std::ios_base::cur)
////    {
////        if (( is_negative(count) || is_lesser(count, remaining)) &&
////            (!is_negative(count) || is_lesser(absolute(count), expended))
////            return set_position(offset count, direction way);
////    }
////
////    // Avoid exceptions. If parameterization is invalid the predictable
////    // return is zero, resetting the position to start. This is non-standard.
////    return 0;
////}