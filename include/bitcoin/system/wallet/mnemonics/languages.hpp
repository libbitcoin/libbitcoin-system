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
#ifndef LIBBITCOIN_SYSTEM_WALLET_LANGUAGES_HPP
#define LIBBITCOIN_SYSTEM_WALLET_LANGUAGES_HPP

#include <string>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/utility/string.hpp>
#include <bitcoin/system/wallet/keys/hd_private.hpp>
#include <bitcoin/system/wallet/mnemonics/language.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

class languages
{
public:
    // static methods
    // ------------------------------------------------------------------------

    /// The language id of the language name, language::none if not found.
    static language from_name(const std::string& name);

    /// The name of the specified language, empty string if not found.
    static std::string to_name(language identifier);

    /// The mnemonic delimiter for the given language.
    static std::string to_delimiter(language identifier);

    /// All languages except reference::ja are joined by an ASCII space.
    static std::string join(const string_list& words, language identifier);

    /// There is no trimming or token compression for reference::ja.
    /// All other languages are split and trimmed on ASCII whitespace.
    static string_list split(const std::string& sentence, language identifier);

    // public methods
    // ------------------------------------------------------------------------

    /// The entropy of the mnemonic (not to be confused with the seed).
    const data_chunk& entropy() const;

    /// The dictionary language of the mnemonic.
    language lingo() const;

    /// The mnemonic sentence, Japanese joined by an ideographic space.
    std::string sentence() const;

    /// The individual words of the mnemonic.
    const string_list& words() const;

    // operators
    // ------------------------------------------------------------------------

    /// True if the object is a valid mnemonic.
    operator bool() const;

    /// Lexical compares of mnemonic sentences.
    bool operator<(const languages& other) const;
    bool operator==(const languages& other) const;
    bool operator!=(const languages& other) const;

    /// Assignment.
    languages& operator=(const languages& other);

protected:
    languages();
    languages(const languages& other);
    languages(const data_chunk& entropy, const string_list& words,
        language identifier);

    // This is only used to improve the chance of wordlist matching.
    static string_list normalize(const string_list& words);

    // These should be const, apart from the need to implement assignment.
    data_chunk entropy_;
    string_list words_;
    language identifier_;
};

} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif
