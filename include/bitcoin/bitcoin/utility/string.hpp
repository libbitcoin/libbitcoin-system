/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_STRING_HPP
#define LIBBITCOIN_STRING_HPP

#include <iterator>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>
#include <bitcoin/bitcoin/define.hpp>

/* NOTE: don't declare 'using namespace foo' in headers. */

namespace libbitcoin {

#define BC_SENTENCE_DELIMITER " "

/**
 * Join a list of strings into a single string, in order.
 * @param[in]  words      The list of strings to join.
 * @param[in]  delimiter  The delimiter, defaults to BC_SENTENCE_DELIMITER.
 * @return                The resulting string.
 */
BC_API std::string join(const std::vector<std::string>& words,
    const std::string& delimiter=BC_SENTENCE_DELIMITER);

/**
 * Split a list of strings into a string vector string, in order, white space
 * delimited.
 * @param[in]  sentence   The string to split.
 * @param[in]  delimiter  The delimeter, defaults to BC_SENTENCE_DELIMITER.
 * @return                The list of resulting strings.
 */
BC_API std::vector<std::string> split(const std::string& sentence,
    const std::string& delimiter=BC_SENTENCE_DELIMITER);

/**
 * Trim a string of whitespace.
 * @param[out] value  The string to trim.
 */
BC_API void trim(std::string& value);

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
