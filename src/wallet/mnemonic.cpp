/*
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/wallet/mnemonic.hpp>

#include <algorithm>
#include <cstdint>
#include <boost/locale.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/binary.hpp>
#include <bitcoin/bitcoin/utility/collection.hpp>
#include <bitcoin/bitcoin/utility/string.hpp>
#include <bitcoin/bitcoin/wallet/dictionary.hpp>
#include "../math/external/pkcs5_pbkdf2.h"

namespace libbitcoin {
namespace bip39 {

// BIP-39 private constants.
constexpr size_t bits_per_word = 11;
constexpr size_t entropy_bit_divisor = 32;
constexpr size_t hmac_iterations = 2048;

// It would be nice if we could do this statically.
static void validate_dictionary()
{
    DEBUG_ONLY(constexpr size_t languages = (size_t)language::unknown);
    BITCOIN_ASSERT_MSG(dictionary.size() == languages,
        "The dictionary does not have the required number of languages.");
}

// Get the dictionary of the language or en if unknown.
static const wordlist& get_dictionary(bip39::language language)
{
    validate_dictionary();

    const auto tongue = if_else(language == bip39::language::unknown,
        bip39::language::en, language);

    const auto it = bip39::dictionary.find(tongue);

    // Guard against lack of uniqueness in dictionary languages by enum.
    BITCOIN_ASSERT(it != bip39::dictionary.end());

    return *(it->second);
}

// Force explicit language specification for ambiguous dictionaries.
static bip39::language resolve_ambiguity(bip39::language language)
{
    return if_else(language == bip39::language::zh_Hans ||
        language == bip39::language::zh_Hant, bip39::language::unknown,
        language);
}

// Detect the language of the wordlist, or return unknown if it's ambiguous.
static bip39::language get_language(const string_list& words)
{
    validate_dictionary();

    if (words.empty())
        return bip39::language::unknown;

    const auto& first_word = words.front();
    for (const auto& dictionary: bip39::dictionary)
        if (find_position(*dictionary.second, first_word) != -1)
            return resolve_ambiguity(dictionary.first);

    return bip39::language::unknown;
}

static uint8_t bip39_shift(size_t bit)
{
    return (1 << (byte_bits - (bit % byte_bits) - 1));
}

static std::string normalize_nfkd(const std::string& value)
{
    using namespace boost::locale;
    const generator locale;
    return normalize(value, norm_type::norm_nfkd, locale("UTF-8"));
}

// extracts entropy/checksum from the mnemonic and rebuilds the word list
// for comparison; returns true if it's a match (valid), false otherwise
static bool validate_mnemonic(const string_list& words,
    const wordlist& dictionary, bip39::language language)
{
    const auto word_count = words.size();
    if ((word_count < min_word_count) || (word_count > max_word_count))
        return false;

    size_t bit = 0;
    data_chunk seed(max_word_count, 0);

    for (const auto& word: words)
    {
        const auto position = find_position(dictionary, word);
        if (position == -1)
            return false;

        for (size_t loop = 0; loop < bits_per_word; loop++, bit++)
        {
            if (position & (1 << (bits_per_word - loop - 1)))
            {
                const auto byte = bit / byte_bits;
                seed[byte] |= bip39_shift(bit);
            }
        }
    }

    const auto size = bit / byte_bits;
    seed.resize(size);

    const auto mnemonic = create_mnemonic(seed, language);
    return std::equal(mnemonic.begin(), mnemonic.end(), words.begin());
}

// The word selection may be ambiguous (zh), which requires specified language.
data_chunk decode_mnemonic(const string_list& mnemonic,
    const std::string& passphrase, bip39::language language)
{
    auto tongue = language;
    if (tongue == bip39::language::unknown)
        tongue = get_language(mnemonic);

    if (tongue == bip39::language::unknown)
        return data_chunk();

    const auto& dictionary = get_dictionary(tongue);

    if (!validate_mnemonic(mnemonic, dictionary, tongue))
        return data_chunk();

    const auto sentence = join(mnemonic);
    const auto salt = normalize_nfkd("mnemonic" + passphrase);

    return to_data_chunk(pkcs5_pbkdf2_hmac_sha512(
        to_data_chunk(sentence), to_data_chunk(salt), hmac_iterations));
}

string_list create_mnemonic(data_slice entropy, bip39::language language)
{
    if ((entropy.size() % seed_multiple) != 0)
        return string_list();

    const auto& dictionary = get_dictionary(language);
    const auto hash = sha256_hash(entropy);
    auto chunk = to_data_chunk(entropy);
    extend_data(chunk, hash);

    const size_t entropy_bits = (entropy.size() * byte_bits);
    const size_t check_bits = (entropy_bits / entropy_bit_divisor);
    const size_t total_bits = (entropy_bits + check_bits);
    const size_t word_count = (total_bits / bits_per_word);

    BITCOIN_ASSERT((total_bits % bits_per_word) == 0);
    BITCOIN_ASSERT((word_count % word_multiple) == 0);

    size_t bit = 0;
    string_list words;

    for (size_t word = 0; word < word_count; word++)
    {
        size_t position = 0;
        for (size_t loop = 0; loop < bits_per_word; loop++)
        {
            bit = (word * bits_per_word + loop);
            position <<= 1;

            const auto byte = bit / byte_bits;

            if ((chunk[byte] & bip39_shift(bit)) > 0)
                position++;
        }

        BITCOIN_ASSERT(position < wordlist_size);
        words.push_back(dictionary[position]);
    }

    BITCOIN_ASSERT(words.size() == ((bit + 1) / bits_per_word));
    return words;
}

} // namespace bip39
} // namespace libbitcoin

