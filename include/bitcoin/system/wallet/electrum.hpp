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
#ifndef LIBBITCOIN_SYSTEM_WALLET_ELECTRUM_HPP
#define LIBBITCOIN_SYSTEM_WALLET_ELECTRUM_HPP

#include <cstddef>
#include <string>
#include <bitcoin/system/compat.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/wallet/dictionary.hpp>
#include <bitcoin/system/wallet/electrum_dictionary.hpp>

#include <boost/multiprecision/cpp_int.hpp>
#include <bitcoin/system/unicode/unicode.hpp>
#include <bitcoin/system/utility/string.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

/// A wallet mnemonic, as defined by the second Electrum implementation.
class BC_API electrum
{
public:
    /// Seed prefix types (unique to Electrum).
    enum class seed_prefix : uint8_t
    {
        old,
        standard,
        witness,
        two_factor_authentication,
        two_factor_authentication_witness,
        none
    };

    /// Limits on entropy size.
    static const size_t electrum::entropy_multiple;
    static const size_t electrum::entropy_minimum;
    static const size_t electrum::entropy_maximum;

    /// Limits on word count.
    static const size_t electrum::word_multiple;
    static const size_t electrum::word_minimum;
    static const size_t electrum::word_maximum;

    /// Japanese mnemonic sentence delimiter.
    static const std::string ideographic_space;

#ifdef WITH_ICU

    /// Valid version1 entropy values (16 or 32 bytes).
    static bool is_version1_entropy_size(size_t size);

    /// Valid version1 word counts (12 or 24 words).
    static bool is_version1_word_count(size_t count);

    /// Valid version2 entropy values (17..64 bytes).
    static bool is_version2_entropy_size(size_t size);
    
    /// Valid version2 word counts (12..46 words).
    static bool is_version2_word_count(size_t count);

    /// Returns true if the menemonic is version1.
    static bool is_version1(const string_list& words,
        reference lexicon=reference::en);

    /// Returns true if the resulting seed has the given prefix.
    static bool has_prefix(const string_list& words,
        seed_prefix prefix=seed_prefix::standard);

    /// Obtain the dictionary corresponding to the reference.
    static const dictionary& to_dictionary(reference lexicon);

    /// Obtain a reference to the dictionary containing all of the words.
    static reference to_reference(const string_list& words,
        reference lexicon=reference::none);

    /// Obtain the enumerated prefix corresponding to the words.
    static seed_prefix to_prefix(const string_list& words);

    /// Obtain the prefix version corresponding to the enumeration.
    static std::string to_version(seed_prefix prefix);

    /// Create a seed from any list of words and passphrase.
    static data_chunk to_seed(const string_list& words,
        const std::string& passphrase="");

    /// Constructors.

    /// The instance should be tested for validity when using these.
    electrum(const electrum& other);
    electrum(const std::string& sentence, reference lexicon=reference::none);
    electrum(const string_list& words, reference lexicon=reference::none);
    electrum(const data_chunk& entropy,
        seed_prefix prefix=seed_prefix::standard,
        reference lexicon=reference::en);

    /////// This constructor guarantees instance validity.
    ////template <size_t Size, std::enable_if_t<Size == 16 || Size == 32, size_t>>
    ////electrum_v1(const byte_array<Size>& entropy,
    ////    seed_prefix prefix=seed_prefix::standard,
    ////    reference lexicon=reference::en)
    ////  : electrum(from_entropy(entropy, prefix, lexicon))
    ////{
    ////}

    /// Operators.
    bool operator<(const electrum& other) const;
    bool operator==(const electrum& other) const;
    bool operator!=(const electrum& other) const;
    electrum& operator=(const electrum& other);
    friend std::istream& operator>>(std::istream& in, electrum& to);
    friend std::ostream& operator<<(std::ostream& out, const electrum& of);

    /// Cast operators.
    operator bool() const;

    /// Serializers.
    std::string sentence() const;

    /// Accessors.
    const data_chunk& entropy() const;
    const string_list& words() const;
    reference lexicon() const;
    seed_prefix prefix() const;

    /// Methods.

    /// Create a seed from member entropy and any passphrase.
    data_chunk to_seed(const std::string& passphrase="") const;

protected:
    /// All langauges except reference::ja are joined by an ASCII space.
    static std::string join(const string_list& words, reference lexicon);

    /// There is no trimming or token compression for reference::ja.
    /// All other langauges are split and trimmed on ASCII whitespace.
    static string_list split(const std::string& sentence, reference lexicon);

    /// This is based on the Electrum "normalize_text" routine.
    static std::string normalize(const std::string& text);

    /// Map entropy to entropy bit count (128 or 256 bits).
    static size_t entropy_bits(const data_slice& entropy);

    /// Map words to entropy bit count (128 or 256 bits).
    static size_t entropy_bits(const string_list& words);

    /// Map entropy to entropy size (16 or 32 bytes).
    static size_t entropy_size(const data_slice& entropy);

    /// Map words to entropy size (16 or 32 bytes).
    static size_t entropy_size(const string_list& words);

    // Map entropy size to word count (12 or 24 words).
    static size_t word_count(const data_slice& entropy);

    // Map words to word count (12 or 24 words).
    static size_t word_count(const string_list& words);

private:
    static electrum from_entropy_v2(const data_slice& entropy,
        seed_prefix prefix, reference lexicon);
    static electrum from_words_v2(const string_list& words,
        reference lexicon);

    static electrum from_entropy_v1(const data_chunk& entropy,
        seed_prefix prefix, reference lexicon);
    static data_chunk from_words_v1(const string_list& words,
        reference lexicon);

    electrum();
    electrum(const data_chunk& entropy, const string_list& words,
        reference lexicon, seed_prefix prefix);

    // These should be const, apart from the need to implement assignment.
    data_chunk entropy_;
    string_list words_;
    reference lexicon_;
    seed_prefix prefix_;

#endif // WITH_ICU
};

} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif
