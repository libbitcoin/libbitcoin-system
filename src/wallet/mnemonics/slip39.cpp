/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#include <bitcoin/system/wallet/mnemonics/slip39.hpp>

#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/stream/stream.hpp>
#include <bitcoin/system/wallet/keys/hd_private.hpp>
#include <bitcoin/system/words/words.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

// The share metadata field widths, which total four words.
constexpr auto identifier_bits = 15_size;
constexpr auto extendable_bits = 1_size;
constexpr auto exponent_bits = 4_size;
constexpr auto identifier_maximum = unmask_right<uint16_t>(identifier_bits);
constexpr auto exponent_maximum = unmask_right<uint8_t>(exponent_bits);

// Only the en dictionary is defined for slip39.
const slip39::dictionaries slip39::dictionaries_
{
    {
        slip39::dictionary{ language::en, words::slip39::en }
    }
};

// static
// ----------------------------------------------------------------------------

bool slip39::is_valid_entropy_size(size_t size) NOEXCEPT
{
    return size == entropy_minimum || size == entropy_maximum;
}

bool slip39::is_valid_word_count(size_t count) NOEXCEPT
{
    return count == word_minimum || count == word_maximum;
}

bool slip39::is_valid_dictionary(language identifier) NOEXCEPT
{
    return dictionaries_.exists(identifier);
}

bool slip39::is_valid_grouping(uint8_t group_threshold,
    const groups& groups) NOEXCEPT
{
    if (groups.empty() || groups.size() > count_maximum)
        return false;

    if (is_zero(group_threshold) || group_threshold > groups.size())
        return false;

    for (const auto& group: groups)
    {
        if (is_zero(group.threshold) || group.threshold > group.count)
            return false;

        if (group.count > count_maximum)
            return false;

        // A group of one share is that group's secret, so sharing it more
        // than once exposes it.
        if (is_one(group.threshold) && group.count > one)
            return false;
    }

    return true;
}

size_t slip39::entropy_size(size_t secret_size, size_t group_threshold,
    const groups& groups) NOEXCEPT
{
    const auto sum = [&](size_t (*size)(size_t)) NOEXCEPT
    {
        auto total = size(group_threshold);

        for (const auto& group: groups)
            total += size(group.threshold);

        return total;
    };

    if (secret_size == shamir128::secret_bytes)
        return sum(&shamir128::entropy_size);

    if (secret_size == shamir256::secret_bytes)
        return sum(&shamir256::entropy_size);

    return zero;
}

slip39::list slip39::to_shares(const data_chunk& secret, const groups& groups,
    uint8_t group_threshold, const data_chunk& entropy, uint16_t identifier,
    bool extendable, uint8_t exponent,
    const std::string& passphrase) NOEXCEPT
{
    if (!is_valid_entropy_size(secret.size()))
        return {};

    if (!is_valid_grouping(group_threshold, groups))
        return {};

    if (identifier > identifier_maximum || exponent > exponent_maximum)
        return {};

    const auto required = entropy_size(secret.size(), group_threshold, groups);

    if (entropy.size() != required)
        return {};

    list out{};

    if (secret.size() == shamir128::secret_bytes)
    {
        if (!share<shamir128>(out, secret, groups, group_threshold, entropy,
            identifier, extendable, exponent, passphrase))
            return {};
    }
    else
    {
        if (!share<shamir256>(out, secret, groups, group_threshold, entropy,
            identifier, extendable, exponent, passphrase))
            return {};
    }

    return out;
}

data_chunk slip39::to_secret(const list& shares,
    const std::string& passphrase) NOEXCEPT
{
    if (shares.empty())
        return {};

    data_chunk out{};
    const auto size = shares.front().entropy().size();

    if (size == shamir128::secret_bytes)
    {
        if (!combine<shamir128>(out, shares, passphrase))
            return {};
    }
    else if (size == shamir256::secret_bytes)
    {
        if (!combine<shamir256>(out, shares, passphrase))
            return {};
    }

    return out;
}

hd_private slip39::to_key(const list& shares, const std::string& passphrase,
    const context& context) NOEXCEPT
{
    const auto secret = to_secret(shares, passphrase);

    if (secret.empty())
        return {};

    return { secret, context.hd_prefixes() };
}

// protected static
// ----------------------------------------------------------------------------

