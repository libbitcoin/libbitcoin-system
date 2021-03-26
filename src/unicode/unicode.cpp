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
#include <bitcoin/system/unicode/unicode.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <iostream>
#include <locale>
#include <mutex>
#include <stdexcept>
#include <string>
#ifdef _MSC_VER
    #include <fcntl.h>
    #include <io.h>
    #include <windows.h>
#endif
#include <boost/filesystem.hpp>
#include <boost/locale.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/limits.hpp>
#include <bitcoin/system/unicode/console_streambuf.hpp>
#include <bitcoin/system/unicode/unicode_istream.hpp>
#include <bitcoin/system/unicode/unicode_ostream.hpp>
#include <bitcoin/system/utility/assert.hpp>
#include <bitcoin/system/utility/data.hpp>

namespace libbitcoin {
namespace system {

// Avoid codecvt as it is deprecated in c++17.
using namespace boost::locale;

typedef std::pair<char32_t, char32_t> utf32_interval;
typedef std::vector<utf32_interval> utf32_intervals;

// Forward declarations for long tables at end of the file.
extern const utf32_intervals chinese_japanese_korean;
extern const utf32_intervals diacritics;

// Boost/std locale paramters.
constexpr auto icu_backend_name = "icu";
constexpr auto utf8_locale_name = "en_US.UTF8";

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

#ifdef WITH_ICU

static bool is_c_whitespace(char32_t value)
{
    // An ascii test would be redundant with the C locale:
    // space(0x20, ' ')
    // form feed(0x0c, '\f')
    // line feed(0x0a, '\n')
    // carriage return (0x0d, '\r')
    // horizontal tab(0x09, '\t')
    // vertical tab(0x0b, '\v')
    return std::isspace(value, std::locale("C"));
}

static bool is_chinese_japanese_or_korean(char32_t value)
{
    return std::any_of(chinese_japanese_korean.begin(),
        chinese_japanese_korean.end(),
        [value](const utf32_interval& interval)
    {
        return interval.first <= value && value <= interval.second;
    });
}

#ifdef _MSC_VER

static NORM_FORM to_win32_normal_form(boost::locale::norm_type form)
{
    switch (form)
    {
        case norm_type::norm_nfkd:
            return NormalizationKD;
        case norm_type::norm_nfkc:
            return NormalizationKC;
        case norm_type::norm_nfd:
            return NormalizationD;

        // NFC is the boost::locale default and this is the full enumeration.
        case norm_type::norm_nfc:
        default:
            return NormalizationC;
    }
}

// Workaround lack of ICU support in published boost-locale packages.
// The ICU library was first added to Windows 10 in [10.0.15063].
// docs.microsoft.com/en-us/windows/win32/intl/international-components-for-unicode--icu-
// Windows XP, Windows Server 2003: No longer supported.
// The required header file and DLL are part of the Microsoft Internationalized
// Domain Name(IDN) Mitigation APIs, which are no longer available for download.
static std::string normal_form(const std::string& value, norm_type form)
{
    if (value.empty())
        return value;

    const auto wide = to_utf16(value);
    const auto size = wide.size();
    const auto source = wide.c_str();
    const auto norm = to_win32_normal_form(form);

    // Guard against int overflow.
    if (size > std::numeric_limits<int>::max())
        return {};

    const auto length = static_cast<int>(size);
    auto result = NormalizeString(norm, source, length, NULL, 0);

    // Will overflow buffer (or is empty).
    if (result <= 0)
        return {};

    auto buffer = std::wstring(result, 0);
    result = NormalizeString(norm, source, length, &buffer.front(), result);

    // Return empty if normalization did not succeed (or is empty).
    return result > 0 ? to_utf8(buffer.substr(0, result)) : std::string{};
}

std::string to_lower(const std::string& value)
{
    if (value.empty())
        return value;

    auto wide = to_utf16(value);
    const auto size = wide.size();

    // Guard against DWORD overflow.
    if (size > std::numeric_limits<DWORD>::max())
        return {};

    // std::vector ensures contiguous bytes.
    const auto length = static_cast<DWORD>(size);

    // CharLowerBuffW ensures conversion in place.
    const auto result = (CharLowerBuffW(&wide.front(), length) == length);

    // Return empty if conversion did not succeed.
    return result ? to_utf8(wide) : std::string{};
}

#else

// One time verifier of the localization backend manager.
// Guard against backend_manager.select(BC_LOCALE_BACKEND) silent failure.
static void validate_localization()
{
    // Thread safe.
    // Creates a new global backend manager and returns the onld one.
    // TODO: it would be more efficient to just store this globally.
    const auto backend_manager = localization_backend_manager::global();

    // Not thread safe (call to validate_localization is guarded).
    const auto available_backends = backend_manager.get_all_backends();
    const auto iterator = std::find(available_backends.cbegin(),
        available_backends.cend(), icu_backend_name);

    if (iterator == available_backends.cend())
        throw std::runtime_error(
            "Unicode normalization test failed, a dependency may be missing.");
}

static std::string normal_form(const std::string& value, norm_type form)
{
    if (value.empty())
        return value;

    std::call_once(icu_mutex, validate_localization);

    // Thread safe.
    auto backend_manager = localization_backend_manager::global();
    backend_manager.select(icu_backend_name);
    const generator locale(backend_manager);
    return normalize(value, form, locale(utf8_locale_name));
}

// Python 2 string.lower() is *locale dependent* (invalid).
// Python 3 string.lower() follows section 3.13 of the Unicode Standard.
std::string to_lower(const std::string& value)
{
    if (value.empty())
        return value;

    std::call_once(icu_mutex, validate_localization);

    // Thread safe.
    auto backend_manager = localization_backend_manager::global();
    backend_manager.select(icu_backend_name);
    const generator locale(backend_manager);
    return boost::locale::to_lower(value, locale(utf8_locale_name));
}

#endif // _MSC_VER

// Normalize strings using unicode nfc normalization.
std::string to_normal_nfc_form(const std::string& value)
{
    return normal_form(value, norm_type::norm_nfc);
}

// Normalize strings using unicode nfkd normalization.
std::string to_normal_nfkd_form(const std::string& value)
{
    return normal_form(value, norm_type::norm_nfkd);
}

// Remove accent characters (diacritics).
std::string to_unaccented_form(const std::string& value)
{
    if (value.empty())
        return value;

    // utf32 ensures each word is a single unicode character.
    auto points = to_utf32(value);

    points.erase(std::remove_if(points.begin(), points.end(),
        [](char32_t point)
        {
            return std::any_of(diacritics.begin(), diacritics.end(),
                [point](const utf32_interval& interval)
                {
                    return interval.first <= point && point <= interval.second;
                });
        }), points.end());

    return to_utf8(points);
}

// Remove spaces between cjk characters.
// utf32 ensures each word is a single character.
std::string to_compressed_cjk_form(const std::string& value)
{
    // utf32 ensures each word is a single unicode character.
    const auto points = to_utf32(value);

    // A character cannot be between two others if there aren't at least three.
    if (points.size() < 3u)
        return value;

    // Copy the first character to the result string.
    std::u32string result{ points.front() };

    // Remove whitespaces between CJK.
    // points[0] cannot be between two characters, so skip it.
    // points[size] cannot be between two characters, so skip it.
    for (size_t point = 1; point < points.size() - 1u; point++)
    {
        if (!(is_c_whitespace(points[point]) &&
            is_chinese_japanese_or_korean(points[point - 1u]) &&
            is_chinese_japanese_or_korean(points[point + 1u])))
        {
            result += points[point];
        }
    }

    // Copy the last character to the result string.
    result += points.back();
    return to_utf8(result);
}

#endif // WITH_ICU

// Convert utf16 wchar_t buffer to utf8 char buffer.
// This is used in wmain for conversion of wide args and environ on Windows.
// Returns zero in case of invalid arguments, including insufficient buffer.
// A null terminating character will not be copied.
size_t to_utf8(char out_to[], size_t to_bytes, const wchar_t from[],
    size_t from_chars)
{
    if (from == nullptr || out_to == nullptr || from_chars == 0 ||
        to_bytes < (from_chars * utf8_max_character_size))
        return 0;

    const auto narrow = to_utf8(std::wstring{ from, &from[from_chars] });
    const auto bytes = narrow.size();

    if (bytes <= to_bytes)
        memcpy(out_to, narrow.data(), bytes);

    return bytes > to_bytes ? 0 : bytes;
}

// Convert utf16 wstring to utf8 string.
std::string to_utf8(const std::wstring& wide)
{
    using namespace boost::locale;
    std::string result;

    try
    {
        // method_type::stop throws conv::conversion_error.
        // Other stop methods skip characters, resulting in hidden failure.
        result = conv::utf_to_utf<char>(wide, conv::method_type::stop);
    }
    catch (const conv::conversion_error&)
    {
        result.clear();
    }

    return result;
}

// Convert utf32 u32string to utf8 string.
std::string to_utf8(const std::u32string& wide)
{
    using namespace boost::locale;
    std::string result;

    try
    {
        // method_type::stop throws conv::conversion_error.
        // Other stop methods skip characters, resulting in hidden failure.
        result = conv::utf_to_utf<char>(wide, conv::method_type::stop);
    }
    catch (const conv::conversion_error&)
    {
        result.clear();
    }

    return result;
}

// Convert utf8 char buffer to utf16 wchar_t buffer, with truncation handling.
// Truncation results from having split the input buffer arbitrarily (stream).
// Returns zero in case of invalid arguments, including insufficient buffer.
// A null terminating character will not be copied.
size_t to_utf16(uint8_t& out_truncated, wchar_t out_to[], size_t to_chars,
    const char from[], size_t from_bytes)
{
    out_truncated = 0;
    if (from == nullptr || out_to == nullptr || from_bytes == 0 ||
        to_chars < from_bytes)
        return 0;

    // Calculate a character break offset of 0..4 bytes.
    out_truncated = offset_to_terminal_utf8_character(from, from_bytes);

    const auto wide = to_utf16({ from, &from[from_bytes - out_truncated] });
    const auto chars = wide.size();

    if (chars <= to_chars)
        wmemcpy(out_to, wide.data(), chars);

    return chars > to_chars ? 0 : chars;
}

// Convert utf8 string to utf16 wstring.
std::wstring to_utf16(const std::string& narrow)
{
    using namespace boost::locale;
    std::wstring result;

    try
    {
        // method_type::stop throws conv::conversion_error.
        // Other stop methods skip characters, resulting in hidden failure.
        result = conv::utf_to_utf<wchar_t>(narrow, conv::method_type::stop);
    }
    catch (const conv::conversion_error&)
    {
        result.clear();
    }

    return result;
}

// Convert utf8 string to utf32 wstring.
// In utf32 each 32 bit word is a single character.
std::u32string to_utf32(const std::string& narrow)
{
    using namespace boost::locale;
    std::u32string result;

    try
    {
        // method_type::stop throws conv::conversion_error.
        // Other stop methods skip characters, resulting in hidden failure.
        result = conv::utf_to_utf<char32_t>(narrow, conv::method_type::stop);
    }
    catch (const conv::conversion_error&)
    {
        result.clear();
    }

    return result;
}

LCOV_EXCL_START("Untestable but visually-verifiable section.")

// Stream functions.
// ----------------------------------------------------------------------------

#ifdef _MSC_VER

static void set_utf8_stdio(FILE* file)
{
    if (_setmode(_fileno(file), _O_U8TEXT) == -1)
        throw std::runtime_error("Could not set STDIO to utf8 mode.");
}

static void set_binary_stdio(FILE* file)
{
    if (_setmode(_fileno(file), _O_BINARY) == -1)
        throw std::runtime_error("Could not set STDIO to binary mode.");
}

#else

static void set_utf8_stdio(FILE*)
{
}

static void set_binary_stdio(FILE*)
{
}

#endif

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

// Internal UTF8 utilities.
// ----------------------------------------------------------------------------

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
uint8_t offset_to_terminal_utf8_character(const char text[], size_t size)
{
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

// Windows environment functions (exposed for testing only).
// ----------------------------------------------------------------------------

#ifdef _MSC_VER

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
        if (wcsnlen_s(argv[arg], bc::max_size_t) == bc::max_size_t)
            return nullptr;

        const auto utf8 = bc::to_utf8(argv[arg]);
        const auto size = utf8.size();

        // Guard terminator addition.
        if (size == bc::max_size_t)
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
    // When working with boost and utf8 narrow characters on Windows the thread
    // must be configured for utf8. When working with boost::filesystem::path
    // the static path object must be imbued with the utf8 locale or paths will
    // be incorrectly translated.
    boost::locale::generator locale;
    std::locale::global(locale(bc::utf8_locale_name));
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

#endif

// Based on Electrum source, which references:
// asahi-net.or.jp/~ax2s-kmtn/ref/unicode/e_asia.html
// More information: en.wikipedia.org/wiki/CJK_characters
const utf32_intervals chinese_japanese_korean
{
    { 0x00004e00, 0x00009fff }, // cjk unified ideographs
    { 0x00003400, 0x00004dbf }, // cjk unified ideographs extension a
    { 0x00020000, 0x0002a6df }, // cjk unified ideographs extension b
    { 0x0002a700, 0x0002b73f }, // cjk unified ideographs extension c
    { 0x0002b740, 0x0002b81f }, // cjk unified ideographs extension d
    { 0x0000f900, 0x0000faff }, // cjk compatibility ideographs
    { 0x0002f800, 0x0002fa1d }, // cjk compatibility ideographs supplement
    { 0x00003190, 0x0000319f }, // kanbun
    { 0x00002e80, 0x00002eff }, // cjk radicals supplement
    { 0x00002f00, 0x00002fdf }, // cjk radicals
    { 0x000031c0, 0x000031ef }, // cjk strokes
    { 0x00002ff0, 0x00002fff }, // ideographic description characters
    { 0x000e0100, 0x000e01ef }, // variation selectors supplement
    { 0x00003100, 0x0000312f }, // bopomofo
    { 0x000031a0, 0x000031bf }, // bopomofo extended
    { 0x0000ff00, 0x0000ffef }, // halfwidth and fullwidth forms
    { 0x00003040, 0x0000309f }, // hiragana
    { 0x000030a0, 0x000030ff }, // katakana
    { 0x000031f0, 0x000031ff }, // katakana phonetic extensions
    { 0x0001b000, 0x0001b0ff }, // kana supplement
    { 0x0000ac00, 0x0000d7af }, // hangul syllables
    { 0x00001100, 0x000011ff }, // hangul jamo
    { 0x0000a960, 0x0000a97f }, // hangul jamo extended a
    { 0x0000d7b0, 0x0000d7ff }, // hangul jamo extended b
    { 0x00003130, 0x0000318f }, // hangul compatibility jamo
    { 0x0000a4d0, 0x0000a4ff }, // lisu
    { 0x00016f00, 0x00016f9f }, // miao
    { 0x0000a000, 0x0000a48f }, // yi syllables
    { 0x0000a490, 0x0000a4cf }, // yi radicals
};

// Combining Diacritics list taken from:
// unicode.org/Public/UNIDATA/PropList.txt
// unicode.org/Public/11.0.0/charts/CodeCharts.pdf
// Section "Combining Diacritical Marks"
const utf32_intervals diacritics
{
    { 0x0000005e, 0x0000005e }, // circumflex accent
    { 0x00000060, 0x00000060 }, // grave accent
    { 0x000000a8, 0x000000a8 }, // diaeresis
    { 0x000000af, 0x000000af }, // macron
    { 0x000000b4, 0x000000b4 }, // acute accent
    { 0x000000b7, 0x000000b7 }, // middle dot
    { 0x000000b8, 0x000000b8 }, // cedilla
    { 0x000002b0, 0x000002c1 }, // modifier letter small h..modifier letter reversed glottal stop
    { 0x000002c2, 0x000002c5 }, // modifier letter left arrowhead..modifier letter down arrowhead
    { 0x000002c6, 0x000002d1 }, // modifier letter circumflex accent..modifier letter half triangular colon
    { 0x000002d2, 0x000002df }, // modifier letter centred right half ring..modifier letter cross accent
    { 0x000002e0, 0x000002e4 }, // modifier letter small gamma..modifier letter small reversed glottal stop
    { 0x000002e5, 0x000002eb }, // modifier letter extra-high tone bar..modifier letter yang departing tone mark
    { 0x000002ec, 0x000002ec }, // modifier letter voicing
    { 0x000002ed, 0x000002ed }, // modifier letter unaspirated
    { 0x000002ee, 0x000002ee }, // modifier letter double apostrophe
    { 0x000002ef, 0x000002ff }, // modifier letter low down arrowhead..modifier letter low left arrow
    { 0x00000300, 0x0000034e }, // * see reference above
    { 0x00000350, 0x0000036f }, // * see reference above
    { 0x00000374, 0x00000374 }, // greek numeral sign
    { 0x00000375, 0x00000375 }, // greek lower numeral sign
    { 0x0000037a, 0x0000037a }, // greek ypogegrammeni
    { 0x00000384, 0x00000385 }, // greek tonos..greek dialytika tonos
    { 0x00000483, 0x00000487 }, // combining cyrillic titlo..combining cyrillic pokrytie
    { 0x00000559, 0x00000559 }, // armenian modifier letter left half ring
    { 0x00000591, 0x000005a1 }, // hebrew accent etnahta..hebrew accent pazer
    { 0x000005a3, 0x000005bd }, // hebrew accent munah..hebrew point meteg
    { 0x000005bf, 0x000005bf }, // hebrew point rafe
    { 0x000005c1, 0x000005c2 }, // hebrew point shin dot..hebrew point sin dot
    { 0x000005c4, 0x000005c4 }, // hebrew mark upper dot
    { 0x0000064b, 0x00000652 }, // arabic fathatan..arabic sukun
    { 0x00000657, 0x00000658 }, // arabic inverted damma..arabic mark noon ghunna
    { 0x000006df, 0x000006e0 }, // arabic small high rounded zero..arabic small high upright rectangular zero
    { 0x000006e5, 0x000006e6 }, // arabic small waw..arabic small yeh
    { 0x000006ea, 0x000006ec }, // arabic empty centre low stop..arabic rounded high stop with filled centre
    { 0x00000730, 0x0000074a }, // syriac pthaha above..syriac barrekh
    { 0x000007a6, 0x000007b0 }, // thaana abafili..thaana sukun
    { 0x000007eb, 0x000007f3 }, // nko combining short high tone..nko combining double dot above
    { 0x000007f4, 0x000007f5 }, // nko high tone apostrophe..nko low tone apostrophe
    { 0x00000818, 0x00000819 }, // samaritan mark occlusion..samaritan mark dagesh
    { 0x000008e3, 0x000008fe }, // arabic turned damma below..arabic damma with dot
    { 0x0000093c, 0x0000093c }, // devanagari sign nukta
    { 0x0000094d, 0x0000094d }, // devanagari sign virama
    { 0x00000951, 0x00000954 }, // devanagari stress sign udatta..devanagari acute accent
    { 0x00000971, 0x00000971 }, // devanagari sign high spacing dot
    { 0x000009bc, 0x000009bc }, // bengali sign nukta
    { 0x000009cd, 0x000009cd }, // bengali sign virama
    { 0x00000a3c, 0x00000a3c }, // gurmukhi sign nukta
    { 0x00000a4d, 0x00000a4d }, // gurmukhi sign virama
    { 0x00000abc, 0x00000abc }, // gujarati sign nukta
    { 0x00000acd, 0x00000acd }, // gujarati sign virama
    { 0x00000afd, 0x00000aff }, // gujarati sign three-dot nukta above..gujarati sign two-circle nukta above
    { 0x00000b3c, 0x00000b3c }, // oriya sign nukta
    { 0x00000b4d, 0x00000b4d }, // oriya sign virama
    { 0x00000bcd, 0x00000bcd }, // tamil sign virama
    { 0x00000c4d, 0x00000c4d }, // telugu sign virama
    { 0x00000cbc, 0x00000cbc }, // kannada sign nukta
    { 0x00000ccd, 0x00000ccd }, // kannada sign virama
    { 0x00000d3b, 0x00000d3c }, // malayalam sign vertical bar virama..malayalam sign circular virama
    { 0x00000d4d, 0x00000d4d }, // malayalam sign virama
    { 0x00000dca, 0x00000dca }, // sinhala sign al-lakuna
    { 0x00000e47, 0x00000e4c }, // thai character maitaikhu..thai character thanthakhat
    { 0x00000e4e, 0x00000e4e }, // thai character yamakkan
    { 0x00000ec8, 0x00000ecc }, // lao tone mai ek..lao cancellation mark
    { 0x00000f18, 0x00000f19 }, // tibetan astrological sign -khyud pa..tibetan astrological sign sdong tshugs
    { 0x00000f35, 0x00000f35 }, // tibetan mark ngas bzung nyi zla
    { 0x00000f37, 0x00000f37 }, // tibetan mark ngas bzung sgor rtags
    { 0x00000f39, 0x00000f39 }, // tibetan mark tsa -phru
    { 0x00000f3e, 0x00000f3f }, // tibetan sign yar tshes..tibetan sign mar tshes
    { 0x00000f82, 0x00000f84 }, // tibetan sign nyi zla naa da..tibetan mark halanta
    { 0x00000f86, 0x00000f87 }, // tibetan sign lci rtags..tibetan sign yang rtags
    { 0x00000fc6, 0x00000fc6 }, // tibetan symbol padma gdan
    { 0x00001037, 0x00001037 }, // myanmar sign dot below
    { 0x00001039, 0x0000103a }, // myanmar sign virama..myanmar sign asat
    { 0x00001087, 0x0000108c }, // myanmar sign shan tone-2..myanmar sign shan council tone-3
    { 0x0000108d, 0x0000108d }, // myanmar sign shan council emphatic tone
    { 0x0000108f, 0x0000108f }, // myanmar sign rumai palaung tone-5
    { 0x0000109a, 0x0000109b }, // myanmar sign khamti tone-1..myanmar sign khamti tone-3
    { 0x000017c9, 0x000017d3 }, // khmer sign muusikatoan..khmer sign bathamasat
    { 0x000017dd, 0x000017dd }, // khmer sign atthacan
    { 0x00001939, 0x0000193b }, // limbu sign mukphreng..limbu sign sa-i
    { 0x00001a75, 0x00001a7c }, // tai tham sign tone-1..tai tham sign khuen-lue karan
    { 0x00001a7f, 0x00001a7f }, // tai tham combining cryptogrammic dot
    { 0x00001ab0, 0x00001abd }, // combining doubled circumflex accent..combining parentheses below
    { 0x00001b34, 0x00001b34 }, // balinese sign rerekan
    { 0x00001b44, 0x00001b44 }, // balinese adeg adeg
    { 0x00001b6b, 0x00001b73 }, // balinese musical symbol combining tegeh..balinese musical symbol combining gong
    { 0x00001baa, 0x00001baa }, // sundanese sign pamaaeh
    { 0x00001bab, 0x00001bab }, // sundanese sign virama
    { 0x00001c36, 0x00001c37 }, // lepcha sign ran..lepcha sign nukta
    { 0x00001c78, 0x00001c7d }, // ol chiki mu ttuddag..ol chiki ahad
    { 0x00001cd0, 0x00001cd2 }, // vedic tone karshana..vedic tone prenkha
    { 0x00001cd3, 0x00001cd3 }, // vedic sign nihshvasa
    { 0x00001cd4, 0x00001ce0 }, // vedic sign yajurvedic midline svarita..vedic tone rigvedic kashmiri independent svarita
    { 0x00001ce1, 0x00001ce1 }, // vedic tone atharvavedic independent svarita
    { 0x00001ce2, 0x00001ce8 }, // vedic sign visarga svarita..vedic sign visarga anudatta with tail
    { 0x00001ced, 0x00001ced }, // vedic sign tiryak
    { 0x00001cf4, 0x00001cf4 }, // vedic tone candra above
    { 0x00001cf7, 0x00001cf7 }, // vedic sign atikrama
    { 0x00001cf8, 0x00001cf9 }, // vedic tone ring above..vedic tone double ring above
    { 0x00001d2c, 0x00001d6a }, // modifier letter capital a..greek subscript small letter chi
    { 0x00001dc4, 0x00001dcf }, // combining macron-acute..combining zigzag below
    { 0x00001df5, 0x00001df9 }, // combining up tack above..combining wide inverted bridge below
    { 0x00001dfd, 0x00001dff }, // combining almost equal to below..combining right arrowhead and down arrowhead below
    { 0x00001fbd, 0x00001fbd }, // greek koronis
    { 0x00001fbf, 0x00001fc1 }, // greek psili..greek dialytika and perispomeni
    { 0x00001fcd, 0x00001fcf }, // greek psili and varia..greek psili and perispomeni
    { 0x00001fdd, 0x00001fdf }, // greek dasia and varia..greek dasia and perispomeni
    { 0x00001fed, 0x00001fef }, // greek dialytika and varia..greek varia
    { 0x00001ffd, 0x00001ffe }, // greek oxia..greek dasia
    { 0x00002cef, 0x00002cf1 }, // coptic combining ni above..coptic combining spiritus lenis
    { 0x00002e2f, 0x00002e2f }, // vertical tilde
    { 0x0000302a, 0x0000302d }, // ideographic level tone mark..ideographic entering tone mark
    { 0x0000302e, 0x0000302f }, // hangul single dot tone mark..hangul double dot tone mark
    { 0x00003099, 0x0000309a }, // combining katakana-hiragana voiced sound mark..combining katakana-hiragana semi-voiced sound mark
    { 0x0000309b, 0x0000309c }, // katakana-hiragana voiced sound mark..katakana-hiragana semi-voiced sound mark
    { 0x000030fc, 0x000030fc }, // katakana-hiragana prolonged sound mark
    { 0x0000a66f, 0x0000a66f }, // combining cyrillic vzmet
    { 0x0000a67c, 0x0000a67d }, // combining cyrillic kavyka..combining cyrillic payerok
    { 0x0000a67f, 0x0000a67f }, // cyrillic payerok
    { 0x0000a69c, 0x0000a69d }, // modifier letter cyrillic hard sign..modifier letter cyrillic soft sign
    { 0x0000a6f0, 0x0000a6f1 }, // bamum combining mark koqndon..bamum combining mark tukwentis
    { 0x0000a717, 0x0000a71f }, // modifier letter dot vertical bar..modifier letter low inverted exclamation mark
    { 0x0000a720, 0x0000a721 }, // modifier letter stress and high tone..modifier letter stress and low tone
    { 0x0000a788, 0x0000a788 }, // modifier letter low circumflex accent
    { 0x0000a7f8, 0x0000a7f9 }, // modifier letter capital h with stroke..modifier letter small ligature oe
    { 0x0000a8c4, 0x0000a8c4 }, // saurashtra sign virama
    { 0x0000a8e0, 0x0000a8f1 }, // combining devanagari digit zero..combining devanagari sign avagraha
    { 0x0000a92b, 0x0000a92d }, // kayah li tone plophu..kayah li tone calya plophu
    { 0x0000a92e, 0x0000a92e }, // kayah li sign cwi
    { 0x0000a953, 0x0000a953 }, // rejang virama
    { 0x0000a9b3, 0x0000a9b3 }, // javanese sign cecak telu
    { 0x0000a9c0, 0x0000a9c0 }, // javanese pangkon
    { 0x0000a9e5, 0x0000a9e5 }, // myanmar sign shan saw
    { 0x0000aa7b, 0x0000aa7b }, // myanmar sign pao karen tone
    { 0x0000aa7c, 0x0000aa7c }, // myanmar sign tai laing tone-2
    { 0x0000aa7d, 0x0000aa7d }, // myanmar sign tai laing tone-5
    { 0x0000aabf, 0x0000aabf }, // tai viet tone mai ek
    { 0x0000aac0, 0x0000aac0 }, // tai viet tone mai nueng
    { 0x0000aac1, 0x0000aac1 }, // tai viet tone mai tho
    { 0x0000aac2, 0x0000aac2 }, // tai viet tone mai song
    { 0x0000aaf6, 0x0000aaf6 }, // meetei mayek virama
    { 0x0000ab5b, 0x0000ab5b }, // modifier breve with inverted breve
    { 0x0000ab5c, 0x0000ab5f }, // modifier letter small heng..modifier letter small u with left hook
    { 0x0000abec, 0x0000abec }, // meetei mayek lum iyek
    { 0x0000abed, 0x0000abed }, // meetei mayek apun iyek
    { 0x0000fb1e, 0x0000fb1e }, // hebrew point judeo-spanish varika
    { 0x0000fe20, 0x0000fe2f }, // combining ligature left half..combining cyrillic titlo right half
    { 0x0000ff3e, 0x0000ff3e }, // fullwidth circumflex accent
    { 0x0000ff40, 0x0000ff40 }, // fullwidth grave accent
    { 0x0000ff70, 0x0000ff70 }, // halfwidth katakana-hiragana prolonged sound mark
    { 0x0000ff9e, 0x0000ff9f }, // halfwidth katakana voiced sound mark..halfwidth katakana semi-voiced sound mark
    { 0x0000ffe3, 0x0000ffe3 }, // fullwidth macron
    { 0x000102e0, 0x000102e0 }, // coptic epact thousands mark
    { 0x00010ae5, 0x00010ae6 }, // manichaean abbreviation mark above..manichaean abbreviation mark below
    { 0x00010d22, 0x00010d23 }, // hanifi rohingya mark sakin..hanifi rohingya mark na khonna
    { 0x00010d24, 0x00010d27 }, // hanifi rohingya sign harbahay..hanifi rohingya sign tassi
    { 0x00010f46, 0x00010f50 }, // sogdian combining dot below..sogdian combining stroke below
    { 0x000110b9, 0x000110ba }, // kaithi sign virama..kaithi sign nukta
    { 0x00011133, 0x00011134 }, // chakma virama..chakma maayyaa
    { 0x00011173, 0x00011173 }, // mahajani sign nukta
    { 0x000111c0, 0x000111c0 }, // sharada sign virama
    { 0x000111ca, 0x000111cc }, // sharada sign nukta..sharada extra short vowel mark
    { 0x00011235, 0x00011235 }, // khojki sign virama
    { 0x00011236, 0x00011236 }, // khojki sign nukta
    { 0x000112e9, 0x000112ea }, // khudawadi sign nukta..khudawadi sign virama
    { 0x0001133c, 0x0001133c }, // grantha sign nukta
    { 0x0001134d, 0x0001134d }, // grantha sign virama
    { 0x00011366, 0x0001136c }, // combining grantha digit zero..combining grantha digit six
    { 0x00011370, 0x00011374 }, // combining grantha letter a..combining grantha letter pa
    { 0x00011442, 0x00011442 }, // newa sign virama
    { 0x00011446, 0x00011446 }, // newa sign nukta
    { 0x000114c2, 0x000114c3 }, // tirhuta sign virama..tirhuta sign nukta
    { 0x000115bf, 0x000115c0 }, // siddham sign virama..siddham sign nukta
    { 0x0001163f, 0x0001163f }, // modi sign virama
    { 0x000116b6, 0x000116b6 }, // takri sign virama
    { 0x000116b7, 0x000116b7 }, // takri sign nukta
    { 0x0001172b, 0x0001172b }, // ahom sign killer
    { 0x00011839, 0x0001183a }, // dogra sign virama..dogra sign nukta
    { 0x00011a34, 0x00011a34 }, // zanabazar square sign virama
    { 0x00011a47, 0x00011a47 }, // zanabazar square subjoiner
    { 0x00011a99, 0x00011a99 }, // soyombo subjoiner
    { 0x00011c3f, 0x00011c3f }, // bhaiksuki sign virama
    { 0x00011d42, 0x00011d42 }, // masaram gondi sign nukta
    { 0x00011d44, 0x00011d45 }, // masaram gondi sign halanta..masaram gondi virama
    { 0x00011d97, 0x00011d97 }, // gunjala gondi virama
    { 0x00016af0, 0x00016af4 }, // bassa vah combining high tone..bassa vah combining high-low tone
    { 0x00016f8f, 0x00016f92 }, // miao tone right..miao tone below
    { 0x00016f93, 0x00016f9f }, // miao letter tone-2..miao letter reformed tone-8
    { 0x0001d167, 0x0001d169 }, // musical symbol combining tremolo-1..musical symbol combining tremolo-3
    { 0x0001d16d, 0x0001d172 }, // musical symbol combining augmentation dot..musical symbol combining flag-5
    { 0x0001d17b, 0x0001d182 }, // musical symbol combining accent..musical symbol combining loure
    { 0x0001d185, 0x0001d18b }, // musical symbol combining doit..musical symbol combining triple tongue
    { 0x0001d1aa, 0x0001d1ad }, // musical symbol combining down bow..musical symbol combining snap pizzicato
    { 0x0001e8d0, 0x0001e8d6 }, // mende kikakui combining number teens..mende kikakui combining number millions
    { 0x0001e944, 0x0001e946 }, // adlam alif lengthener..adlam gemination mark
    { 0x0001e948, 0x0001e94a }, // adlam consonant modifier..adlam nukta
};

} // namespace system
} // namespace libbitcoin
