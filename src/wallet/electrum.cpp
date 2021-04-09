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

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <string>
#include <vector>
#include <boost/multiprecision/cpp_int/import_export.hpp>
#include <boost/program_options.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/formats/base_16.hpp>
#include <bitcoin/system/math/checksum.hpp>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/unicode/unicode.hpp>
#include <bitcoin/system/utility/assert.hpp>
#include <bitcoin/system/utility/binary.hpp>
#include <bitcoin/system/utility/collection.hpp>
#include <bitcoin/system/utility/container_sink.hpp>
#include <bitcoin/system/utility/container_source.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/utility/istream_reader.hpp>
#include <bitcoin/system/utility/ostream_writer.hpp>
#include <bitcoin/system/wallet/electrum_dictionary.hpp>
#include <bitcoin/system/wallet/mnemonic.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

// public constants
// ----------------------------------------------------------------------------

// V1 supports 128 or 256 bits of entropy.
const size_t electrum::entropy_multiple = 4;
const size_t electrum::entropy_minimum = 4u * entropy_multiple;
const size_t electrum::entropy_maximum = 8u * entropy_multiple;

// V1 supports 12 or 24 words (128 or 256 bits) of entropy.
const size_t electrum::word_multiple = 3;
const size_t electrum::word_minimum = 4u * word_multiple;
const size_t electrum::word_maximum = 8u * word_multiple;

// BIP39 requires japanese mnemonic sentences join by an ideographic space.
const std::string electrum::ideographic_space = "\xe3\x80\x80";

#ifdef WITH_ICU

// local constants
// ----------------------------------------------------------------------------

// Seed prefixes.
static const auto version_old = "";
static const auto version_standard = "01";
static const auto version_witness = "100";
static const auto version_two_factor_authentication = "101";
static const auto version_two_factor_authentication_witness = "102";

static const auto seed_version = to_chunk(std::string("Seed version"));
static const auto passphrase_prefix = "electrum";
static const auto ascii_space = "\x20";
constexpr size_t hmac_iterations = 2048;

// 2^11 = 2048 implies 11 bits exactly indexes every possible dictionary word.
static const size_t index_bits = system::log2(dictionary_size);

// local functions
// ----------------------------------------------------------------------------

// This substr comparison can be replaced by std::string in c++20.
inline bool starts_with(const std::string value, electrum::seed_prefix prefix)
{
    const auto version = electrum::to_version(prefix);
    return value.substr(0, version.length()) == version;
};

// ENDIANNESS: export_bits and import_bits use natural byte order, so these
// must be modified to be endianess-explicit. They should be added to the
// serializer classes as to/from big/little endian template overrides.
// Bytes can be masked and shifted out, and added and shifted in.

// TODO: move to data.hpp.
template <typename CppInt>
static data_chunk to_data(const CppInt& value)
{
    data_chunk data(electrum::entropy_maximum);
    boost::multiprecision::export_bits(value, std::back_inserter(data),
        byte_bits);
    return data;
}

// TODO: move to data.hpp.
template <typename CppInt>
CppInt from_data(const data_slice& data)
{
    CppInt value;
    boost::multiprecision::import_bits(value, data.begin(), data.end());
    return value;
}

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

// Some of these are trivial, but included for symmetry.

size_t electrum::entropy_bits(const data_slice& entropy)
{
    return entropy_size(entropy) * byte_bits;
}

size_t electrum::entropy_bits(const string_list& words)
{
    // Version1 only, 3 words are indexed by 32 bits.
    return word_count(words) * sizeof(uint32_t) / word_multiple;
}

size_t electrum::entropy_size(const data_slice& entropy)
{
    return entropy.size();
}

size_t electrum::entropy_size(const string_list& words)
{
    // Version1 only.
    return entropy_bits(words) / byte_bits;
}

size_t electrum::word_count(const data_slice& entropy)
{
    // Version1 only, 3 words are indexed by 32 bits.
    return entropy_bits(entropy) / sizeof(uint32_t);
}

size_t electrum::word_count(const string_list& words)
{
    return words.size();
}

// public
// ----------------------------------------------------------------------------

bool electrum::is_version1_entropy_size(size_t size)
{
    // Only 16 or 32 bytes.
    return size == entropy_minimum || size == entropy_maximum;
}

bool electrum::is_version1_word_count(size_t count)
{
    // Only 12 or 24 words.
    return count == word_minimum || count == word_maximum;
}

