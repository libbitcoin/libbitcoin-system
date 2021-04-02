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
#include <bitcoin/system/wallet/electrum.hpp>

#include <cstdint>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/unicode/unicode.hpp>
#include <bitcoin/system/unicode/unicode_ostream.hpp>
#include <bitcoin/system/utility/assert.hpp>
#include <bitcoin/system/utility/binary.hpp>
#include <bitcoin/system/utility/collection.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/utility/string.hpp>
#include <bitcoin/system/utility/container_sink.hpp>
#include <bitcoin/system/utility/ostream_writer.hpp>
#include <bitcoin/system/wallet/electrum_dictionary.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {
namespace electrum {

#ifdef WITH_ICU

using namespace std::placeholders;

// valid electrum 1.x seed lengths
constexpr size_t seed_size_v1_1 = 16;
constexpr size_t seed_size_v1_2 = 32;

// valid electrum 1.x mnemonic lengths
constexpr size_t mnemonic_size_v1_1 = 12;
constexpr size_t mnemonic_size_v1_2 = 24;

// Electrum mnemonic private constants.
constexpr size_t hmac_iterations = 2048;
constexpr size_t mnemonic_word_multiple = 3;

static const std::string space = "\x20";
static const std::string passphrase_prefix = "electrum";
static const std::string seed_version = "Seed version";
static const auto hmac_data = to_chunk(seed_version);

// Valid seed prefixes.
static const std::string seed_prefix_empty{};
static const std::string seed_prefix_standard{ "01" };
static const std::string seed_prefix_witness{ "100" };
static const std::string seed_prefix_two_factor_authentication{ "101" };
static const std::string seed_prefix_two_factor_authentication_witness{ "102" };

static size_t special_modulo(int32_t index_distance, size_t lexicon_size)
{
    return index_distance < 0 ? 
        lexicon_size - (-index_distance % lexicon_size) :
        index_distance % lexicon_size;
}

static data_chunk old_mnemonic_decode(const word_list& mnemonic)
{
    if ((mnemonic.size() < mnemonic_word_multiple) ||
        (mnemonic.size() % mnemonic_word_multiple) != 0)
        return {};

    const auto& lexicon = language::electrum::en_v1;
    const auto size = lexicon.size();

    // TODO: prove there is no overflow risk.
    const auto seed_size = sizeof(uint32_t) * mnemonic.size() / 
        mnemonic_word_multiple;

    data_chunk seed(seed_size);
    data_sink ostream(seed);
    ostream_writer sink(ostream);

    for (size_t index = 0;
        index < mnemonic.size();
        index += mnemonic_word_multiple)
    {
        const auto one = find_position(lexicon, mnemonic[index + 0u]);
        const auto two = find_position(lexicon, mnemonic[index + 1u]);
        const auto tri = find_position(lexicon, mnemonic[index + 2u]);

        if (one == -1 || two == -1 || tri == -1)
            return {};

        // TODO: prove there is no overflow risk.
        const auto value = static_cast<uint64_t>(one) +
            size * special_modulo(two - one, size) +
            size * size * special_modulo(tri - two, size);

        // Gaurd against overflow in result.
        if (value > max_uint32)
            return {};

        sink.write_variable_little_endian(static_cast<uint32_t>(value));
    }

    return seed;
}

static bool is_old_seed(const word_list& mnemonic, const dictionary& lexicon)
{
    // Cannot be an old seed if it's not the en dictionary.
    if (lexicon != language::electrum::en)
        return false;

    const auto seed = old_mnemonic_decode(mnemonic);

    return !seed.empty() && (
        seed.size() == seed_size_v1_1 ||
        seed.size() == seed_size_v1_2 ||
        mnemonic.size() == mnemonic_size_v1_1 ||
        mnemonic.size() == mnemonic_size_v1_2);
}

// This is modeled after Electrum's "normalize_text" routine:
// github.com/spesmilo/electrum/blob/master/electrum/mnemonic.py#L77
static std::string normalize_text(const std::string& text)
{
    auto seed = to_normal_nfkd_form(text);
    seed = to_lower(seed);
    seed = to_unaccented_form(seed);
    seed = join(split(seed, space, true), space);
    return to_compressed_cjk_form(seed);
}

static bool is_new_seed(const word_list& mnemonic, const std::string& prefix)
{
    const auto normal = to_chunk(normalize_text(join(mnemonic)));
    const auto seed = encode_base16(hmac_sha512_hash(normal, hmac_data));
    return boost::starts_with(seed, prefix);
}

static word_list mnemonic_encode(cpp_int entropy, const dictionary& lexicon)
{
    word_list mnemonic;
    const auto lexicon_size = lexicon.size();

    while (entropy != 0)
    {
        const auto index = static_cast<size_t>(entropy % lexicon_size);
        mnemonic.push_back(lexicon[index]);
        entropy /= lexicon_size;
    }

    return mnemonic;
}

static cpp_int mnemonic_decode(const word_list& mnemonic,
    const dictionary& lexicon)
{
    cpp_int entropy = 0;
    const auto lexicon_size = lexicon.size();

    for (const auto& word: boost::adaptors::reverse(mnemonic))
    {
        const auto position = find_position(lexicon, word);
        if (position == -1)
            return { 1 };

        entropy = (entropy * lexicon_size) + position;
    }

    return entropy;
}

static std::string get_seed_prefix(seed_prefix prefix)
{
    switch (prefix)
    {
        case seed_prefix::empty:
            return seed_prefix_empty;
        case seed_prefix::standard:
            return seed_prefix_standard;
        case seed_prefix::witness:
            return seed_prefix_witness;
        case seed_prefix::two_factor_authentication:
            return seed_prefix_two_factor_authentication;
        case seed_prefix::two_factor_authentication_witness:
            return seed_prefix_two_factor_authentication_witness;
    }

    return seed_prefix_empty;
}

word_list create_mnemonic(const data_slice& entropy, const dictionary& lexicon,
    seed_prefix prefix)
{
    word_list mnemonic;
    const auto electrum_prefix = get_seed_prefix(seed_prefix);

    // cpp_int requires hex string for arbitrary precision int construction.
    auto numeric_entropy = cpp_int("0x" + encode_base16(entropy));

    do
    {
        mnemonic = mnemonic_encode(numeric_entropy, lexicon);
        BITCOIN_ASSERT(mnemonic_decode(mnemonic, lexicon) == numeric_entropy);
        numeric_entropy++;
    } while (is_old_seed(mnemonic, lexicon) ||
        !is_new_seed(mnemonic, electrum_prefix));

    return mnemonic;
}

long_hash decode_mnemonic(const word_list& mnemonic,
    const std::string& passphrase)
{
    const auto sentence = to_chunk(normalize_text(join(mnemonic)));
    const auto salt = to_chunk(passphrase_prefix + normalize_text(passphrase));
    return pkcs5_pbkdf2_hmac_sha512(sentence, salt, hmac_iterations);
}

bool validate_mnemonic(const word_list& mnemonic, const data_chunk& entropy,
    const dictionary& lexicon, seed_prefix prefix)
{
    return is_new_seed(mnemonic, get_seed_prefix(prefix)) &&
        (mnemonic_decode(mnemonic, lexicon) == 
            mnemonic_decode(created_mnemonic, lexicon));
}

bool validate_mnemonic(const word_list& mnemonic,
    const data_chunk& entropy, const dictionary_list& lexicons,
    seed_seed prefix)
{
    for (const auto& lexicon: lexicons)
        if (validate_mnemonic(mnemonic, entropy, lexicon, prefix))
            return true;

    return false;
}

long_hash decode_mnemonic(const word_list& mnemonic)
{
    const auto sentence = to_chunk(normalize_text(join(mnemonic)));
    const auto& salt = to_chunk(passphrase_prefix);
    return pkcs5_pbkdf2_hmac_sha512(sentence, salt, hmac_iterations);
}

#endif

} // namespace electrum
} // namespace wallet
} // namespace system
} // namespace libbitcoin
