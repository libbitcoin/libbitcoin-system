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
#include <bitcoin/bitcoin/math/external/pkcs5_pbkdf2.h>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/binary.hpp>
#include <bitcoin/bitcoin/utility/general.hpp>
#include <bitcoin/bitcoin/wallet/dictionary.hpp>

namespace libbitcoin {
namespace bip39 {

// BIP-39 private constants.
constexpr size_t bits_per_word = 11;
constexpr size_t entropy_bit_divisor = 32;
constexpr size_t hmac_iterations = 2048;
DEBUG_ONLY(constexpr size_t dictionary_length = 2048;)

// It would be nice if we could do this statically.
static void validate_dictionary()
{
    BITCOIN_ASSERT_MSG(dictionary.size() == sizeof(bip39::language),
        "The dictionary does not have the required number of languages.");

#ifndef NDEBUG
    for (const auto& dictionary: bip39::dictionary)
    {
        BITCOIN_ASSERT_MSG(dictionary.second.size() == dictionary_length,
            "A dictionary does not have the required number of elements.");
    }
#endif
}

static const string_list& get_dictionary(bip39::language language)
{
    validate_dictionary();

    const auto it = bip39::dictionary.find(language);

    // Guards against lack of uniqueness in dictionary languages.
    BITCOIN_ASSERT(it != bip39::dictionary.end());
    return it->second;
}

static bip39::language get_language(const string_list& words)
{
    validate_dictionary();

    if (words.empty())
        return bip39::language::en;

    const auto& first_word = words.front();

    for (const auto& dictionary : bip39::dictionary)
        if (find_position(dictionary.second, first_word) != -1)
            return dictionary.first;

    return bip39::language::en;
}

static std::string normalize_nfkd(const std::string& value)
{
    using namespace boost::locale;
    const generator locale;
    return normalize(value, norm_type::norm_nfkd, locale(""));
}

static uint8_t bip39_shift(size_t bit)
{
    return (1 << (byte_bits - (bit % byte_bits) - 1));
}

// extracts entropy/checksum from the mnemonic and rebuilds the word list
// for comparison; returns true if it's a match (valid), false otherwise
static bool validate_mnemonic(const string_list& words,
    const string_list& dictionary, bip39::language language)
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

data_chunk decode_mnemonic(const string_list& mnemonic,
    const std::string& passphrase)
{
    const auto language = get_language(mnemonic);
    const auto& dictionary = get_dictionary(language);

    if (!validate_mnemonic(mnemonic, dictionary, language))
        return data_chunk();

    const auto sentence = join(mnemonic);
    const auto salt = normalize_nfkd("mnemonic" + passphrase);
    const auto salt_chunk = to_data_chunk(salt);

    long_hash hash;
    if (pkcs5_pbkdf2_hmac_sha512(sentence, salt_chunk, hmac_iterations, hash))
        return to_data_chunk(hash);

    return data_chunk();
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

        BITCOIN_ASSERT(position < dictionary_length);
        words.push_back(dictionary[position]);
    }

    BITCOIN_ASSERT(words.size() == (bit / bits_per_word));
    return words;
}

} // namespace bip39
} // namespace libbitcoin

