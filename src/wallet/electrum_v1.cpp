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
#include <bitcoin/system/wallet/electrum_v1.hpp>

#include <cstdint>
#include <cstddef>
#include <string>
#include <boost/program_options.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/unicode/unicode.hpp>
#include <bitcoin/system/utility/container_sink.hpp>
#include <bitcoin/system/utility/container_source.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/utility/istream_reader.hpp>
#include <bitcoin/system/utility/ostream_writer.hpp>
#include <bitcoin/system/utility/string.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

// local constants
// ----------------------------------------------------------------------------

constexpr auto size0 = uint32_t{ 1u };
constexpr auto size1 = static_cast<int32_t>(electrum_v1::dictionary::size());
constexpr auto size2 = size1 * size1;
constexpr auto ideographic_space = "\xe3\x80\x80";
constexpr auto ascii_space = "\x20";

// private static
// ----------------------------------------------------------------------------

const electrum_v1::dictionaries electrum_v1::dictionaries_
{
    {
        electrum_v1::dictionary{ language::en, electrum_v1_en },
        electrum_v1::dictionary{ language::pt, electrum_v1_pt }
    }
};

// github.com/spesmilo/electrum/blob/1d8b1ef69897ccb94f337a10993ca5d2b7a46741/electrum/old_mnemonic.py#L1669
string_list electrum_v1::encode(const data_chunk& entropy, language identifier)
{
    string_list words;
    words.reserve(word_count(entropy));
    data_source source(entropy);
    istream_reader reader(source);

    // Entropy size and dictionary existence must have been validated.
    while (!reader.is_exhausted())
    {
        // TODO: verify endianness.
        const auto value = reader.read_4_bytes_little_endian();

        const auto one = (value / size0 + 0x0) % size1;
        const auto two = (value / size1 + one) % size1;
        const auto tri = (value / size2 + two) % size1;

        words.push_back(dictionaries_.at(one, identifier));
        words.push_back(dictionaries_.at(two, identifier));
        words.push_back(dictionaries_.at(tri, identifier));
    }

    return words;
}

// github.com/spesmilo/electrum/blob/1d8b1ef69897ccb94f337a10993ca5d2b7a46741/electrum/old_mnemonic.py#L1682
data_chunk electrum_v1::decode(const string_list& words, language identifier)
{
    data_chunk entropy;
    entropy.reserve(entropy_size(words));
    data_sink sink(entropy);
    ostream_writer writer(sink);

    // Word count and dictionary membership must have been validated.
    for (auto word = words.begin(); word != words.end();)
    {
        const auto one = dictionaries_.index(*word++, identifier);
        const auto two = dictionaries_.index(*word++, identifier);
        const auto tri = dictionaries_.index(*word++, identifier);

        const auto value =
            floored_modulo(one - 0x0, size1) * size0 +
            floored_modulo(two - one, size1) * size1 +
            floored_modulo(tri - two, size1) * size2;

        // TODO: verify endianness.
        // Bound: max value is word1[size-1], word2[0], word3[size-1] (safe).
        writer.write_4_bytes_little_endian(static_cast<uint32_t>(value));
    }

    sink.flush();
    return entropy;
}

// protected static
// ----------------------------------------------------------------------------

size_t electrum_v1::entropy_bits(const data_slice& entropy)
{
    return entropy.size() * byte_bits;
}

size_t electrum_v1::entropy_bits(const string_list& words)
{
    return (words.size() * sizeof(uint32_t)) / word_multiple;
}

size_t electrum_v1::entropy_size(const string_list& words)
{
    return entropy_bits(words) / byte_bits;
}

size_t electrum_v1::word_count(const data_slice& entropy)
{
    return entropy_bits(entropy) / sizeof(uint32_t);
}

std::string electrum_v1::normalize(const std::string& text)
{
    return system::join(system::split(text));
}

string_list electrum_v1::normalize(const string_list& words)
{
    return system::split(system::join(words));
}

std::string electrum_v1::join(const string_list& words, language identifier)
{
    return system::join(words, identifier == language::ja ?
        ideographic_space : ascii_space);
}