template <typename Algorithm>
bool slip39::share(list& out, const data_chunk& secret, const groups& groups,
    uint8_t group_threshold, const data_chunk& entropy, uint16_t identifier,
    bool extendable, uint8_t exponent,
    const std::string& passphrase) NOEXCEPT
{
    using secret_t = typename Algorithm::secret_t;
    using shares_t = typename Algorithm::shares_t;

    secret_t master{};
    std::copy(secret.cbegin(), secret.cend(), master.begin());
    const auto encrypted = Algorithm::encrypt(master, passphrase, exponent,
        identifier, extendable);

    // Entropy is consumed by the group sharing and then by each group.
    auto next = entropy.begin();
    const auto take = [&next](size_t size) NOEXCEPT
    {
        const data_slice slice(next, std::next(next, size));
        std::advance(next, size);
        return slice;
    };

    const auto count = groups.size();
    const auto group_entropy = Algorithm::entropy_size(group_threshold);
    shares_t group_shares{};

    if (!Algorithm::split(group_shares, encrypted, group_threshold, count,
        take(group_entropy)))
        return false;

    for (auto index = zero; index < count; ++index)
    {
        const auto& group = groups[index];
        const auto member_entropy = Algorithm::entropy_size(group.threshold);
        shares_t member_shares{};

        if (!Algorithm::split(member_shares, group_shares[index].value,
            group.threshold, group.count, take(member_entropy)))
            return false;

        for (const auto& member: member_shares)
        {
            const fields fields
            {
                identifier,
                extendable,
                exponent,
                narrow_cast<uint8_t>(index),
                group_threshold,
                narrow_cast<uint8_t>(count),
                member.index,
                group.threshold
            };

            const auto value = to_chunk(member.value);
            out.push_back(slip39{ value, encoder(value, fields), fields });
        }
    }

    return true;
}

template <typename Algorithm>
bool slip39::combine(data_chunk& out, const list& shares,
    const std::string& passphrase) NOEXCEPT
{
    using secret_t = typename Algorithm::secret_t;
    using shares_t = typename Algorithm::shares_t;

    struct member_group
    {
        uint8_t index;
        uint8_t threshold;
        shares_t shares;
    };

    const auto& first = shares.front();
    const auto identifier = first.identifier();
    const auto extendable = first.extendable();
    const auto exponent = first.exponent();
    const auto group_threshold = first.group_threshold();
    const auto group_count = first.group_count();
    std_vector<member_group> groups{};

    for (const auto& share: shares)
    {
        if (!share || share.entropy().size() != Algorithm::secret_bytes)
            return false;

        // All shares of a master secret carry the same share set metadata.
        if (share.identifier() != identifier ||
            share.extendable() != extendable ||
            share.exponent() != exponent ||
            share.group_threshold() != group_threshold ||
            share.group_count() != group_count)
            return false;

        const auto match = [&share](const member_group& group) NOEXCEPT
        {
            return group.index == share.group_index();
        };

        auto group = std::find_if(groups.begin(), groups.end(), match);

        if (group == groups.end())
        {
            groups.push_back({ share.group_index(),
                share.member_threshold(), {} });
            group = std::prev(groups.end());
        }
        else if (group->threshold != share.member_threshold())
        {
            return false;
        }

        secret_t value{};
        const auto& entropy = share.entropy();
        std::copy(entropy.cbegin(), entropy.cend(), value.begin());
        group->shares.push_back({ share.member_index(), value });
    }

    // A threshold of groups is required, and no more, as is a threshold of
    // shares within each of those groups.
    if (groups.size() != group_threshold)
        return false;

    shares_t group_shares{};
    group_shares.reserve(groups.size());

    for (const auto& group: groups)
    {
        secret_t secret{};

        if (!Algorithm::recover(secret, group.shares, group.threshold))
            return false;

        group_shares.push_back({ group.index, secret });
    }

    secret_t encrypted{};

    if (!Algorithm::recover(encrypted, group_shares, group_threshold))
        return false;

    const auto secret = Algorithm::decrypt(encrypted, passphrase, exponent,
        identifier, extendable);

    out = to_chunk(secret);
    return true;
}

slip39 slip39::from_words(const string_list& words) NOEXCEPT
{
    const auto tidy = try_normalize(words);
    data_chunk value{};
    fields fields{};

    if (!decoder(value, fields, tidy))
        return {};

    return { value, tidy, fields };
}

string_list slip39::encoder(const data_chunk& value,
    const fields& fields) NOEXCEPT
{
    const auto count = value.size() == entropy_minimum ? word_minimum :
        word_maximum;
    const auto value_words = count - fixed_words;
    const auto padding = word_bits * value_words - to_bits(value.size());

    // The share value is left padded with zeros to a multiple of the word
    // size, so the metadata and value are written as a bit stream and then
    // read back as words.
    data_chunk buffer{};
    write::bits::data sink(buffer);
    sink.write_bits(fields.identifier, identifier_bits);
    sink.write_bits(to_int<uint64_t>(fields.extendable), extendable_bits);
    sink.write_bits(fields.exponent, exponent_bits);
    sink.write_bits(fields.group_index, index_bits);
    sink.write_bits(sub1(fields.group_threshold), index_bits);
    sink.write_bits(sub1(fields.group_count), index_bits);
    sink.write_bits(fields.member_index, index_bits);
    sink.write_bits(sub1(fields.member_threshold), index_bits);
    sink.write_bits(zero, padding);

    for (const auto byte: value)
        sink.write_bits(byte, byte_bits);

    sink.flush();

    shamir::rs1024::words_t coded{};
    coded.reserve(count);
    read::bits::copy source(buffer);

    for (auto word = zero; word < count - checksum_words; ++word)
    {
        const auto read = source.read_bits(word_bits);
        coded.push_back(narrow_cast<shamir::rs1024::word_t>(read));
    }

    const auto checksum = shamir::rs1024::checksum(coded, fields.extendable);
    coded.insert(coded.end(), checksum.cbegin(), checksum.cend());

    dictionaries::search indexes(coded.size());
    std::transform(coded.cbegin(), coded.cend(), indexes.begin(),
        [](shamir::rs1024::word_t word) NOEXCEPT
        {
            return wide_cast<size_t>(word);
        });

    return dictionaries_.at(indexes, language::en);
}

