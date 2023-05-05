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
#ifndef LIBBITCOIN_SYSTEM_STREAM_IOSTREAM_IOSTREAM_IPP
#define LIBBITCOIN_SYSTEM_STREAM_IOSTREAM_IOSTREAM_IPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)

template <typename Buffer, typename Character>
INLINE iostream<Buffer, Character>::iostream(Buffer& buffer) NOEXCEPT
  : position_(buffer.data()),
    begin_(position_),
    end_(begin_ + buffer.size()),
    state_(goodbit)
{
}

template <typename Buffer, typename Character>
INLINE iostream<Buffer, Character>::iostream(uint8_t* begin,
    ptrdiff_t size) NOEXCEPT
  : position_(begin),
    begin_(position_),
    end_(begin_ + size),
    state_(goodbit)
{
}

template <typename Buffer, typename Character>
INLINE typename iostream<Buffer, Character>::iostate
iostream<Buffer, Character>::rdstate() const NOEXCEPT
{
    return state_;
}

template <typename Buffer, typename Character>
INLINE void
iostream<Buffer, Character>::setstate(iostate state) NOEXCEPT
{
    state_ |= state;
}

template <typename Buffer, typename Character>
INLINE void
iostream<Buffer, Character>::clear(iostate state) NOEXCEPT
{
    state_ = state;
}

// private
template <typename Buffer, typename Character>
INLINE bool
iostream<Buffer, Character>::is_overflow(pos_type size) const NOEXCEPT
{
    return (state_ != goodbit) || (size > (end_ - position_));
}

BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin

#endif
