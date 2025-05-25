/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/radix/radix.hpp>
#include <bitcoin/system/unicode/unicode.hpp>
#include <bitcoin/system/wallet/context.hpp>
#include <bitcoin/system/wallet/keys/hd_private.hpp>
#include <bitcoin/system/words/words.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

// local constants
// ----------------------------------------------------------------------------

// 2^11 = 2048 implies 11 bits exactly indexes every possible dictionary word.
static const auto index_bits = narrow_cast<uint8_t>(
    system::floored_log2(mnemonic::dictionary::size()));

// private static
// ----------------------------------------------------------------------------

const mnemonic::dictionaries mnemonic::dictionaries_
{
    {
        mnemonic::dictionary{ language::en, words::mnemonic::en },
        mnemonic::dictionary{ language::es, words::mnemonic::es },
        mnemonic::dictionary{ language::it, words::mnemonic::it },
        mnemonic::dictionary{ language::fr, words::mnemonic::fr },
        mnemonic::dictionary{ language::cs, words::mnemonic::cs },
        mnemonic::dictionary{ language::pt, words::mnemonic::pt },
        mnemonic::dictionary{ language::ja, words::mnemonic::ja },
        mnemonic::dictionary{ language::ko, words::mnemonic::ko },
        mnemonic::dictionary{ language::zh_Hans, words::mnemonic::zh_Hans },
        mnemonic::dictionary{ language::zh_Hant, words::mnemonic::zh_Hant }
    }
};

// protected static (coders)
// ----------------------------------------------------------------------------

// Entropy requires wordlist mapping because of the checksum.
string_list mnemonic::encoder(const data_chunk& entropy,
    language identifier) NOEXCEPT
{
    // Bytes are the base2048 encoding, so this is byte decoding.
    const auto buffer = splice(entropy, { checksum_byte(entropy) });
    return decode_base2048_list(buffer, identifier);
}

// Entropy requires wordlist mapping because of the checksum.
data_chunk mnemonic::decoder(const string_list& words,
    language identifier) NOEXCEPT
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

long_hash mnemonic::seeder(const string_list& words,
    const std::string& passphrase) NOEXCEPT
{
    constexpr size_t hmac_iterations = 2048;
    constexpr auto passphrase_prefix = "mnemonic";

    // Passphrase is limited to ascii (normal) if HAVE_ICU undefind.
    std::string phrase{ passphrase };

    LCOV_EXCL_START("Always succeeds unless HAVE_ICU undefined.")

    // Unlike Electrum, BIP39 does not perform any further normalization.
    if (!to_compatibility_decomposition(phrase))
        return {};

    LCOV_EXCL_STOP()

    // Words are in normal (lower, nfkd) form, even without ICU.
    return pbkd<sha512>::key<long_hash_size>(system::join(words),
        passphrase_prefix + phrase, hmac_iterations);
}

uint8_t mnemonic::checksum_byte(const data_chunk& entropy) NOEXCEPT
{
    // The high order bits of the first sha256_hash byte are the checksum.
    // Only 4, 5, 6, 7, or 8 bits of the hash are used (based on size).
    const auto mask_bits = byte_bits - checksum_bits(entropy);
    const auto checksum_mask = max_uint8 << mask_bits;
    return sha256_hash(entropy).front() & checksum_mask;
}

// protected static (sizers)
// ----------------------------------------------------------------------------

size_t mnemonic::checksum_bits(const data_slice& entropy) NOEXCEPT
{
    return entropy.size() / entropy_multiple;
}

size_t mnemonic::checksum_bits(const string_list& words) NOEXCEPT
{
    return words.size() / word_multiple;
}

size_t mnemonic::entropy_bits(const data_slice& entropy) NOEXCEPT
{
    return to_bits(entropy.size());
}

size_t mnemonic::entropy_bits(const string_list& words) NOEXCEPT
{
    return words.size() * index_bits - checksum_bits(words);
}

