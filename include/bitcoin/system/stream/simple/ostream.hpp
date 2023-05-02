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
#ifndef LIBBITCOIN_SYSTEM_STREAM_SIMPLE_OSTREAM_HPP
#define LIBBITCOIN_SYSTEM_STREAM_SIMPLE_OSTREAM_HPP

#include <ios>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)

/// Support for high level output operations on a byte buffer.
template <typename Sink, typename Character = char>
class ostream
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(ostream);

    using char_type = Character;
    using pos_type = typename std::basic_ios<char_type>::pos_type;
    using failure = typename std::ios_base::failure;

    using iostate = uint8_t;
    static constexpr iostate goodbit = 0;
    static constexpr iostate eofbit  = 1;
    static constexpr iostate failbit = 2;
    static constexpr iostate badbit  = 4;

    /// Construct the object.
    inline ostream(Sink& sink) NOEXCEPT;

    /// Return the relative output position indicator (zero-based).
    virtual inline pos_type tellp() const NOEXCEPT;

    /// Return state flags.
    virtual inline iostate rdstate() const NOEXCEPT;

    /// Set the stream error flags state in addition to currently set flags.
    virtual inline void setstate(iostate state) NOEXCEPT;

    /// Set the stream error state flags by assigning the state value.
    virtual inline void clear(iostate state=goodbit) NOEXCEPT;

    /// Write a block of characters, sets badbit on overflow.
    virtual inline void write(const char_type* data, pos_type size) NOEXCEPT;

    /// Synchronize with the underlying storage device.
    virtual inline void flush() NOEXCEPT;

private:
    inline bool is_overflow(pos_type size) const NOEXCEPT;

    uint8_t* position_;
    uint8_t* begin_;
    uint8_t* end_;
    iostate state_;
};

BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/stream/simple/ostream.ipp>

#endif