bool electrum::is_version2_entropy_size(size_t size)
{
    // Electrum has no explicit minimum or maximum strengths.
    //
    // Libbitcoin v1 upper bounds (based on integer choice).
    // A 64 byte seed (512 / 11) is 46 words (6 unused bits).
    // This limits strength to 506 bits (BIP39 is 256).
    // The upper limit also guards against uint512_t overflow.
    //
    // Libbitcoin v1 lower bounds (based on safety).
    // A 132 bit (exactly 12 word) seed is the Electrum default.
    // A 128 bit seed is the BIP39 minium, but this 11 Electrum words.
    // So our limits are 132 to 506 bits (12 to 46) words.

    // 17..64
    return ((132u + 7u) / 8u) >= size && size <= ((506u + 7u) / 8u);
}

bool electrum::is_version2_word_count(size_t count)
{
    // Electrum has no explicit minimum or maximum strengths.
    //
    // Libbitcoin v1 upper bounds (based on integer choice).
    // A 46 word seed (46 * 11) is 506 bits.
    // This limits strength to 506 bits (BIP39 is 256).
    // The upper limit also guards against uint512_t overflow.
    //
    // Libbitcoin v1 lower bounds (based on safety).
    // A 132 bit (exactly 12 word) seed is the Electrum default.
    // A 128 bit seed is the BIP39 minium, but this 11 Electrum words.
    // So our limits are 132 to 506 bits (12 to 46) words.

    // 12..46
    return (132u / 11u) >= count && count <= (506u / 11u);
}

bool electrum::is_version1(const string_list& words, reference lexicon)
{
    return !from_words_v1(words, lexicon).empty();
}

// github.com/spesmilo/electrum/blob/master/electrum/mnemonic.py#L77
std::string electrum::normalize(const std::string& text)
{
    auto seed = to_normal_nfkd_form(text);
    seed = to_lower(seed);
    seed = to_unaccented_form(seed);
    seed = system::join(system::split(seed));
    return to_compressed_cjk_form(seed);
}

bool electrum::has_prefix(const string_list& words, seed_prefix prefix)
{
    // In Electrum 2.7, there was a breaking change in key derivation for
    // two_factor_authentication. Unfortunately the seed prefix was reused,
    // and now we can only distinguish them based on number of words. :(
    // github.com/spesmilo/electrum/blob/master/electrum/mnemonic.py#L258
    if (prefix == seed_prefix::two_factor_authentication)
        if (words.size() != 12u && words.size() < 20u)
            return false;

    const auto sentence = to_chunk(normalize(system::join(words)));
    const auto seed = encode_base16(hmac_sha512_hash(sentence, seed_version));
    return starts_with(seed, prefix);
}

// DOES normalize the words and passphrase.
// DOES NOT ensure the seed produces any prefix.
// DOES NOT ensure the words are in any dictionary.
data_chunk electrum::to_seed(const string_list& words,
    const std::string& passphrase)
{
    const auto sentence = to_chunk(normalize(system::join(words)));
    const auto salt = to_chunk(passphrase_prefix + normalize(passphrase));
    const auto seed = pkcs5_pbkdf2_hmac_sha512(sentence, salt, hmac_iterations);
    return to_chunk(seed);
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

// Electrum normalizes the dictionary on read.
// We use the BIP39 technique which expects prenormalized dictionaries.
// The binary search presumes that all dictionaries are lexically sorted.
reference electrum::to_reference(const string_list& words, reference lexicon)
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

    for (const dictionary& dictionary: language::electrum::all)
        if (contained(dictionary))
            return dictionary.name;

    return reference::none;
}

electrum::seed_prefix electrum::to_prefix(const string_list& words)
{
    if (is_version1(words))
        return seed_prefix::old;
    if (has_prefix(words, seed_prefix::standard))
        return seed_prefix::standard;
    if (has_prefix(words, seed_prefix::witness))
        return seed_prefix::witness;
    if (has_prefix(words, seed_prefix::two_factor_authentication))
        return seed_prefix::two_factor_authentication;
    if (has_prefix(words, seed_prefix::two_factor_authentication_witness))
        return seed_prefix::two_factor_authentication_witness;

    return seed_prefix::none;
}

