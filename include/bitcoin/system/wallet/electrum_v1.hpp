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
#ifndef LIBBITCOIN_SYSTEM_WALLET_ELECTRUM_V1_HPP
#define LIBBITCOIN_SYSTEM_WALLET_ELECTRUM_V1_HPP

#include <cstddef>
#include <string>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/utility/string.hpp>
#include <bitcoin/system/wallet/dictionary.hpp>
#include <bitcoin/system/wallet/dictionaries.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

/// TODO: determine if there is an Electrum v1 seed algorithm.
/// A wallet mnemonic, as defined by the first Electrum implementation.
class BC_API electrum_v1
{
public:
    typedef wallet::dictionary<1626> dictionary;
    typedef wallet::dictionaries<2, dictionary::size()> dictionaries;

    /// Publish Electrum v1 word lists.
    static const dictionary::words en;
    static const dictionary::words pt;

    /// Supports 128 or 256 bits of entropy.
    static constexpr size_t entropy_multiple = 4;
    static constexpr size_t entropy_minimum = 4u * entropy_multiple;
    static constexpr size_t entropy_maximum = 8u * entropy_multiple;

    /// Supports 12 or 24 words (128 or 256 bits) of entropy.
    static constexpr size_t word_multiple = 3;
    static constexpr size_t word_minimum = 4u * word_multiple;
    static constexpr size_t word_maximum = 8u * word_multiple;

    /// The two arrays of entropy sizes supported by Electrum v1.
    typedef byte_array<entropy_minimum> minimum_entropy;
    typedef byte_array<entropy_maximum> maximum_entropy;

    /// Valid dictionaries (en, pt).
    static bool is_valid_dictionary(language identifier);

    /// Valid entropy values (16 or 32 bytes).
    static bool is_valid_entropy_size(size_t size);

    /// Valid word counts (12 or 24 words).
    static bool is_valid_word_count(size_t count);

    /// The dictionary, limited by identifier, that contains all words.
    static language contained_by(const string_list& words, language identifier);

    /// The instance should be tested for validity when using these.
    electrum_v1(const electrum_v1& other);
    electrum_v1(const std::string& sentence, language identifier=language::none);
    electrum_v1(const string_list& words, language identifier=language::none);
    electrum_v1(const data_chunk& entropy, language identifier=language::en);

    /// These constructors guarantee instance validity.
    electrum_v1(const minimum_entropy& entropy, language identifier=language::en);
    electrum_v1(const maximum_entropy& entropy, language identifier=language::en);

    /// Lexical compares of mnemonic sentences.
    bool operator<(const electrum_v1& other) const;
    bool operator==(const electrum_v1& other) const;
    bool operator!=(const electrum_v1& other) const;
    electrum_v1& operator=(const electrum_v1& other);

    /// Deserialize/serialize a mnemonic sentence.
    friend std::istream& operator>>(std::istream& in, electrum_v1& to);
    friend std::ostream& operator<<(std::ostream& out, const electrum_v1& of);

    /// True if the object is a valid mnemonic.
    operator bool() const;

    /// The mnemonic sentence.
    /// Japanese mnemonics are joined by an ideographic space (BIP39).
    std::string sentence() const;

    /// The entropy of the mnemonic, or empty if invalid.
    const data_chunk& entropy() const;

    /// The individual words of the mnemonic, or empty if invalid.
    const string_list& words() const;

    /// The dictionary language of the mnemonic, or none if invalid.
    language lingo() const;

protected:
    /// Constructors.
    electrum_v1();
    electrum_v1(const data_chunk& entropy, const string_list& words,
        language identifier);

    /// Map entropy to entropy bit count (128 or 256 bits).
    static size_t entropy_bits(const data_slice& entropy);

    /// Map words to entropy bit count (128 or 256 bits).
    static size_t entropy_bits(const string_list& words);

    /// Map words to entropy size (16 or 32 bytes).
    static size_t entropy_size(const string_list& words);

    /// Map entropy size to word count (12 or 24 words).
    static size_t word_count(const data_slice& entropy);

    /// Compresses whitespace only.
    static std::string normalize(const std::string& text);

    /// Compresses whitespace only.
    static string_list normalize(const string_list& words);

    /// All languages except reference::ja are joined by an ASCII space.
    static std::string join(const string_list& words, language identifier);

    /// There is no trimming or token compression for reference::ja.
    /// All other languages are split and trimmed on ASCII whitespace.
    static string_list split(const std::string& sentence, language identifier);

private:
    static data_chunk decode(const string_list& words, language identifier);
    static string_list encode(const data_chunk& entropy, language identifier);
    static electrum_v1 from_entropy(const data_chunk& entropy, language identifier);
    static electrum_v1 from_words(const string_list& words, language identifier);

    // All Electrum v1 dictionaries, from <dictionaries/electrum_v1.cpp>.
    static const dictionaries dictionaries_;

    // These should be const, apart from the need to implement assignment.
    data_chunk entropy_;
    string_list words_;
    language identifier_;
};

} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif
