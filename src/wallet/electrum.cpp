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
#include <boost/program_options.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/unicode/unicode.hpp>
#include <bitcoin/system/utility/assert.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/wallet/electrum_dictionary.hpp>

#include <bitcoin/system/unicode/unicode_ostream.hpp>
#include <bitcoin/system/utility/binary.hpp>
#include <bitcoin/system/utility/collection.hpp>
#include <bitcoin/system/utility/string.hpp>
#include <bitcoin/system/utility/container_sink.hpp>
#include <bitcoin/system/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

// public constants
// ----------------------------------------------------------------------------

// Supports 128 or 256 bits of entropy.
const size_t electrum::entropy_multiple = 4;
const size_t electrum::entropy_size_small = 4u * entropy_multiple;
const size_t electrum::entropy_size_large = 8u * entropy_multiple;

// Supports 12 or 24 words (128 or 256 bits) of entropy.
const size_t electrum::word_multiple = 3;
const size_t electrum::word_count_small = 4u * word_multiple;
const size_t electrum::word_count_large = 28u * word_multiple;

// TODO?
// BIP39 requires japanese mnemonic sentences join by an ideographic space.
const std::string electrum::ideographic_space = "\xe3\x80\x80";

// local constants
// ----------------------------------------------------------------------------

// Seed prefixes.
static const auto seed_prefix_empty = "";
static const auto seed_prefix_standard = "01";
static const auto seed_prefix_witness = "100";
static const auto seed_prefix_two_factor_authentication = "101";
static const auto seed_prefix_two_factor_authentication_witness = "102";

constexpr size_t hmac_iterations = 2048;
static const std::string seed_version = "Seed version";
static const auto passphrase_prefix = "electrum";
static const auto ascii_space = "\x20";

// 3 words are indexed by 32 bits.
static const size_t index_bits = sizeof(uint32_t);

// static protected
// ----------------------------------------------------------------------------

std::string electrum::join(const string_list& words, reference lexicon)
{
    return system::join(words, lexicon == reference::ja ?
        ideographic_space : ascii_space);
}

string_list electrum::split(const std::string& sentence, reference lexicon)
{
    return lexicon == reference::ja ?
        split_regex(sentence, ideographic_space) :
        system::split(sentence, ascii_space);
}

#ifdef WITH_ICU

// github.com/spesmilo/electrum/blob/master/electrum/mnemonic.py#L77
std::string electrum::normalize_text(const std::string& text)
{
    auto seed = to_normal_nfkd_form(text);
    seed = to_lower(seed);
    seed = to_unaccented_form(seed);
    seed = system::join(system::split(seed));
    return to_compressed_cjk_form(seed);
}

// Some of these are trivial, but included for symmetry.

size_t electrum::entropy_bits(const data_slice& entropy)
{
    return entropy_size(entropy) * byte_bits;
}

size_t electrum::entropy_bits(const string_list& words)
{
    // 12 words = 16 bytes, 128 bits (3 words = 32 bits).
    // 24 words = 32 bytes, 256 bits (3 words = 32 bits).
    return word_count(words) * index_bits / word_multiple;
}

size_t electrum::entropy_size(const data_slice& entropy)
{
    return entropy.size();
}

size_t electrum::entropy_size(const string_list& words)
{
    return entropy_bits(words) / byte_bits;
}

size_t electrum::word_count(const data_slice& entropy)
{
    return entropy_bits(entropy) / index_bits;
}

size_t electrum::word_count(const string_list& words)
{
    return words.size();
}

#endif

// public
// ----------------------------------------------------------------------------

bool electrum::is_valid_entropy_size(size_t size)
{
    return size == entropy_size_small || size == entropy_size_large;
}

bool electrum::is_valid_word_count(size_t count)
{
    return count == word_count_small || count == word_count_large;
}

bool electrum::is_version1(const string_list& words, reference lexicon)
{
    return !from_words_v1(words, lexicon).empty();
}

bool electrum::is_version2(const string_list& words, seed_prefix prefix,
    reference lexicon)
{
    // Note that this check is not in Electrum, but it only ensures we *create*
    // word lists of the full/expected size, which we call "valid".
    if (!electrum::is_valid_word_count(words.size()))
        return false;

    static const auto hmac_data = to_chunk(seed_version);
    const auto predicate = to_prefix(prefix);

    const auto sentence = to_chunk(normalize_text(system::join(words)));
    const auto seed = encode_base16(hmac_sha512_hash(sentence, hmac_data));
    return seed.substr(0, predicate.length()) == predicate;
}

