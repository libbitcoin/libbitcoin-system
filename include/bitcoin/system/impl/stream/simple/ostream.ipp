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
#ifndef LIBBITCOIN_SYSTEM_STREAM_SIMPLE_OSTREAM_IPP
#define LIBBITCOIN_SYSTEM_STREAM_SIMPLE_OSTREAM_IPP

#include <algorithm>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

template <typename Sink, typename Character>
inline ostream<Sink, Character>::ostream(Sink& sink) NOEXCEPT
  : position_(sink.data()),
    begin_(position_),
    end_(begin_ + sink.size()),
    state_(goodbit)
{
}

template <typename Sink, typename Character>
inline typename ostream<Sink, Character>::pos_type
ostream<Sink, Character>::tellp() const NOEXCEPT
{
    return static_cast<pos_type>(position_ - begin_);
}

template <typename Sink, typename Character>
inline typename ostream<Sink, Character>::iostate
ostream<Sink, Character>::rdstate() const NOEXCEPT
{
    return state_;
}

template <typename Sink, typename Character>
inline void
ostream<Sink, Character>::setstate(iostate state) NOEXCEPT
{
    state_ |= state;
}

template <typename Sink, typename Character>
inline void
ostream<Sink, Character>::clear(iostate state) NOEXCEPT
{
    state_ = state;
}

template <typename Sink, typename Character>
inline void
ostream<Sink, Character>::write(const char_type* data, pos_type size) NOEXCEPT
{
    if (is_overflow(size))
    {
        setstate(badbit);
        return;
    }

    BC_PUSH_WARNING(NO_UNSAFE_COPY_N)
    std::copy_n(data, size, position_);
    BC_POP_WARNING()

    position_ += size;
}

template <typename Sink, typename Character>
inline void
ostream<Sink, Character>::flush() NOEXCEPT
{
    //  no-op.
}

// private
template <typename Sink, typename Character>
inline bool
ostream<Sink, Character>::is_overflow(pos_type size) const NOEXCEPT
{
    return (state_ != goodbit) || (size > (end_ - position_));
}

} // namespace system
} // namespace libbitcoin

#endif
