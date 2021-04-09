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
#include <bitcoin/system/wallet/mnemonic.hpp>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/checksum.hpp>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/unicode/unicode.hpp>
#include <bitcoin/system/utility/assert.hpp>
#include <bitcoin/system/utility/binary.hpp>
#include <bitcoin/system/utility/collection.hpp>
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

// Words are encoded in 11 bits and therefore are not byte aligned.
// As a consequence bit ordering matters. Bits are serialized to entropy bytes
// in big-endian order. This can be observed in the reference implementation:
// github.com/trezor/python-mnemonic/blob/master/mnemonic/mnemonic.py#L152-L162

// public constants
// ----------------------------------------------------------------------------

// Supports 128 to 256 bits of entropy.
const size_t mnemonic::entropy_multiple = 4;
const size_t mnemonic::entropy_minimum = 4u * entropy_multiple;
const size_t mnemonic::entropy_maximum = 8u * entropy_multiple;

// Supports 12 to 24 words (128 to 256 bits) of entropy.
const size_t mnemonic::word_multiple = 3;
const size_t mnemonic::word_minimum = 4u * word_multiple;
const size_t mnemonic::word_maximum = 8u * word_multiple;

// BIP39 requires japanese mnemonic sentences join by an ideographic space.
const std::string mnemonic::ideographic_space = "\xe3\x80\x80";

// local constants
// ----------------------------------------------------------------------------

static constexpr size_t hmac_iterations = 2048;
static const auto passphrase_prefix = "mnemonic";
static const auto ascii_space = "\x20";

// 2^11 = 2048 implies 11 bits exactly indexes every possible dictionary word.
static const size_t index_bits = system::log2(dictionary_size);

// static protected
// ----------------------------------------------------------------------------

std::string mnemonic::join(const string_list& words, reference lexicon)
{
    return system::join(words, lexicon == reference::ja ?
        ideographic_space : ascii_space);
}

string_list mnemonic::split(const std::string& sentence, reference lexicon)
{
    return lexicon == reference::ja ?
        split_regex(sentence, ideographic_space) :
        system::split(sentence, ascii_space);
}

uint8_t mnemonic::checksum_byte(const data_slice& entropy)
{
    // The high order bits of the first sha256_hash byte are the checksum.
    // Only 4, 5, 6, 7, or 8 bits of the hash are used (based on size).
    const auto mask_bits = byte_bits - checksum_bits(entropy);
    const auto checksum_mask = max_uint8 << mask_bits;
    return sha256_hash(entropy).front() & checksum_mask;
}

// Some of these are trivial, but included for symmetry.

size_t mnemonic::checksum_bits(const data_slice& entropy)
{
    return entropy_size(entropy) / entropy_multiple;
}

size_t mnemonic::checksum_bits(const string_list& words)
{
    return word_count(words) / word_multiple;
}

size_t mnemonic::entropy_bits(const data_slice& entropy)
{
    return entropy_size(entropy) * byte_bits;
}

size_t mnemonic::entropy_bits(const string_list& words)
{
    return word_count(words) * index_bits - checksum_bits(words);
}

size_t mnemonic::entropy_size(const data_slice& entropy)
{
    return entropy.size();
}

size_t mnemonic::entropy_size(const string_list& words)
{
    return entropy_bits(words) / byte_bits;
}

size_t mnemonic::word_count(const data_slice& entropy)
{
    return (entropy_bits(entropy) + checksum_bits(entropy)) / index_bits;
}

size_t mnemonic::word_count(const string_list& words)
{
    return words.size();
}

// public
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

const dictionary& mnemonic::to_dictionary(reference lexicon)
{
    switch (lexicon)
    {
        case reference::es:
            return wallet::language::es;
        case reference::it:
            return wallet::language::it;
        case reference::fr:
            return wallet::language::fr;
        case reference::cs:
            return wallet::language::cs;
        case reference::pt:
            return wallet::language::pt;
        case reference::ja:
            return wallet::language::ja;
        case reference::ko:
            return wallet::language::ko;
        case reference::zh_Hans:
            return wallet::language::zh_Hans;
        case reference::zh_Hant:
            return wallet::language::zh_Hant;

        // English is the default for 'none'.
        case reference::en:
        case reference::none:
        default:
            return wallet::language::en;
    }
}

// BIP39 expects prenormalized dictionaries.
// The binary search presumes that all dictionaries are lexically sorted.
reference mnemonic::to_reference(const string_list& words, reference lexicon)
{
    const auto contained = [&](const dictionary& dictionary)
    {
        const auto in_dictionary = [&](const std::string& word)
        {
            return contains(dictionary.words, word);
        };

        return std::all_of(words.begin(), words.end(), in_dictionary);
    };

    if (lexicon != reference::none)
        return contained(to_dictionary(lexicon)) ? lexicon : reference::none;

    for (const dictionary& dictionary: language::all)
        if (contained(dictionary))
            return dictionary.name;

    return reference::none;
}

#ifdef WITH_ICU

std::string mnemonic::normalize(const std::string& text)
{
    return to_normal_nfkd_form(text);
}

// DOES normalize the words and passphrase.
// DOES NOT ensure the words are in any dictionary.
data_chunk mnemonic::to_seed(const string_list& words,
    const std::string& passphrase)
{
    const auto sentence = to_chunk(normalize(system::join(words)));
    const auto salt = to_chunk(passphrase_prefix + normalize(passphrase));
    const auto seed = pkcs5_pbkdf2_hmac_sha512(sentence, salt, hmac_iterations);
    return to_chunk(seed);
}