const dictionary& electrum::to_dictionary(reference lexicon)
{
    switch (lexicon)
    {
        case reference::es:
            return wallet::language::es;
            return wallet::language::ja;
        case reference::zh_Hans:
            return wallet::language::zh_Hans;

        // English is the default for 'none'.
        case reference::en:
        case reference::none:
        default:
            return wallet::language::en;
    }
}

reference electrum::to_reference(const string_list& words)
{
    for (const dictionary& dictionary: language::electrum::all)
    {
        const auto in_dictionary = [&](const std::string& word)
        {
            return contains(dictionary.words, word);
        };

        if (std::all_of(words.begin(), words.end(), in_dictionary))
            return dictionary.name;
    }

    return reference::none;
}

std::string electrum::to_prefix(seed_prefix prefix)
{
    switch (prefix)
    {
        case seed_prefix::standard:
            return seed_prefix_standard;
        case seed_prefix::witness:
            return seed_prefix_witness;
        case seed_prefix::two_factor_authentication:
            return seed_prefix_two_factor_authentication;
        case seed_prefix::two_factor_authentication_witness:
            return seed_prefix_two_factor_authentication_witness;
        case seed_prefix::empty:
        default:
            return seed_prefix_empty;
    }
}

#ifdef WITH_ICU

// DOES normalize the words and passphrase.
// DOES NOT ensure the words are in any dictionary.
data_chunk electrum::to_seed(const string_list& words,
    const std::string& passphrase)
{
    const auto sentence = to_chunk(normalize_text(system::join(words)));
    const auto salt = to_chunk(passphrase_prefix + normalize_text(passphrase));
    const auto seed = pkcs5_pbkdf2_hmac_sha512(sentence, salt, hmac_iterations);
    return to_chunk(seed);
}

#endif

// electrum
// ----------------------------------------------------------------------------

electrum::electrum()
  : entropy_(), words_(), lexicon_(reference::none)
{
}

electrum::electrum(const electrum& other)
  : entropy_(other.entropy_), words_(other.words_), lexicon_(other.lexicon_)
{
}

electrum::electrum(const std::string& sentence, reference lexicon)
  : electrum(split(sentence, lexicon), lexicon)
{
}

// TODO: can we accept from_words_v1 here as well?
electrum::electrum(const string_list& words, reference lexicon)
  : electrum(from_words_v2(words, lexicon))
{
}

electrum::electrum(const data_chunk& entropy, seed_prefix prefix,
    reference lexicon)
  : electrum(from_entropy(entropy, prefix, lexicon))
{
}

// protected
electrum::electrum(const data_chunk& entropy, const string_list& words,
    reference lexicon)
  : entropy_(entropy), words_(words), lexicon_(lexicon)
{
}

// Factories (private).
// ----------------------------------------------------------------------------

electrum electrum::from_entropy(const data_slice& entropy,
    seed_prefix prefix, reference lexicon)
{
    // Valid entropy values (16 or 32 bytes).
    if (!is_valid_entropy_size(entropy.size()))
        return {};

    string_list words;
    const auto& dictionary = to_dictionary(lexicon);

    const auto encode = [&](size_t buffer)
    {
        string_list words;
        words.reserve(word_count(entropy));

        // The final numeric entropy must round trip, not necessarily the entropy.
        while (buffer != 0)
        {
            const auto index = buffer % dictionary_size;
            words.push_back(dictionary.words[index]);
            buffer /= dictionary_size;
        }

        return words;
    };

    // TODO: starts at entropy and ends at zero.
    ////data_chunk buffer{ entropy.begin(), entropy.end() };

    // Repeat and increment buffer (entropy) until not v1 and is v2.
    // This just grinds away until not v1 and the v2 prefix matches.
    for (size_t buffer = 0xffffffff; buffer != 0; ++buffer)
    {
        words = encode(buffer);

        // TODO: ensure we always get the right number of words?!
        if (!is_version1(words, lexicon) && is_version2(words, prefix, lexicon))
            break;
    }

    // TODO: translate entropy before final increment (MAY NOT BE WHAT WAS GIVEN!).
    // The original entropy will return the same wordlist, so could keep it.
    // But the entropy that produces the words is what is expected to round trip.
    return { { entropy.begin(), entropy.end() }, words, lexicon };
}

