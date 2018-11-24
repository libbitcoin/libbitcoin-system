/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/unicode/console_streambuf.hpp>

#include <cstddef>
#include <iostream>
#include <new>
#include <streambuf>
#include <bitcoin/system/math/limits.hpp>

#ifdef _MSC_VER
#include <windows.h>

// Get Windows input handle.
static LPVOID get_input_handle()
{
    auto handle = GetStdHandle(STD_INPUT_HANDLE);
    if (handle == INVALID_HANDLE_VALUE || handle == nullptr)
        throw std::ios_base::failure("Failed to get input handle.");

    return handle;
}
#endif

namespace libbitcoin {
namespace system {

// This class/mathod is a no-op on non-windows platforms.
// When working in Windows console set font to "Lucida Console".
// This is the factory method to privately instantiate a singleton class.
void console_streambuf::initialize(
#ifdef _MSC_VER
    size_t size)
#else
    size_t)
#endif
{
#ifdef _MSC_VER
    // Set the console to operate in UTF-8 for this process.
    if (SetConsoleCP(CP_UTF8) == FALSE)
        throw std::ios_base::failure("Failed to set console to utf8.");

    DWORD console_mode;
    if (GetConsoleMode(get_input_handle(), &console_mode) != FALSE)
    {
        // Hack for faulty std::wcin translation of non-ASCII keyboard input.
        static console_streambuf buffer(*std::wcin.rdbuf(), size);
        std::wcin.rdbuf(&buffer);
    }
#endif
}

console_streambuf::console_streambuf(
#ifdef _MSC_VER
    const std::wstreambuf& stream_buffer, size_t size)
#else
    const std::wstreambuf&, size_t)
#endif
#ifdef _MSC_VER
  : buffer_size_(size), buffer_(new wchar_t[buffer_size_]),
    std::wstreambuf(stream_buffer)
#endif
{
}

console_streambuf::~console_streambuf()
{
#ifdef _MSC_VER
    delete[] buffer_;
#endif
}

std::streamsize console_streambuf::xsgetn(
#ifdef _MSC_VER
    wchar_t* buffer, std::streamsize size)
#else
    wchar_t*, std::streamsize)
#endif
{
    std::streamsize read_size = 0;

#ifdef _MSC_VER
    DWORD read_bytes;
    const auto result = ReadConsoleW(get_input_handle(), buffer,
        static_cast<DWORD>(size), &read_bytes, nullptr);

    if (result == FALSE)
        throw std::iostream::failure("Failed to read from console.");

    read_size = static_cast<std::streamsize>(read_bytes);
#endif

    return read_size;
}

std::wstreambuf::int_type console_streambuf::underflow()
{
#ifdef _MSC_VER
    if (gptr() == nullptr || gptr() >= egptr())
    {
        const auto length = xsgetn(buffer_, buffer_size_);
        if (length > 0)
            setg(buffer_, buffer_, &buffer_[length]);
    }

    if (gptr() == nullptr || gptr() >= egptr())
        return traits_type::eof();
#endif

    return traits_type::to_int_type(*gptr());
}

} // namespace system
} // namespace libbitcoin
