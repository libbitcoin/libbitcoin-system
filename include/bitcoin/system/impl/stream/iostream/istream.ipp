/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_STREAM_IOSTREAM_ISTREAM_IPP
#define LIBBITCOIN_SYSTEM_STREAM_IOSTREAM_ISTREAM_IPP

#include <algorithm>
#include <type_traits>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)

template <typename Buffer, typename Character>
INLINE istream<Buffer, Character>::istream(const Buffer& buffer) NOEXCEPT
  : position_(buffer.data()),
    begin_(position_),
    end_(begin_ + buffer.size()),
    state_(goodbit)
{
}

template <typename Buffer, typename Character>
INLINE istream<Buffer, Character>::istream(const uint8_t* begin,
    ptrdiff_t size) NOEXCEPT
  : position_(begin),
    begin_(position_),
    end_(begin_ + size),
    state_(goodbit)
{
}

template <typename Buffer, typename Character>
INLINE typename istream<Buffer, Character>::iostate
istream<Buffer, Character>::rdstate() const NOEXCEPT
{
    return state_;
}

template <typename Buffer, typename Character>
INLINE void
istream<Buffer, Character>::setstate(iostate state) NOEXCEPT
{
    state_ |= state;
}

template <typename Buffer, typename Character>
INLINE void
istream<Buffer, Character>::clear(iostate state) NOEXCEPT
{
    state_ = state;
}

template <typename Buffer, typename Character>
INLINE typename istream<Buffer, Character>::pos_type
istream<Buffer, Character>::tellg() const NOEXCEPT
{
    return static_cast<pos_type>(position_ - begin_);
}

template <typename Buffer, typename Character>
INLINE istream<Buffer, Character>&
istream<Buffer, Character>::seekg(off_type offset, seekdir direction) NOEXCEPT
{
    if (state_ != goodbit)
        return *this;

    using namespace system;
    switch (direction)
    {
        case beg:
        {
            if (is_negative(offset) || (offset > (end_ - begin_)))
            {
                setstate(badbit);
                break;
            }

            position_ = begin_ + offset;
            break;
        }
        case cur:
        {
            if ((is_negative(offset) && (offset < (begin_ - position_))) ||
                (is_positive(offset) && (offset > (end_ - position_))))
            {
                setstate(badbit);
                break;
            }

            position_ = position_ + offset;
            break;
        }
        case end:
        {
            if (is_positive(offset) || (offset < (begin_ - end_)))
            {
                setstate(badbit);
                break;
            }

            position_ = end_ + offset;
            break;
        }
        default:
        {
            setstate(failbit);
            break;
        }
    }

    return *this;
}

template <typename Buffer, typename Character>
INLINE typename istream<Buffer, Character>::int_type
istream<Buffer, Character>::peek() NOEXCEPT
{
    constexpr auto eof = std::char_traits<Character>::eof();

    if (is_overflow(1))
    {
        setstate(badbit);
        return eof;
    }

    const uint8_t value = *position_;
    return system::sign_cast<int_type>(value);
}

template <typename Buffer, typename Character>
INLINE void
istream<Buffer, Character>::read(char_type* data, pos_type size) NOEXCEPT
{
    if (is_overflow(size))
    {
        setstate(badbit);
        return;
    }

    BC_PUSH_WARNING(NO_UNSAFE_COPY_N)
    std::copy_n(position_, size, data);
    BC_POP_WARNING()

    position_ += size;
}

// private
template <typename Buffer, typename Character>
constexpr bool
istream<Buffer, Character>::is_positive(off_type value) NOEXCEPT
{
    return !is_zero(value) && !system::is_negative(value);
}

// private
template <typename Buffer, typename Character>
INLINE bool
istream<Buffer, Character>::is_overflow(pos_type size) const NOEXCEPT
{
    return (state_ != goodbit) || (size > (end_ - position_));
}

BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin

#endif
