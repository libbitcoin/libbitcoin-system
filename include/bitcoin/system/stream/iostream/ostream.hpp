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
#ifndef LIBBITCOIN_SYSTEM_STREAM_IOSTREAM_OSTREAM_HPP
#define LIBBITCOIN_SYSTEM_STREAM_IOSTREAM_OSTREAM_HPP

#include <ios>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Support for high level output operations on a byte buffer.
/// Cannot derive from ostream and cannot make both share an interface.
/// So this is duck-typed to the subset of std::ostream required by writers.
template <typename Character = char>
class ostream
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(ostream);

    using char_type = Character;
    using int_type = typename std::basic_ios<char_type>::int_type;
    using off_type = typename std::basic_ios<char_type>::off_type;
    using pos_type = typename std::basic_ios<char_type>::pos_type;
    using failure = typename std::ios_base::failure;

    using iostate = uint8_t;
    static constexpr iostate goodbit = 0;
    static constexpr iostate eofbit  = 1;
    static constexpr iostate failbit = 2;
    static constexpr iostate badbit  = 4;

    /// Construct the object.
    template <typename Buffer>
    ostream(Buffer& buffer) NOEXCEPT;
    ostream(uint8_t* begin, ptrdiff_t size) NOEXCEPT;

    /// Return state flags.
    virtual inline iostate rdstate() const NOEXCEPT;

    /// Set the stream error flags state in addition to currently set flags.
    virtual inline void setstate(iostate state) NOEXCEPT;

    /// Set the stream error state flags by assigning the state value.
    virtual inline void clear(iostate state=goodbit) NOEXCEPT;

    /// Return the relative output position indicator (zero-based).
    virtual inline pos_type tellp() const NOEXCEPT;

    /// Write a block of characters, sets badbit on overflow.
    virtual void write(const char_type* data, std::streamsize count) NOEXCEPT;

    /// Synchronize with the underlying storage device (no-op).
    virtual void flush() NOEXCEPT;

private:
    bool is_overflow(pos_type size) const NOEXCEPT;

    uint8_t* position_;
    uint8_t* begin_;
    uint8_t* end_;
    iostate state_;
};

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/stream/iostream/ostream.ipp>

#endif