std::string electrum::to_version(seed_prefix prefix)
{
    switch (prefix)
    {
        case seed_prefix::standard:
            return version_standard;
        case seed_prefix::witness:
            return version_witness;
        case seed_prefix::two_factor_authentication:
            return version_two_factor_authentication;
        case seed_prefix::two_factor_authentication_witness:
            return version_two_factor_authentication_witness;
        case seed_prefix::old:
        case seed_prefix::none:
        default:
            return version_old;
    }
}

// electrum
// ----------------------------------------------------------------------------

electrum::electrum()
  : entropy_(), words_(), lexicon_(reference::none), prefix_(seed_prefix::none)
{
}

electrum::electrum(const electrum& other)
  : entropy_(other.entropy_), words_(other.words_), lexicon_(other.lexicon_),
    prefix_(other.prefix_)
{
}

electrum::electrum(const std::string& sentence, reference lexicon)
  : electrum(split(sentence, lexicon), lexicon)
{
}

electrum::electrum(const string_list& words, reference lexicon)
  : electrum(from_words_v2(words, lexicon))
{
}

electrum::electrum(const data_chunk& entropy, seed_prefix prefix,
    reference lexicon)
  : electrum(from_entropy_v2(entropy, prefix, lexicon))
{
}

// protected
electrum::electrum(const data_chunk& entropy, const string_list& words,
    reference lexicon, seed_prefix prefix)
  : entropy_(entropy), words_(words), lexicon_(lexicon), prefix_(prefix)
{
}

// Factories (private).
// ----------------------------------------------------------------------------

// Version 2.

static string_list encode_v2(const dictionary& dictionary, uint512_t value)
{
    string_list words;

    for (; value != 0; value /= dictionary_size)
    {
        const auto position = static_cast<uint32_t>(value % dictionary_size);
        words.push_back(dictionary.words[position]);
    }

    return words;
}

static uint512_t decode_v2(const dictionary& dictionary, const string_list& words)
{
    uint512_t value{ 0 };

    for (const auto& word: boost::adaptors::reverse(words))
        value = value * dictionary_size + find_position(dictionary.words, word);

    return value;
}

// Electrum grinds randoms on a sequential nonce until the entropy is high
// enough to ensure population of words that represent the intended bit
// strength of the seed. But we do not use an internal PRNG and instead
// only accept base entropy from the caller. We want determinism from the
// entropy, and we cannot truly increase randomness. So we maintain the
// intended strength while bounding the value to produce exact word count.
// github.com/spesmilo/electrum/blob/master/electrum/mnemonic.py#L190-L205
static uint512_t bounded_entropy(const data_slice& entropy)
{
    auto value = from_data<uint512_t>(entropy);

    // pow2(index_bits) = dictionary_size.
    // log2(dictionary_size) = index_bits.
    // >> 11 bits (one word) is the same as / 2048.
    // << 11 bits (one word) is the same as * 2048.

    const auto bytes = entropy.size();
    const auto count = (bytes * byte_bits) / index_bits;
    const auto strength = count * index_bits;

    // The minimum acceptable value, to prevent low word count.
    constexpr uint512_t one{ 1 };
    const auto minimum = (one << (strength - index_bits / 2u));

    // A mask of unused entropy, to prevent high word count.
    constexpr uint512_t zero{ 0 };
    const auto mask = (~zero >> (512u - strength));

    // Mask the value to prevent high word count.
    value &= mask;

    // Allows half of the index bits for grinding before reduced word count.
    if (value < minimum)
    {
        // Invert and remask the value (Electrum loops prng on a nonce).
        value = ~value;
        value &= mask;
    }

    // The value will produce the exact word count with no entropy loss.
    return value;
}

electrum electrum::from_entropy_v2(const data_slice& entropy,
    seed_prefix prefix, reference lexicon)
{
    if (!is_version2_entropy_size(entropy.size()))
        return false;

    string_list words;
    auto found = false;
    auto value = bounded_entropy(entropy);
    const auto& dictionary = to_dictionary(lexicon);

    const auto bytes = entropy.size();
    const auto count = (bytes * byte_bits) / index_bits;
    const auto strength = count * index_bits;

    // The minimum value to prevent low word count.
    constexpr uint512_t one{ 1 };
    const auto minimum = (one << (strength - index_bits));

    // This just grinds away until exhausted or prefix found.
    for (; !found && value >= minimum; --value)
    {
        // TODO: create word structure that holds dictionary positions.
        words = encode_v2(dictionary, value);

        // TODO: use word structure that holds dictionary positions.
        // TODO: this will preclude the need for repeat lookups here.
        // Safety check, really just debugging code.
        if (decode_v2(dictionary, words) == 0u)
        {
            BITCOIN_ASSERT_MSG(false, "Electrum seed failed to round trip.");
            return {};
        }

        // Avoid collisions with Electrum v1 and BIP39 mnemonics.
        if (is_version1(words, lexicon) || wallet::mnemonic(words))
            continue;

        found = has_prefix(words, prefix);
    }

    // Very bad luck.
    if (!found)
        return {};

    // The ground entropy produces the specified prefix.
    // The ground entropy must round trip, not necessarily the paramter.
    return { to_data(value), words, lexicon, prefix };
}

