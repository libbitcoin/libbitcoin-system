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
#include <bitcoin/system/wallet/mnemonic.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <boost/program_options.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/unicode/unicode.hpp>
#include <bitcoin/system/utility/assert.hpp>
#include <bitcoin/system/utility/binary.hpp>
#include <bitcoin/system/utility/collection.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/utility/string.hpp>
#include <bitcoin/system/wallet/dictionary.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

// BIP-39 private constants.
static constexpr size_t bits_per_word = 11;
static constexpr size_t entropy_bit_divisor = 32;
static constexpr size_t hmac_iterations = 2048;
static const auto passphrase_prefix = "mnemonic";

// BIP-39 public limits.
const size_t mnemonic::word_multiple = 3;
const size_t mnemonic::word_minimum = 12;
const size_t mnemonic::word_maximum = 24;
const size_t mnemonic::entropy_multiple = 4;
const size_t mnemonic::entropy_minumum = 16;
const size_t mnemonic::entropy_maximum = 32;

inline uint8_t bip39_shift(size_t bit)
{
    return static_cast<uint8_t>(1u << (byte_bits - (bit % byte_bits) - 1u));
}

// Valid entropy sizes: 16, 20, 24, 28, 32.
static bool is_valid_entropy_size(size_t size)
{
    return ((size % mnemonic::entropy_multiple) == 0u &&
        size >= mnemonic::entropy_minumum &&
        size <= mnemonic::entropy_maximum);
}

// Valid word counts: 12, 15, 18, 21, 24.
static bool is_valid_word_count(size_t count)
{
    return ((count % mnemonic::word_multiple) == 0u &&
        count >= mnemonic::word_minimum &&
        count <= mnemonic::word_maximum);
}

const dictionary& mnemonic::to_dictionary(reference lexicon)
{
    switch (lexicon)
    {
        case reference::es:
            return wallet::language::es;
        case reference::it:
            return wallet::language::it;
        case reference::fr:
            return wallet::language::fr;
        case reference::cs:
            return wallet::language::cs;
        case reference::pt:
            return wallet::language::pt;
        case reference::ja:
            return wallet::language::ja;
        case reference::ko:
            return wallet::language::ko;
        case reference::zh_Hans:
            return wallet::language::zh_Hans;
        case reference::zh_Hant:
            return wallet::language::zh_Hant;

        // English is the default for 'none'.
        case reference::en:
        case reference::none:
        default:
            return wallet::language::en;
    }
}

reference mnemonic::to_reference(const string_list& words)
{
    for (const dictionary& dictionary: language::all)
    {
        const auto in_dictionary = [&](const std::string& word)
        {
            return contains(dictionary.words, word);
        };

        if (std::all_of(words.begin(), words.end(), in_dictionary))
            return dictionary.name;
    }

    return reference::none;
}

#ifdef WITH_ICU

// DOES normalize the word list.
// DOES NOT ensure the word list is in any dictionary.
long_hash mnemonic::to_seed(const string_list& words,
    const std::string& passphrase)
{
    const auto sentence = to_normal_nfkd_form(join(words));
    const auto salt = to_normal_nfkd_form(passphrase_prefix + passphrase);
    return pkcs5_pbkdf2_hmac_sha512(to_chunk(sentence), to_chunk(salt),
        hmac_iterations);
}

#endif

// mnemonic
// ----------------------------------------------------------------------------

mnemonic::mnemonic()
  : mnemonic(data_chunk{})
{
}

mnemonic::mnemonic(const mnemonic& other)
  : entropy_(other.entropy_), words_(other.words_), lexicon_(other.lexicon_)
{
}

mnemonic::mnemonic(const std::string& sentence, reference lexicon)
  : mnemonic(split(sentence), lexicon)
{
}

mnemonic::mnemonic(const data_slice& entropy, reference lexicon)
  : mnemonic(from_entropy(entropy, lexicon))
{
}

mnemonic::mnemonic(const string_list& words, reference lexicon)
  : mnemonic(from_words(words, lexicon))
{
}

// protected
mnemonic::mnemonic(const data_chunk& entropy, const string_list& words,
    reference lexicon)
  : entropy_(entropy), words_(words), lexicon_(lexicon)
{
}

// Factories.
// ----------------------------------------------------------------------------

