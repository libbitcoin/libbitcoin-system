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
#include <bitcoin/system/wallet/languages.hpp>

#include <algorithm>
#include <map>
#include <string>
#include <boost/program_options.hpp>
#include <bitcoin/system/utility/string.hpp>
#include <bitcoin/system/wallet/language.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

// local definitions
// ----------------------------------------------------------------------------

constexpr auto ideographic_space = "\xe3\x80\x80";
constexpr auto ascii_space = "\x20";

// All languages, dictionary-independent.
// Dictionaries are collections of words in one of these langauges.
// There can be multiple dictionaries for a given language identifier.
static const std::map<language, std::string> map
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
        [&](const auto& pair)
        {
            return pair.second == name;
        });

    return it != map.end() ? it->first : language::none;
}

std::string languages::to_name(language identifier)
{
    const auto it = std::find_if(map.begin(), map.end(),
        [&](const auto& pair)
        {
            return pair.first == identifier;
        });

    return it != map.end() ? it->second : "";
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

std::string languages::join(const string_list& words,
    language identifier) const
{
    return system::join(words, identifier == language::ja ?
        ideographic_space : ascii_space);
}

string_list languages::split(const std::string& sentence,
    language identifier) const
{
    return identifier == language::ja ?
        split_regex(sentence, ideographic_space) :
        system::split(sentence, ascii_space);
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

std::ostream& operator<<(std::ostream& out, const languages& of)
{
    out << of.sentence();
    return out;
}

} // namespace wallet
} // namespace system
} // namespace libbitcoin
