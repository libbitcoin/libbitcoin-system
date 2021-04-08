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
        empty = 0,
        standard = 1,
        witness = 100,
        two_factor_authentication = 101,
        two_factor_authentication_witness = 102
    };

    /// Limits on entropy size.
    static const size_t electrum::entropy_multiple;
    static const size_t electrum::entropy_size_small;
    static const size_t electrum::entropy_size_large;

    /// Limits on word count.
    static const size_t electrum::word_multiple;
    static const size_t electrum::word_count_small;
    static const size_t electrum::word_count_large;

    /// Japanese mnemonic sentence delimiter.
    static const std::string ideographic_space;

    /// Valid entropy values (16 or 32 bytes).
    static bool is_valid_entropy_size(size_t size);

    /// Valid word counts (12 or 24 words).
    static bool is_valid_word_count(size_t count);

    /// Returns true if the menemonic is version1/version2.
    static bool is_version1(const string_list& words,
        reference lexicon=reference::en);
    static bool is_version2(const string_list& words,
        seed_prefix prefix=seed_prefix::standard,
        reference lexicon=reference::none);

    /// Obtain the dictionary corresponding to the reference.
    static const dictionary& to_dictionary(reference lexicon);

    /// Obtain a reference to the dictionary containing all of the words.
    static reference to_reference(const string_list& words);

    /// Obtain the prefix corresponding to the enumeration.
    static std::string to_prefix(seed_prefix prefix);

#ifdef WITH_ICU
    /// Seed creation requires a Unicode build (for text normalization).
    static data_chunk to_seed(const string_list& words,
        const std::string& passphrase="");
#endif

    /// Constructors.

    /// The instance should be tested for validity when using these.
    electrum(const electrum& other);
    electrum(const std::string& sentence, reference lexicon=reference::none);
    electrum(const string_list& words, reference lexicon=reference::none);
    electrum(const data_chunk& entropy,
        seed_prefix prefix=seed_prefix::standard,
        reference lexicon=reference::en);

    /// This constructor guarantees instance validity.
    template <size_t Size, std::enable_if_t<Size == 16 || Size == 32, size_t>>
    electrum(const byte_array<Size>& entropy,
        seed_prefix prefix=seed_prefix::standard,
        reference lexicon=reference::en)
      : electrum(from_entropy(entropy, prefix, lexicon))
    {
    }

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

#ifdef WITH_ICU
    /// Methods.
    data_chunk to_seed(const std::string& passphrase="") const;
#endif

protected:
    /// All langauges except reference::ja are joined by an ASCII space.
    static std::string join(const string_list& words, reference lexicon);

    /// There is no trimming or token compression for reference::ja.
    /// All other langauges are split and trimmed on ASCII whitespace.
    static string_list split(const std::string& sentence, reference lexicon);

#ifdef WITH_ICU
    /// This is based on the Electrum "normalize_text" routine.
    static std::string electrum::normalize_text(const std::string& text);
#endif

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
    static electrum from_entropy(const data_slice& entropy,
        seed_prefix prefix, reference lexicon);
    static electrum from_words_v2(const string_list& words,
        reference lexicon);
    static data_chunk from_words_v1(const string_list& words,
        reference lexicon);

    electrum();
    electrum(const data_chunk& entropy, const string_list& words,
        reference lexicon);

    // These should be const, apart from the need to implement assignment.
    data_chunk entropy_;
    string_list words_;
    reference lexicon_;
};

} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif
