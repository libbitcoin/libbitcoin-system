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
#ifndef LIBBITCOIN_SYSTEM_STRING_HPP
#define LIBBITCOIN_SYSTEM_STRING_HPP

#include <string>
#include <vector>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

namespace libbitcoin {
namespace system {

typedef std::vector<std::string> string_list;

inline bool starts_with(const std::string& value, const std::string& prefix)
{
    return value.substr(0, prefix.length()) == prefix;
}

/**
 * Convert a text string to the specified type.
 * @param      <Value>    The converted type.
 * @param[out] out_value  The parsed value.
 * @param[in]  text       The text to convert.
 * @param[in]  trim       True if text should be trimmed.
 * @return                True if successful.
 */
template <typename Value>
bool deserialize(Value& out_value, const std::string& text, bool trim);

/**
 * Deserialize the tokens of a text string to a vector of the inner type.
 * @param      <Value>         The inner type.
 * @param[out] out_collection  The parsed vector value.
 * @param[in]  text            The text to convert.
 * @param[in]  trim            True if text values should be trimmed.
 * @return                     True if successful.
 */
template <typename Value>
bool deserialize(std::vector<Value>& out_collection, const std::string& text,
    bool trim);

/**
 * Conveniently convert an instance of the specified type to string.
 * @param      <Value>   The type to serialize.
 * @param[in]  out_value The instance to convert.
 * @param[in]  fallback  The text to populate if value is empty.
 * @return               The serialized value.
 */
template <typename Value>
std::string serialize(const Value& out_value, const std::string& fallback="");

/**
 * Join a list of strings into a single string, in order.
 * @param[in]  words      The list of strings to join.
 * @param[in]  delimiter  The delimiter, defaults to " ".
 * @return                The resulting string.
 */
BC_API std::string join(const string_list& words,
    const std::string& delimiter=" ");

/**
 * Split a sentence on one character into string vector, compresses multiples.
 * @param[in]  sentence    The string to split.
 * @param[in]  delimiters  The set of independent delimeters, defaults to " ".
 * @param[in]  trim        Trim the sentence for whitespace, defaults to true.
 * @return                 The list of resulting strings.
 */
BC_API string_list split(const std::string& sentence,
    const std::string& delimiters=" ", bool trim=true);

/**
 * Split a sentence on a regular expression into a string vector.
 * @param[in]  sentence    The string to split.
 * @param[in]  delimiters  The splitting phrase, defaults to " ".
 * @return                 The list of resulting strings.
 */
BC_API string_list split_regex(const std::string& sentence,
    const std::string& phrase);

/**
 * Copy bytes to a new string.
 * @param[in]  source  The iterable collection of bytes to copy.
 * @returns            A new string with copied bytes.
 */
BC_API std::string to_string(const data_slice& source);

/**
 * Copy character array into a new data chunk.
 * @param[in]  source  The character array to copy.
 * @returns            A new string with copied characters.
 */
BC_API data_chunk to_chunk(const char source[]);

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/utility/string.ipp>

#endif
