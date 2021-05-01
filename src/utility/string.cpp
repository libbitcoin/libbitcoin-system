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
#include <sstream>
#include <string>
#include <bitcoin/system/utility/data.hpp>

namespace libbitcoin {
namespace system {

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

bool is_ascii(const std::string& text)
{
    return std::all_of(text.begin(), text.end(), is_ascii_character);
}

bool is_ascii_character(char character)
{
    return (character & 0x80) == 0;
}

// This is locale independent and ensures compiler consistency.
// The default "C" locale is not always present for std::isspace.
bool is_ascii_whitespace(char character)
{
    return
        character == 0x20 ||
        character == '\t' ||
        character == '\n' ||
        character == '\v' ||
        character == '\f' ||
        character == '\r';
}

std::string join(const string_list& tokens, const std::string& delimiter)
{
    if (tokens.empty())
        return {};

    // Start with the first token.
    std::stringstream sentence;
    sentence << tokens.front();

    // Add remaining tokens preceded by delimiters.
    for (auto token = std::next(tokens.begin()); token != tokens.end(); ++token)
        sentence << delimiter << *token;

    return sentence.str();
}

string_list split(const std::string& text, const std::string& delimiter,
    bool trim, bool compress)
{
    string_list tokens;

    // Apply trimming and compression when pushing tokens.
    const auto push = [trim, &tokens](std::string& token, bool compress)
    {
        if (trim)
            system::trim(token);

        if (!(compress && token.empty()))
            tokens.push_back(token);
    };

    size_t next = 0;
    const auto size = delimiter.size();
    auto position = text.find(delimiter, next);

    // Push all but the last token.
    while (position != std::string::npos)
    {
        push(text.substr(next, position - next), compress);
        next = position + size;
        position = text.find(delimiter, next);
    }

    // Do not compress if only one (empty) token.
    compress &= !tokens.empty();

    // Push last token (delimiter not found).
    push(text.substr(next, text.length() - next), compress);
    return tokens;
}

bool starts_with(const std::string& text, const std::string& prefix)
{
    // Guarded against prefix length greater than string length.
    return prefix.length() <= text.length() &&
        (text.substr(0, prefix.length()) == prefix);
}

std::string to_string(const data_slice& bytes)
{
    return bytes.to_string();
}

std::string trim_copy(const std::string& text)
{
    auto copy = text;
    trim(copy);
    return copy;
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

} // namespace system
} // namespace libbitcoin
