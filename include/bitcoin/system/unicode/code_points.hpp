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
#ifndef LIBBITCOIN_SYSTEM_UNICODE_CODE_POINTS_HPP
#define LIBBITCOIN_SYSTEM_UNICODE_CODE_POINTS_HPP

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace libbitcoin {
namespace system {

typedef std::vector<std::string> string_list;
typedef std::pair<char32_t, char32_t> utf32_interval;
typedef std::vector<utf32_interval> utf32_intervals;

/// The space (0x00000020) character is the only ASCII separator.
extern const std::string ascii_space;
extern const string_list ascii_separators;
extern const string_list ascii_whitespace;

/// The ideographic space (0x00003000) character is required for BIP39.
extern const std::string ideographic_space;
extern const string_list unicode_separators;
extern const string_list unicode_whitespace;

/// ASCII subset of Unicode tables.
constexpr size_t char_separators_count = 1;
constexpr size_t char_whitespace_count = 6;
extern const char char_separators[char_separators_count];
extern const char char_whitespace[char_whitespace_count];

/// Unicode character subsets.
constexpr size_t char32_separators_count = 17;
constexpr size_t char32_whitespace_count = 25;
extern const char32_t char32_separators[char32_separators_count];
extern const char32_t char32_whitespace[char32_whitespace_count];

/// Unicode character subset ranges.
extern const utf32_intervals chinese_japanese_korean;
extern const utf32_intervals diacritics;

} // namespace system
} // namespace libbitcoin

#endif
