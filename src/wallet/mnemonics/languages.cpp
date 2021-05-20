/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/wallet/mnemonics/languages.hpp>

#include <algorithm>
#include <map>
#include <string>
#include <bitcoin/system/unicode/ascii.hpp>
#include <bitcoin/system/unicode/code_points.hpp>
#include <bitcoin/system/unicode/normalization.hpp>
#include <bitcoin/system/utility/string.hpp>
#include <bitcoin/system/wallet/mnemonics/language.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

// local definitions
// ----------------------------------------------------------------------------

typedef std::map<language, std::string> language_map;

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

language languages::from_name(const std::string& name)
{
    const auto it = std::find_if(map.begin(), map.end(),
        [&](const language_map::value_type& pair)
        {
            return pair.second == name;
        });

    return it != map.end() ? it->first : language::none;
}

std::string languages::to_name(language identifier)
{
    const auto it = std::find_if(map.begin(), map.end(),
        [&](const language_map::value_type& pair)
        {
            return pair.first == identifier;
        });

    return it != map.end() ? it->second : "";
}

std::string languages::to_delimiter(language identifier)
{
    return identifier == language::ja ? ideographic_space : ascii_space;
}

std::string languages::join(const string_list& words, language identifier)
{
    return system::join(words, to_delimiter(identifier));
}

string_list languages::split(const std::string& sentence, language)
{
    return system::split(sentence, unicode_separators, unicode_whitespace);
}

// protected
string_list languages::try_normalize(const string_list& words)
{
    auto sentence = ascii_to_lower(system::join(words));
    to_compatibility_decomposition(sentence);
    to_lower(sentence);
    return system::split(sentence);
}

// protected constructors
// ----------------------------------------------------------------------------

languages::languages()
  : entropy_(), words_(), identifier_(language::none)
{
}

languages::languages(const languages& other)
  : entropy_(other.entropy_), words_(other.words_),
    identifier_(other.identifier_)
{
}

languages::languages(const data_chunk& entropy, const string_list& words,
    language identifier)
  : entropy_(entropy), words_(words), identifier_(identifier)
{
}

// public methods
// ----------------------------------------------------------------------------

const data_chunk& languages::entropy() const
{
    return entropy_;
}

language languages::lingo() const
{
    return identifier_;
}

std::string languages::sentence() const
{
    return join(words(), lingo());
}

const string_list& languages::words() const
{
    return words_;
}

// operators
// ----------------------------------------------------------------------------

languages::operator bool() const
{
    return !entropy_.empty();
}

languages& languages::operator=(const languages& other)
{
    entropy_ = other.entropy_;
    words_ = other.words_;
    identifier_ = other.identifier_;
    return *this;
}

bool languages::operator<(const languages& other) const
{
    return sentence() < other.sentence();
}

bool languages::operator==(const languages& other) const
{
    // Words and entropy are equivalent (one is a cache of the other).
    return entropy_ == other.entropy_ && identifier_ == other.identifier_;
}

bool languages::operator!=(const languages& other) const
{
    return !(*this == other);
}

} // namespace wallet
} // namespace system
} // namespace libbitcoin
