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
#include <bitcoin/system/wallet/mnemonic.hpp>

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/utility/container_sink.hpp>
#include <bitcoin/system/utility/container_source.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/utility/exceptions.hpp>
#include <bitcoin/system/utility/istream_bit_reader.hpp>
#include <bitcoin/system/utility/istream_reader.hpp>
#include <bitcoin/system/utility/ostream_bit_writer.hpp>
#include <bitcoin/system/utility/ostream_writer.hpp>
#include <bitcoin/system/utility/string.hpp>
#include <bitcoin/system/wallet/dictionary.hpp>
#include <bitcoin/system/wallet/hd_private.hpp>
#include <bitcoin/system/wallet/language.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

// local constants
// ----------------------------------------------------------------------------

constexpr auto passphrase_prefix = "mnemonic";
constexpr size_t hmac_iterations = 2048;

// 2^11 = 2048 implies 11 bits exactly indexes every possible dictionary word.
static const auto index_bits = static_cast<uint8_t>(
    system::log2(mnemonic::dictionary::size()));

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

// Words are encoded in 11 bits and therefore are not byte aligned.
// As a consequence bit ordering matters. Bits are serialized to entropy bytes
// in big-endian order. This can be observed in the reference implementation:
// github.com/trezor/python-mnemonic/blob/master/mnemonic/mnemonic.py#L152-L162

string_list mnemonic::encoder(const data_chunk& entropy, language identifier)
{
    // Read eleven bits into an index (0..2047).
    const auto read_index = [](istream_bit_reader& reader)
    {
        return static_cast<uint32_t>(reader.read_bits(index_bits));
    };

    dictionary::search indexes(word_count(entropy));
    const auto checksum = data_chunk{ checksum_byte(entropy) };
    const auto buffer = build_chunk({ entropy, checksum });

    // Word indexes are not byte aligned, high-to-low bit reader required.
    data_source source(buffer);
    istream_reader byte_reader(source);
    istream_bit_reader bit_reader(byte_reader);

    // Create a search index.
    for (auto& index: indexes)
        index = read_index(bit_reader);

    return dictionaries_.at(indexes, identifier);
}

data_chunk mnemonic::decoder(const string_list& words, language identifier)
{
    // Write an index into eleven bits (0..2047).
    const auto write_index = [](ostream_bit_writer& writer, int32_t index)
    {
        writer.write_bits(static_cast<size_t>(index), index_bits);
    };

    // Reserve buffer to include entropy and checksum, always one byte.
    data_chunk buffer;
    buffer.reserve(entropy_size(words) + 1u);
    const auto indexes = dictionaries_.index(words, identifier);

    // Word indexes are not byte aligned, high-to-low bit writer required.
    data_sink sink(buffer);
    ostream_writer byte_writer(sink);
    ostream_bit_writer bit_writer(byte_writer);

    // Convert word indexes to entropy.
    for (const auto index: indexes)
        write_index(bit_writer, index);

    bit_writer.flush();
    sink.flush();

    // Entropy is always byte aligned.
    const data_chunk entropy{ buffer.begin(), std::prev(buffer.end()) };

    // Checksum is in high order bits of last buffer byte, zero-padded.
    return buffer.back() == checksum_byte(entropy) ? entropy : data_chunk{};
}

bool mnemonic::normalized(const string_list& words)
{
    return contained_by(words) != language::none;
}

std::string mnemonic::normalizer(const std::string& text)
{
#ifdef WITH_ICU
    return to_normal_nfkd_form(text);
#else
    return text;
#endif
}

hd_private mnemonic::seeder(const string_list& words,
    const std::string& passphrase, uint64_t chain)
{
    // Words are in normal form, even without ICU.
    const auto sentence = to_chunk(system::join(words));

    // Passphrase is limited to ascii (normal) if without ICU.
    const auto salt = to_chunk(passphrase_prefix + normalizer(passphrase));
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

// protected
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

    // Save original entropy and derived normal words.
    return { entropy, encoder(entropy, identifier), identifier };
}

mnemonic mnemonic::from_words(const string_list& words, language identifier)
{
    if (!is_valid_word_count(words.size()))
        return {};

    // Normalize is non-critical here, unnormalized words will be rejected.
    const auto tokens = system::split(normalizer(system::join(words)));
    const auto lexicon = contained_by(words, identifier);

    if (lexicon == language::none)
        return {};

    if (identifier != language::none && lexicon != identifier)
        return {};

    const auto entropy = decoder(tokens, identifier);

    // Checksum verification failed.
    if (entropy.empty())
        return {};

    // Save normalized words and derived entropy, original words are discarded.
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

    // Passphrase normalization is necessary, however ASCII is normal.
    if (!with_icu() && !is_ascii(passphrase))
        return {};

    return seeder(words(), passphrase, chain);
}

// operators
// ------------------------------------------------------------------------

std::istream& operator>>(std::istream& in, mnemonic& to)
{
    std::string value;
    in >> value;
    to = mnemonic(value);

    if (!to)
        throw istream_exception(value);

    return in;
}

} // namespace wallet
} // namespace system
} // namespace libbitcoin
