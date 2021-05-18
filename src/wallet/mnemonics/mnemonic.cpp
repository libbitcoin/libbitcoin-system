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
#include <bitcoin/system/wallet/mnemonics/mnemonic.hpp>

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <string>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/formats/base_2048.hpp>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/unicode/ascii.hpp>
#include <bitcoin/system/unicode/normalization.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/utility/exceptions.hpp>
#include <bitcoin/system/utility/string.hpp>
#include <bitcoin/system/wallet/mnemonics/dictionary.hpp>
#include <bitcoin/system/wallet/keys/hd_private.hpp>
#include <bitcoin/system/wallet/mnemonics/language.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

// local constants
// ----------------------------------------------------------------------------

constexpr auto passphrase_prefix = "mnemonic";
constexpr size_t hmac_iterations = 2048;

// 2^11 = 2048 implies 11 bits exactly indexes every possible dictionary word.
static const auto index_bits = static_cast<uint8_t>(
    system::floored_log2(mnemonic::dictionary::size()));

// private static
// ----------------------------------------------------------------------------

const mnemonic::dictionaries mnemonic::dictionaries_
{
    {
        mnemonic::dictionary{ language::en, mnemonic::en },
        mnemonic::dictionary{ language::es, mnemonic::es },
        mnemonic::dictionary{ language::it, mnemonic::it },
        mnemonic::dictionary{ language::fr, mnemonic::fr },
        mnemonic::dictionary{ language::cs, mnemonic::cs },
        mnemonic::dictionary{ language::pt, mnemonic::pt },
        mnemonic::dictionary{ language::ja, mnemonic::ja },
        mnemonic::dictionary{ language::ko, mnemonic::ko },
        mnemonic::dictionary{ language::zh_Hans, mnemonic::zh_Hans },
        mnemonic::dictionary{ language::zh_Hant, mnemonic::zh_Hant }
    }
};

// Entropy requires wordlist mapping because of the checksum.
string_list mnemonic::encoder(const data_chunk& entropy, language identifier)
{
    // Bytes are the base2048 encoding, so this is byte decoding.
    const auto checksum = data_chunk{ checksum_byte(entropy) };
    const auto buffer = build_chunk({ entropy, checksum });
    return decode_base2048_list(buffer, identifier);
}

// Entropy requires wordlist mapping because of the checksum.
data_chunk mnemonic::decoder(const string_list& words, language identifier)
{
    // Words are the base2048 decoding, so this is word encoding.
    data_chunk buffer;
    if (!encode_base2048_list(buffer, words, identifier))
        return {};

    // Entropy is always byte aligned.
    // Checksum is in high order bits of last buffer byte, zero-padded.
    const data_chunk entropy{ buffer.begin(), std::prev(buffer.end()) };
    return buffer.back() == checksum_byte(entropy) ? entropy : data_chunk{};
}

hd_private mnemonic::seeder(const string_list& words,
    const std::string& passphrase, uint64_t chain)
{
    // Passphrase is limited to ascii (normal) if without ICU.
    // Unlike electrum, BIP39 does not lower case passphrases, trim whitespace
    // remove diacritics, or compress cjk.
#ifdef WITH_ICU
    auto pass = to_compatibility_demposition(passphrase);
#else
    auto pass = passphrase;
    if (!is_ascii(pass))
        return {};
#endif

    // Words are in normal (nfkd) form, even without ICU.
    const auto sentence = to_chunk(system::join(words));
    const auto salt = to_chunk(passphrase_prefix + pass);
    const auto seed = pkcs5_pbkdf2_hmac_sha512(sentence, salt, hmac_iterations);
    const auto part = system::split(seed);

    // The object will be false if the secret (left) does not ec verify.
    return hd_private(part.first, part.second, chain);
}

// protected static
// ----------------------------------------------------------------------------

uint8_t mnemonic::checksum_byte(const data_slice& entropy)
{
    // The high order bits of the first sha256_hash byte are the checksum.
    // Only 4, 5, 6, 7, or 8 bits of the hash are used (based on size).
    const auto mask_bits = byte_bits - checksum_bits(entropy);
    const auto checksum_mask = max_uint8 << mask_bits;
    return sha256_hash(entropy).front() & checksum_mask;
}

