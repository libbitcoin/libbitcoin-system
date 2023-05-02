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
#ifndef LIBBITCOIN_SYSTEM_STREAM_SIMPLE_IOSTREAM_HPP
#define LIBBITCOIN_SYSTEM_STREAM_SIMPLE_IOSTREAM_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/stream/simple/istream.hpp>
#include <bitcoin/system/stream/simple/ostream.hpp>

namespace libbitcoin {
namespace system {

// The only multiple inheritance conflicts are resolved below.
BC_PUSH_WARNING(DIAMOND_INHERITANCE)

/// Support for high level input and output operations on a byte buffer.
template <typename Buffer, typename Character = char>
class iostream
  : public istream<Buffer, Character>,
    public ostream<Buffer, Character>
{
    DEFAULT_COPY_MOVE(iostream);

    /// Common based for state methods, just pick one.
    using base = istream<Buffer, Character>;

    /// Construct the object.
    iostream(Buffer& buffer) NOEXCEPT
      : istream<Buffer, Character>(buffer),
        ostream<Buffer, Character>(buffer)
    {
    }

    // Two base destructor calls order is unimportant.
    ~iostream() override = default;

    inline typename base::iostate rdstate() const NOEXCEPT override
    {
        return base::rdstate();
    }

    inline void setstate(typename base::iostate state) NOEXCEPT override
    {
        base::setstate(state);
    }

    inline void clear(
        typename base::iostate state=base::goodbit) NOEXCEPT override
    {
        base::clear(state);
    }
};

BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin

#endif
