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
#ifndef LIBBITCOIN_SYSTEM_STREAM_IOSTREAM_OSTREAM_IPP
#define LIBBITCOIN_SYSTEM_STREAM_IOSTREAM_OSTREAM_IPP

#include <algorithm>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)

template <typename Buffer, typename Character>
INLINE typename iostream<Buffer, Character>::pos_type
iostream<Buffer, Character>::tellp() const NOEXCEPT
{
    return static_cast<pos_type>(position_ - begin_);
}

template <typename Buffer, typename Character>
INLINE void
iostream<Buffer, Character>::write(const char_type* data, pos_type size) NOEXCEPT
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

template <typename Buffer, typename Character>
INLINE void
iostream<Buffer, Character>::flush() NOEXCEPT
{
}

BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin

#endif
