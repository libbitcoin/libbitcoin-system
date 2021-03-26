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
#ifndef LIBBITCOIN_SYSTEM_UNICODE_HPP
#define LIBBITCOIN_SYSTEM_UNICODE_HPP

#include <cstddef>
#include <iostream>
#include <string>
#include <utility>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/utility/data.hpp>

// Regarding Unicode design for Windows:
//
// Windows and other environments, such as Java, that supported Unicode prior
// to the advent of utf8 utilize 16 bit characters. These are typically encoded
// as wchar_t in C++. Unicode no longer fits in 16 bits and as such these
// implementations now require variable length character encoding just as utf8.
// Windows support for UCS-2 was added in Windows NT 3.1. Support for UTF-16
// was added in Windows 2000. So its wide characters are also variable length.
// boost.org/doc/libs/1_75_0/libs/locale/doc/html/recommendations_and_myths.html
//
// Libbitcoin embraces the "utf8 everywhere" design: utf8everywhere.org
// The objective is to use utf8 as the canonical string encoding, pushing
// wchar_t translation to the edge (stdio, argv, O/S and external API calls).
// The macro BC_USE_LIBBITCOIN_MAIN does most of the heavy lifting to ensure
// that stdio and argv is configured for utf8. The 'to_utf' functions are
// provided for API translations.

// Regarding Unicode source files in VC++ builds:
//
// Don't use #pragma execution_character_set("utf-8") in Windows.
// This instruction causes sources to be explicitly interpreted as utf8.
// However this technique improperly encodes literals ("フ" for example).
// Instead use non-BOM UTF encoded files. To do this use "save as..."
// utf8 without a "signature" (BOM) in the Visual Studio save dialog.
// This is the typical file format for C++ sources in other environments.

// Regarding use of boost:
//
// When working with boost and utf8 narrow characters on Windows the thread
// must be configured for utf8. When working with boost::filesystem::path the
// static path object must be imbued with the utf8 locale or paths will be
// incorrectly translated.

// Regarding Unicode in console applications:
//
// BC_USE_LIBBITCOIN_MAIN should be declared prior to bc::system::main() in a
// console application. This enables Unicode argument and environment
// processing in Windows. This macro implements main() and forwards to
// bc::system::main(), which should be implemented as if it was main() with
// the expectation that argv is utf8.
//
// Do not use std::cout|std::cerr|std::cin (aborts on assertion):
// std::cout << "text";
// std::cerr << "text";
// std::string text;
// std::cin >> text;
//
// Do not use the L qualifier when source is UTF-8 w/out BOM (mangles output):
// const auto utf16 = L"acción.кошка.日本国";
// std::wcout << utf16;

