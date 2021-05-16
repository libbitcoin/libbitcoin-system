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
#include <bitcoin/system/utility/string.hpp>

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <sstream>
#include <string>
#include <bitcoin/system/utility/data.hpp>

namespace libbitcoin {
namespace system {

constexpr size_t ascii_separators8_count = 1;
constexpr size_t ascii_whitespace8_count = 6;
extern const char ascii_separators8[ascii_separators8_count];
extern const char ascii_whitespace8[ascii_whitespace8_count];

// Utilities.
// ----------------------------------------------------------------------------

// All string utilities are based on utf8 encoded values.
// Every byte of a non-ascii utf8 character has high order bit set.
    
std::string ascii_to_lower(const std::string& text)
{
    auto copy = text;

    const auto to_lower = [](char value)
    {
        return 'A' <= value && value <= 'Z' ? value + ('a' - 'A') : value;
    };

    std::transform(text.begin(), text.end(), copy.begin(), to_lower);
    return copy;
}

std::string ascii_to_upper(const std::string& text)
{
    auto copy = text;

    const auto to_upper = [](char value)
    {
        return 'a' <= value && value <= 'z' ? value + ('A' - 'a') : value;
    };

    std::transform(text.begin(), text.end(), copy.begin(), to_upper);
    return copy;
}

bool has_mixed_ascii_case(const std::string& text)
{
    auto lower = false;
    auto upper = false;
    
    const auto set_lower_and_upper = [&](char character)
    {
        lower |= ('a' <= character && character <= 'z');
        upper |= ('A' <= character && character <= 'Z');
    };

    std::for_each(text.begin(), text.end(), set_lower_and_upper);
    return lower && upper;
}

inline bool is_ascii_character(char character)
{
    // en.wikipedia.org/wiki/ASCII
    return (character & 0x80) == 0;
}

static bool is_ascii_whitespace(char character)
{
    for (size_t space = 0; space < ascii_whitespace8_count; ++space)
        if (character == ascii_whitespace8[space])
            return true;

    return false;
}

bool is_ascii(const std::string& text)
{
    return std::all_of(text.begin(), text.end(), is_ascii_character);
}

std::string join(const string_list& tokens, const std::string& delimiter)
{
    if (tokens.empty())
        return {};

    // Start with the first token.
    std::ostringstream sentence;
    sentence << tokens.front();

    // Add remaining tokens preceded by delimiters.
    for (auto token = std::next(tokens.begin()); token != tokens.end(); ++token)
        sentence << delimiter << *token;

    return sentence.str();
}

void reduce(string_list& tokens, bool trim, bool compress)
{
    static const std::string empty{};

    if (tokens.empty())
        return;

    if (trim)
        for (auto& token: tokens)
            system::trim(token);

    if (compress)
        tokens.erase(std::remove(tokens.begin(), tokens.end(), empty),
            tokens.end());

    if (tokens.empty())
        tokens.push_back({});
}

string_list reduce_copy(const string_list& tokens, bool trim, bool compress)
{
    auto copy = tokens;
    reduce(copy, trim, compress);
    return copy;
}

size_t replace(std::string& text, const std::string& from, const std::string& to)
{
    size_t count = 0;

    for (auto position = text.find(from, 0);
        position != std::string::npos;
        position = text.find(from, position + to.length()))
    {
        ++count;
        text.replace(position, from.length(), to);
    }

    return count;
}

static string_list splitter(const std::string& text, const std::string& delimiter,
    bool trim, bool compress)
{
    size_t start = 0;
    string_list tokens;

    // Push all but the last token.
    for (auto position = text.find(delimiter, 0);
        position != std::string::npos;
        position = text.find(delimiter, start))
    {
        tokens.push_back(text.substr(start, position - start));
        start = position + delimiter.length();
    }

    // Push last token (delimiter not found).
    tokens.push_back(text.substr(start, text.length() - start));
    reduce(tokens, trim, compress);
    return tokens;
}

string_list split(const std::string& text, const string_list& delimiters,
    bool trim, bool compress)
{
    if (delimiters.empty())
        return { trim ? trim_copy(text) : text };

    // Avoids copying text if only one delimiter.
    if (delimiters.size() == 1u)
        return splitter(text, delimiters.front(), trim, compress);

    auto copy = text;
    const auto first = delimiters.front();

    // Replace all other delimiters with the first delimiter.
    for (auto it = std::next(delimiters.begin()); it != delimiters.end(); ++it)
        replace(copy, *it, first);

    // Split copy on the first delimiter.
    return splitter(copy, first, trim, compress);
}

string_list split(const std::string& text, const std::string& delimiter,
    bool trim, bool compress)
{
    return split(text, string_list{ delimiter }, trim, compress);
}

string_list split(const std::string& text, bool trim, bool compress)
{
    return split(text, ascii_separators, trim, compress);
}

bool starts_with(const std::string& text, const std::string& prefix)
{
    return text.find(prefix, 0) == 0u;
}

std::string to_string(const data_slice& bytes)
{
    return bytes.to_string();
}

void trim(std::string& text)
{
    // Find the first non-space or end and erase to that point.
    const auto first = std::find_if_not(text.begin(), text.end(),
        is_ascii_whitespace);
    text.erase(text.begin(), first);

    // Find the last non-space or rend and erase from that point.
    const auto last = std::find_if_not(text.rbegin(), text.rend(),
        is_ascii_whitespace);
    text.erase(last.base(), text.end());
    text.shrink_to_fit();
}

std::string trim_copy(const std::string& text)
{
    auto copy = text;
    trim(copy);
    return copy;
}

// ASCII reference data.
// ----------------------------------------------------------------------------
// en.wikipedia.org/wiki/Whitespace_character

const char ascii_separators8[]
{
    // ASCII separator characters.
    0x20  // space ' '
};

const std::string ascii_space
{
    ascii_separators8[0]
};

const string_list ascii_separators
{
    { ascii_separators8[0] }
};

const char ascii_whitespace8[]
{
    // ASCII whitespace characters (C whitespace).
    0x09, // character tabulation '\t'
    0x0a, // line feed '\n'
    0x0b, // line tabulation '\v'
    0x0c, // form feed '\f'
    0x0d, // carriage return '\r'
    0x20  // space ' '
};

const string_list ascii_whitespace
{
    { ascii_whitespace8[0] },
    { ascii_whitespace8[1] },
    { ascii_whitespace8[2] },
    { ascii_whitespace8[3] },
    { ascii_whitespace8[4] },
    { ascii_whitespace8[5] }
};

} // namespace system
} // namespace libbitcoin
