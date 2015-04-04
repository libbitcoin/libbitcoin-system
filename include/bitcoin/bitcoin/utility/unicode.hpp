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

// This pragma causes sources to be explicitly interpreted as UTF8.
// Otherwise UTF8 literals must be saved in UTF8 source files
// without a BOM ("signature" in the Visual Studio save dialog).
// BUGBUG: this technique improperly encodes "フ" (for example),
// so instead use non-BOM UTF encoded files.
// #pragma execution_character_set("utf-8")

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

