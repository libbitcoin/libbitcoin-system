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
#include <bitcoin/system/wallet/mnemonics/electrum.hpp>

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/formats/base_2048.hpp>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/unicode/unicode.hpp>
#include <bitcoin/system/utility/collection.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/utility/exceptions.hpp>
#include <bitcoin/system/utility/string.hpp>
#include <bitcoin/system/wallet/mnemonics/electrum_v1.hpp>
#include <bitcoin/system/wallet/keys/hd_private.hpp>
#include <bitcoin/system/wallet/mnemonics/language.hpp>
#include <bitcoin/system/wallet/mnemonics/mnemonic.hpp>

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
    system::floored_log2(electrum::dictionary::size()));

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

// Entropy is an entirely private (internal) format.
string_list electrum::encoder(const data_chunk& entropy, language identifier)
{
    // Bytes are the base2048 encoding, so this is byte decoding.
    return decode_base2048_list(entropy, identifier);
}

// Entropy is an entirely private (internal) format.
data_chunk electrum::decoder(const string_list& words, language identifier)
{
    // Words are the base2048 decoding, so this is word encoding.
    data_chunk out;
    return encode_base2048_list(out, words, identifier) ? out : data_chunk{};
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

    // Normalize entropy to the wordlist by managing its pad bits.
    const auto entropy_size = usable_size(hash);
    hash.resize(entropy_size);
    const auto padding_mask = 0xff << unused_bits(hash);

    // This just grinds away until exhausted or prefix found.
    // On the first iteration the usable entropy is unchanged.
    // On the first iteration one entropy byte may be discarded.
    // On subsequent iterations size will be reduced from 512 bytes.
    // Previously discovered entropy round trips, matching on the first pass.
    while (limit-- > 0u)
    {
        hash[entropy_size - 1u] &= padding_mask;
        words = encoder(hash, identifier);

        // TODO: enable once electrum_v1 and mnemonic are tested.
        // Avoid collisions with Electrum v1 and BIP39 mnemonics.
        ////if (!electrum_v1(words, identifier) && !mnemonic(words) &&...
        if (is_version(words, prefix))
            return { hash, words };

        // This replaces Electrum's prng with determinism.
        hash = to_chunk(sha512_hash(hash));
        hash.resize(entropy_size);
    }

    return {};
}

// Electrum uses the same normalization function for words and passphrases.
// There is no entropy impact on lower casing seed words, but by lowering the
// passphrase there is a material entropy loss. This is presumably an oversight
// arising from reuse of the normalization function.
// github.com/spesmilo/electrum/blob/master/electrum/mnemonic.py#L77
hd_private electrum::seeder(const string_list& words,
    const std::string& passphrase, uint64_t chain)
{
    // Passphrase normalization is necessary, but ASCII is already normal.
#ifdef WITH_ICU
    auto pass = to_lower(to_compatibility_demposition(passphrase));
#else
    auto pass = ascii_to_lower(passphrase);
    if (!is_ascii(pass))
        return {};
#endif

    // With normalized dictionaries there is no benefit and a material
    // performance cost in applying this additional normalization to words,
    // so we apply it only in seeding, which is a compatibility requirement.

    // Remove diacritics from the nfkd form.
    pass = to_unaccented_form(pass);

    // Normalize ascii whitespace to a single 0x20 between each word.
    pass = system::join(system::split(pass));

    // Remove cjk separators from the nfkd form.
    // This is an unnecessary transform for the existing dictionaries when
    // using only words from those dictionaries as there are no matches.
    // See electrum dictionary tests for more information.
    pass = to_compressed_cjk_form(pass);

    // Words are in normal (nfkd) form, even without ICU.
    const auto sentence = to_chunk(system::join(words));
    const auto salt = to_chunk(passphrase_prefix + pass);
    const auto seed = pkcs5_pbkdf2_hmac_sha512(sentence, salt, hmac_iterations);
    const auto part = system::split(seed);

    // The object will be false if the secret (left) does not ec verify.
    return { part.first, part.second, chain };
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
    const auto sentence = to_chunk(system::join(words));
    const auto seed = encode_base16(hmac_sha512_hash(sentence, seed_version));
    return starts_with(seed, to_version(prefix));
}

// protected static
// ----------------------------------------------------------------------------

size_t electrum::entropy_bits(const data_slice& entropy)
{
    // The number of bits for the given number of bytes.
    return entropy.size() * byte_bits;
}

size_t electrum::entropy_bits(const string_list& words)
{
    // The number of bits for the given number of words.
    return words.size() * index_bits;
}