size_t mnemonic::checksum_bits(const data_slice& entropy)
{
    return entropy.size() / entropy_multiple;
}

size_t mnemonic::checksum_bits(const string_list& words)
{
    return words.size() / word_multiple;
}

size_t mnemonic::entropy_bits(const data_slice& entropy)
{
    return entropy.size() * byte_bits;
}

size_t mnemonic::entropy_bits(const string_list& words)
{
    return words.size() * index_bits - checksum_bits(words);
}

size_t mnemonic::entropy_size(const string_list& words)
{
    return entropy_bits(words) / byte_bits;
}

size_t mnemonic::word_count(const data_slice& entropy)
{
    return (entropy_bits(entropy) + checksum_bits(entropy)) / index_bits;
}

// public static
// ----------------------------------------------------------------------------

language mnemonic::contained_by(const string_list& words, language identifier)
{
    return dictionaries_.contains(words, identifier);
}

bool mnemonic::is_valid_dictionary(language identifier)
{
    return dictionaries_.exists(identifier);
}

bool mnemonic::is_valid_entropy_size(size_t size)
{
    return ((size % entropy_multiple) == 0u &&
        size >= entropy_minimum && size <= entropy_maximum);
}

bool mnemonic::is_valid_word_count(size_t count)
{
    return ((count % word_multiple) == 0u &&
        count >= word_minimum && count <= word_maximum);
}

// construction
// ----------------------------------------------------------------------------

mnemonic::mnemonic()
  : languages()
{
}

mnemonic::mnemonic(const mnemonic& other)
  : languages(other)
{
}

mnemonic::mnemonic(const std::string& sentence, language identifier)
  : mnemonic(split(sentence, identifier), identifier)
{
}

mnemonic::mnemonic(const string_list& words, language identifier)
  : mnemonic(from_words(words, identifier))
{
}

mnemonic::mnemonic(const data_chunk& entropy, language identifier)
  : mnemonic(from_entropy(entropy, identifier))
{
}

// protected
mnemonic::mnemonic(const data_chunk& entropy, const string_list& words,
    language identifier)
  : languages(entropy, words, identifier)
{
}

// private methods
// ----------------------------------------------------------------------------

mnemonic mnemonic::from_entropy(const data_chunk& entropy, language identifier)
{
    if (!is_valid_entropy_size(entropy.size()))
        return {};

    if (!dictionaries_.exists(identifier))
        return {};

    // Save entropy and derived words.
    return { entropy, encoder(entropy, identifier), identifier };
}

mnemonic mnemonic::from_words(const string_list& words, language identifier)
{
    if (!is_valid_word_count(words.size()))
        return {};

    // Normalize to improve chance of dictionary matching.
    const auto tokens = normalize(words);
    const auto lexicon = contained_by(words, identifier);

    if (lexicon == language::none)
        return {};

    if (identifier != language::none && lexicon != identifier)
        return {};

    // HACK: There are 100 same words in en/fr, all with distinct indexes.
    // If unspecified and matches en then check fr, since en is searched first.
    if (identifier == language::none && lexicon == language::en &&
        contained_by(tokens, language::fr) == language::fr)
        return {};

    const auto entropy = decoder(tokens, identifier);

    // Checksum verification failed.
    if (entropy.empty())
        return {};

    // Save derived entropy and dictionary words, originals are discarded.
    return { entropy, tokens, lexicon };
}

// public
// ----------------------------------------------------------------------------

hd_private mnemonic::to_seed(const std::string& passphrase,
    uint64_t chain) const
{
    // Preclude derivation from an invalid object state.
    if (!(*this))
        return {};

    return seeder(words(), passphrase, chain);
}

// operators
// ------------------------------------------------------------------------

std::istream& operator>>(std::istream& in, mnemonic& to)
{
    std::istreambuf_iterator<char> begin(in), end;
    std::string value(begin, end);
    to = mnemonic(value);

    if (!to)
        throw istream_exception(value);

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