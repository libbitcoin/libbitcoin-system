/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/unicode/utf8_everywhere/utf8_environment.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <iostream>
#include <locale>
#include <string>
#ifdef _MSC_VER
    #include <fcntl.h>
    #include <io.h>
    #include <windows.h>
#endif
#include <boost/filesystem.hpp>
#include <boost/locale.hpp>
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/utility/exceptions.hpp>
#include <bitcoin/system/utility/string.hpp>
#include <bitcoin/system/unicode/conversion.hpp>

// TODO: rename the classes in these sources.
#include <bitcoin/system/unicode/utf8_everywhere/utf8_console_streambuf.hpp>
#include <bitcoin/system/unicode/utf8_everywhere/utf8_istream.hpp>
#include <bitcoin/system/unicode/utf8_everywhere/utf8_ostream.hpp>

namespace libbitcoin {
namespace system {

// Stream utilities.
// ----------------------------------------------------------------------------

#ifdef _MSC_VER

static void set_utf8_stdio(FILE* file)
{
    if (_setmode(_fileno(file), _O_U8TEXT) == -1)
        throw runtime_exception("Could not set STDIO to utf8 mode.");
}

static void set_binary_stdio(FILE* file)
{
    if (_setmode(_fileno(file), _O_BINARY) == -1)
        throw runtime_exception("Could not set STDIO to binary mode.");
}

#else // _MSC_VER

static void set_utf8_stdio(FILE*)
{
}

static void set_binary_stdio(FILE*)
{
}

#endif

// Set stdio to use UTF8 translation on Win32.
void set_utf8_stdio()
{
    set_utf8_stdin();
    set_utf8_stdout();
    set_utf8_stderr();
}

// Set stdio to use UTF8 translation on Win32.
void set_utf8_stdin()
{
    set_utf8_stdio(stdin);
}

// Set stdio to use UTF8 translation on Win32.
void set_utf8_stdout()
{
    set_utf8_stdio(stdout);
}

// Set stdio to use UTF8 translation on Win32.
void set_utf8_stderr()
{
    set_utf8_stdio(stderr);
}

// Set stdio to use UTF8 translation on Win32.
void set_binary_stdin()
{
    set_binary_stdio(stdin);
}

// Set stdio to use UTF8 translation on Win32.
void set_binary_stdout()
{
    set_binary_stdio(stdout);
}

// C-style UTF8/UTF16 conversions.
// ----------------------------------------------------------------------------

// All non-leading bytes of utf8 have the same two bit prefix.
inline bool is_utf8_trailing_byte(char byte)
{
    // 10xxxxxx
    return ((0xc0 & byte) == 0x80);
}

// Determine if the full sequence is a valid utf8 character.
static bool is_utf8_leading_byte(char byte, size_t size)
{
    BITCOIN_ASSERT(size <= utf8_max_character_size);

    // See tools.ietf.org/html/rfc3629#section-3 for definition.
    switch (size)
    {
    case 1:
        // 0xxxxxxx
        return ((0x80 & byte) == 0x00);
    case 2:
        // 110xxxxx 10xxxxxx
        return ((0xe0 & byte) == 0xc0);
    case 3:
        // 1110xxxx 10xxxxxx 10xxxxxx
        return ((0xf0 & byte) == 0xe0);
    case 4:
        // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        return ((0xf8 & byte) == 0xf0);
    default:
        return false;
    }
}

// TODO: walk backwards to the first leading byte (performance).
// The number of bytes of a partial utf8 character at the end of text.
// This assumes that the text is well formed utf8 but truncated at any point.
size_t utf8_remainder_size(const char text[], size_t size)
{
    if (size == 0u)
        return 0;

    for (size_t length = 1; length <= std::min(utf8_max_character_size, size);
        ++length)
    {
        const auto byte = text[size - length];

        // Skip trailing bytes.
        if (is_utf8_trailing_byte(byte))
            continue;

        // If not a valid character length, return truncated length.
        return !is_utf8_leading_byte(byte, length) ? length : 0;
    }

    // The text is not well-formed utf8, assume no truncation.
    return 0;
}

// Convert utf16 wchar_t buffer to utf8 char buffer.
// This is used in wmain for conversion of wide args and environ on Win32.
size_t to_utf8(char out_to[], size_t to_bytes, const wchar_t from[],
    size_t from_chars)
{
    if (from == nullptr || out_to == nullptr || from_chars == 0 ||
        to_bytes < (from_chars * utf8_max_character_size))
        return 0;

    const std::wstring wide{ from, &from[from_chars] };
    const auto narrow = to_utf8(wide);
    const auto bytes = narrow.size();

    if (bytes <= to_bytes)
        std::memcpy(out_to, narrow.data(), bytes);

    return bytes > to_bytes ? 0 : bytes;
}

// Convert utf8 char buffer to utf16 wchar_t buffer, with truncation handling.
// Truncation results from having split the input buffer arbitrarily (stream).
size_t to_utf16(size_t& remainder, wchar_t out_to[], size_t to_chars,
    const char from[], size_t from_bytes)
{
    remainder = 0;
    if (from == nullptr || out_to == nullptr || from_bytes == 0 ||
        to_chars < from_bytes)
        return 0;

    // Calculate a character break offset of 0..3 bytes.
    remainder = utf8_remainder_size(from, from_bytes);

    const std::string narrow{ from, &from[from_bytes - remainder] };
    const auto wide = to_utf16(narrow);
    const auto chars = wide.size();

    if (chars <= to_chars)
        std::wmemcpy(out_to, wide.data(), chars);

    return chars > to_chars ? 0 : chars;
}

#ifdef _MSC_VER

// Stream utilities.
// ----------------------------------------------------------------------------

// The width of utf16 stdio buffers.
constexpr size_t utf16_buffer_size = 256;

// Ensure console_streambuf::initialize is called only once.
static std::once_flag io_mutex;

// Static initializer for bc::system::cin.
std::istream& cin_stream()
{
    std::call_once(io_mutex, console_streambuf::initialize, utf16_buffer_size);
    static unicode_istream input(std::cin, std::wcin, utf16_buffer_size);
    return input;
}

// Static initializer for bc::system::cout.
std::ostream& cout_stream()
{
    std::call_once(io_mutex, console_streambuf::initialize, utf16_buffer_size);
    static unicode_ostream output(std::cout, std::wcout, utf16_buffer_size);
    return output;
}

// Static initializer for bc::system::cerr.
std::ostream& cerr_stream()
{
    std::call_once(io_mutex, console_streambuf::initialize, utf16_buffer_size);
    static unicode_ostream error(std::cerr, std::wcerr, utf16_buffer_size);
    return error;
}

// BC_USE_LIBBITCOIN_MAIN
// ----------------------------------------------------------------------------

void free_environment(char* environment[])
{
    if (environment != nullptr)
    {
        for (auto index = 0; environment[index] != nullptr; ++index)
            std::free(environment[index]);

        std::free(environment);
    }
}

// Convert UTF16/wchar_t argument buffer to utf8/char argument buffer.
// Caller (or compiler, in case of environment replacement) must free.
char** allocate_environment(int argc, wchar_t* argv[])
{
    // Allocate argument pointer array.
    const auto size = static_cast<size_t>(argc) + 1u;
    const auto buffer = std::malloc(size * sizeof(char*));
    const auto arguments = reinterpret_cast<char**>(buffer);

    // Out of memory.
    if (arguments == nullptr)
        return nullptr;

    // Convert each argument, allocate and assign to pointer array.
    for (auto arg = 0; arg < argc; arg++)
    {
        // Guard overflow of std::wstring assignment.
        if (wcsnlen_s(argv[arg], max_size_t) == max_size_t)
            return nullptr;

        const auto utf8 = to_utf8(argv[arg]);
        const auto size = utf8.size();

        // Guard terminator addition.
        if (size == max_size_t)
            return nullptr;

        arguments[arg] = reinterpret_cast<char*>(std::malloc(size + 1u));

        // Out of memory.
        if (arguments[arg] == nullptr)
        {
            free_environment(arguments);
            return nullptr;
        }

        std::copy_n(utf8.begin(), size, arguments[arg]);

        // Row terminator.
        arguments[arg][size] = '\0';
    }

    // Table terminator.
    arguments[argc] = '\0';
    return arguments;
}

// Convert UTF16/wchar_t environment buffer to utf8/char environment buffer.
char** allocate_environment(wchar_t* environment[])
{
    int count;
    for (count = 0; environment[count] != nullptr; count++);
    return allocate_environment(count, environment);
}

int call_utf8_main(int argc, wchar_t* argv[],
    int(*main)(int argc, char* argv[]))
{
    // When working with boost and utf8 narrow characters on Win32 the thread
    // must be configured for utf8. When working with boost::filesystem::path
    // the static path object must be imbued with the utf8 locale or paths will
    // be incorrectly translated.
    constexpr auto utf8_locale_name = "en_US.UTF8";
    boost::locale::generator locale;
    std::locale::global(locale(utf8_locale_name));
    boost::filesystem::path::imbue(std::locale());

    auto backup = environ;
    auto environment = allocate_environment(_wenviron);

    // If a new environment cannot be allocated control returns immediately.
    if (environment == nullptr)
        return 0;

    environ = environment;

    // If new arguments cannot be allocated control returns immediately.
    auto arguments = allocate_environment(argc, argv);
    if (arguments == nullptr)
    {
        free_environment(arguments);
        environ = backup;
        return 0;
    }

    const auto result = (main)(argc, arguments);
    free_environment(arguments);
    free_environment(environ);
    environ = backup;
    return result;
}

#endif // _MSC_VER

} // namespace system
} // namespace libbitcoin