bool slip39::decoder(data_chunk& value, fields& out,
    const string_list& words) NOEXCEPT
{
    if (!is_valid_word_count(words.size()))
        return false;

    const auto indexes = dictionaries_.index(words, language::en);

    if (std::any_of(indexes.cbegin(), indexes.cend(),
        [](const int32_t& index) NOEXCEPT { return is_negative(index); }))
        return false;

    shamir::rs1024::words_t coded(indexes.size());
    std::transform(indexes.cbegin(), indexes.cend(), coded.begin(),
        [](const int32_t& index) NOEXCEPT
        {
            return possible_sign_narrow_cast<shamir::rs1024::word_t>(index);
        });

    data_chunk buffer{};
    write::bits::data sink(buffer);

    for (const auto word: coded)
        sink.write_bits(word, word_bits);

    sink.flush();

    // The extendable flag selects the checksum customization, so it is read
    // from the share before the share is verified.
    read::bits::copy source(buffer);
    const auto identifier = source.read_bits(identifier_bits);
    const auto extendable = is_one(source.read_bits(extendable_bits));

    if (!shamir::rs1024::verify(coded, extendable))
        return false;

    const auto exponent = source.read_bits(exponent_bits);
    const auto group_index = source.read_bits(index_bits);
    const auto group_threshold = add1(source.read_bits(index_bits));
    const auto group_count = add1(source.read_bits(index_bits));
    const auto member_index = source.read_bits(index_bits);
    const auto member_threshold = add1(source.read_bits(index_bits));

    if (group_index >= group_count || group_threshold > group_count)
        return false;

    const auto size = words.size() == word_minimum ? entropy_minimum :
        entropy_maximum;
    const auto value_words = words.size() - fixed_words;
    const auto padding = word_bits * value_words - to_bits(size);

    if (!is_zero(source.read_bits(padding)))
        return false;

    value.clear();
    value.reserve(size);

    for (auto byte = zero; byte < size; ++byte)
    {
        const auto read = source.read_bits(byte_bits);
        value.push_back(narrow_cast<uint8_t>(read));
    }

    out =
    {
        narrow_cast<uint16_t>(identifier),
        extendable,
        narrow_cast<uint8_t>(exponent),
        narrow_cast<uint8_t>(group_index),
        narrow_cast<uint8_t>(group_threshold),
        narrow_cast<uint8_t>(group_count),
        narrow_cast<uint8_t>(member_index),
        narrow_cast<uint8_t>(member_threshold)
    };

    return true;
}

// construction
// ----------------------------------------------------------------------------

slip39::slip39() NOEXCEPT
  : languages(), fields_{}
{
}

slip39::slip39(const std::string& sentence) NOEXCEPT
  : slip39(languages::split(sentence, language::en))
{
}

slip39::slip39(const string_list& words) NOEXCEPT
  : slip39(from_words(words))
{
}

slip39::slip39(const data_chunk& value, const string_list& words,
    const fields& fields) NOEXCEPT
  : languages(value, words, language::en), fields_(fields)
{
}

// properties
// ----------------------------------------------------------------------------

uint16_t slip39::identifier() const NOEXCEPT
{
    return fields_.identifier;
}

bool slip39::extendable() const NOEXCEPT
{
    return fields_.extendable;
}

uint8_t slip39::exponent() const NOEXCEPT
{
    return fields_.exponent;
}

uint8_t slip39::group_index() const NOEXCEPT
{
    return fields_.group_index;
}

uint8_t slip39::group_threshold() const NOEXCEPT
{
    return fields_.group_threshold;
}

uint8_t slip39::group_count() const NOEXCEPT
{
    return fields_.group_count;
}

uint8_t slip39::member_index() const NOEXCEPT
{
    return fields_.member_index;
}

uint8_t slip39::member_threshold() const NOEXCEPT
{
    return fields_.member_threshold;
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace wallet
} // namespace system
} // namespace libbitcoin
