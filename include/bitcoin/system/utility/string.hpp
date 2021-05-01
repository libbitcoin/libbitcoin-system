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
#include <bitcoin/system/utility/data_slice.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace libbitcoin {
namespace system {

typedef std::vector<std::string> string_list;

/// Convert each ASCII letter in text to lower case.
BC_API std::string ascii_to_lower(const std::string& text);

/// Convert each ASCII letter in text to upper case.
BC_API std::string ascii_to_upper(const std::string& text);

/// True if text has upper and lower ASCII case letters.
BC_API bool has_mixed_ascii_case(const std::string& text);

/// True if all characters are in the ASCII subset of UTF8.
BC_API bool is_ascii(const std::string& text);

/// True if the character is in the ASCII subset of UTF8.
BC_API bool is_ascii_character(char character);

/// True if the character is an ASCII "whitespace" [\x20,\t,\n,\v,\f,\r].
BC_API bool is_ascii_whitespace(char character);

/// Join a list of string tokens in order into a single string.
/// Delimiter defaults to ASCII space (0x20), and can be any length.
BC_API std::string join(const string_list& tokens,
    const std::string& delimiter="\x20");

/// Split text into a list of tokens, always returns at least one.
/// Delimiter can be any length, defaults to ASCII space (0x20).
/// Trimming applies to each token independently, defaults to true.
/// Compression removes empty tokens, defaults to true.
BC_API string_list split(const std::string& text,
    const std::string& delimiter="\x20", bool trim=true, bool compress=true);

/// Return true if text starts with the prefix.
BC_API bool starts_with(const std::string& text, const std::string& prefix);

/// Convert a data slice to text, uint8_t is cast to char.
BC_API std::string to_string(const data_slice& bytes);

/// Trim ASCII whitespace from both ends of text.
BC_API void trim(std::string& text);

/// Trim ASCII whitespace from both ends of text and return result.
BC_API std::string trim_copy(const std::string& text);

} // namespace system
} // namespace libbitcoin

#endif