size_t electrum::entropy_size(size_t bit_strength)
{
    // The required number of bytes to achieve the given bit strength.
    return ceilinged_divide(bit_strength, byte_bits);
}

size_t electrum::entropy_size(const string_list& words)
{
    // The required number of bytes for the given number of words.
    return ceilinged_divide(entropy_bits(words), byte_bits);
}

size_t electrum::word_count(const data_slice& entropy)
{
    // The number of words that can be derived from the given entropy size.
    return entropy_bits(entropy) / index_bits;
}

size_t electrum::word_count(size_t bit_strength)
{
    // The required number of words to achieve the given bit strength.
    return ceilinged_divide(bit_strength, index_bits);
}

uint8_t electrum::unused_bits(const data_slice& entropy)
{
    // 0..10 unused bits are possible given arbitrary entropy size.
    return entropy_bits(entropy) % index_bits;
}

uint8_t electrum::unused_bytes(const data_slice& entropy)
{
    // 0..10 unused bits implies we can discard up to one byte.
    return unused_bits(entropy) / byte_bits;
}

size_t electrum::usable_size(const data_slice& entropy)
{
    return entropy.size() - unused_bytes(entropy);
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

    // Normalize to improve chance of dictionary matching.
    const auto tokens = normalize(words);

    // Tokens are accepted if they are contained by any single dictionary.
    if (contained_by(tokens) == language::none)
        return {};

    return validator(words, prefix);
}

electrum::seed_prefix electrum::to_prefix(const string_list& words)
{
    // Electrum rejects seed creation when a seed would conflict with v1.
    // So if it validates under electrum v1 it cannot be a v2 seed.
    // This is possible given dictionary overlap and 12 or 24 words.
    // Any set of 12 or 24 words in the v1 dictionary will validate.
    if (electrum_v1(words))
        return seed_prefix::old;

    // Electrum rejects seed creation when a seed would conflict with bip39.
    // So if it validates under bip39/mnemonic it cannot be a v2 seed.
    // Possible given shared dictionaries and 12, 15, 18, 21, or 24 words.
    // But a bip39/mnemonic seed (words) incorporates a checksum, so unlikely.
    if (mnemonic(words))
        return seed_prefix::bip39;

    // Normalize to improve chance of dictionary matching.
    const auto tokens = normalize(words);

    // Tokens are accepted if they are contained by any single dictionary.
    if (contained_by(tokens) == language::none)
        return {};

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

    if (!is_valid_seed_prefix(prefix))
        return {};

    if (!dictionaries_.exists(identifier))
        return {};

    const auto result = grinder(entropy, prefix, identifier, grind_limit);

    // Not your lucky day.
    if (result.words.empty())
        return {};

    // Save derived words and ground entropy, original is discarded.
    return { result.entropy, result.words, identifier, prefixer(result.words) };
}

electrum electrum::from_words(const string_list& words, language identifier)
{
    if (!is_valid_word_count(words.size()))
        return {};

    // Normalize to improve chance of dictionary matching.
    const auto tokens = normalize(words);
    const auto lexicon = contained_by(tokens, identifier);

    if (lexicon == language::none)
        return {};

    if (identifier != language::none && lexicon != identifier)
        return {};

    // HACK: There are 100 same words in en/fr, all with distinct indexes.
    // If unspecified and matches en then check fr, since en is searched first.
    if (identifier == language::none && lexicon == language::en &&
        contained_by(tokens, language::fr) == language::fr)
        return {};

    // Save derived entropy and dictionary words, originals are discarded.
    return { decoder(tokens, identifier), tokens, lexicon, prefixer(tokens) };
}

// public
// ----------------------------------------------------------------------------

electrum::seed_prefix electrum::prefix() const
{
    return prefix_;
}

hd_private electrum::to_seed(const std::string& passphrase,
    uint64_t chain) const
{
    // Preclude derivation from an invalid object state.
    if (!(*this))
        return {};

    return seeder(words(), passphrase, chain);
}

// operators
// ------------------------------------------------------------------------

std::istream& operator>>(std::istream& in, electrum& to)
{
    std::istreambuf_iterator<char> begin(in), end;
    std::string value(begin, end);
    to = electrum(value);

    if (!to)
        throw istream_exception(value);

    return in;
}

std::ostream& operator<<(std::ostream& out, const electrum& of)
{
    out << of.sentence();
    return out;
}

} // namespace wallet
} // namespace system
} // namespace libbitcoin