size_t mnemonic::entropy_size(const string_list& words) NOEXCEPT
{
    return to_floored_bytes(entropy_bits(words));
}

size_t mnemonic::word_count(const data_slice& entropy) NOEXCEPT
{
    return (entropy_bits(entropy) + checksum_bits(entropy)) / index_bits;
}

// protected static (checkers)
// ----------------------------------------------------------------------------

bool mnemonic::is_ambiguous(const string_list& words, language requested,
    language derived) NOEXCEPT
{
    // HACK: There are 100 same words in en/fr, all with distinct indexes.
    // If matches en and unspecified then check fr, since en is searched first.
    return
        derived == language::en &&
        requested == language::none &&
        contained_by(words, language::fr) == language::fr;
}

// public static
// ----------------------------------------------------------------------------

language mnemonic::contained_by(const string_list& words,
    language identifier) NOEXCEPT
{
    return dictionaries_.contains(words, identifier);
}

bool mnemonic::is_valid_dictionary(language identifier) NOEXCEPT
{
    return dictionaries_.exists(identifier);
}

bool mnemonic::is_valid_entropy_size(size_t size) NOEXCEPT
{
    return (is_zero(size % entropy_multiple) &&
        size >= entropy_minimum && size <= entropy_maximum);
}

bool mnemonic::is_valid_word_count(size_t count) NOEXCEPT
{
    return (is_zero(count % word_multiple) &&
        count >= word_minimum && count <= word_maximum);
}

// construction
// ----------------------------------------------------------------------------

mnemonic::mnemonic() NOEXCEPT
  : languages()
{
}

mnemonic::mnemonic(const std::string& sentence, language identifier) NOEXCEPT
  : mnemonic(split(sentence, identifier), identifier)
{
}

mnemonic::mnemonic(const string_list& words, language identifier) NOEXCEPT
  : mnemonic(from_words(words, identifier))
{
}

mnemonic::mnemonic(const data_chunk& entropy, language identifier) NOEXCEPT
  : mnemonic(from_entropy(entropy, identifier))
{
}

// protected
mnemonic::mnemonic(const data_chunk& entropy, const string_list& words,
    language identifier) NOEXCEPT
  : languages(entropy, words, identifier)
{
}

// protected (factories)
// ----------------------------------------------------------------------------

mnemonic mnemonic::from_entropy(const data_chunk& entropy,
    language identifier) NOEXCEPT
{
    if (!is_valid_entropy_size(entropy.size()))
        return {};

    if (!dictionaries_.exists(identifier))
        return {};

    // Save entropy and derived words.
    return { entropy, encoder(entropy, identifier), identifier };
}

mnemonic mnemonic::from_words(const string_list& words,
    language identifier) NOEXCEPT
{
    if (!is_valid_word_count(words.size()))
        return {};

    // Normalize to improve chance of dictionary matching.
    const auto tokens = try_normalize(words);
    const auto lexicon = contained_by(tokens, identifier);

    if (lexicon == language::none)
        return {};

    if (identifier != language::none && lexicon != identifier)
        return {};

    // HACK: en-fr dictionary ambiguity.
    if (is_ambiguous(tokens, identifier, lexicon))
        return {};

    const auto entropy = decoder(tokens, lexicon);

    // Checksum verification failed.
    if (entropy.empty())
        return {};

    // Save derived entropy and dictionary words, originals are discarded.
    return { entropy, tokens, lexicon };
}

// public methods
// ----------------------------------------------------------------------------

long_hash mnemonic::to_seed(const std::string& passphrase) const NOEXCEPT
{
    if (!(*this))
        return {};

    return seeder(words(), passphrase);
}

hd_private mnemonic::to_key(const std::string& passphrase,
    const context& context) const NOEXCEPT
{
    if (!(*this))
        return {};

    // The key will be invalid if the secret does not ec verify.
    return { to_chunk(to_seed(passphrase)), context.hd_prefixes() };
}

} // namespace wallet
} // namespace system
} // namespace libbitcoin
