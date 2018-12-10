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
#include <bitcoin/bitcoin/unicode/unicode.hpp>

#include <cstddef>
#include <cstring>
#include <cwchar>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>
#include <boost/locale.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/limits.hpp>
#include <bitcoin/bitcoin/unicode/console_streambuf.hpp>
#include <bitcoin/bitcoin/unicode/unicode_istream.hpp>
#include <bitcoin/bitcoin/unicode/unicode_ostream.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>

#ifdef _MSC_VER
    #include <fcntl.h>
    #include <io.h>
#endif

namespace libbitcoin {

using namespace boost::locale;

// The width of utf16 stdio buffers.
constexpr size_t utf16_buffer_size = 256;

// Local definition for max number of bytes in a utf8 character.
constexpr size_t utf8_max_character_size = 4;

// Ensure console_streambuf::initialize is called only once.
static std::once_flag io_mutex;

#ifdef WITH_ICU

// Ensure validate_localization is called only once.
static std::once_flag icu_mutex;

#endif

// Static initializer for bc::cin.
std::istream& cin_stream()
{
    std::call_once(io_mutex, console_streambuf::initialize, utf16_buffer_size);
    static unicode_istream input(std::cin, std::wcin, utf16_buffer_size);
    return input;
}

// Static initializer for bc::cout.
std::ostream& cout_stream()
{
    std::call_once(io_mutex, console_streambuf::initialize, utf16_buffer_size);
    static unicode_ostream output(std::cout, std::wcout, utf16_buffer_size);
    return output;
}

// Static initializer for bc::cerr.
std::ostream& cerr_stream()
{
    std::call_once(io_mutex, console_streambuf::initialize, utf16_buffer_size);
    static unicode_ostream error(std::cerr, std::wcerr, utf16_buffer_size);
    return error;
}

#ifdef WITH_ICU

// The backend selection is ignored if invalid (in this case on Windows).
static std::string normal_form(const std::string& value, norm_type form)
{
    auto backend_manager = localization_backend_manager::global();
    backend_manager.select(BC_LOCALE_BACKEND);
    const generator locale(backend_manager);
    return normalize(value, form, locale(BC_LOCALE_UTF8));
}

// One time verifier of the localization backend manager. This is
// necessary because boost::normalize will fail silently to perform
// normalization if the ICU dependency is missing.
static void validate_localization()
{
    const auto backend_manager = localization_backend_manager::global();
    const auto available_backends = backend_manager.get_all_backends();
    const auto iterator = std::find(available_backends.cbegin(),
        available_backends.cend(), BC_LOCALE_BACKEND);

    if (iterator == available_backends.cend())
        throw std::runtime_error(
            "Unicode normalization test failed, a dependency may be missing.");
}

// Normalize strings using unicode nfc normalization.
std::string to_normal_nfc_form(const std::string& value)
{
    std::call_once(icu_mutex, validate_localization);
    return normal_form(value, norm_type::norm_nfc);
}

// Normalize strings using unicode nfkd normalization.
std::string to_normal_nfkd_form(const std::string& value)
{
    std::call_once(icu_mutex, validate_localization);
    return normal_form(value, norm_type::norm_nfkd);
}

// The backend selection is ignored if invalid (in this case on Windows).
std::string to_lower(const std::string& value)
{
    std::call_once(icu_mutex, validate_localization);
    auto backend = localization_backend_manager::global();
    backend.select(BC_LOCALE_BACKEND);
    const generator locale(backend);
    return boost::locale::to_lower(value, locale(BC_LOCALE_UTF8));
}

#endif

void free_environment(char* environment[])
{
    if (environment != nullptr)
    {
        for (auto index = 0; environment[index] != nullptr; ++index)
            std::free(environment[index]);

        std::free(environment);
    }
}

// Convert wchar_t environment buffer to utf8 environment buffer.
char** allocate_environment(wchar_t* environment[])
{
    int count;
    for (count = 0; environment[count] != nullptr; count++);
    return allocate_environment(count, environment);
}

// Convert wchar_t argument buffer to utf8 argument buffer.
// Caller (or compiler, in case of environment replacement) must free.
char** allocate_environment(int argc, wchar_t* argv[])
{
    // Allocate argument pointer array.
    auto arguments = (char**)std::malloc((argc + 1) * sizeof(char*));
    arguments[argc] = nullptr;

    // Convert each argument, allocate and assign to pointer array.
    for (auto arg = 0; arg < argc; arg++)
    {
        const auto utf8 = to_utf8(argv[arg]);
        const auto size = utf8.size();
        arguments[arg] = (char*)std::malloc(size + 1);
        std::copy_n(utf8.begin(), size, arguments[arg]);
        arguments[arg][size] = '\0';
    }

    return arguments;
}

// Convert wstring to utf8 string.
std::string to_utf8(const std::wstring& wide)
{
    using namespace boost::locale;
    return conv::utf_to_utf<char>(wide, conv::method_type::stop);
}

// Convert wchar_t buffer to utf8 char buffer.
size_t to_utf8(char out[], size_t out_bytes, const wchar_t in[],
    size_t in_chars)
{
    BITCOIN_ASSERT(in != nullptr);
    BITCOIN_ASSERT(out != nullptr);
    BITCOIN_ASSERT(out_bytes >= utf8_max_character_size * in_chars);

    if (in_chars == 0)
        return 0;

    size_t bytes = 0;

    try
    {
        const auto narrow = to_utf8({ in, &in[in_chars] });
        bytes = narrow.size();

        if (bytes <= out_bytes)
            memcpy(out, narrow.data(), bytes);
    }
    catch (const boost::locale::conv::conversion_error&)
    {
        bytes = 0;
    }

    if (bytes == 0)
        throw std::istream::failure("utf-16 to utf-8 conversion failure");

    if (bytes > out_bytes)
        throw std::ios_base::failure("utf8 buffer is too small");

    return bytes;
}

// All non-leading bytes of utf8 have the same two bit prefix.
static bool is_utf8_trailing_byte(char byte)
{
    // 10xxxxxx
    return ((0xC0 & byte) == 0x80);
}

// Determine if the full sequence is a valid utf8 character.
static bool is_utf8_character_sequence(const char sequence[], uint8_t bytes)
{
    BITCOIN_ASSERT(bytes <= utf8_max_character_size);

    // See tools.ietf.org/html/rfc3629#section-3 for definition.
    switch (bytes)
    {
        case 1:
            // 0xxxxxxx
            return
                ((0x80 & sequence[0]) == 0x00);
        case 2:
            // 110xxxxx 10xxxxxx
            return
                ((0xE0 & sequence[0]) == 0xC0) &&
                is_utf8_trailing_byte(sequence[1]);
        case 3:
            // 1110xxxx 10xxxxxx 10xxxxxx
            return
                ((0xF0 & sequence[0]) == 0xE0) &&
                is_utf8_trailing_byte(sequence[1]) &&
                is_utf8_trailing_byte(sequence[2]);
        case 4:
            // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            return
                ((0xF8 & sequence[0]) == 0xF0) &&
                is_utf8_trailing_byte(sequence[1]) &&
                is_utf8_trailing_byte(sequence[2]) &&
                is_utf8_trailing_byte(sequence[3]);
        default:;
    }

    return false;
}

// Determine if the text is terminated by a valid utf8 character.
static bool is_terminal_utf8_character(const char text[], size_t size)
{
    BITCOIN_ASSERT(text != nullptr);

    // Walk back up to the max length of a utf8 character.
    for (uint8_t length = 1; length <= utf8_max_character_size &&
        length < size; length++)
    {
        const auto start = size - length;
        const auto sequence = &text[start];
        if (is_utf8_character_sequence(sequence, length))
            return true;
    }

    return false;
}

// This optimizes character split detection by taking advantage of utf8
// character recognition so we don't have to convert in full up to 3 times.
// This does not guarantee that the entire string is valid as utf8, just that a
// returned offset follows the last byte of a utf8 terminal char if it exists.
static uint8_t offset_to_terminal_utf8_character(const char text[], size_t size)
{
    BITCOIN_ASSERT(text != nullptr);

    // Walk back up to the max length of a utf8 character.
    for (uint8_t unread = 0; unread < utf8_max_character_size &&
        unread < size; unread++)
    {
        const auto length = size - unread;
        if (is_terminal_utf8_character(text, length))
            return unread;
    }

    return 0;
}

// Convert utf8 char buffer to wchar_t buffer, with truncation handling.
size_t to_utf16(wchar_t out[], size_t out_chars, const char in[],
    size_t in_bytes, uint8_t& truncated)
{
    BITCOIN_ASSERT(in != nullptr);
    BITCOIN_ASSERT(out != nullptr);
    BITCOIN_ASSERT(out_chars >= in_bytes);

    // Calculate a character break offset of 0..4 bytes.
    truncated = offset_to_terminal_utf8_character(in, in_bytes);

    if (in_bytes == 0)
        return 0;

    size_t chars = 0;

    try
    {
        const auto wide = to_utf16({ in, &in[in_bytes - truncated] });
        chars = wide.size();

        if (chars <= out_chars)
            wmemcpy(out, wide.data(), chars);
    }
    catch (const boost::locale::conv::conversion_error&)
    {
        chars = 0;
    }

    if (chars == 0)
        throw std::ostream::failure("utf-8 to utf-16 conversion failure");

    if (chars > out_chars)
        throw std::ios_base::failure("utf16 buffer is too small");

    return chars;
}

// Convert utf8 string to wstring.
std::wstring to_utf16(const std::string& narrow)
{
    using namespace boost::locale;
    return conv::utf_to_utf<wchar_t>(narrow, conv::method_type::stop);
}

LCOV_EXCL_START("Untestable but visually-verifiable section.")

static void set_utf8_stdio(
#ifdef _MSC_VER
    FILE* file)
#else
    FILE*)