electrum electrum::from_words_v2(const string_list& words, reference lexicon)
{
    // Valid word counts (12 or 24 words).
    if (!is_valid_word_count(words.size()))
        return {};

    // Locate the one dictionary of all given words.
    const auto words_lexicon = to_reference(words);

    // Words must be from a supported dictionary and match specifaction.
    if (words_lexicon == reference::none ||
        (words_lexicon != lexicon && lexicon != reference::none))
        return {};

    // Words are verified to be from a supported dictionary.
    const auto& dictionary = to_dictionary(words_lexicon);

    // TODO: starts at zero and ends after all words processed.
    data_chunk entropy;
    ////entropy.reserve(entropy_size(words));
    size_t buffer = 0;

    // The final numeric entropy must round trip, not necessarily the entropy.
    for (const auto& word: boost::adaptors::reverse(words))
    {
        buffer = (buffer * dictionary_size) +
            find_position(dictionary.words, word);
    }

    // TODO: translate entropy derived from words.
    return { entropy, words, words_lexicon };
}

// TODO: convert to base for this class.
// We can convert an old mnemonic to entropy and could reverse this algorithm.
data_chunk electrum::from_words_v1(const string_list& words, reference lexicon)
{
    if (!electrum::is_valid_word_count(words.size()))
        return {};

    if (lexicon != reference::en)
        return {};

    // Locate the one dictionary of all given words.
    const auto words_lexicon = to_reference(words);

    // Words must be from a supported dictionary and match specifaction.
    if (words_lexicon != reference::en)
        return {};

    const auto size = dictionary_size_v1;
    const auto& dictionary = language::electrum::en_v1;

    const auto modulo = [&](int32_t index_distance)
    {
        return index_distance < 0 ?
            size - (-index_distance % size) :
            index_distance % size;
    };

    data_chunk entropy(entropy_size(words));
    data_sink sink(entropy);
    ostream_writer writer(sink);

    // TODO: review the loop termination change.
    for (size_t index = 0; index < word_count(words);
        index += word_multiple)
    {
        const auto one = find_position(dictionary, words[index + 0u]);
        const auto two = find_position(dictionary, words[index + 1u]);
        const auto tri = find_position(dictionary, words[index + 2u]);

        const auto value = static_cast<uint32_t>(
            one +
            size * modulo(two - one) + 
            size * size * modulo(tri - two));

        // The maximal value is achived by word1[1625], word2[0], word3[1625].
        // This cannot overflow int32_t or the cast to uint32_t.
        ////constexpr uint64_t bound =
        ////    (1u    *                           1625u) +
        ////    (1626u *          (1626u - (-(0u - 1625u)) % 1626u)) +
        ////    (1626u * 1626u *            ((1625u - 0u)  % 1626u));
        ////static_assert(bound <= 0x7fffffff, "overflow");

        writer.write_variable_little_endian(value);
    }

    sink.flush();
    return entropy;
}

// Cast operators.
// ----------------------------------------------------------------------------

electrum::operator bool() const
{
    return is_valid_entropy_size(entropy_.size());
}

// Serializer.
// ----------------------------------------------------------------------------

std::string electrum::sentence() const
{
    return join(words(), lexicon_);
}

// Accessors.
// ----------------------------------------------------------------------------

const data_chunk& electrum::entropy() const
{
    return entropy_;
}

const string_list& electrum::words() const
{
    return words_;
}

reference electrum::lexicon() const
{
    return lexicon_;
}

// Methods.
// ----------------------------------------------------------------------------

#ifdef WITH_ICU

data_chunk electrum::to_seed(const std::string& passphrase) const
{
    // Words are generated from seed, so always from a supported dictionary.
    return to_seed(words(), passphrase);
}

#endif

// Operators.
// ----------------------------------------------------------------------------

electrum& electrum::operator=(const electrum& other)
{
    entropy_ = other.entropy_;
    words_ = other.words_;
    lexicon_ = other.lexicon_;
    return *this;
}

bool electrum::operator<(const electrum& other) const
{
    return sentence() < other.sentence();
}

bool electrum::operator==(const electrum& other) const
{
    // Words and entropy are equivalent (one is a cache of the other).
    return entropy_ == other.entropy_ && lexicon_ == other.lexicon_;
}

bool electrum::operator!=(const electrum& other) const
{
    return !(*this == other);
}

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

std::ostream& operator<<(std::ostream& out, const electrum& of)
{
    out << of.sentence();
    return out;
}

} // namespace wallet
} // namespace system
} // namespace libbitcoin
