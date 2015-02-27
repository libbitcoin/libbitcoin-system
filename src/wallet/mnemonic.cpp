/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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

#include <boost/random.hpp>
#include <boost/locale.hpp>

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/formats/base16.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/binary.hpp>
#include <bitcoin/bitcoin/utility/general.hpp>
#include <bitcoin/bitcoin/math/external/pkcs5_pbkdf2.h>

namespace libbitcoin {

static dictionary master_dictionary;

// populate master dictionary with common word lists of requested
// supported languages
static void initialize_master_dictionary(bip39_language language)
{
    if (master_dictionary.find(language) == master_dictionary.end()) {
        switch(language) {
            case bip39_language::ja:
                master_dictionary[language] = common_words_JAPANESE;
                break;
            case bip39_language::es:
                master_dictionary[language]  = common_words_SPANISH;
                break;
            case bip39_language::zh:
                master_dictionary[language]  = common_words_CHINESE;
                break;
            case bip39_language::en:
            default:
                master_dictionary[language]  = common_words_ENGLISH;
                break;
        }
    }
    BITCOIN_ASSERT(not master_dictionary.empty());
}

static bip39_language detect_language_of_words(const string_list& words)
{
    bip39_language languages[] = {
        bip39_language::en, bip39_language::ja,
        bip39_language::zh, bip39_language::es
    };

    std::string first_word = words[0];
    for(auto language : languages) {
        initialize_master_dictionary(language);
        string_list& word_list = master_dictionary[language];
        BITCOIN_ASSERT(word_list.size() == 2048);
        if (std::find(word_list.begin(), word_list.end(), first_word) !=
            word_list.end()) {
            return language;
        }
    }
    BITCOIN_ASSERT(not master_dictionary.empty());
    return bip39_language::en;
}

static string_list& get_common_words(bip39_language language)
{
    if (master_dictionary.find(language) == master_dictionary.end()) {
        initialize_master_dictionary(language);
    }
    BITCOIN_ASSERT(master_dictionary.find(language) !=
                   master_dictionary.end());
    return master_dictionary[language];
}

static std::string normalize_string(std::string str)
{
    boost::locale::generator gen;
    std::locale::global(gen(""));

    return boost::locale::normalize(
        str, boost::locale::norm_type::norm_nfkd);
}


static void pkcs5_pbkdf2_hmac_sha512(
    std::string passphrase, std::string salt,
    int iterations, data_chunk& seed, unsigned int output_len)
{
    unsigned int i;
    unsigned char digest[max_digest_buf_len] = {0};

    BITCOIN_ASSERT(output_len < max_digest_buf_len);

    const char *pptr = (const char *)passphrase.c_str();
    const unsigned char *sptr = (const unsigned char *)salt.c_str();

    pkcs5_pbkdf2(pptr, passphrase.length(), sptr, salt.length(),
                 (unsigned char *)digest, output_len, iterations);

    for (i = 0; i < output_len; i++) {
        seed.push_back(digest[i]);
    }
}

static inline uint16_t index_of(const std::string& word, string_list& common_words)
{
    BITCOIN_ASSERT(common_words != nullptr);
    return std::find(common_words.begin(), common_words.end(), word)
        - common_words.begin();
}

// extracts entropy/cksum from the mnemonic and rebuilds the word list
// for comparison; returns true if it's a match (valid), false otherwise
static bool validate_mnemonic(
    const string_list& words, string_list& common_words,
    bip39_language language)
{
    data_chunk seed;
    uint8_t data[max_word_count] = {0};
    unsigned int i, j, word_index, error_count = 0;
    unsigned int bit_index = 0, word_count = words.size();

    BITCOIN_ASSERT(common_words.size() == 2048);
    BITCOIN_ASSERT((word_count >= 12) && (word_count < max_word_count));

    for(i = 0; i < word_count; i++) {
        word_index = index_of(words[i], common_words);
        if (word_index > 2047) {
            break;
        }
        if (common_words[word_index] != words[i]) {
            break;
        }
        for (j = 0; j < 11; j++) {
            if (word_index & (1 << (10 - j))) {
                data[bit_index / 8] |= 1 << (7 - (bit_index % 8));
            }
            bit_index++;
        }
    }
    BITCOIN_ASSERT(bit_index == (word_count * 11));

    for(i = 0; i < ((word_count * 11) / 8); i++) {
        seed.push_back(data[i]);
    }

    string_list test_mnemonic = encode_mnemonic(seed, language);
    if (words.size() == test_mnemonic.size()) {
        unsigned int i;
        for(i = 0; i < words.size(); i++) {
            error_count |= (words[i] != test_mnemonic[i]);
        }
    }
    return (error_count == 0);
}

string_list encode_mnemonic(data_slice seed, bip39_language language)
{
    string_list result;
    unsigned int i, j;
    uint16_t index = 0, bit_index;

    BITCOIN_ASSERT((seed.size() % 4) == 0);

    data_chunk bip39data = to_data_chunk(seed);
    hash_digest hash = sha256_hash(bip39data);
    for(i = 0; i < hash.size(); i++) {
        bip39data.push_back(hash[i]);
    }

    string_list common_words = get_common_words(language);
    BITCOIN_ASSERT(common_words.size() == 2048);

    unsigned int num_seed_bits = (seed.size() * 8);
    unsigned int cksum_bits = (bip39data.size() / 4);
    BITCOIN_ASSERT(((num_seed_bits + cksum_bits) % 11) == 0);

    unsigned int word_count = ((num_seed_bits + cksum_bits) / 11);
    BITCOIN_ASSERT((word_count % 3) == 0);

    for(i = 0; i < word_count; i++) {
        for (j = 0; j < 11; j++) {
            bit_index = (i * 11 + j);
            index <<= 1;
            index += (bip39data[bit_index / 8] &
                      (1 << (7 - (bit_index % 8)))) > 0;
        }
        if (index > 2047) {
            break;
        }
        result.push_back(common_words[index]);
        index = 0;
    }
    BITCOIN_ASSERT(result.size() == (num_bits / 11));

    return result;
}

data_chunk decode_mnemonic(
    const string_list& words, const std::string& passphrase)
{
    data_chunk seed;
    const int output_len = 64;
    const int num_iterations = 2048;

    std::string salt = normalize_string("mnemonic" + passphrase);

    bip39_language language = detect_language_of_words(words);
    string_list common_words = get_common_words(language);
    BITCOIN_ASSERT(common_words.size() == 2048);

    if (!validate_mnemonic(words, common_words, language)) {
        return seed;
    }

    std::string mnemonic_passphrase = join(words, " ");

    pkcs5_pbkdf2_hmac_sha512(
        mnemonic_passphrase, salt, num_iterations, seed, output_len);

    return seed;
}

} // namespace libbitcoin

