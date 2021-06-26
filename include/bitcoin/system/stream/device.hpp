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
#ifndef LIBBITCOIN_SYSTEM_STREAM_DEVICE_HPP
#define LIBBITCOIN_SYSTEM_STREAM_DEVICE_HPP

#include <iostream>
#include <locale>
#include <utility>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/system/type_constraints.hpp>

namespace libbitcoin {
namespace system {
    
/// Virtual base class for boost::iostreams::stream devices.
template <typename Container>
class device
{
public:
    /// device types
    typedef char char_type;
    typedef std::streamsize size_type;
    typedef typename Container::value_type value_type;

    /// seekable types
    typedef std::streampos position;
    typedef std::ios_base::seekdir direction;
    typedef boost::iostreams::stream_offset offset;
    typedef std::pair<char_type*, char_type*> sequence;

    //// close is implemented as a no-op by default.
    //// localizable devices (required for indirect output with std_io_tag).
    //// void imbue(const std::locale& loc) noexcept;
    //// optimally_buffered_tag (required for ostream_tag indirect devices).
    //// size_type optimal_buffer_size() const noexcept;
    //// peekable sources (may not be required for direct devices).
    //// bool putback(char_type) noexcept;
    //// Required for indirect input/output devices (apparently only filters).
    //// char_type get() noexcept;
    //// void put(Sink value, char_type character) noexcept;

    /// seekable input/output (required for direct devices).
    sequence input_sequence() noexcept;
    sequence output_sequence() noexcept;

    /// input/output devices (may not be required for direct devices).
    size_type read(char_type* buffer, size_type count) noexcept;
    size_type write(const char_type* buffer, size_type count) noexcept;

protected:
    device(size_type remaining) noexcept;
    virtual sequence do_sequence() noexcept;
    virtual void do_read(value_type* to, size_type size) noexcept;
    virtual void do_write(const value_type* from, size_type size) noexcept;

    size_type remaining_;
};

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/stream/device.ipp>

#endif
