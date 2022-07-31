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
#include <bitcoin/system/unicode/utf8_everywhere/console_streambuf.hpp>

#include <iostream>
#include <new>
#include <streambuf>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

#ifdef HAVE_MSC

// Get Windows input handle.
static LPVOID get_input_handle() THROWS
{
    const auto handle = GetStdHandle(STD_INPUT_HANDLE);
    if (handle == INVALID_HANDLE_VALUE || handle == nullptr)
        throw runtime_exception("Failed to get input handle.");

    return handle;
}

// Hack for faulty std::wcin translation of non-ASCII keyboard input.
void console_streambuf::initialize(size_t stream_buffer_size) THROWS
{
    // Set the console to operate in UTF-8 for this process.
    if (SetConsoleCP(CP_UTF8) == FALSE)
        throw runtime_exception("Failed to set console to utf8.");

    DWORD console_mode;
    if (GetConsoleMode(get_input_handle(), &console_mode) != FALSE)
    {
        static console_streambuf buffer(*std::wcin.rdbuf(), stream_buffer_size);
        std::wcin.rdbuf(&buffer);
    }
}

console_streambuf::console_streambuf(
    const std::wstreambuf& stream_buffer, size_t size) THROWS
    : buffer_size_(size), buffer_(new wchar_t[buffer_size_]),
    std::wstreambuf(stream_buffer)
{
}

console_streambuf::~console_streambuf() NOEXCEPT
{
    delete[] buffer_;
}

std::streamsize console_streambuf::xsgetn(wchar_t* buffer,
    std::streamsize size) THROWS
{
    DWORD read_bytes;

    if (ReadConsoleW(get_input_handle(), buffer, static_cast<DWORD>(size),
        &read_bytes, nullptr) == FALSE)
        throw runtime_exception("Failed to read from console.");

    return static_cast<std::streamsize>(read_bytes);
}

std::wstreambuf::int_type console_streambuf::underflow() THROWS
{
    if (gptr() == nullptr || gptr() >= egptr())
    {
        const auto length = xsgetn(buffer_, buffer_size_);
        if (length > 0)
            setg(buffer_, buffer_, &buffer_[length]);
    }

    return (gptr() == nullptr || gptr() >= egptr()) ? traits_type::eof() :
        traits_type::to_int_type(*gptr());
}

#else

void console_streambuf::initialize(size_t) THROWS
{
}

console_streambuf::console_streambuf(const std::wstreambuf&,
    size_t) THROWS
{
}

console_streambuf::~console_streambuf() NOEXCEPT
{
}

std::streamsize console_streambuf::xsgetn(wchar_t*,
    std::streamsize) THROWS
{
    return 0;
}

std::wstreambuf::int_type console_streambuf::underflow() THROWS
{
    return traits_type::to_int_type(*gptr());
}

#endif

} // namespace system
} // namespace libbitcoin