// DOES NOT normalize the mapped dictionary words.
mnemonic mnemonic::from_entropy(const data_slice& entropy, reference lexicon)
{
    if (!is_valid_entropy_size(entropy.size()))
        return {};

    const auto entropy_bits = (entropy.size() * byte_bits);
    const auto check_bits = (entropy_bits / entropy_bit_divisor);
    const auto total_bits = (entropy_bits + check_bits);
    const auto word_count = (total_bits / bits_per_word);
    BITCOIN_ASSERT((total_bits % bits_per_word) == 0u);
    BITCOIN_ASSERT((word_count % mnemonic_word_multiple) == 0u);

    size_t bit = 0;
    string_list words;
    const auto& dictionary = to_dictionary(lexicon);
    const auto data = build_chunk({ entropy, sha256_hash(entropy) });

    for (size_t word = 0; word < word_count; word++)
    {
        size_t position = 0;
        for (size_t index = 0; index < bits_per_word; index++)
        {
            bit = (word * bits_per_word + index);
            position <<= 1;

            if ((data[bit / byte_bits] & bip39_shift(bit)) > 0u)
                position++;
        }

        BITCOIN_ASSERT(position < dictionary_size);
        words.push_back(dictionary.words[position]);
    }

    BITCOIN_ASSERT(words.size() == ((bit + 1u) / bits_per_word));
    return { {entropy.begin(), entropy.end() }, words, lexicon
};
}

// DOES NOT normalize the specified words.
// DOES NOT normalize the compared dictionary words.
// DOES NOT support un-validatable word lists (use to_seed).
mnemonic mnemonic::from_words(const string_list& words, reference lexicon)
{
    if (!is_valid_word_count(words.size()))
        return {};

    // Locate the one dictionary of all given words.
    const auto actual_lexicon = to_reference(words);

    // Words are not from a supported dictionary or do not match specification.
    if (actual_lexicon == reference::none ||
        (actual_lexicon != lexicon && lexicon != reference::none))
        return {};

    // Words are from a supported dictionary.
    const auto& dictionary = to_dictionary(actual_lexicon);

    const auto total_bits = bits_per_word * words.size();
    const auto check_bits = total_bits / (entropy_bit_divisor + 1u);
    const auto entropy_bits = total_bits - check_bits;
    BITCOIN_ASSERT((entropy_bits % byte_bits) == 0u);

    size_t bit = 0;
    data_chunk entropy((total_bits + byte_bits - 1u) / byte_bits, 0);

    for (const auto& word: words)
    {
        // Position must be valid since the language is validated.
        const auto position = find_position(dictionary.words, word);
        BITCOIN_ASSERT(position >= 0 && position < dictionary_size);

        for (size_t index = 0; index < bits_per_word; index++, bit++)
            if ((position & (1u << (bits_per_word - index - 1u))) != 0)
                entropy[bit / byte_bits] |= bip39_shift(bit);
    }

    entropy.resize(entropy_bits / byte_bits);
    return { entropy, words, actual_lexicon };
}

// Cast operators.
// ----------------------------------------------------------------------------

mnemonic::operator bool() const
{
    return is_valid_entropy_size(entropy_.size());
}

mnemonic::operator reference() const
{
    return lexicon_;
}

mnemonic::operator const data_chunk&() const
{
    return entropy_;
}

// Serializer.
// ----------------------------------------------------------------------------

std::string mnemonic::sentence() const
{
    // A valid Japanese word list is joined by ideographic spaces, others ASCII.
    return join(words(), (lexicon_ == reference::ja) ? "\0xE3\0x80\0x80" :
        "\0x20");
}

// Accessors.
// ----------------------------------------------------------------------------

const data_chunk& mnemonic::entropy() const
{
    return entropy_;
}

const string_list& mnemonic::words() const
{
    return words_;
}

reference mnemonic::lexicon() const
{
    return lexicon_;
}

// Methods.
// ----------------------------------------------------------------------------

#ifdef WITH_ICU

long_hash mnemonic::to_seed(const std::string& passphrase) const
{
    // Words are generated from seed, so always from a supported dictionary.
    return to_seed(words(), passphrase);
}

#endif

// Operators.
// ----------------------------------------------------------------------------

mnemonic& mnemonic::operator=(const mnemonic& other)
{
    entropy_ = other.entropy_;
    words_ = other.words_;
    lexicon_ = other.lexicon_;
    return *this;
}

bool mnemonic::operator<(const mnemonic& other) const
{
    return sentence() < other.sentence();
}

bool mnemonic::operator==(const mnemonic& other) const
{
    // Words and entropy are equivalent (one is a cache of the other).
    return entropy_ == other.entropy_ && lexicon_ == other.lexicon_;
}

bool mnemonic::operator!=(const mnemonic& other) const
{
    return !(*this == other);
}

std::istream& operator>>(std::istream& in, mnemonic& to)
{
    std::string value;
    in >> value;
    to = mnemonic(value);

    if (!to)
    {
        using namespace boost::program_options;
        BOOST_THROW_EXCEPTION(invalid_option_value(value));
    }

    return in;
}

std::ostream& operator<<(std::ostream& out, const mnemonic& of)
{
    out << of.sentence();
    return out;
}

} // namespace wallet
} // namespace system
} // namespace libbitcoin
