/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/wallet/mnemonic.hpp>

#include <algorithm>
#include <cstdint>
#include <boost/locale.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/unicode/unicode.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/binary.hpp>
#include <bitcoin/bitcoin/utility/collection.hpp>
#include <bitcoin/bitcoin/utility/string.hpp>
#include <bitcoin/bitcoin/wallet/dictionary.hpp>
#include "../math/external/pkcs5_pbkdf2.h"

namespace libbitcoin {
namespace wallet {

// BIP-39 private constants.
static constexpr size_t bits_per_word = 11;
static constexpr size_t entropy_bit_divisor = 32;
static constexpr size_t hmac_iterations = 2048;
static const char* passphrase_prefix = "mnemonic";

inline uint8_t bip39_shift(size_t bit)
{
    return (1 << (byte_bits - (bit % byte_bits) - 1));
}

bool validate_mnemonic(const word_list& words, const dictionary& lexicon)
{
    const auto word_count = words.size();
    if ((word_count % mnemonic_word_multiple) != 0)
        return false;

    const auto total_bits = bits_per_word * word_count;
    const auto check_bits = total_bits / (entropy_bit_divisor + 1);
    const auto entropy_bits = total_bits - check_bits;

    BITCOIN_ASSERT((entropy_bits % byte_bits) == 0);

    size_t bit = 0;
    data_chunk data((total_bits + byte_bits - 1) / byte_bits, 0);

    for (const auto& word: words)
    {
        const auto position = find_position(lexicon, word);
        if (position == -1)
            return false;

        for (size_t loop = 0; loop < bits_per_word; loop++, bit++)
        {
            if (position & (1 << (bits_per_word - loop - 1)))
            {
                const auto byte = bit / byte_bits;
                data[byte] |= bip39_shift(bit);
            }
        }
    }

    data.resize(entropy_bits / byte_bits);

    const auto mnemonic = create_mnemonic(data, lexicon);
    return std::equal(mnemonic.begin(), mnemonic.end(), words.begin());
}

word_list create_mnemonic(data_slice entropy, const dictionary &lexicon)
{
    if ((entropy.size() % mnemonic_seed_multiple) != 0)
        return word_list();

    const size_t entropy_bits = (entropy.size() * byte_bits);
    const size_t check_bits = (entropy_bits / entropy_bit_divisor);
    const size_t total_bits = (entropy_bits + check_bits);
    const size_t word_count = (total_bits / bits_per_word);

    BITCOIN_ASSERT((total_bits % bits_per_word) == 0);
    BITCOIN_ASSERT((word_count % mnemonic_word_multiple) == 0);

    const auto data = build_chunk({entropy, sha256_hash(entropy)});

    size_t bit = 0;
    word_list words;

    for (size_t word = 0; word < word_count; word++)
    {
        size_t position = 0;
        for (size_t loop = 0; loop < bits_per_word; loop++)
        {
            bit = (word * bits_per_word + loop);
            position <<= 1;

            const auto byte = bit / byte_bits;

            if ((data[byte] & bip39_shift(bit)) > 0)
                position++;
        }

        BITCOIN_ASSERT(position < dictionary_size);
        words.push_back(lexicon[position]);
    }

    BITCOIN_ASSERT(words.size() == ((bit + 1) / bits_per_word));
    return words;
}

bool validate_mnemonic(const word_list& mnemonic,
    const dictionary_list& lexicons)
{
    for (const auto& lexicon: lexicons)
        if (validate_mnemonic(mnemonic, *lexicon))
            return true;

    return false;
}

long_hash decode_mnemonic(const word_list& mnemonic)
{
    const auto sentence = join(mnemonic);
    const std::string salt(passphrase_prefix);
    return pkcs5_pbkdf2_hmac_sha512(to_chunk(sentence), to_chunk(salt),
        hmac_iterations);
}

#ifdef WITH_ICU

long_hash decode_mnemonic(const word_list& mnemonic,
    const std::string& passphrase)
{
    const auto sentence = join(mnemonic);
    const std::string prefix(passphrase_prefix);
    const auto salt = to_normal_nfkd_form(prefix + passphrase);
    return pkcs5_pbkdf2_hmac_sha512(to_chunk(sentence), to_chunk(salt),
        hmac_iterations);
}

#endif

} // namespace wallet
} // namespace libbitcoin
