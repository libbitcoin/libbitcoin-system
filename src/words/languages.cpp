/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/words/languages.hpp>

#include <algorithm>
#include <string>
#include <unordered_map>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/unicode/unicode.hpp>

namespace libbitcoin {
namespace system {
namespace words {

// local definitions
// ----------------------------------------------------------------------------

typedef std::unordered_map<language, const char*> language_map;

// All languages, dictionary-independent.
// Dictionaries are collections of words in one of these languages.
// There can be multiple dictionaries for a given language identifier.

static const language_map map
{
    { language::en, "en" },
    { language::es, "es" },
    { language::it, "it" },
    { language::fr, "fr" },
    { language::cs, "cs" },
    { language::pt, "pt" },
    { language::ja, "ja" },
    { language::ko, "ko" },
    { language::zh_Hans, "zh_Hans" },
    { language::zh_Hant, "zh_Hant" }
};

// static methods
// ----------------------------------------------------------------------------

language languages::from_name(const std::string& name) NOEXCEPT
{
    const auto it = std::find_if(map.begin(), map.end(),
        [&](const language_map::value_type& pair) NOEXCEPT
        {
            return pair.second == name;
        });

    return it != map.end() ? it->first : language::none;
}

std::string languages::to_name(language identifier) NOEXCEPT
{
    const auto it = std::find_if(map.begin(), map.end(),
        [&](const language_map::value_type& pair) NOEXCEPT
        {
            return pair.first == identifier;
        });

    return it != map.end() ? it->second : "";
}

std::string languages::to_delimiter(language identifier) NOEXCEPT
{
    return identifier == language::ja ? ideographic_space : ascii_space;
}

std::string languages::join(const string_list& words,
    language identifier) NOEXCEPT
{
    // Language is specialized for joining in japanese.
    return system::join(words, to_delimiter(identifier));
}

string_list languages::split(const std::string& sentence, language) NOEXCEPT
{
    // Language is not currently specialized for splitting.
    return system::split(sentence, unicode_separators, unicode_whitespace);
}

// protected
string_list languages::try_normalize(const string_list& words) NOEXCEPT
{
    string_list normal(words.size());

    // This is only used for dictionary matching.
    // All dictionaries are confirmed via test cases to be lower/nfkd.
    std::transform(words.begin(), words.end(), normal.begin(),
        [](const std::string& word) NOEXCEPT
        {
            auto token = ascii_to_lower(trim_copy(word, unicode_whitespace));
            to_compatibility_decomposition(token);
            to_lower(token);
            return token;
        });

    return normal;
}

// constructors
// ----------------------------------------------------------------------------

// protected
languages::languages() NOEXCEPT
  : entropy_(), words_(), identifier_(language::none)
{
}

languages::languages(const languages& other) NOEXCEPT
  : entropy_(other.entropy_), words_(other.words_),
    identifier_(other.identifier_)
{
}

// protected
languages::languages(const data_chunk& entropy, const string_list& words,
    language identifier) NOEXCEPT
  : entropy_(entropy), words_(words), identifier_(identifier)
{
}

// public methods
// ----------------------------------------------------------------------------

const data_chunk& languages::entropy() const NOEXCEPT
{
    return entropy_;
}

language languages::lingo() const NOEXCEPT
{
    return identifier_;
}

std::string languages::sentence() const NOEXCEPT
{
    return join(words(), lingo());
}

const string_list& languages::words() const NOEXCEPT
{
    return words_;
}

// operators
// ----------------------------------------------------------------------------

languages::operator bool() const NOEXCEPT
{
    return !entropy_.empty();
}

languages& languages::operator=(const languages& other) NOEXCEPT
{
    entropy_ = other.entropy_;
    words_ = other.words_;
    identifier_ = other.identifier_;
    return *this;
}

bool languages::operator<(const languages& other) const NOEXCEPT
{
    return sentence() < other.sentence();
}

bool languages::operator==(const languages& other) const NOEXCEPT
{
    // Words and entropy are equivalent except in the case of electrum_v1
    // overflows. Comparing here prevents the need for electrum_v1 override.
    return entropy_ == other.entropy_ && identifier_ == other.identifier_ &&
        words_ == other.words_;
}

bool languages::operator!=(const languages& other) const NOEXCEPT
{
    return !(*this == other);
}

} // namespace words
} // namespace system
} // namespace libbitcoin
