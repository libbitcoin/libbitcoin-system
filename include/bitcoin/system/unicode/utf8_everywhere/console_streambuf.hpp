/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_UNICODE_UTF8_EVERYWHERE_CONSOLE_STREAMBUF_HPP
#define LIBBITCOIN_SYSTEM_UNICODE_UTF8_EVERYWHERE_CONSOLE_STREAMBUF_HPP

#include <streambuf>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Not thread safe, virtual.
/// Class to patch Windows stdin keyboard input, file input is not a problem.
/// This class and members are no-ops when called in non-vc++ builds.
/// When working in Windows console set font to "Lucida Console".
class BC_API console_streambuf
  : public std::wstreambuf
{
public:
    /// Initialize stdio to use utf8 translation on Windows.
    static void initialize(size_t stream_buffer_size) THROWS;

protected:
    /// Protected construction, use static initialize method.
    console_streambuf(const std::wstreambuf& stream_buffer,
        size_t stream_buffer_size) THROWS;

    /// Delete stream buffer.
    virtual ~console_streambuf() NOEXCEPT;

    /// Alternate console read.
    virtual std::streamsize xsgetn(wchar_t* buffer,
        std::streamsize size) THROWS;

    /// Alternate console read.
    virtual std::wstreambuf::int_type underflow() THROWS;

#ifdef HAVE_MSC
private:
    // These are not thread safe.

    // The constructed buffer size.
    size_t buffer_size_;

    // The dynamically-allocated buffers.
    wchar_t* buffer_;
#endif
};

} // namespace system
} // namespace libbitcoin

#endif
