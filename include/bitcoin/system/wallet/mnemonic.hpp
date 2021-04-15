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
#ifndef LIBBITCOIN_SYSTEM_WALLET_MNEMONIC_HPP
#define LIBBITCOIN_SYSTEM_WALLET_MNEMONIC_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/utility/string.hpp>
#include <bitcoin/system/wallet/dictionary.hpp>
#include <bitcoin/system/wallet/dictionaries.hpp>
#include <bitcoin/system/wallet/hd_private.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

/// A wallet mnemonic, as defined by BIP39.
class BC_API mnemonic
{
public:
    typedef wallet::dictionary<2048> dictionary;
    typedef wallet::dictionaries<10, dictionary::size()> dictionaries;

    /// Publish BIP39 word lists.
    static const dictionary::words en;
    static const dictionary::words es;
    static const dictionary::words it;
    static const dictionary::words fr;
    static const dictionary::words cs;
    static const dictionary::words pt;
    static const dictionary::words ja;
    static const dictionary::words ko;
    static const dictionary::words zh_Hans;
    static const dictionary::words zh_Hant;

    /// Supports 128 to 256 bits of entropy, in multiples of 32.
    static constexpr size_t entropy_multiple = 4;
    static constexpr size_t entropy_minimum = 4u * entropy_multiple;
    static constexpr size_t entropy_maximum = 8u * entropy_multiple;

    /// Supports 12 to 24 words (128 to 256 bits), in multiples of 3.
    static constexpr size_t word_multiple = 3;
    static constexpr size_t word_minimum = 4u * word_multiple;
    static constexpr size_t word_maximum = 8u * word_multiple;

    /// Valid entropy values (16, 20, 24, 28, or 32 bytes).
    static bool is_valid_entropy_size(size_t size);

    /// Valid word counts (12, 15, 18, 21, or 24 words).
    static bool is_valid_word_count(size_t count);

    /// Valid dictionaries (en, es, it, fr, cs, pt, ja, ko, zh_Hans, zh_Hant).
    static bool is_valid_dictionary(language identifier);

    /// Create a seed from a valid number of *any* words and passphrase.
    /// If invalid or WITH_ICU not defined this returns a zeroized hash.
    static hd_private to_seed(const string_list& words,
        const std::string& passphrase="", uint64_t chain=hd_private::mainnet);

    /// The instance should be tested for validity after construction.
    mnemonic(const mnemonic& other);
    mnemonic(const std::string& sentence, language identifier=language::none);
    mnemonic(const string_list& words, language identifier=language::none);
    mnemonic(const data_chunk& entropy, language identifier=language::en);

    /// Lexical compares of mnemonic sentences.
    bool operator<(const mnemonic& other) const;
    bool operator==(const mnemonic& other) const;
    bool operator!=(const mnemonic& other) const;
    mnemonic& operator=(const mnemonic& other);

    /// Deserialize/serialize a mnemonic sentence.
    friend std::istream& operator>>(std::istream& in, mnemonic& to);
    friend std::ostream& operator<<(std::ostream& out, const mnemonic& of);

    /// True if the object is a valid mnemonic.
    operator bool() const;

    /// The mnemonic sentence.
    /// Japanese mnemonics are joined by an ideographic space (BIP39).
    std::string sentence() const;

    /// The entropy of the mnemonic (not to be confused with the seed).
    const data_chunk& entropy() const;

    /// The individual words of the mnemonic.
    const string_list& words() const;

    /// The dictionary language of the mnemonic.
    language lingo() const;

    /// The seed derived from mnemonic entropy and an optional passphrase.
    /// If invalid or WITH_ICU not defined this returns a zeroized hash.
    hd_private to_seed(const std::string& passphrase="",
        uint64_t chain=hd_private::mainnet) const;

protected:
    // Constructors.
    mnemonic();
    mnemonic(const data_chunk& entropy, const string_list& words,
        language identifier);

    /// Derive the checksum byte from entropy, stored in high order bits.
    static uint8_t checksum_byte(const data_slice& entropy);

    /// Map entropy to checksum bit count (4, 5, 6, 7, or 8 bits).
    static size_t checksum_bits(const data_slice& entropy);

    /// Map words to checksum bit count (4, 5, 6, 7, or 8 bits).
    static size_t checksum_bits(const string_list& words);

    /// Map entropy to entropy bit count (128, 160, 192, 224, or 256 bits).
    static size_t entropy_bits(const data_slice& entropy);

    /// Map words to entropy bit count (128, 160, 192, 224, or 256 bits).
    static size_t entropy_bits(const string_list& words);

    /// Map words to entropy size (16, 20, 24, 28, or 32 bytes).
    static size_t entropy_size(const string_list& words);

    /// Map entropy size to word count (12, 15, 18, 21, or 24 words).
    static size_t word_count(const data_slice& entropy);

    /// Applies the nfkd form only.
    static std::string normalize(const std::string& text);

    /// Applies the nfkd form only.
    static string_list normalize(const string_list& words);

    /// All languages except reference::ja are joined by an ASCII space.
    static std::string join(const string_list& words, language identifier);

    /// There is no trimming or token compression for reference::ja.
    /// All other languages are split and trimmed on ASCII whitespace.
    static string_list split(const std::string& sentence, language identifier);

private:
    static string_list encode(const data_chunk& entropy, language identifier);
    static data_chunk decode(const string_list& words, language identifier);
    static mnemonic from_entropy(const data_chunk& entropy, language identifier);
    static mnemonic from_words(const string_list& words, language identifier);

    // All Electrum v1 dictionaries, from <dictionaries/mnemonic.cpp>.
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
