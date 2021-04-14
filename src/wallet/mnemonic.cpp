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
#include <boost/program_options.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/unicode/unicode.hpp>
#include <bitcoin/system/utility/container_sink.hpp>
#include <bitcoin/system/utility/container_source.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/utility/istream_bit_reader.hpp>
#include <bitcoin/system/utility/istream_reader.hpp>
#include <bitcoin/system/utility/ostream_bit_writer.hpp>
#include <bitcoin/system/utility/ostream_writer.hpp>
#include <bitcoin/system/utility/string.hpp>
#include <bitcoin/system/wallet/dictionary.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

// TODO: implement WITH_ICU

// Words are encoded in 11 bits and therefore are not byte aligned.
// As a consequence bit ordering matters. Bits are serialized to entropy bytes
// in big-endian order. This can be observed in the reference implementation:
// github.com/trezor/python-mnemonic/blob/master/mnemonic/mnemonic.py#L152-L162

// local constants
// ----------------------------------------------------------------------------

constexpr auto ideographic_space = "\xe3\x80\x80";
constexpr auto ascii_space = "\x20";
constexpr auto passphrase_prefix = "mnemonic";
constexpr size_t hmac_iterations = 2048;

// 2^11 = 2048 implies 11 bits exactly indexes every possible dictionary word.
static const auto index_bits = static_cast<uint8_t>(
    system::log2(mnemonic::dictionary::size()));

// private static
// ----------------------------------------------------------------------------

string_list mnemonic::encode(const data_chunk& entropy, language language)
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

    return dictionaries_.at(indexes, language);
}

data_chunk mnemonic::decode(const string_list& words, language language)
{
    // Write an index into eleven bits (0..2047).
    const auto write_index = [](ostream_bit_writer& writer, int32_t index)
    {
        writer.write_bits(static_cast<size_t>(index), index_bits);
    };

    // Reserve buffer to include entropy and checksum, always one byte.
    data_chunk buffer;
    buffer.reserve(entropy_size(words) + 1u);
    const auto indexes = dictionaries_.index(words, language);

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

std::string mnemonic::normalize(const std::string& text)
{
    return to_normal_nfkd_form(text);
}

string_list mnemonic::normalize(const string_list& words)
{
    return system::split(normalize(system::join(words)));
}

// BIP39 requires japanese mnemonic sentences join by an ideographic space.
std::string mnemonic::join(const string_list& words, language language)
{
    return system::join(words, language == language::ja ?
        ideographic_space : ascii_space);
}

string_list mnemonic::split(const std::string& sentence, language language)
{
    return language == language::ja ?
        split_regex(sentence, ideographic_space) :
        system::split(sentence, ascii_space);
}

// public static
// ----------------------------------------------------------------------------

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

bool mnemonic::is_valid_dictionary(language language)
{
    return dictionaries_.exists(language);
}

data_chunk mnemonic::to_seed(const string_list& words,
    const std::string& passphrase)
{
    const auto sentence = to_chunk(normalize(system::join(words)));
    const auto salt = to_chunk(passphrase_prefix + normalize(passphrase));
    const auto seed = pkcs5_pbkdf2_hmac_sha512(sentence, salt, hmac_iterations);
    return to_chunk(seed);
}

// construction
// ----------------------------------------------------------------------------

mnemonic::mnemonic()
  : entropy_(), words_(), language_(language::none)
{
}

mnemonic::mnemonic(const mnemonic& other)
  : entropy_(other.entropy_), words_(other.words_), language_(other.language_)
{
}

mnemonic::mnemonic(const std::string& sentence, language language)
  : mnemonic(split(sentence, language), language)
{
}

mnemonic::mnemonic(const string_list& words, language language)
  : mnemonic(from_words(words, language))
{
}

mnemonic::mnemonic(const data_chunk& entropy, language language)
  : mnemonic(from_entropy(entropy, language))
{
}

// protected
mnemonic::mnemonic(const data_chunk& entropy, const string_list& words,
    language language)
  : entropy_(entropy), words_(words), language_(language)
{
}

mnemonic mnemonic::from_entropy(const data_chunk& entropy, language language)
{
    if (!is_valid_entropy_size(entropy.size()))
        return {};

    if (!dictionaries_.exists(language))
        return {};

    // Save original entropy and derived words.
    return { entropy, encode(entropy, language), language };
}

mnemonic mnemonic::from_words(const string_list& words, language language)
{
    if (!is_valid_word_count(words.size()))
        return {};

    const auto tokens = normalize(words);
    const auto lexicon = dictionaries_.contains(words, language);

    if (lexicon == language::none)
        return {};

    if (language != language::none && lexicon != language)
        return {};

    const auto entropy = decode(tokens, language);

    // Checksum verification failed.
    if (entropy.empty())
        return {};

    // Save normalized words and derived entropy, original words are discarded.
    return { entropy, tokens, lexicon };
}

// public methods
// ----------------------------------------------------------------------------

std::string mnemonic::sentence() const
{
    return join(words(), language_);
}

const data_chunk& mnemonic::entropy() const
{
    return entropy_;
}

const string_list& mnemonic::words() const
{
    return words_;
}

language mnemonic::lingo() const
{
    return language_;
}

data_chunk mnemonic::to_seed(const std::string& passphrase) const
{
    // Words are generated from seed, so always from a supported dictionary.
    return to_seed(words(), passphrase);
}

// Operators.
// ----------------------------------------------------------------------------

mnemonic& mnemonic::operator=(const mnemonic& other)
{
    entropy_ = other.entropy_;
    words_ = other.words_;
    language_ = other.language_;
    return *this;
}

bool mnemonic::operator<(const mnemonic& other) const
{
    return sentence() < other.sentence();
}

bool mnemonic::operator==(const mnemonic& other) const
{
    // Words and entropy are equivalent (one is a cache of the other).
    return entropy_ == other.entropy_ && language_ == other.language_;
}

bool mnemonic::operator!=(const mnemonic& other) const
{
    return !(*this == other);
}

std::istream& operator>>(std::istream& in, mnemonic& to)
{
    std::string value;
    in >> value;
    to = mnemonic(value);

    if (!to)
    {
        using namespace boost::program_options;
        BOOST_THROW_EXCEPTION(invalid_option_value(value));
    }

    return in;
}

std::ostream& operator<<(std::ostream& out, const mnemonic& of)
{
    out << of.sentence();
    return out;
}

mnemonic::operator bool() const
{
    return is_valid_entropy_size(entropy_.size());
}

} // namespace wallet
} // namespace system
} // namespace libbitcoin
