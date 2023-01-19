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
#include <bitcoin/system/unicode/utf8_everywhere/environment.hpp>

#include <algorithm>
#include <cwchar>
#include <filesystem>
#include <iostream>
#include <locale>
#include <mutex>
#include <utility>
#ifdef HAVE_MSC
    #include <fcntl.h>
    #include <io.h>
    #include <shlobj.h>
#endif
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/unicode/ascii.hpp>
#include <bitcoin/system/unicode/conversion.hpp>
#include <bitcoin/system/unicode/utf8_everywhere/console_streambuf.hpp>
#include <bitcoin/system/unicode/utf8_everywhere/unicode_istream.hpp>
#include <bitcoin/system/unicode/utf8_everywhere/unicode_ostream.hpp>

namespace libbitcoin {
namespace system {
    
BC_PUSH_WARNING(NO_ARRAY_TO_POINTER_DECAY)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
BC_PUSH_WARNING(NO_MALLOC_OR_FREE)
BC_PUSH_WARNING(NO_UNSAFE_COPY_N)

// All pointers (except (*main)) are guarded, warning is invalid.
BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)

// Stream utilities.
// ----------------------------------------------------------------------------

LCOV_EXCL_START("Untestable but visually-verifiable section.")

#ifdef HAVE_MSC

static void set_utf8_stdio(FILE* file) THROWS
{
    if (_setmode(_fileno(file), _O_U8TEXT) == -1)
        throw runtime_exception{ "Could not set STDIO to utf8 mode." };
}

static void set_binary_stdio(FILE* file) THROWS
{
    if (_setmode(_fileno(file), _O_BINARY) == -1)
        throw runtime_exception{ "Could not set STDIO to binary mode." };
}

#else // HAVE_MSC

static void set_utf8_stdio(FILE*) THROWS
{
}

static void set_binary_stdio(FILE*) THROWS
{
}

#endif

// Set stdio to use UTF8 translation on Win32.
void set_utf8_stdio() THROWS
{
    set_utf8_stdin();
    set_utf8_stdout();
    set_utf8_stderr();
}

// Set stdio to use UTF8 translation on Win32.
void set_utf8_stdin() THROWS
{
    set_utf8_stdio(stdin);
}

// Set stdio to use UTF8 translation on Win32.
void set_utf8_stdout() THROWS
{
    set_utf8_stdio(stdout);
}

// Set stdio to use UTF8 translation on Win32.
void set_utf8_stderr() THROWS
{
    set_utf8_stdio(stderr);
}

// Set stdio to use UTF8 translation on Win32.
void set_binary_stdin() THROWS
{
    set_binary_stdio(stdin);
}

// Set stdio to use UTF8 translation on Win32.
void set_binary_stdout() THROWS
{
    set_binary_stdio(stdout);
}

LCOV_EXCL_STOP()

// C-style UTF8/UTF16 conversions.
// ----------------------------------------------------------------------------

// All non-leading bytes of utf8 have the same two bit prefix.
constexpr bool is_utf8_trailing_byte(char byte) NOEXCEPT
{
    // 10xxxxxx
    return ((0xc0 & byte) == 0x80);
}

// Determine if the full sequence is a valid utf8 character.
constexpr bool is_utf8_leading_byte(char byte, size_t size) NOEXCEPT
{
    BC_ASSERT(size <= utf8_max_character_size);

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
size_t utf8_remainder_size(const char text[], size_t size) NOEXCEPT
{
    if (is_zero(size) || is_null(text))
        return zero;

    for (auto length = one; length <= std::min(utf8_max_character_size, size);
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
    return zero;
}

// Convert utf16 wchar_t buffer to utf8 char buffer.
// This is used in wmain for conversion of wide args and environ on Win32.
size_t to_utf8(char out_to[], size_t to_bytes, const wchar_t from[],
    size_t from_chars) NOEXCEPT
{
    if (is_null(from) || is_null(out_to) || is_zero(from_chars) ||
        to_bytes < (from_chars * utf8_max_character_size))
        return zero;

    const std::wstring wide(from, &from[from_chars]);
    const auto narrow = to_utf8(wide);
    const auto bytes = narrow.size();

    if (bytes <= to_bytes)
        std::copy_n(narrow.data(), bytes, out_to);

    return bytes > to_bytes ? zero : bytes;
}

// Convert utf8 char buffer to utf16 wchar_t buffer, with truncation handling.
// Truncation results from having split the input buffer arbitrarily (stream).
size_t to_utf16(size_t& remainder, wchar_t out_to[], size_t to_chars,
    const char from[], size_t from_bytes) NOEXCEPT
{
    remainder = zero;
    if (is_null(from) || is_null(out_to) || is_zero(from_bytes) ||
        to_chars < from_bytes)
        return zero;

    // Calculate a character break offset of 0..3 bytes.
    remainder = utf8_remainder_size(from, from_bytes);

    const std::string narrow{ from, &from[from_bytes - remainder] };
    const auto wide = to_utf16(narrow);
    const auto chars = wide.size();
    if (chars <= to_chars)
        std::copy_n(wide.data(), chars, out_to);

    return chars > to_chars ? zero : chars;
}

// Stream utilities.
// ----------------------------------------------------------------------------

#ifdef HAVE_MSC

// The width of utf16 stdio buffers.
constexpr size_t utf16_buffer_size = 256;

// Ensure console_streambuf::initialize is called only once for the process.
static std::once_flag io_mutex;

// Static initializer for bc::system::cin.
std::istream& cin_stream() THROWS
{
    std::call_once(io_mutex, console_streambuf::initialize, utf16_buffer_size);
    static unicode_istream input(std::cin, std::wcin, utf16_buffer_size);
    return input;
}

// Static initializer for bc::system::cout.
std::ostream& cout_stream() THROWS
{
    std::call_once(io_mutex, console_streambuf::initialize, utf16_buffer_size);
    static unicode_ostream output(std::cout, std::wcout, utf16_buffer_size);
    return output;
}

// Static initializer for bc::system::cerr.
std::ostream& cerr_stream() THROWS
{
    std::call_once(io_mutex, console_streambuf::initialize, utf16_buffer_size);
    static unicode_ostream error(std::cerr, std::wcerr, utf16_buffer_size);
    return error;
}

static std::string config_directory() NOEXCEPT
{
    wchar_t directory[MAX_PATH];
    const auto result = SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA, NULL,
        SHGFP_TYPE_CURRENT, directory);
    return SUCCEEDED(result) ? to_utf8(directory) : "";
}

#else

std::istream& cin_stream() THROWS
{
    return std::cin;
}

std::ostream& cout_stream() THROWS
{
    return std::cout;
}

std::ostream& cerr_stream() THROWS
{
    return std::cerr;
}

static std::string config_directory() NOEXCEPT
{
#if defined(SYSCONFDIR)
    return SYSCONFDIR;
#else
    return {};
#endif
}

#endif // HAVE_MSC

std::filesystem::path default_config_path(
    const std::filesystem::path& subdirectory) NOEXCEPT
{
    static const auto directory = std::filesystem::path{ config_directory() };
    return directory / subdirectory;
}

#ifdef HAVE_MSC

// BC_USE_LIBBITCOIN_MAIN
// ----------------------------------------------------------------------------

void free_environment(char* environment[]) NOEXCEPT
{
    if (environment != nullptr)
    {
        for (auto index = 0; !is_null(environment[index]); ++index)
            std::free(environment[index]);

        std::free(environment);
    }
}

// Convert UTF16/wchar_t argument buffer to utf8/char argument buffer.
// Caller (or compiler, in case of environment replacement) must free.
char** allocate_environment(int argc, wchar_t* argv[]) NOEXCEPT
{
    // Allocate argument pointer array.
    auto size = add1(static_cast<size_t>(argc));
    const auto buffer = std::malloc(size * sizeof(char*));
    const auto arguments = static_cast<char**>(buffer);

    // Out of memory.
    if (is_null(arguments) || is_null(argv))
        return nullptr;

    // Convert each argument, allocate and assign to pointer array.
    for (auto arg = 0; arg < argc; arg++)
    {
        // Guard overflow of std::wstring assignment.
        if (wcsnlen_s(argv[arg], max_size_t) == max_size_t)
            return nullptr;

        const auto utf8 = to_utf8(argv[arg]);
        size = utf8.size();

        // Guard terminator addition.
        if (size == max_size_t)
            return nullptr;

        arguments[arg] = static_cast<char*>(std::malloc(add1(size)));

        // Out of memory (arguments[] is assured).
        BC_PUSH_WARNING(NO_READ_OVERRUN)
        if (is_null(arguments[arg]))
        BC_POP_WARNING()
        {
            free_environment(arguments);
            return nullptr;
        }

        std::copy_n(utf8.begin(), size, arguments[arg]);

        // Row terminator (arguments[][] is assured).
        BC_PUSH_WARNING(NO_DEREFERENCE_NULL_POINTER)
        arguments[arg][size] = '\0';
        BC_POP_WARNING()
    }

    // Table terminator (add1 ensures arguments[]).
    BC_PUSH_WARNING(NO_DEREFERENCE_NULL_POINTER)
    BC_PUSH_WARNING(NO_WRITE_OVERRUN)
    arguments[argc] = nullptr;
    BC_POP_WARNING()
    BC_POP_WARNING()
    return arguments;
}

// Convert UTF16/wchar_t environment buffer to utf8/char environment buffer.
char** allocate_environment(wchar_t* environment[]) NOEXCEPT
{
    if (is_null(environment))
        return nullptr;

    int count;
    for (count = 0; !is_null(environment[count]); count++);
    return allocate_environment(count, environment);
}

int call_utf8_main(int argc, wchar_t* argv[],
    int(*main)(int argc, char* argv[])) NOEXCEPT
{
    // TODO: verify std::filesystem::path is inbued as it was with boost.
    constexpr auto utf8_locale_name = "en_US.UTF8";
    boost::locale::generator locale;
    std::locale::global(locale(utf8_locale_name));

    auto backup = environ;
    auto environment = allocate_environment(_wenviron);

    // If a new environment cannot be allocated control returns immediately.
    if (is_null(environment) || is_null(main))
        return 0;

    environ = environment;

    // If new arguments cannot be allocated control returns immediately.
    auto arguments = allocate_environment(argc, argv);
    if (is_null(arguments))
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

// docs.microsoft.com/windows/win32/api/fileapi/nf-fileapi-getfullpathnamew
std::wstring to_fully_qualified_path(
    const std::filesystem::path& path) NOEXCEPT
{
    const auto replace_all = [](std::string text, char from, char to) NOEXCEPT
    {
        for (auto position = text.find(from); position != std::string::npos;
            position = text.find(from, position + sizeof(char)))
        {
            text.replace(position, sizeof(char), { to });
        }

        return text;
    };

    // Separator normalization required by use of length extender.
    const auto normal = to_utf16(replace_all(path.string(), '/', '\\'));

    // GetFullPathName is not thread safe. If another thread calls
    // SetCurrentDirectory during a call of GetFullPathName the value may be
    // corrupted as process-static storage is used to retain the directory.
    auto size = GetFullPathNameW(normal.c_str(), 0, NULL, NULL);
    if (is_zero(size))
        return {};

    // Despite contradictory documentation, this accepts long relative paths
    // and converts them to fully-qualified, without an extension prefix.
    // This also converts "considered relative" paths (with ".." segments).
    // Add the prefix after calling as required in order to use a long path.
    std::vector<wchar_t> directory(size);
    size = GetFullPathNameW(normal.c_str(), size, directory.data(), NULL);
    if (is_zero(size))
        return {};

    // The returned size does not include the null terminator, and cannot
    // exceed the original, but does become smaller, so resize accordingly.
    return { directory.begin(), std::next(directory.begin(), size) };
}

#endif // HAVE_MSC

#ifdef HAVE_MSC
// Use to_extended_path with APIs that compile to wide with HAVE_MSC defined
// and to UTF8 with HAVE_MSC undefined. This includes some boost APIs and some
// Win32 API extensions to std libs - such as std::ofstream and std::ifstream.
// Otherwise use in any Win32 (W) APIs with HAVE_MSC defined, such as we do in 
// interprocess_lock::open_file -> CreateFileW, since the boost wrapper only
// calls CreateFileA. The length extension prefix requires Win32 (W) APIs.
std::wstring to_extended_path(const std::filesystem::path& path) NOEXCEPT
{
    // The length extension prefix works only with a fully-qualified path.
    // However this includes "considered relative" paths (with ".." segments).
    // That is of no consequence here because those will also be converted.
    const auto full = to_fully_qualified_path(path);
    return (full.length() > MAX_PATH) ? L"\\\\?\\" + full : full;
}
#else
std::string to_extended_path(const std::filesystem::path& path) NOEXCEPT
{
    return path.string();
}
#endif // HAVE_MSC

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
