/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <string>
#include <vector>
#include <boost/program_options.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/unicode/unicode.hpp>
#include <bitcoin/system/utility/collection.hpp>
#include <bitcoin/system/utility/container_sink.hpp>
#include <bitcoin/system/utility/container_source.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/utility/istream_reader.hpp>
#include <bitcoin/system/utility/istream_bit_reader.hpp>
#include <bitcoin/system/utility/ostream_writer.hpp>
#include <bitcoin/system/utility/ostream_bit_writer.hpp>
#include <bitcoin/system/utility/string.hpp>
#include <bitcoin/system/wallet/electrum_v1.hpp>
#include <bitcoin/system/wallet/hd_private.hpp>
#include <bitcoin/system/wallet/language.hpp>
#include <bitcoin/system/wallet/mnemonic.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

// local constants
// ----------------------------------------------------------------------------

constexpr auto passphrase_prefix = "electrum";
constexpr size_t hmac_iterations = 2048;

constexpr size_t grind_limit = 0xffff;
constexpr size_t minimum_two_factor_authentication_words = 20;
static const auto seed_version = to_chunk("Seed version");

// Seed prefixes.
// The values for old, bip39 and none are not actual prefixes but included
// here for consistency when handling electrum exceptional conditions.
// The values are selected such that they contain invalid hexidecimal
// characters and therefore cannot match an actual seed prefix.
static const auto version_old = "old";
static const auto version_bip39 = "bip39";
static const auto version_standard = "01";
static const auto version_witness = "100";
static const auto version_two_factor_authentication = "101";
static const auto version_two_factor_authentication_witness = "102";
static const auto version_none = "none";

// 2^11 = 2048 implies 11 bits exactly indexes every possible dictionary word.
static const auto index_bits = static_cast<uint8_t>(
    system::log2(electrum::dictionary::size()));

// private static
// ----------------------------------------------------------------------------

const electrum::dictionaries electrum::dictionaries_
{
    {
        electrum::dictionary{ language::en, electrum::en },
        electrum::dictionary{ language::es, electrum::es },
        electrum::dictionary{ language::it, electrum::it },
        electrum::dictionary{ language::fr, electrum::fr },
        electrum::dictionary{ language::cs, electrum::cs },
        electrum::dictionary{ language::pt, electrum::pt },
        electrum::dictionary{ language::ja, electrum::ja },
        electrum::dictionary{ language::ko, electrum::ko },
        electrum::dictionary{ language::zh_Hans, electrum::zh_Hans },
        electrum::dictionary{ language::zh_Hant, electrum::zh_Hant }
    }
};

// Words are encoded in 11 bits and therefore are not byte aligned.
// As a consequence bit ordering matters. Bits are serialized to entropy bytes
// in big-endian order.
// TODO: provide Electrum reference URL.

string_list electrum::encoder(const data_chunk& entropy, language identifier)
{
    string_list words;

    // We can use the bit reader here (exactly the same as BIP39).
    // BIP39 entropy is extended by a checksum byte, so length is equivalent.
    // Shift right 11 bits (one word) is the same as / 2048.
    // This is equivalent to a read low 11 bits and shift left 11 bits.
    // That can be done by reading 11 bits at a time, hi-to-low.
    ////for (; number != 0; number /= 2048)
    ////    words.push_back(dictionaries_.at(number % dictionary::size()));

    // Read eleven bits into an index (0..2047).
    const auto read_index = [](istream_bit_reader& reader)
    {
        // TODO: verify endianness.
        return static_cast<uint32_t>(reader.read_bits(index_bits));
    };

    dictionary::search indexes(word_count(entropy));

    // Word indexes are not byte aligned, high-to-low bit reader required.
    data_source source(entropy);
    istream_reader byte_reader(source);
    istream_bit_reader bit_reader(byte_reader);

    // Skip unused high order bits, as n*2^11 may not be byte aligned.
    bit_reader.skip(unused_bits(entropy));

    // Create a search index.
    for (auto& index: indexes)
        index = read_index(bit_reader);

    return dictionaries_.at(indexes, identifier);
}

