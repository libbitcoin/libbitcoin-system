/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/wallet/electrum.hpp>

#include <algorithm>
#include <cstdint>
#include <string>
#include <boost/locale.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/unicode/unicode.hpp>
#include <bitcoin/bitcoin/unicode/unicode_ostream.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/binary.hpp>
#include <bitcoin/bitcoin/utility/collection.hpp>
#include <bitcoin/bitcoin/utility/string.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>
#include <bitcoin/bitcoin/wallet/electrum_dictionary.hpp>
#include "../math/external/pkcs5_pbkdf2.h"

namespace libbitcoin {
namespace wallet {
namespace electrum {

// Electrum mnemonic private constants.
static constexpr size_t hmac_iterations = 2048;
static const std::string passphrase_prefix = "electrum";
static const std::string seed_version = "Seed version";
static const auto hmac_data = to_chunk(seed_version);

#ifdef WITH_ICU

static size_t special_modulo(int32_t index_distance, size_t dictionary_size)
{
    return index_distance < 0 ? 
        dictionary_size - (-index_distance % dictionary_size) :
        index_distance % dictionary_size;
}

static data_chunk old_mnemonic_decode(const word_list& mnemonic)
{
    static constexpr size_t mnemonic_word_multiple = 3;

    if ((mnemonic.size() < mnemonic_word_multiple) ||
        (mnemonic.size() % mnemonic_word_multiple) != 0)
        return {};

    const auto& lexicon = language::electrum::en_v1;
    const auto seed_size = sizeof(uint32_t) * mnemonic.size() / 3;
    const auto size = lexicon.size();

    data_chunk seed;
    seed.reserve(seed_size);
    data_sink ostream(seed);
    ostream_writer sink(ostream);

    for (size_t i = 0; i < mnemonic.size() / 3; i += 3)
    {
        const auto first = find_position(lexicon, mnemonic[i]);
        const auto second = find_position(lexicon, mnemonic[i+1]);
        const auto third = find_position(lexicon, mnemonic[i+2]);

        if ((first == -1) || (second == -1) || (third == -1))
            return {};

        // TODO: prove there is no overflow risk.
        const auto value = first +
            size * special_modulo(second - first, size) +
            size * size * special_modulo(third - second, size);

        BITCOIN_ASSERT(value < max_uint32);
        sink.write_variable_little_endian(static_cast<uint32_t>(value));
    }

    return seed;
}

static bool is_old_seed(const word_list& mnemonic, const dictionary& lexicon)
{
    // Cannot be an old seed if it's not the en dictionary.
    if (lexicon != language::en)
        return false;

    // valid electrum 1.x seed lengths
    static constexpr size_t seed_size_v1_1 = 16;
    static constexpr size_t seed_size_v1_2 = 32;

    // valid electrum 1.x mnemonic lengths
    static constexpr size_t mnemonic_size_v1_1 = 12;
    static constexpr size_t mnemonic_size_v1_2 = 24;

    const auto seed = old_mnemonic_decode(mnemonic);

    return !seed.empty() && (
        seed.size() == seed_size_v1_1 ||
        seed.size() == seed_size_v1_2 ||
        mnemonic.size() == mnemonic_size_v1_1 ||
        mnemonic.size() == mnemonic_size_v1_2);
}

static bool is_new_seed(const word_list& mnemonic, const data_slice& prefix)
{
    const auto normal = to_chunk(to_normal_nfkd_form(join(mnemonic)));
    const auto seed = hmac_sha512_hash(normal, hmac_data);
    return std::equal(prefix.begin(), prefix.end(), seed.begin());
}

static word_list mnemonic_encode(cpp_int entropy, const dictionary& lexicon)
{
    word_list mnemonic;
    const auto dictionary_size = lexicon.size();

    while (entropy != 0)
    {
        const cpp_int index = entropy % dictionary_size;
        mnemonic.push_back(lexicon[static_cast<size_t>(index)]);
        entropy /= dictionary_size;
    }

    return mnemonic;
}

static cpp_int mnemonic_decode(const word_list& mnemonic,
    const dictionary& lexicon)
{
    cpp_int entropy = 0;
    const auto dictionary_size = lexicon.size();

    for (const auto& word: boost::adaptors::reverse(mnemonic))
    {
        const auto position = find_position(lexicon, word);
        if (position == -1)
            return { 1 };

        entropy *= dictionary_size + position;
    }

    return entropy;
}

static data_chunk get_seed_prefix(seed prefix)
{
    switch (prefix)
    {
        case seed::standard:
            return seed_prefix_standard;
        case seed::witness:
            return seed_prefix_witness;
        case seed::two_factor_authentication:
            return seed_prefix_two_factor_authentication;
        default:
            return {};
    }
}

word_list create_mnemonic(const data_chunk& entropy, const dictionary& lexicon,
    seed prefix)
{
    word_list mnemonic;
    const auto electrum_prefix = get_seed_prefix(prefix);

    // cpp_int requires hex string for arbitrary precision int construction.
    auto numeric_entropy = cpp_int("0x" + encode_base16(entropy));

    do
    {
        mnemonic = mnemonic_encode(numeric_entropy++, lexicon);
        BITCOIN_ASSERT(mnemonic_decode(mnemonic, lexicon) == 0);
    } while (is_old_seed(mnemonic, lexicon) ||
        !is_new_seed(mnemonic, electrum_prefix));

    return mnemonic;
}

long_hash decode_mnemonic(const word_list& mnemonic,
    const std::string& passphrase)
{
    const auto sentence = join(mnemonic);
    const auto salt = to_normal_nfkd_form(passphrase_prefix + passphrase);
    return pkcs5_pbkdf2_hmac_sha512(to_chunk(sentence), to_chunk(salt),
        hmac_iterations);
}

bool validate_mnemonic(const word_list& mnemonic, const dictionary& lexicon,
    seed prefix)
{
    return is_new_seed(mnemonic, get_seed_prefix(prefix)) &&
        (mnemonic_decode(mnemonic, lexicon) == 0);
}

bool validate_mnemonic(const word_list& mnemonic,
    const dictionary_list& lexicons, const seed prefix)
{
    for (const auto& lexicon: lexicons)
        if (validate_mnemonic(mnemonic, *lexicon, prefix))
            return true;

    return false;
}

#endif

// This operation does not require normalization support
long_hash decode_mnemonic(const word_list& mnemonic)
{
    const auto sentence = join(mnemonic);
    const auto& salt = passphrase_prefix;
    return pkcs5_pbkdf2_hmac_sha512(to_chunk(sentence), to_chunk(salt),
        hmac_iterations);
}

} // namespace electrum
} // namespace wallet
} // namespace libbitcoin