#endif // WITH_ICU

// mnemonic
// ----------------------------------------------------------------------------

mnemonic::mnemonic()
  : entropy_(), words_(), lexicon_(reference::none)
{
}

mnemonic::mnemonic(const mnemonic& other)
  : entropy_(other.entropy_), words_(other.words_), lexicon_(other.lexicon_)
{
}

mnemonic::mnemonic(const std::string& sentence, reference lexicon)
  : mnemonic(split(sentence, lexicon), lexicon)
{
}

mnemonic::mnemonic(const string_list& words, reference lexicon)
  : mnemonic(from_words(words, lexicon))
{
}

mnemonic::mnemonic(const data_chunk& entropy, reference lexicon)
  : mnemonic(from_entropy(entropy, lexicon))
{
}

// protected
mnemonic::mnemonic(const data_chunk& entropy, const string_list& words,
    reference lexicon)
  : entropy_(entropy), words_(words), lexicon_(lexicon)
{
}

// Factories (private).
// ----------------------------------------------------------------------------

// DOES NOT normalize the mapped dictionary words.
mnemonic mnemonic::from_entropy(const data_chunk& entropy, reference lexicon)
{
    // Valid entropy values (16, 20, 24, 28, or 32 bytes).
    if (!is_valid_entropy_size(entropy.size()))
        return {};

    // Read eleven bits into an index (0..2047).
    const auto read_index = [](istream_bit_reader& reader)
    {
        // Guard: 11 bits cannot exceed size_t and 11 does not exceed uint8_t.
        return static_cast<size_t>(reader.read_bits(
            static_cast<uint8_t>(index_bits)));
    };

    string_list words(word_count(entropy));
    const auto& dictionary = to_dictionary(lexicon);
    const auto buffer = build_chunk(
    {
        entropy,
        data_chunk{ checksum_byte(entropy) }
    });

    // Word indexes are not byte aligned, high-to-low bit reader required.
    data_source source(buffer);
    istream_reader byte_reader(source);
    istream_bit_reader bit_reader(byte_reader);

    // Select words from the dictionary (words.size() * index_bits are read).
    for (auto word = words.begin(); word != words.end(); ++word)
        *word = dictionary.words[read_index(bit_reader)];

    return { entropy, words, lexicon };
}

// DOES NOT normalize the specified words.
// DOES NOT normalize the compared dictionary words.
// DOES NOT support un-validatable word lists (use to_seed).
mnemonic mnemonic::from_words(const string_list& words, reference lexicon)
{
    // Valid word counts (12, 15, 18, 21, or 24 words).
    if (!is_valid_word_count(words.size()))
        return {};

    // Write an index into eleven bits (0..2047).
    const auto write_index = [](ostream_bit_writer& writer, int index)
    {
        // Index must be valid (positive) since the language is validated.
        // Guard: 2047 cannot exceed size_t and 11 does not exceed uint8_t.
        writer.write_bits(static_cast<size_t>(index),
            static_cast<uint8_t>(index_bits));
    };

    // Locate the one dictionary of all given words.
    const auto words_lexicon = to_reference(words, lexicon);

    // Words must be from a supported dictionary and match if specified.
    if (words_lexicon == reference::none)
        return {};

    // Reserve buffer to include entropy and checksum, always one byte.
    data_chunk buffer;
    buffer.reserve(entropy_size(words) + 1u);
    const auto& dictionary = to_dictionary(words_lexicon);

    // Word indexes are not byte aligned, high-to-low bit writer required.
    data_sink sink(buffer);
    ostream_writer byte_writer(sink);
    ostream_bit_writer bit_writer(byte_writer);

    // Convert word indexes to entropy (words.size() * index_bits are written).
    for (const auto& word: words)
        write_index(bit_writer, find_position(dictionary.words, word));

    bit_writer.flush();
    sink.flush();

    const data_chunk entropy{ buffer.begin(), std::prev(buffer.end()) };

    // Checksum is zero-padded in last byte
    if (buffer.back() != checksum_byte(entropy))
        return {};

    // Entropy is always byte aligned.
    return { entropy, words, words_lexicon };
}

// Cast operators.
// ----------------------------------------------------------------------------

mnemonic::operator bool() const
{
    return is_valid_entropy_size(entropy_.size());
}

// Serializer.
// ----------------------------------------------------------------------------

std::string mnemonic::sentence() const
{
    return join(words(), lexicon_);
}

// Accessors.
// ----------------------------------------------------------------------------

const data_chunk& mnemonic::entropy() const
{
    return entropy_;
}

const string_list& mnemonic::words() const
{
    return words_;
}

reference mnemonic::lexicon() const
{
    return lexicon_;
}

// Methods.
// ----------------------------------------------------------------------------

#ifdef WITH_ICU

data_chunk mnemonic::to_seed(const std::string& passphrase) const
{
    // Words are generated from seed, so always from a supported dictionary.
    return to_seed(words(), passphrase);
}

#endif // WITH_ICU

// Operators.
// ----------------------------------------------------------------------------

mnemonic& mnemonic::operator=(const mnemonic& other)
{
    entropy_ = other.entropy_;
    words_ = other.words_;
    lexicon_ = other.lexicon_;
    return *this;
}

bool mnemonic::operator<(const mnemonic& other) const
{
    return sentence() < other.sentence();
}

bool mnemonic::operator==(const mnemonic& other) const
{
    // Words and entropy are equivalent (one is a cache of the other).
    return entropy_ == other.entropy_ && lexicon_ == other.lexicon_;
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

} // namespace wallet
} // namespace system
} // namespace libbitcoin