#endif
{
#ifdef _MSC_VER
    if (_setmode(_fileno(file), _O_U8TEXT) == -1)
        throw std::runtime_error("Could not set STDIO to utf8 mode.");
#endif
}

static void set_binary_stdio(
#ifdef _MSC_VER
    FILE* file)
#else
    FILE*)
#endif
{
#ifdef _MSC_VER
    if (_setmode(_fileno(file), _O_BINARY) == -1)
        throw std::runtime_error("Could not set STDIO to binary mode.");
#endif
}

// Set stdio to use UTF8 translation on Windows.
void set_utf8_stdio()
{
    set_utf8_stdin();
    set_utf8_stdout();
    set_utf8_stderr();
}

// Set stdio to use UTF8 translation on Windows.
void set_utf8_stdin()
{
    set_utf8_stdio(stdin);
}

// Set stdio to use UTF8 translation on Windows.
void set_utf8_stdout()
{
    set_utf8_stdio(stdout);
}

// Set stdio to use UTF8 translation on Windows.
void set_utf8_stderr()
{
    set_utf8_stdio(stderr);
}

// Set stdio to use UTF8 translation on Windows.
void set_binary_stdin()
{
    set_binary_stdio(stdin);
}

// Set stdio to use UTF8 translation on Windows.
void set_binary_stdout()
{
    set_binary_stdio(stdout);
}

LCOV_EXCL_STOP()

} // namespace libbitcoin
