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
#ifndef LIBBITCOIN_SYSTEM_WALLET_MNEMONIC_HPP
#define LIBBITCOIN_SYSTEM_WALLET_MNEMONIC_HPP

#include <cstddef>
#include <string>
#include <type_traits>
#include <bitcoin/system/compat.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/utility/string.hpp>
#include <bitcoin/system/wallet/dictionary.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

/// A wallet mnemonic, as defined by BIP39.
class BC_API mnemonic
{
public:
    /// Limits on entropy size.
    static const size_t entropy_multiple;
    static const size_t entropy_minumum;
    static const size_t entropy_maximum;

    /// Limits on word count.
    static const size_t word_multiple;
    static const size_t word_minimum;
    static const size_t word_maximum;

    /// Japanese mnemonic sentence delimiter.
    static const std::string ideographic_space;

    /// Valid entropy values (16, 20, 24, 28, or 32 bytes).
    static bool is_valid_entropy_size(size_t size);

    /// Valid word counts (12, 15, 18, 21, or 24 words).
    static bool is_valid_word_count(size_t count);

    /// Obtain the dictionary corresponding to the reference.
    static const dictionary& to_dictionary(reference lexicon);

    /// Obtain a reference to the dictionary containing all of the words.
    static reference to_reference(const string_list& words);

#ifdef WITH_ICU
    /// Seed creation requires a Unicode build (for text normalization).
    static data_chunk to_seed(const string_list& words,
        const std::string& passphrase="");
#endif

    /// Constructors.

    /// The instance should be tested for validity when using these.
    mnemonic(const mnemonic& other);
    mnemonic(const std::string& sentence, reference lexicon=reference::none);
    mnemonic(const string_list& words, reference lexicon=reference::none);
    mnemonic(const data_chunk& entropy, reference lexicon=reference::en);

    /// This constructor guarantees instance validity.
    template <size_t Size, std::enable_if_t<Size % entropy_multiple == 0u &&
        Size >= entropy_minumum && Size <= entropy_maximum, size_t>>
    mnemonic(const byte_array<Size>& entropy, reference lexicon=reference::en)
      : mnemonic(from_entropy(entropy, lexicon))
    {
    }

    /// Operators.
    bool operator<(const mnemonic& other) const;
    bool operator==(const mnemonic& other) const;
    bool operator!=(const mnemonic& other) const;
    mnemonic& operator=(const mnemonic& other);
    friend std::istream& operator>>(std::istream& in, mnemonic& to);
    friend std::ostream& operator<<(std::ostream& out, const mnemonic& of);

    /// Cast operators.
    operator bool() const;

    /// Serializers.
    std::string sentence() const;

    /// Accessors.
    const data_chunk& entropy() const;
    const string_list& words() const;
    reference lexicon() const;

#ifdef WITH_ICU
    /// Seed creation requires a Unicode build (for text normalization).
    data_chunk to_seed(const std::string& passphrase="") const;
#endif

protected:
    /// All langauges except reference::ja are joined by an ASCII space.
    static std::string join(const string_list& words, reference lexicon);

    /// There is no trimming or token compression for reference::ja.
    /// All other langauges are split and trimmed on ASCII whitespace.
    static string_list split(const std::string& sentence, reference lexicon);

#ifdef WITH_ICU
    /// This only applies nfkd form.
    static std::string normalize_text(const std::string& text);
#endif

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

    /// Map entropy to entropy size (16, 20, 24, 28, or 32 bytes).
    static size_t entropy_size(const data_slice& entropy);

    /// Map words to entropy size (16, 20, 24, 28, or 32 bytes).
    static size_t entropy_size(const string_list& words);

    // Map entropy size to word count (12, 15, 18, 21, or 24 words).
    static size_t word_count(const data_slice& entropy);

    // Map words to word count (12, 15, 18, 21, or 24 words).
    static size_t word_count(const string_list& words);

private:
    static mnemonic from_entropy(const data_chunk& entropy,
        reference lexicon);
    static mnemonic from_words(const string_list& words,
        reference lexicon);

    mnemonic();
    mnemonic(const data_chunk& entropy, const string_list& words,
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
