/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/unicode/utf8_everywhere/stdio.hpp>
#ifdef HAVE_MSC
    #include <fcntl.h>
    #include <io.h>
    #include <windows.h>
#else
    #include <termios.h>
#endif
#include <mutex>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/unicode/utf8_everywhere/console_streambuf.hpp>
#include <bitcoin/system/unicode/utf8_everywhere/unicode_istream.hpp>
#include <bitcoin/system/unicode/utf8_everywhere/unicode_ostream.hpp>

namespace libbitcoin {
namespace system {

LCOV_EXCL_START("Untestable but visually-verifiable section.")


// The width of utf16 stdio buffers.
constexpr size_t utf16_buffer_size = 256;

// Ensure console_streambuf::initialize is called only once for the process.
static std::once_flag io_mutex{};

// Static initializer for bc::system::cin.
std::istream& cin_stream() THROWS
{
    std::call_once(io_mutex, console_streambuf::set_input, utf16_buffer_size);
    static unicode_istream input(std::cin, std::wcin, utf16_buffer_size);
    return input;
}

// Static initializer for bc::system::cout.
std::ostream& cout_stream() THROWS
{
    std::call_once(io_mutex, console_streambuf::set_output);
    static unicode_ostream output(std::cout, std::wcout, utf16_buffer_size);
    return output;
}

// Static initializer for bc::system::cerr.
std::ostream& cerr_stream() THROWS
{
    std::call_once(io_mutex, console_streambuf::set_output);
    static unicode_ostream error(std::cerr, std::wcerr, utf16_buffer_size);
    return error;
}

inline void set_utf8_stdio(FILE* file) THROWS
{
    if (_setmode(_fileno(file), _O_U8TEXT) == -1)
        throw runtime_exception{ "Could not set STDIO to utf8 mode." };
}

inline void set_binary_stdio(FILE* file) THROWS
{
    if (_setmode(_fileno(file), _O_BINARY) == -1)
        throw runtime_exception{ "Could not set STDIO to binary mode." };
}

#ifndef _WIN32
void unset_console_echo()
{
    termios terminal{};
    tcgetattr(0, &terminal);
    terminal.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &terminal);
}}

void unset_console_echo() 
{
    termios terminal{};
    tcgetattr(0, &terminal);
    terminal.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &terminal);
}
#else
void set_console_echo() 
{
    // TODO: Windows console echo handling if needed
}
void unset_console_echo() 
{
    // TODO: Windows console echo handling if needed
}
#endif



std::istream& cin_stream() THROWS { return std::cin; }
std::ostream& cout_stream() THROWS { return std::cout; }
std::ostream& cerr_stream() THROWS {  return std::cerr; }
inline void set_utf8_stdio(FILE*) THROWS {}
inline void set_binary_stdio(FILE*) THROWS {}

#ifndef _WIN32
void set_console_echo() {
    termios terminal{};
    tcgetattr(0, &terminal);
    terminal.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &terminal);
}

void unset_console_echo() {
    termios terminal{};
    tcgetattr(0, &terminal);
    terminal.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &terminal);
}
#else
void set_console_echo() {
    // TODO: Windows console echo handling if needed
}
void unset_console_echo() {
    // TODO: Windows console echo handling if needed
}
#endif



void set_utf8_stdio() THROWS
{
    set_utf8_stdin();
    set_utf8_stdout();
    set_utf8_stderr();
}

void set_utf8_stdin() THROWS
{
    set_utf8_stdio(stdin);
}

void set_utf8_stdout() THROWS
{
    set_utf8_stdio(stdout);
}

void set_utf8_stderr() THROWS
{
    set_utf8_stdio(stderr);
}

void set_binary_stdin() THROWS
{
    set_binary_stdio(stdin);
}

void set_binary_stdout() THROWS
{
    set_binary_stdio(stdout);
}

LCOV_EXCL_STOP()

} // namespace system
} // namespace libbitcoin
