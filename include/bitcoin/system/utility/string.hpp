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

/// The space (0x20) character is the only ASCII separator.
extern const std::string ascii_space;
extern const string_list ascii_separators;
extern const string_list ascii_whitespace;

// ASCII.
// ----------------------------------------------------------------------------
// TODO: move to unicode/ascii.hpp/.cpp

/// Convert each ASCII letter in text to lower case.
BC_API std::string ascii_to_lower(const std::string& text);

/// Convert each ASCII letter in text to upper case.
BC_API std::string ascii_to_upper(const std::string& text);

/// True if text has upper and lower ASCII case letters.
BC_API bool has_mixed_ascii_case(const std::string& text);

/// True if all characters are in the ASCII subset of UTF8 [<128].
BC_API bool is_ascii(const std::string& text);

// Utilities.
// ----------------------------------------------------------------------------

/// Join a list of string tokens in order into a single string.
/// Delimiter defaults to ASCII space (0x20), and can be any length.
BC_API std::string join(const string_list& tokens,
    const std::string& delimiter=ascii_space);

// TODO: replace trim parameter with trim_tokens default parameter
/// Trim and compress string tokens.
/// ASCII trimming, compression removes empty tokens, except last.
BC_API void reduce(string_list& tokens, bool trim=true, bool compress=true);

// TODO: replace trim parameter with trim_tokens default parameter
/// Trim and compress string tokens and return result.
/// ASCII trimming, compression removes empty tokens, except last.
BC_API string_list reduce_copy(const string_list& tokens, bool trim=true,
    bool compress=true);

/// Replace all instances of 'from' with 'to', return number of replacements.
BC_API size_t replace(std::string& text, const std::string& from,
    const std::string& to);

/// Split text into a list of tokens, always returns at least one.
/// ASCII trimming, compression removes empty tokens, except last.
BC_API string_list split(const std::string& text, bool trim=true,
    bool compress=true);

/// Split text into a list of tokens, always returns at least one.
/// ASCII trimming, compression removes empty tokens, except last.
BC_API string_list split(const std::string& text, const std::string& delimiter,
    bool trim=true, bool compress=true);

// TODO: replace trim parameter with trim_tokens default parameter.
// TODO: default delimiters parameter to unicode_separators value.
// TODO: default delimiters parameter to unicode_whitespace value.
/// Split text into a list of tokens, always returns at least one.
/// ASCII trimming, compression removes empty tokens, except last.
BC_API string_list split(const std::string& text, const string_list& delimiters,
    bool trim=true, bool compress=true);

/// Return true if text starts with the prefix.
BC_API bool starts_with(const std::string& text, const std::string& prefix);

/// Convert a data slice to text, uint8_t is cast to char.
BC_API std::string to_string(const data_slice& bytes);

// TODO: add trim_tokens default parameter.
/// Trim ASCII whitespace from both ends of text.
BC_API void trim(std::string& text);

// TODO: add trim_tokens default parameter.
/// Trim ASCII whitespace from both ends of text and return result.
BC_API std::string trim_copy(const std::string& text);

} // namespace system
} // namespace libbitcoin

#endif