namespace libbitcoin {
namespace system {

/**
 * Use bc::system::cin in place of std::cin, see BC_USE_LIBBITCOIN_MAIN.
 */
std::istream& cin_stream();

/**
 * Use bc::system::cout in place of std::cout, see BC_USE_LIBBITCOIN_MAIN.
 */
std::ostream& cout_stream();

/**
 * Use bc::system::cerr in place of std::cerr, see BC_USE_LIBBITCOIN_MAIN.
 */
std::ostream& cerr_stream();

#ifdef WITH_ICU

/**
 * Normalize a string value using nfc normalization.
 * Failure is indicated by empty string return for non-empty value.
 * This function requires the ICU dependency.
 * @param[in]  value  The value to normalize.
 * @return            The normalized value.
 */
BC_API std::string to_normal_nfc_form(const std::string& value);

/**
 * Normalize a string value using nfkd normalization.
 * Failure is indicated by empty string return for non-empty value.
 * This function requires the ICU dependency.
 * @param[in]  value  The value to normalize.
 * @return            The normalized value.
 */
BC_API std::string to_normal_nfkd_form(const std::string& value);

/**
 * Lowercase a string value with locale awareness.
 * This function requires the ICU dependency.
 * @param[in]  value  The value to lowercase.
 * @return            The lowercased value.
 */
BC_API std::string to_lower(const std::string& value);

/**
 * Remove accent characters (diacritics).
 * This function requires the ICU dependency.
 * @param[in]  value  The value to scrub.
 * @return            The scrubbed value.
 */
std::string to_unaccented_form(const std::string& value);

/**
 * Remove spaces between chinese, japanese, and korean characters.
 * This function requires the ICU dependency.
 * @param[in]  value  The value to scrub.
 * @return            The scrubbed value.
 */
std::string to_compressed_cjk_form(const std::string& value);

#endif // WITH_ICU

/**
 * Convert a wide (UTF16/wchar_t) array to narrow (UTF8/char).
 * @param[in]  out_to      The converted narrow array.
 * @param[in]  to_bytes    The allocated number of bytes in 'out_to'.
 * @param[in]  from        The wide character array to convert.
 * @param[in]  from_chars  The number of 'from' wide characters to convert.
 * @return                 The number of bytes converted.
 */
BC_API size_t to_utf8(char out_to[], size_t to_bytes, const wchar_t from[],
    size_t from_chars);

/**
 * Convert a wide (UTF16/wchar_t) string to narrow (UTF8/char).
 * @param[in]  wide  The utf16 string to convert.
 * @return           The resulting utf8 string.
 */
BC_API std::string to_utf8(const std::wstring& wide);

/**
 * Convert a wide (UTF32/char32_t) string to narrow (UTF8/char).
 * @param[in]  wide  The utf32 string to convert.
 * @return           The resulting utf8 string.
 */
BC_API std::string to_utf8(const std::u32string& wide);

/**
 * Convert a narrow (UTF8/char) array to wide (UTF16/wchar_t).
 * This is designed for buffering, where the narrow array may have been
 * truncated in the middle of a multiple byte character. The terminating
 * offset is returned in the 'out_truncated' out parameter.
 * @param[in]  out_truncated  The number of 'from' bytes [0..3] truncated.
 * @param[in]  out_to         The converted wide array.
 * @param[in]  to_chars       The allocated number of wide chars in 'out_to'.
 * @param[in]  from           The narrow array to convert.
 * @param[in]  from_bytes     The number of 'from' bytes to convert.
 * @return                    The number of characters converted.
 */
BC_API size_t to_utf16(uint8_t& out_truncated, wchar_t out_to[],
    size_t to_chars, const char from[], size_t from_bytes);

/**
 * Convert a narrow (UTF8/char) string to wide (UTF16/wchar_t).
 * @param[in]  narrow  The utf8 string to convert.
 * @return             The resulting utf16 string.
 */
BC_API std::wstring to_utf16(const std::string& narrow);

/**
 * Convert a narrow (UTF8/char) string to wide (UTF32/char32_t).
 * @param[in]  narrow  The utf8 string to convert.
 * @return             The resulting utf32 string.
 */
BC_API std::u32string to_utf32(const std::string& narrow);

/**
 * Initialize windows to use UTF8 for stdio. This cannot be uninitialized and
 * once set bc stdio must be used in place of std stdio.
 */
BC_API void set_utf8_stdio();

/**
 * Initialize windows to use UTF8 for stdin. This cannot be uninitialized and
 * once set bc::system::cin must be used in place of std::cin.
 */
BC_API void set_utf8_stdin();

/**
 * Initialize windows to use UTF8 for stdout. This cannot be uninitialized and
 * once set bc::system::cout must be used in place of std::cout.
 */
BC_API void set_utf8_stdout();

/**
 * Initialize windows to use UTF8 for stderr. This cannot be uninitialized and
 * once set bc::system::cerr must be used in place of std::cerr.
 */
BC_API void set_utf8_stderr();

/**
 * Initialize windows to use binary for stdin. This cannot be uninitialized.
 */
BC_API void set_binary_stdin();

/**
 * Initialize windows to use binary for stdout. This cannot be uninitialized.
 */
BC_API void set_binary_stdout();

#ifdef _MSC_VER

// Exposed for testing only, use BC_USE_LIBBITCOIN_MAIN.
uint8_t offset_to_terminal_utf8_character(const char text[], size_t size);
void free_environment(char* environment[]);
char** allocate_environment(wchar_t* environment[]);
char** allocate_environment(int argc, wchar_t* argv[]);
int call_utf8_main(int argc, wchar_t* argv[],
    int(*main)(int argc, char* argv[]));

#define BC_USE_LIBBITCOIN_MAIN \
    namespace libbitcoin \
    { \
        std::istream& cin = cin_stream(); \
        std::ostream& cout = cout_stream(); \
        std::ostream& cerr = cerr_stream(); \
        int main(int argc, char* argv[]); \
    } \
    \
    int wmain(int argc, wchar_t* argv[]) \
    { \
        return libbitcoin::call_utf8_main(argc, argv, \
            &libbitcoin::main); \
    }

#else

#define BC_USE_LIBBITCOIN_MAIN \
    namespace libbitcoin \
    { \
        std::istream& cin = std::cin; \
        std::ostream& cout = std::cout; \
        std::ostream& cerr = std::cerr; \
        int main(int argc, char* argv[]); \
    } \
    \
    int main(int argc, char* argv[]) \
    { \
        return libbitcoin::main(argc, argv); \
    }

#endif

} // namespace libbitcoin

#endif