data_chunk electrum::decoder(const string_list& words, language identifier)
{
    // We can use the bit writer here (same as BIP39).
    // BIP39 entropy is extended by a checksum byte, so length is equivalent.
    // This is equivalent to a shift left 11 bits and write low 11 bits.
    // << 11 bits (one word) is the same as * 2048.
    // There is no need to reverse when writing big-endian.
    ////uint512_t value{ 0 };
    ////const auto indexes = dictionaries_.index(words, language);
    ////for (const auto index: boost::adaptors::reverse(indexes))
    ////    value = value * 2048 + index;

    // Write an index into eleven bits (0..2047).
    const auto write_index = [](ostream_bit_writer& writer, int32_t index)
    {
        // TODO: verify endianness.
        writer.write_bits(static_cast<size_t>(index), index_bits);
    };

    // Reserve buffer to include entropy and checksum, always one byte.
    data_chunk entropy;
    entropy.reserve(entropy_size(words));
    const auto indexes = dictionaries_.index(words, identifier);

    // Word indexes are not byte aligned, high-to-low bit writer required.
    data_sink sink(entropy);
    ostream_writer byte_writer(sink);
    ostream_bit_writer bit_writer(byte_writer);

    // Pad unused high order bits with zero, as n*2^11 may not be byte aligned.
    bit_writer.write_bits(0, unused_bits(entropy));

    // Convert word indexes to entropy.
    for (const auto index: indexes)
        write_index(bit_writer, index);

    bit_writer.flush();
    sink.flush();

    return entropy;
}

// Electrum also grinds randoms on a sequential nonce until the entropy is high
// enough to ensure population of words that represent the intended bit
// strength of the seed. But we do not use an internal PRNG and instead
// only accept base entropy from the caller. We want determinism from the
// entropy, and we cannot truly increase randomness. So we maintain entropy
// strength by using full provided entropy and produce exact word count.
// This is done by iterating a hash function so an arbitrary limit is required.
// The Electrum prng minimum value technique sacrifices 11 bits of entropy by
// discarding any prng value that is below 2^(strength-11).
// github.com/spesmilo/electrum/blob/master/electrum/mnemonic.py#L190-L205
electrum::result electrum::grinder(const data_chunk& entropy, seed_prefix prefix,
    language identifier, size_t limit)
{
    string_list words;
    data_chunk hash(entropy);
    const auto size = usable_size(entropy);

    // This just grinds away until exhausted or prefix found.
    // On the first iteration the usable entropy is unchnaged.
    // On the first iteration one entropy byte may be discarded.
    // On subsequent iterations size will be reduced from 512 bytes.
    // This allows the entropy to round trip after it is sufficient.

    while (limit-- != 0u)
    {
        hash.resize(size);
        words = encoder(hash, identifier);

        // Avoid collisions with Electrum v1 and BIP39 mnemonics.
        if (!electrum_v1(words, identifier) && !mnemonic(words) &&
            is_version(words, prefix))
                return { hash, words };

        // This replaces Electrum's prng with determinism.
        hash = to_chunk(sha512_hash(hash));
    }

    return {};
}

bool electrum::normalized(const string_list& words)
{
    return contained_by(words) != language::none;
}

// Electrum uses the same normalization function for words and passphrases.
// There is no entropy impact on lower casing seed words, but by lowering the
// passphrase there is a material entropy loss. This is presumably an oversight
// arising from reuse of the normalization function.
// github.com/spesmilo/electrum/blob/master/electrum/mnemonic.py#L77
std::string electrum::normalizer(const std::string& text)
{
#ifdef WITH_ICU
    auto seed = to_normal_nfkd_form(text);
    seed = to_lower(seed);
    seed = to_unaccented_form(seed);
    seed = system::join(system::split(seed));
    return to_compressed_cjk_form(seed);
#else
    return ascii_to_lower(text);
#endif
}

hd_private electrum::seeder(const string_list& words,
    const std::string& passphrase, uint64_t chain)
{
    // Words are in normal form, even without ICU.
    const auto sentence = to_chunk(system::join(words));

    // Passphrase is limited to ascii (normal) if without ICU.
    const auto salt = to_chunk(passphrase_prefix + normalizer(passphrase));
    const auto seed = pkcs5_pbkdf2_hmac_sha512(sentence, salt, hmac_iterations);
    const auto part = system::split(seed);

    // The object will be false if the secret (left) does not ec verify.
    return hd_private(part.left, part.right, chain);
}

electrum::seed_prefix electrum::prefixer(const string_list& words)
{
    // Words are in normal form, even without ICU.
    if (is_version(words, seed_prefix::standard))
        return seed_prefix::standard;
    if (is_version(words, seed_prefix::witness))
        return seed_prefix::witness;
    if (is_version(words, seed_prefix::two_factor_authentication))
        return seed_prefix::two_factor_authentication;
    if (is_version(words, seed_prefix::two_factor_authentication_witness))
        return seed_prefix::two_factor_authentication_witness;

    return seed_prefix::none;
}