electrum electrum::from_words_v2(const string_list& words, reference lexicon)
{
    if (!is_version2_word_count(words.size()))
        return false;

    // TODO: Electrum normalizes words here.

    // TODO: set word indexes.
    // Locate the one dictionary of all given words.
    const auto words_lexicon = to_reference(words, lexicon);

    // Words must be from a supported dictionary and match if specified.
    if (words_lexicon == reference::none)
        return {};

    const auto value = decode_v2(to_dictionary(words_lexicon), words);

    // The provided entropy is not guaranteed to satisfy any prefix.
    // This method allows contstruction with any set of one dictionary words.
    return { to_data(value), words, words_lexicon, to_prefix(words) };
}

// Version 1.
// TODO: convert to base for this class.
// TODO: in from_entropy_v1 Electrum checks for 16 or 32 bytes.
// TODO: dictionary could return an array of indexes to prevent redundant word searches.
// github.com/spesmilo/electrum/blob/1d8b1ef69897ccb94f337a10993ca5d2b7a46741/electrum/old_mnemonic.py#L1669

electrum electrum::from_entropy_v1(const data_chunk& entropy,
    seed_prefix prefix, reference lexicon)
{
    if (!electrum::is_version1_entropy_size(entropy.size()))
        return {};

    if (lexicon != reference::en)
        return {};

    static const auto size = dictionary_size_v1;

    string_list words;
    data_source source(entropy);
    istream_reader reader(source);

    // TODO: write this based on Electrum v1.
    // TODO: then move v1 to a new base class "electrum_v1".
    //// auto word = reader.read_4_bytes_little_endian();

    ////for i in range(len(message)//8):
    ////    word = message[8*i:8*i+8]
    ////    x = int(word, 16)
    ////    w1 = (x%size)
    ////    w2 = ((x//size) + w1)%n
    ////    w3 = ((x//size//size) + w2)%n
    ////    out += [wordlist[w1], wordlist[w2], wordlist[w3]]

    return words;
}

data_chunk electrum::from_words_v1(const string_list& words, reference lexicon)
{
    if (!electrum::is_version1_word_count(words.size()))
        return {};

    if (lexicon != reference::en)
        return {};

    // TODO: Electrum normalizes words here.

     // Locate the one dictionary of all given words.
    const auto words_lexicon = to_reference(words);

    // Words must be from a supported dictionary and match specification.
    if (words_lexicon != reference::en)
        return {};

    // github.com/spesmilo/electrum/blob/1d8b1ef69897ccb94f337a10993ca5d2b7a46741/electrum/old_mnemonic.py#L1682
    const auto decode = [](const string_list& words)
    {
        static const auto size = dictionary_size_v1;
        static const auto& dictionary = language::electrum::en_v1;

        // TODO: review this against Electrum.
        const auto modulo = [&](int32_t index_distance)
        {
            return index_distance < 0 ?
                size - (-index_distance % size) :
                index_distance % size;
        };

        data_chunk entropy(entropy_size(words));
        data_sink sink(entropy);
        ostream_writer writer(sink);

        for (size_t index = 0; index < words.size();
            index += word_multiple)
        {
            // TODO: convert dictionary struct to class.
            // TODO: add public contains and position members.
            // TODO: implement position using binary search algorithm.
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
    };

    return decode(words);
}

// Cast operators.
// ----------------------------------------------------------------------------

electrum::operator bool() const
{
    return !entropy_.empty();
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

electrum::seed_prefix electrum::prefix() const
{
    return prefix_;
}

// Methods.
// ----------------------------------------------------------------------------

data_chunk electrum::to_seed(const std::string& passphrase) const
{
    // Words are generated from seed, so always from a supported dictionary.
    return to_seed(words(), passphrase);
}

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

#endif

} // namespace wallet
} // namespace system
} // namespace libbitcoin
