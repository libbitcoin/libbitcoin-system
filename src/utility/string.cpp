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
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string_regex.hpp>
#include <bitcoin/system/utility/data.hpp>

namespace libbitcoin {
namespace system {

std::string ascii_to_lower(const std::string& text)
{
    const auto to_lower = [](uint8_t value)
    {
        return 'A' <= value && value <= 'Z' ? value + ('a' - 'A') : value;
    };

    std::string lower = text;
    std::transform(text.begin(), text.end(), lower.begin(),
        [&](uint8_t character)
        {
            return to_lower(character);
        });

    return lower;
}

bool is_ascii(const std::string& text)
{
    return std::all_of(text.begin(), text.end(), [](uint8_t character)
    {
        return character < 0x80;
    });
}

std::string join(const string_list& words, const std::string& delimiter)
{
    return boost::join(words, delimiter);
}

// Note that use of token_compress_on may cause unexpected results when
// working with CSV-style lists that accept empty elements.
string_list split(const std::string& sentence, const std::string& delimiters,
    bool trim)
{
    string_list words;
    const auto compress = boost::token_compress_on;
    const auto delimit = boost::is_any_of(delimiters);

    if (trim)
    {
        const auto trimmed = boost::trim_copy(sentence);
        boost::split(words, trimmed, delimit, compress);
    }
    else
        boost::split(words, sentence, delimit, compress);

    return words;
}

string_list split_regex(const std::string& sentence,
    const std::string& phrase)
{
    string_list words;
    return boost::algorithm::split_regex(words, sentence, boost::regex(phrase));
}

bool starts_with(const std::string& value, const std::string& prefix)
{
    return value.substr(0, prefix.length()) == prefix;
}

std::string to_string(const data_slice& source)
{
    std::string out(source.begin(), source.end());
    return out;
}

data_chunk to_chunk(const char source[])
{
    std::string iterable = source;
    return to_chunk(iterable);
}

} // namespace system
} // namespace libbitcoin