bool electrum::validator(const string_list& words, seed_prefix prefix)
{
    // Words are in normal form, even without ICU.
    const auto sentence = to_chunk(normalizer(system::join(words)));
    const auto seed = encode_base16(hmac_sha512_hash(sentence, seed_version));
    return starts_with(seed, to_version(prefix));
}

// protected static
// ----------------------------------------------------------------------------

size_t electrum::entropy_bits(const data_slice& entropy)
{
    return (entropy.size() * byte_bits) / index_bits;
}

size_t electrum::entropy_bits(const string_list& words)
{
    return words.size() * index_bits;
}

size_t electrum::entropy_size(size_t entropy_bits)
{
    return ceilinged_divide(entropy_bits, byte_bits);
}

size_t electrum::entropy_size(const string_list& words)
{
    return ceilinged_divide(entropy_bits(words), byte_bits);
}

size_t electrum::word_count(const data_slice& entropy)
{
    return ceilinged_divide(entropy_bits(entropy), index_bits);
}

size_t electrum::word_count(size_t entropy_bits)
{
    return ceilinged_divide(entropy_bits, index_bits);
}

uint8_t electrum::unused_bits(const data_slice& entropy)
{
    // 0..10 unused bits are possible given arbitrary entropy size.
    return entropy_bits(entropy) % index_bits;
}

bool electrum::unused_byte(const data_slice& entropy)
{
    // 0..10 unused bits implies we can discard up to one byte.
    return unused_bits(entropy) / byte_bits;
}

size_t electrum::usable_size(const data_slice& entropy)
{
    return entropy.size() - (unused_byte(entropy) ? 1u : 0u);
}

// public static
// ----------------------------------------------------------------------------

language electrum::contained_by(const string_list& words, language identifier)
{
    return dictionaries_.contains(words, identifier);
}

bool electrum::is_valid_dictionary(language identifier)
{
    return dictionaries_.exists(identifier);
}

// Electrum has no explicit minimum or maximum strengths.
// Upper bounds here are based on use of a 512 bit hash function
// in grind() and transportation of entropy in byte vectors.
// A 64 byte seed (512 / 11) is 46 words (6 unused bits).
// This limits strength to 506 bits (BIP39 is 256).
// Lower bounds here are based on seed strength.
// A 132 bit (exactly 12 word) seed is the Electrum default.
// A 128 bit seed is the BIP39 minium, but this 11 Electrum words.
// So our limits are 132 to 506 bits (12 to 46) words.

bool electrum::is_valid_entropy_size(size_t size)
{
    return size >= entropy_size(strength_minimum) &&
        size <= entropy_size(strength_maximum);
}

bool electrum::is_valid_word_count(size_t count)
{
    return count >= word_count(strength_minimum) &&
        count <= word_count(strength_maximum);
}

bool electrum::is_valid_seed_prefix(seed_prefix prefix)
{
    switch (prefix)
    {
        case seed_prefix::standard:
        case seed_prefix::witness:
        case seed_prefix::two_factor_authentication:
        case seed_prefix::two_factor_authentication_witness:
            return true;
        default:
            return false;
    }
}

bool electrum::is_valid_two_factor_authentication_size(size_t count)
{
    // In Electrum 2.7, there was a breaking change in key derivation for
    // two_factor_authentication. Unfortunately the seed prefix was reused,
    // and now we can only distinguish them based on number of words. :(
    // github.com/spesmilo/electrum/blob/master/electrum/mnemonic.py#L258
    return count == word_count(strength_minimum) ||
        count >= minimum_two_factor_authentication_words;
}

bool electrum::is_version(const string_list& words, seed_prefix prefix)
{
    if (!is_valid_seed_prefix(prefix))
        return false;

    // HACK: see comment in is_valid_two_factor_authentication_size.
    if (prefix == seed_prefix::two_factor_authentication &&
        !is_valid_two_factor_authentication_size(words.size()))
        return false;

    if (!with_icu() && !normalized(words))
        return false;

    return validator(words, prefix);
}