string_list electrum_v1::split(const std::string& sentence, language identifier)
{
    return identifier == language::ja ?
        split_regex(sentence, ideographic_space) :
        system::split(sentence, ascii_space);
}

// public static
// ----------------------------------------------------------------------------

bool electrum_v1::is_valid_dictionary(language identifier)
{
    return dictionaries_.exists(identifier);
}

bool electrum_v1::is_valid_entropy_size(size_t size)
{
    return size == entropy_minimum || size == entropy_maximum;
}

bool electrum_v1::is_valid_word_count(size_t count)
{
    return count == word_minimum || count == word_maximum;
}

// construction
// ----------------------------------------------------------------------------

electrum_v1::electrum_v1()
  : entropy_(), words_(), identifier_(language::none)
{
}

electrum_v1::electrum_v1(const electrum_v1& other)
  : entropy_(other.entropy_), words_(other.words_),
    identifier_(other.identifier_)
{
}

electrum_v1::electrum_v1(const std::string& sentence, language identifier)
  : electrum_v1(split(sentence, identifier), identifier)
{
}

electrum_v1::electrum_v1(const string_list& words, language identifier)
  : electrum_v1(from_words(words, identifier))
{
}

electrum_v1::electrum_v1(const data_chunk& entropy, language identifier)
  : electrum_v1(from_entropy(entropy, identifier))
{
}

electrum_v1::electrum_v1(const minimum_entropy& entropy, language identifier)
  : electrum_v1(from_entropy(to_chunk(entropy), identifier))
{
}

electrum_v1::electrum_v1(const maximum_entropy& entropy, language identifier)
  : electrum_v1(from_entropy(to_chunk(entropy), identifier))
{
}

// protected
electrum_v1::electrum_v1(const data_chunk& entropy, const string_list& words,
    language identifier)
  : entropy_(entropy), words_(words), identifier_(identifier)
{
}

// private
electrum_v1 electrum_v1::from_entropy(const data_chunk& entropy,
    language identifier)
{
    if (!is_valid_entropy_size(entropy.size()))
        return {};

    if (!dictionaries_.exists(identifier))
        return {};

    // Save original entropy and derived words.
    return encode(entropy, identifier);
}

// private
electrum_v1 electrum_v1::from_words(const string_list& words,
    language identifier)
{
    if (!is_valid_word_count(words.size()))
        return {};

    const auto tokens = normalize(words);
    const auto lexicon = dictionaries_.contains(tokens, identifier);

    if (lexicon == language::none)
        return {};

    if (identifier != language::none && lexicon != identifier)
        return {};

    // Save normalized words and derived entropy, original words are discarded.
    return decode(tokens, lexicon);
}

// public methods
// ----------------------------------------------------------------------------

std::string electrum_v1::sentence() const
{
    return join(words(), lingo());
}

const data_chunk& electrum_v1::entropy() const
{
    return entropy_;
}

const string_list& electrum_v1::words() const
{
    return words_;
}

language electrum_v1::lingo() const
{
    return identifier_;
}

// operators
// ----------------------------------------------------------------------------

electrum_v1& electrum_v1::operator=(const electrum_v1& other)
{
    entropy_ = other.entropy_;
    words_ = other.words_;
    identifier_ = other.identifier_;
    return *this;
}

bool electrum_v1::operator<(const electrum_v1& other) const
{
    return sentence() < other.sentence();
}

bool electrum_v1::operator==(const electrum_v1& other) const
{
    // Words and entropy are equivalent (one is a cache of the other).
    return entropy_ == other.entropy_ && identifier_ == other.identifier_;
}

bool electrum_v1::operator!=(const electrum_v1& other) const
{
    return !(*this == other);
}

std::istream& operator>>(std::istream& in, electrum_v1& to)
{
    std::string value;
    in >> value;
    to = electrum_v1(value);

    if (!to)
    {
        using namespace boost::program_options;
        BOOST_THROW_EXCEPTION(invalid_option_value(value));
    }

    return in;
}

std::ostream& operator<<(std::ostream& out, const electrum_v1& of)
{
    out << of.sentence();
    return out;
}

electrum_v1::operator bool() const
{
    return !entropy_.empty();
}

} // namespace wallet
} // namespace system
} // namespace libbitcoin
