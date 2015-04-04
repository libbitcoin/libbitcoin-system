/**
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_UNICODE_HPP
#define LIBBITCOIN_UNICODE_HPP

#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#ifdef _MSC_VER
    #include <bitcoin/bitcoin/utility/unicode.hpp>
    #include <bitcoin/bitcoin/utility/unicode_streams.hpp>
#endif

// Regarding Unicode design for Windows:
//
// Windows and other environments, such as Java, that supported Unicode prior
// to the advent of utf8 utilize 16 bit characters. These are typically encoded
// as wchar_t in C++. Unicode no longer fits in 16 bits and as such these
// implementations now require variable length character encoding just as utf8.
//
// Libbitcoin embraces the "utf8 everywhere" design: http://utf8everywhere.org
// The objective is to use utf8 as the canonical string encoding, pushing
// wchar_t translation to the edge (stdio, argv, O/S and external API calls).
// The macro BC_USE_LIBBITCOIN_MAIN does most of the heavy lifting to ensure
// that stdio and argv is configured for utf8. The functions 'widen' and 
// 'narrow' are provided for API translation.

// Regarding Unicode source files in VC++ builds:
//
// Don't use #pragma execution_character_set("utf-8") in Windows.
// This instruction causes sources to be explicitly interpreted as UTF8.
// However this technique improperly encodes literals ("フ" for example).
// Instead use non-BOM UTF encoded files. To do this use "save as..."
// without a "signature" (BOM) in the Visual Studio save dialog.

// Regarding Unicode in console applications:
//
// BC_USE_LIBBITCOIN_MAIN should be declared prior to bc::main() in a console
// application. This enables Unicode stdio and argument processing in Windows.
// This macro implements main() and forwards to bc::main(), which should be 
// implemented as if it was main() with the expectation that argv is utf8.
//
// Do not use cout|cerr|cin (aborts on assertion):
// std::cout << "text";
// std::cerr << "text";
// std::string text;
// std::cin >> text;
//
// Do not use the L qualifier when source is UTF-8 w/out BOM (mangles output):
// const auto utf16 = L"acción.кошка.日本国";
// std::wcout << utf16;

#ifdef _MSC_VER
#define BC_USE_LIBBITCOIN_MAIN \
    _BC_DECLARE_LIBBITCOIN_MAIN \
    int wmain(int argc, wchar_t* argv[]) \
    { \
        bc::unicode_streams::initialize(); \
        auto buffer = libbitcoin::narrow(argc, argv); \
        auto args = reinterpret_cast<char**>(&buffer[0]); \
        return libbitcoin::main(argc, args); \
    }
#else
#define BC_USE_LIBBITCOIN_MAIN \
    _BC_DECLARE_LIBBITCOIN_MAIN \
    int main(int argc, char* argv[]) \
    { \
        return libbitcoin::main(argc, argv); \
    }
#endif

#define _BC_DECLARE_LIBBITCOIN_MAIN \
namespace libbitcoin { \
    int main(int argc, char* argv[]); \
}

namespace libbitcoin {

/**
 * Caller should reinterpret result as:
 * auto args = reinterpret_cast<char**>(&buffer[0])
 * @param[in]  argc  The number of elements in argv.
 * @param[in]  argv  The wide command line arguments
 * @return           A buffer holding the narrow version of argv.
 */
BC_API data_chunk narrow(int argc, wchar_t* argv[]);

/**
 * Convert a wchar_t (presumed UTF16) string to wide (UTF8/char).
 * @param[in]  wide  The utf16 string to convert.
 * @return           The resulting utf8 string.
 */
BC_API std::string narrow(const std::wstring& wide);

/**
 * Convert a char (presumed UTF8) string to wide (UTF16/wchar_t).
 * @param[in]  narrow  The utf8 string to convert.
 * @return             The resulting utf16 string.
 */
BC_API std::wstring widen(const std::string& narrow);

} // namespace libbitcoin

#endif

