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
#include <bitcoin/system/wallet/mnemonics/electrum_v1.hpp>

#include <cstddef>
#include <cstdint>
#include <string>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/data/string.hpp>
#include <bitcoin/system/iostream/iostream.hpp>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/wallet/context.hpp>
#include <bitcoin/system/wallet/keys/ec_private.hpp>
#include <bitcoin/system/wallet/keys/hd_private.hpp>
#include <bitcoin/system/wallet/mnemonics/language.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

// local constants
// ----------------------------------------------------------------------------

constexpr size_t stretch_iterations = 100000;
constexpr auto size0 = uint32_t{ 1 };
constexpr auto size1 = static_cast<uint32_t>(electrum_v1::dictionary::size());
constexpr auto size2 = size1 * size1;

// private static
// ----------------------------------------------------------------------------

const electrum_v1::dictionaries electrum_v1::dictionaries_
{
    {
        electrum_v1::dictionary{ language::en, electrum_v1::en },
        electrum_v1::dictionary{ language::pt, electrum_v1::pt }
    }
};

// protected static (coders)
// ----------------------------------------------------------------------------

// electrum/old_mnemonic.py#L1669
string_list electrum_v1::encoder(const data_chunk& entropy, language identifier)
{
    string_list words;
    words.reserve(word_count(entropy));
    data_source source(entropy);
    istream_reader reader(source);

    // Entropy size and dictionary existence must have been validated.
    while (!reader.is_exhausted())
    {
        const auto value = reader.read_4_bytes_little_endian();

        // Pos quotient integer div/mod is floor in c++ and python[2/3].
        const auto one = (value / size0 + 0x0) % size1;
        const auto two = (value / size1 + one) % size1;
        const auto tri = (value / size2 + two) % size1;

        words.push_back(dictionaries_.at(one, identifier));
        words.push_back(dictionaries_.at(two, identifier));
        words.push_back(dictionaries_.at(tri, identifier));
    }

    return words;
}

// electrum/old_mnemonic.py#L1682
data_chunk electrum_v1::decoder(const string_list& words, language identifier)
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

        // Neg quotient integer div/mod is ceil in c++ and floor in python[2/3].
        const auto value =
            floored_modulo(one - 0x0, size1) * size0 +
            floored_modulo(two - one, size1) * size1 +
            floored_modulo(tri - two, size1) * size2;

        // Floored modulo with positive divisor is always positive.
        // Bound: max value is word1[size-1], word2[0], word3[size-1] (safe).
        writer.write_4_bytes_little_endian(static_cast<uint32_t>(value));
    }

    sink.flush();
    return entropy;
}

hash_digest electrum_v1::strecher(const data_chunk& entropy)
{
    auto streched = entropy;
    for (size_t i = 0; i < stretch_iterations; ++i)
        streched = sha256_hash_chunk(build_chunk({ streched, entropy }));

    return to_array<hash_size>(streched);
}

// protected static (sizers)
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

// public static
// ----------------------------------------------------------------------------

language electrum_v1::contained_by(const string_list& words,
    language identifier)
{
    return dictionaries_.contains(words, identifier);
}

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
  : languages()
{
}

electrum_v1::electrum_v1(const electrum_v1& other)
  : languages(other)
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
  : languages(entropy, words, identifier)
{
}

// protected static (factories)
// ----------------------------------------------------------------------------

electrum_v1 electrum_v1::from_entropy(const data_chunk& entropy,
    language identifier) const
{
    if (!is_valid_entropy_size(entropy.size()))
        return {};

    if (!dictionaries_.exists(identifier))
        return {};

    // Save entropy and derived words.
    return { entropy, encoder(entropy, identifier), identifier };
}

electrum_v1 electrum_v1::from_words(const string_list& words,
    language identifier) const
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

    // Save derived entropy and dictionary words, originals are discarded.
    return { decoder(tokens, lexicon), tokens, lexicon };
}

// public methods
// ----------------------------------------------------------------------------

ec_private electrum_v1::to_seed(const context& context) const
{
    if (!(*this))
        return {};

    return { ec_scalar{ strecher(entropy_) } };
}

hd_private electrum_v1::to_key(const context& context) const
{
    if (!(*this))
        return {};

    // The key will be false if the secret does not ec verify.
    return { to_chunk(to_seed().secret()), context.hd_prefixes };
}

} // namespace wallet
} // namespace system
} // namespace libbitcoin