electrum::seed_prefix electrum::to_prefix(const string_list& words)
{
    // Electrum rejects seed creation wha a seed would conflict with v1.
    // So if it validates under electrum v1 it cannot be a v2 seed.
    // This is possible given dictionary overlap and 12 or 24 words.
    // Any set of 12 or 24 words in the v1 dictionary will validate.
    // TODO: compute random words collision probability from word overlap.
    if (electrum_v1(words))
        return seed_prefix::old;

    // Electrum rejects seed creation if it conflicts with bip39.
    // So if it validates under bip39/mnemonic it cannot be a v2 seed.
    // Possible given shared dictionaries and 12, 15, 18, 21, or 24 words.
    // But a bip39/mnemonic seed (words) incorporates a checksum, so unlikely.
    // TODO: compute random words bip39 checksum collision probability.
    if (mnemonic(words))
        return seed_prefix::bip39;

    // In a non-ICU build these are likely to produce 'none' if the specified
    // words have not been prenormalized. However a collision on the version
    // bits of the resulting hash is certainly possible. So we exclude the
    // possibility of a mismatch by requiring prenormalized words.
    if (!with_icu() && !normalized(words))
        return seed_prefix::none;

    return prefixer(words);
}

std::string electrum::to_version(seed_prefix prefix)
{
    switch (prefix)
    {
        case seed_prefix::old:
            return version_old;
        case seed_prefix::bip39:
            return version_bip39;
        case seed_prefix::standard:
            return version_standard;
        case seed_prefix::witness:
            return version_witness;
        case seed_prefix::two_factor_authentication:
            return version_two_factor_authentication;
        case seed_prefix::two_factor_authentication_witness:
            return version_two_factor_authentication_witness;
        case seed_prefix::none:
        default:
            return version_none;
    }
}

// construction
// ----------------------------------------------------------------------------

electrum::electrum()
  : electrum_v1(), prefix_(seed_prefix::none)
{
}

electrum::electrum(const electrum& other)
  : electrum_v1(other), prefix_(other.prefix_)
{
}

electrum::electrum(const std::string& sentence, language identifier)
  : electrum(split(sentence, identifier), identifier)
{
}

electrum::electrum(const string_list& words, language identifier)
  : electrum(from_words(words, identifier))
{
}

electrum::electrum(const data_chunk& entropy, seed_prefix prefix,
    language identifier)
  : electrum(from_entropy(entropy, prefix, identifier))
{
}

// protected
electrum::electrum(const data_chunk& entropy, const string_list& words,
    language identifier, seed_prefix prefix)
  : electrum_v1(entropy, words, identifier), prefix_(prefix)
{
}

electrum electrum::from_entropy(const data_chunk& entropy, seed_prefix prefix,
    language identifier)
{
    if (!is_valid_entropy_size(entropy.size()))
        return {};

    if (!dictionaries_.exists(identifier))
        return {};

    if (!is_valid_seed_prefix(prefix))
        return {};

    const auto result = grinder(entropy, prefix, identifier, grind_limit);

    // Not your lucky day.
    if (result.words.empty())
        return {};

    // Save found entropy and normal words, originals are discarded.
    return { result.entropy, result.words, identifier, prefixer(result.words) };
}

electrum electrum::from_words(const string_list& words, language identifier)
{
    if (!is_valid_word_count(words.size()))
        return {};

    if (!with_icu() && !normalized(words))
        return {};

    const auto tokens = system::split(normalizer(system::join(words)));
    const auto lexicon = contained_by(words, identifier);

    if (lexicon == language::none)
        return {};

    if (identifier != language::none && lexicon != identifier)
        return {};

    // Save normalized words and derived entropy, original words are discarded.
    return { decoder(tokens, identifier), tokens, lexicon, prefixer(tokens) };
}

// public
// ----------------------------------------------------------------------------

hd_private electrum::to_seed(const std::string& passphrase,
    uint64_t chain) const
{
    // Preclude derivation from an invalid object state.
    if (!(*this))
        return {};

    // Passphrase normalization is necessary, however ASCII is normal.
    if (!with_icu() && !is_ascii(passphrase))
        return {};

    return seeder(words(), passphrase, chain);
}

// operators
// ------------------------------------------------------------------------

std::istream& operator>>(std::istream& in, electrum& to)
{
    std::string value;
    in >> value;
    to = electrum(value);

    if (!to)
    {
        using namespace boost::program_options;
        BOOST_THROW_EXCEPTION(invalid_option_value(value));
    }

    return in;
}

} // namespace wallet
} // namespace system
} // namespace libbitcoin
