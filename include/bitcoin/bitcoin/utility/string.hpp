/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_STRING_HPP
#define LIBBITCOIN_STRING_HPP

#include <string>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

namespace libbitcoin {

typedef std::vector<std::string> string_list;

/**
 * Convert a text string to the specified type.
 * @param      <Value>  The converted type.
 * @param[in]  text     The text to convert.
 * @param[in]  trim     True if value should be trimmed before conversion.
 * return               The parsed value.
 */
template <typename Value>
Value deserialize(const std::string& text, bool trim);

/**
 * Read an input stream to the specified type.
 * @param      <Value>  The converted type.
 * @param[in]  input    The stream to convert.
 * @param[in]  trim     True if value should be trimmed before conversion.
 * return               The parsed value.
 */
template <typename Value>
Value deserialize(std::istream& input, bool trim);

/**
 * Convert a text string to the specified type.
 * @param      <Value>  The converted type.
 * @param[out] value    The parsed value.
 * @param[in]  text     The text to convert.
 * @param[in]  trim     True if value should be trimmed before conversion.
 */
template <typename Value>
void deserialize(Value& value, const std::string& text, bool trim);

/**
 * Read an input stream to the specified type.
 * @param      <Value>  The converted type.
 * @param[out] value    The parsed value.
 * @param[in]  input    The stream to convert.
 * @param[in]  trim     True if value should be trimmed before conversion.
 */
template <typename Value>
void deserialize(Value& value, std::istream& input, bool trim);

/**
 * Deserialize the tokens of a text string to a vector of the inner type.
 * @param      <Value>     The inner type.
 * @param[out] collection  The parsed vector value.
 * @param[in]  text        The text to convert.
 * @param[in]  trim        True if value should be trimmed before conversion.
 */
template <typename Value>
void deserialize(std::vector<Value>& collection, const std::string& text,
    bool trim);

/**
 * Deserialize a satoshi item from the specified binary data.
 * @param      <Item>  The type of the item to parse.
 * @param[out] item    The deserialized item.
 * @param[in]  data    The binary data.
 * @return             True if a item was parsed.
 */
template <typename Item>
bool deserialize_satoshi_item(Item& item, const data_chunk& data);

/**
 * Conveniently convert an instance of the specified type to string.
 * @param      <Value>   The type to serialize.
 * @param[in]  value     The instance to convert.
 * @param[in]  fallback  The text to populate if value is empty.
 * @return               The serialized value.
 */
template <typename Value>
std::string serialize(const Value& value, const std::string& fallback="");

/**
 * Serialize the specified satoshi item to binary data.
 * The data will be resized as necessary to fit the item.
 * @param       <Item>  The type of the item.
 * @param[out] data     The binary data.
 * @param[in]  item     The satoshi item.
 */
template <typename Item>
data_chunk serialize_satoshi_item(const Item& item);


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

} // namespace libbitcoin

#include <bitcoin/bitcoin/impl/utility/string.ipp>

#endif
