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

/**
 * Copy data elements to a new string.
 * @param[in]  source  The collection of elements to cast.
 * @returns            A new string with copyed elements.
 */
BC_API std::string to_string(const data_slice& source);

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
 * Split a list of strings into a string vector string, in order, white space
 * delimited.
 * @param[in]  sentence   The string to split.
 * @param[in]  delimiter  The delimeter, defaults to " ".
 * @param[in]  trim       Trim the sentence for whitespace, defaults to true.
 * @return                The list of resulting strings.
 */
BC_API string_list split(const std::string& sentence,
    const std::string& delimiter=" ", bool trim=true);

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/utility/string.ipp>

#endif
