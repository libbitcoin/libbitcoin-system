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
#ifndef LIBBITCOIN_SYSTEM_STREAM_SIMPLE_ISTREAM_HPP
#define LIBBITCOIN_SYSTEM_STREAM_SIMPLE_ISTREAM_HPP

#include <algorithm>
#include <ios>
#include <string>
#include <type_traits>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)

/// Support for high level input operations on a byte buffer.
template <typename Source, typename Character = char>
class istream
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(istream);

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

    using seekdir = uint8_t;
    static constexpr seekdir beg = 0;
    static constexpr seekdir cur = 1;
    static constexpr seekdir end = 2;

    /// Construct the object.
    inline istream(const Source& source) NOEXCEPT
      : position_(source.data()),
        begin_(position_),
        end_(begin_ + source.size()),
        state_(goodbit)
    {
    }

    /// Return the relative input position indicator (zero-based).
    virtual inline pos_type tellg() const NOEXCEPT
    {
        return static_cast<pos_type>(position_ - begin_);
    }

    /// Set the relative input position indicator (zero-based).
    virtual inline istream& seekg(off_type offset, seekdir direction) NOEXCEPT
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

    /// Return state flags.
    virtual inline iostate rdstate() const NOEXCEPT
    {
        return state_;
    }

    /// Set the stream error flags state in addition to currently set flags.
    virtual inline void setstate(iostate state) NOEXCEPT
    {
        state_ |= state;
    }

    /// Set the stream error state flags by assigning the state value.
    virtual inline void clear(iostate state=goodbit) NOEXCEPT
    {
        state_ = state;
    }

    /// Read a block of characters, sets badbit on underflow.
    virtual inline void read(char_type* data, pos_type size) NOEXCEPT
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

    /// Read the next character without advancing, sets badbit on underflow.
    virtual inline int_type peek() NOEXCEPT
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

private:
    static constexpr bool is_positive(off_type value) NOEXCEPT
    {
        return !is_zero(value) && !system::is_negative(value);
    }

    inline bool is_overflow(pos_type size) const NOEXCEPT
    {
        return (state_ != goodbit) || (size > (end_ - position_));
    }

    const uint8_t* position_;
    const uint8_t* begin_;
    const uint8_t* end_;
    iostate state_;
};

BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin

#endif
