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
#include "../../test.hpp"
#include "slip39.hpp"

BC_AUTO_TEST_SUITE(slip39_tests)

using namespace test::mnemonics_slip39;
using namespace bc::system::wallet;

// The single share, two of three, 256 bit and extendable vectors.
constexpr auto single = 0_size;
constexpr auto invalid_checksum = 1_size;
constexpr auto two_of_three = 3_size;
constexpr auto long_single = 19_size;
constexpr auto extendable_single = 41_size;

// public static
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(slip39__word_counts__always__twenty_and_fifty_nine)
{
    BC_REQUIRE_EQUAL(slip39::word_minimum, 20u);
    BC_REQUIRE_EQUAL(slip39::word_maximum, 59u);
}

BC_AUTO_TEST_CASE(slip39__is_valid_entropy_size__valid__true)
{
    BC_REQUIRE(slip39::is_valid_entropy_size(16));
    BC_REQUIRE(slip39::is_valid_entropy_size(18));
    BC_REQUIRE(slip39::is_valid_entropy_size(24));
    BC_REQUIRE(slip39::is_valid_entropy_size(32));
    BC_REQUIRE(slip39::is_valid_entropy_size(64));
}

BC_AUTO_TEST_CASE(slip39__is_valid_entropy_size__invalid__false)
{
    BC_REQUIRE(!slip39::is_valid_entropy_size(0));
    BC_REQUIRE(!slip39::is_valid_entropy_size(14));
    BC_REQUIRE(!slip39::is_valid_entropy_size(15));
    BC_REQUIRE(!slip39::is_valid_entropy_size(17));
    BC_REQUIRE(!slip39::is_valid_entropy_size(31));
    BC_REQUIRE(!slip39::is_valid_entropy_size(33));
    BC_REQUIRE(!slip39::is_valid_entropy_size(65));
    BC_REQUIRE(!slip39::is_valid_entropy_size(66));
}

BC_AUTO_TEST_CASE(slip39__is_valid_word_count__valid__true)
{
    BC_REQUIRE(slip39::is_valid_word_count(20));
    BC_REQUIRE(slip39::is_valid_word_count(22));
    BC_REQUIRE(slip39::is_valid_word_count(23));
    BC_REQUIRE(slip39::is_valid_word_count(27));
    BC_REQUIRE(slip39::is_valid_word_count(33));
    BC_REQUIRE(slip39::is_valid_word_count(59));
}

BC_AUTO_TEST_CASE(slip39__is_valid_word_count__invalid__false)
{
    BC_REQUIRE(!slip39::is_valid_word_count(0));
    BC_REQUIRE(!slip39::is_valid_word_count(19));
    BC_REQUIRE(!slip39::is_valid_word_count(21));
    BC_REQUIRE(!slip39::is_valid_word_count(24));
    BC_REQUIRE(!slip39::is_valid_word_count(32));
    BC_REQUIRE(!slip39::is_valid_word_count(34));
    BC_REQUIRE(!slip39::is_valid_word_count(60));
}

BC_AUTO_TEST_CASE(slip39__is_valid_dictionary__en__true)
{
    BC_REQUIRE(slip39::is_valid_dictionary(language::en));
    BC_REQUIRE(!slip39::is_valid_dictionary(language::es));
    BC_REQUIRE(!slip39::is_valid_dictionary(language::none));
}

BC_AUTO_TEST_CASE(slip39__is_valid_passphrase__printable_ascii__true)
{
    BC_REQUIRE(slip39::is_valid_passphrase(""));
    BC_REQUIRE(slip39::is_valid_passphrase("TREZOR"));
    BC_REQUIRE(slip39::is_valid_passphrase(" ~"));
}

BC_AUTO_TEST_CASE(slip39__is_valid_passphrase__not_printable_ascii__false)
{
    BC_REQUIRE(!slip39::is_valid_passphrase("\t"));
    BC_REQUIRE(!slip39::is_valid_passphrase("\x7f"));
    BC_REQUIRE(!slip39::is_valid_passphrase("caf\xc3\xa9"));
}

BC_AUTO_TEST_CASE(slip39__is_valid_grouping__valid__true)
{
    BC_REQUIRE(slip39::is_valid_grouping(1, { { 1, 1 } }));
    BC_REQUIRE(slip39::is_valid_grouping(1, { { 2, 3 } }));
    BC_REQUIRE(slip39::is_valid_grouping(1, { { 2, 3 }, { 3, 5 } }));
    BC_REQUIRE(slip39::is_valid_grouping(2, { { 2, 3 }, { 1, 1 }, { 16, 16 } }));
}

BC_AUTO_TEST_CASE(slip39__is_valid_grouping__invalid__false)
{
    BC_REQUIRE(!slip39::is_valid_grouping(1, {}));
    BC_REQUIRE(!slip39::is_valid_grouping(0, { { 1, 1 } }));
    BC_REQUIRE(!slip39::is_valid_grouping(2, { { 1, 1 } }));
    BC_REQUIRE(!slip39::is_valid_grouping(1, { { 0, 1 } }));
    BC_REQUIRE(!slip39::is_valid_grouping(1, { { 3, 2 } }));
    BC_REQUIRE(!slip39::is_valid_grouping(1, { { 1, 2 } }));
    BC_REQUIRE(!slip39::is_valid_grouping(1, { { 17, 17 } }));
}

BC_AUTO_TEST_CASE(slip39__entropy_size__single_share__zero)
{
    BC_REQUIRE_EQUAL(slip39::entropy_size(16, 1, { { 1, 1 } }), 0u);
    BC_REQUIRE_EQUAL(slip39::entropy_size(32, 1, { { 1, 1 } }), 0u);
}

BC_AUTO_TEST_CASE(slip39__entropy_size__shared__digest_random_per_sharing)
{
    BC_REQUIRE_EQUAL(slip39::entropy_size(16, 1, { { 2, 3 } }), 12u);
    BC_REQUIRE_EQUAL(slip39::entropy_size(32, 1, { { 2, 3 } }), 28u);
    BC_REQUIRE_EQUAL(slip39::entropy_size(16, 2, { { 2, 3 }, { 2, 3 }, { 1, 1 } }), 36u);
    BC_REQUIRE_EQUAL(slip39::entropy_size(16, 1, { { 2, 3 }, { 3, 5 } }), 40u);
    BC_REQUIRE_EQUAL(slip39::entropy_size(24, 1, { { 2, 3 } }), 20u);
    BC_REQUIRE_EQUAL(slip39::entropy_size(64, 1, { { 2, 3 } }), 60u);
}

BC_AUTO_TEST_CASE(slip39__entropy_size__invalid_secret_size__zero)
{
    BC_REQUIRE_EQUAL(slip39::entropy_size(17, 1, { { 2, 3 } }), 0u);
    BC_REQUIRE_EQUAL(slip39::entropy_size(66, 1, { { 2, 3 } }), 0u);
}

// to_shares
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(slip39__to_shares__invalid_secret_size__empty)
{
    const data_chunk secret(17, 0x00_u8);
    BC_REQUIRE(slip39::to_shares(secret, { { 1, 1 } }, 1, {}, 0, false, 0).empty());
}

BC_AUTO_TEST_CASE(slip39__to_shares__invalid_grouping__empty)
{
    const data_chunk secret(16, 0x00_u8);
    BC_REQUIRE(slip39::to_shares(secret, { { 1, 2 } }, 1, {}, 0, false, 0).empty());
}

BC_AUTO_TEST_CASE(slip39__to_shares__wrong_entropy_size__empty)
{
    const data_chunk secret(16, 0x00_u8);
    const data_chunk entropy(11, 0x00_u8);
    BC_REQUIRE(slip39::to_shares(secret, { { 2, 3 } }, 1, entropy, 0, false, 0).empty());
}

BC_AUTO_TEST_CASE(slip39__to_shares__identifier_above_maximum__empty)
{
    const data_chunk secret(16, 0x00_u8);
    BC_REQUIRE(slip39::to_shares(secret, { { 1, 1 } }, 1, {}, 32768, false, 0).empty());
}

BC_AUTO_TEST_CASE(slip39__to_shares__exponent_above_maximum__empty)
{
    const data_chunk secret(16, 0x00_u8);
    BC_REQUIRE(slip39::to_shares(secret, { { 1, 1 } }, 1, {}, 0, false, 16).empty());
}

BC_AUTO_TEST_CASE(slip39__to_shares__invalid_passphrase__empty)
{
    const data_chunk secret(16, 0x00_u8);
    BC_REQUIRE(slip39::to_shares(secret, { { 1, 1 } }, 1, {}, 0, false, 0, "caf\xc3\xa9").empty());
}

BC_AUTO_TEST_CASE(slip39__to_shares__single_share__expected_share)
{
    const auto secret = base16_chunk("bb54aac4b89dc868ba37d9cc21b2cece");
    const auto shares = slip39::to_shares(secret, { { 1, 1 } }, 1, {}, 7470, false, 0, passphrase);
    BC_REQUIRE_EQUAL(shares.size(), 1u);

    const auto& share = shares.front();
    BC_REQUIRE(share);
    BC_REQUIRE_EQUAL(share.words().size(), slip39::word_minimum);
    BC_REQUIRE_EQUAL(share.identifier(), 7470u);
    BC_REQUIRE(!share.extendable());
    BC_REQUIRE_EQUAL(share.exponent(), 0u);
    BC_REQUIRE_EQUAL(share.group_index(), 0u);
    BC_REQUIRE_EQUAL(share.group_threshold(), 1u);
    BC_REQUIRE_EQUAL(share.group_count(), 1u);
    BC_REQUIRE_EQUAL(share.member_index(), 0u);
    BC_REQUIRE_EQUAL(share.member_threshold(), 1u);
}

BC_AUTO_TEST_CASE(slip39__to_shares__groups__expected_count_and_fields)
{
    const auto secret = base16_chunk("bb54aac4b89dc868ba37d9cc21b2cece");
    const slip39::groups groups{ { 2, 3 }, { 2, 3 }, { 1, 1 } };
    const data_chunk entropy(slip39::entropy_size(secret.size(), 2, groups), 0x42_u8);
    const auto shares = slip39::to_shares(secret, groups, 2, entropy, 7470, false, 0, passphrase);
    BC_REQUIRE_EQUAL(shares.size(), 7u);
    BC_REQUIRE_EQUAL(shares.at(0).group_index(), 0u);
    BC_REQUIRE_EQUAL(shares.at(0).member_index(), 0u);
    BC_REQUIRE_EQUAL(shares.at(0).member_threshold(), 2u);
    BC_REQUIRE_EQUAL(shares.at(3).group_index(), 1u);
    BC_REQUIRE_EQUAL(shares.at(3).member_index(), 0u);
    BC_REQUIRE_EQUAL(shares.at(6).group_index(), 2u);
    BC_REQUIRE_EQUAL(shares.at(6).member_threshold(), 1u);
    BC_REQUIRE_EQUAL(shares.at(6).group_threshold(), 2u);
    BC_REQUIRE_EQUAL(shares.at(6).group_count(), 3u);
}

// to_secret
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(slip39__to_secret__empty__empty)
{
    BC_REQUIRE(slip39::to_secret({}, passphrase).empty());
}

BC_AUTO_TEST_CASE(slip39__to_secret__invalid_share__empty)
{
    const slip39::list shares{ slip39{ vectors.at(invalid_checksum).mnemonics.front() } };
    BC_REQUIRE(slip39::to_secret(shares, passphrase).empty());
}

BC_AUTO_TEST_CASE(slip39__to_secret__invalid_passphrase__empty)
{
    const auto secret = base16_chunk("bb54aac4b89dc868ba37d9cc21b2cece");
    const auto shares = slip39::to_shares(secret, { { 1, 1 } }, 1, {}, 7470, false, 0, passphrase);
    BC_REQUIRE(slip39::to_secret(shares, "caf\xc3\xa9").empty());
}

BC_AUTO_TEST_CASE(slip39__to_secret__single_share__round_trips)
{
    const auto secret = base16_chunk("bb54aac4b89dc868ba37d9cc21b2cece");
    const auto shares = slip39::to_shares(secret, { { 1, 1 } }, 1, {}, 7470, false, 0, passphrase);
    BC_REQUIRE_EQUAL(slip39::to_secret(shares, passphrase), secret);
}

BC_AUTO_TEST_CASE(slip39__to_secret__threshold_of_groups__round_trips)
{
    const auto secret = base16_chunk("bb54aac4b89dc868ba37d9cc21b2cece");
    const slip39::groups groups{ { 2, 3 }, { 2, 3 }, { 1, 1 } };
    const data_chunk entropy(slip39::entropy_size(secret.size(), 2, groups), 0x42_u8);
    const auto shares = slip39::to_shares(secret, groups, 2, entropy, 7470, false, 0, passphrase);
    const slip39::list subset{ shares.at(1), shares.at(2), shares.at(3), shares.at(5) };
    BC_REQUIRE_EQUAL(slip39::to_secret(subset, passphrase), secret);
}

BC_AUTO_TEST_CASE(slip39__to_secret__one_of_two_groups__round_trips)
{
    const auto secret = base16_chunk("bb54aac4b89dc868ba37d9cc21b2cece");
    const slip39::groups groups{ { 2, 3 }, { 3, 5 } };
    const data_chunk entropy(slip39::entropy_size(secret.size(), 1, groups), 0x42_u8);
    const auto shares = slip39::to_shares(secret, groups, 1, entropy, 7470, false, 0, passphrase);
    BC_REQUIRE_EQUAL(shares.size(), 8u);
    BC_REQUIRE_EQUAL(shares.at(0).group_threshold(), 1u);
    BC_REQUIRE_EQUAL(shares.at(0).group_count(), 2u);

    const slip39::list first{ shares.at(0), shares.at(2) };
    const slip39::list second{ shares.at(3), shares.at(5), shares.at(7) };
    BC_REQUIRE_EQUAL(slip39::to_secret(first, passphrase), secret);
    BC_REQUIRE_EQUAL(slip39::to_secret(second, passphrase), secret);
}

BC_AUTO_TEST_CASE(slip39__to_secret__one_of_two_groups_both_groups__empty)
{
    const auto secret = base16_chunk("bb54aac4b89dc868ba37d9cc21b2cece");
    const slip39::groups groups{ { 2, 3 }, { 3, 5 } };
    const data_chunk entropy(slip39::entropy_size(secret.size(), 1, groups), 0x42_u8);
    const auto shares = slip39::to_shares(secret, groups, 1, entropy, 7470, false, 0, passphrase);
    const slip39::list subset{ shares.at(0), shares.at(2), shares.at(3), shares.at(5), shares.at(7) };
    BC_REQUIRE(slip39::to_secret(subset, passphrase).empty());
}

BC_AUTO_TEST_CASE(slip39__to_secret__one_group_short__empty)
{
    const auto secret = base16_chunk("bb54aac4b89dc868ba37d9cc21b2cece");
    const slip39::groups groups{ { 2, 3 }, { 2, 3 }, { 1, 1 } };
    const data_chunk entropy(slip39::entropy_size(secret.size(), 2, groups), 0x42_u8);
    const auto shares = slip39::to_shares(secret, groups, 2, entropy, 7470, false, 0, passphrase);
    const slip39::list subset{ shares.at(0), shares.at(1) };
    BC_REQUIRE(slip39::to_secret(subset, passphrase).empty());
}

BC_AUTO_TEST_CASE(slip39__to_secret__one_member_short__empty)
{
    const auto secret = base16_chunk("bb54aac4b89dc868ba37d9cc21b2cece");
    const slip39::groups groups{ { 2, 3 }, { 2, 3 }, { 1, 1 } };
    const data_chunk entropy(slip39::entropy_size(secret.size(), 2, groups), 0x42_u8);
    const auto shares = slip39::to_shares(secret, groups, 2, entropy, 7470, false, 0, passphrase);
    const slip39::list subset{ shares.at(0), shares.at(3), shares.at(6) };
    BC_REQUIRE(slip39::to_secret(subset, passphrase).empty());
}

BC_AUTO_TEST_CASE(slip39__to_secret__more_groups_than_threshold__empty)
{
    const auto secret = base16_chunk("bb54aac4b89dc868ba37d9cc21b2cece");
    const slip39::groups groups{ { 2, 3 }, { 2, 3 }, { 1, 1 } };
    const data_chunk entropy(slip39::entropy_size(secret.size(), 2, groups), 0x42_u8);
    const auto shares = slip39::to_shares(secret, groups, 2, entropy, 7470, false, 0, passphrase);
    const slip39::list subset{ shares.at(0), shares.at(1), shares.at(3), shares.at(4), shares.at(6) };
    BC_REQUIRE(slip39::to_secret(subset, passphrase).empty());
}

BC_AUTO_TEST_CASE(slip39__to_secret__wrong_passphrase__other_secret)
{
    const auto secret = base16_chunk("bb54aac4b89dc868ba37d9cc21b2cece");
    const auto shares = slip39::to_shares(secret, { { 1, 1 } }, 1, {}, 7470, false, 0, passphrase);
    const auto other = slip39::to_secret(shares, "other");
    BC_REQUIRE_EQUAL(other.size(), secret.size());
    BC_REQUIRE_NE(other, secret);
}

BC_AUTO_TEST_CASE(slip39__to_secret__256_bit_extendable__round_trips)
{
    const auto secret = base16_chunk("c36d0cb0fe8ee3f0e5c00d9b11fa5a6e4bbd2d5aa2dc0a3e4e56c0b8a5c3e5d7");
    const slip39::groups groups{ { 3, 5 } };
    const data_chunk entropy(slip39::entropy_size(secret.size(), 1, groups), 0x17_u8);
    const auto shares = slip39::to_shares(secret, groups, 1, entropy, 1234, true, 1, passphrase);
    BC_REQUIRE_EQUAL(shares.size(), 5u);
    BC_REQUIRE_EQUAL(shares.front().words().size(), 33u);
    BC_REQUIRE(shares.front().extendable());
    BC_REQUIRE_EQUAL(shares.front().exponent(), 1u);

    const slip39::list subset{ shares.at(4), shares.at(0), shares.at(2) };
    BC_REQUIRE_EQUAL(slip39::to_secret(subset, passphrase), secret);
}

BC_AUTO_TEST_CASE(slip39__to_secret__192_bit_sentences__round_trips)
{
    const auto secret = base16_chunk("c36d0cb0fe8ee3f0e5c00d9b11fa5a6e4bbd2d5aa2dc0a3e");
    const slip39::groups groups{ { 2, 3 } };
    const data_chunk entropy(slip39::entropy_size(secret.size(), 1, groups), 0x17_u8);
    const auto shares = slip39::to_shares(secret, groups, 1, entropy, 1234, false, 0, passphrase);
    BC_REQUIRE_EQUAL(shares.size(), 3u);
    BC_REQUIRE_EQUAL(shares.front().words().size(), 27u);

    const slip39::list subset{ slip39{ shares.at(2).sentence() }, slip39{ shares.at(0).sentence() } };
    BC_REQUIRE(subset.at(0));
    BC_REQUIRE(subset.at(1));
    BC_REQUIRE_EQUAL(subset.at(0).entropy(), shares.at(2).entropy());
    BC_REQUIRE_EQUAL(slip39::to_secret(subset, passphrase), secret);
}

BC_AUTO_TEST_CASE(slip39__to_secret__512_bit_sentences__round_trips)
{
    const auto secret = base16_chunk("c36d0cb0fe8ee3f0e5c00d9b11fa5a6e4bbd2d5aa2dc0a3e4e56c0b8a5c3e5d7bb54aac4b89dc868ba37d9cc21b2cece11bc609d21747c49ba78c0701293e417");
    const slip39::groups groups{ { 2, 3 } };
    const data_chunk entropy(slip39::entropy_size(secret.size(), 1, groups), 0x17_u8);
    const auto shares = slip39::to_shares(secret, groups, 1, entropy, 1234, true, 0, passphrase);
    BC_REQUIRE_EQUAL(shares.size(), 3u);
    BC_REQUIRE_EQUAL(shares.front().words().size(), slip39::word_maximum);

    const slip39::list subset{ slip39{ shares.at(1).sentence() }, slip39{ shares.at(2).sentence() } };
    BC_REQUIRE(subset.at(0));
    BC_REQUIRE(subset.at(1));
    BC_REQUIRE_EQUAL(subset.at(1).entropy().size(), slip39::entropy_maximum);
    BC_REQUIRE_EQUAL(slip39::to_secret(subset, passphrase), secret);
}

// construct
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(slip39__construct__default__invalid)
{
    const slip39 instance{};
    BC_REQUIRE(!instance);
    BC_REQUIRE(instance.entropy().empty());
    BC_REQUIRE(instance.words().empty());
    BC_REQUIRE_EQUAL(instance.identifier(), 0u);
}

BC_AUTO_TEST_CASE(slip39__construct__sentence__expected)
{
    const slip39 instance{ vectors.at(single).mnemonics.front() };
    BC_REQUIRE(instance);
    BC_REQUIRE(instance.lingo() == language::en);
    BC_REQUIRE_EQUAL(instance.words().size(), slip39::word_minimum);
    BC_REQUIRE_EQUAL(instance.identifier(), 7945u);
    BC_REQUIRE(!instance.extendable());
    BC_REQUIRE_EQUAL(instance.exponent(), 0u);
    BC_REQUIRE_EQUAL(instance.group_index(), 0u);
    BC_REQUIRE_EQUAL(instance.group_threshold(), 1u);
    BC_REQUIRE_EQUAL(instance.group_count(), 1u);
    BC_REQUIRE_EQUAL(instance.member_index(), 0u);
    BC_REQUIRE_EQUAL(instance.member_threshold(), 1u);
    BC_REQUIRE_EQUAL(encode_base16(instance.entropy()), "11bc609d21747c49ba78c0701293e417");
}

BC_AUTO_TEST_CASE(slip39__construct__shared_sentence__expected)
{
    const slip39 instance{ vectors.at(two_of_three).mnemonics.front() };
    BC_REQUIRE(instance);
    BC_REQUIRE_EQUAL(instance.identifier(), 25653u);
    BC_REQUIRE_EQUAL(instance.exponent(), 2u);
    BC_REQUIRE_EQUAL(instance.member_index(), 2u);
    BC_REQUIRE_EQUAL(instance.member_threshold(), 2u);
    BC_REQUIRE_EQUAL(encode_base16(instance.entropy()), "08fb14b66e692e25dfe2edf53289ed62");
}

BC_AUTO_TEST_CASE(slip39__construct__extendable_sentence__expected)
{
    const slip39 instance{ vectors.at(extendable_single).mnemonics.front() };
    BC_REQUIRE(instance);
    BC_REQUIRE(instance.extendable());
    BC_REQUIRE_EQUAL(instance.identifier(), 29019u);
    BC_REQUIRE_EQUAL(instance.exponent(), 3u);
}

BC_AUTO_TEST_CASE(slip39__construct__long_sentence__expected)
{
    const slip39 instance{ vectors.at(long_single).mnemonics.front() };
    BC_REQUIRE(instance);
    BC_REQUIRE_EQUAL(instance.words().size(), 33u);
    BC_REQUIRE_EQUAL(instance.entropy().size(), 32u);
    BC_REQUIRE_EQUAL(instance.identifier(), 29172u);
}

BC_AUTO_TEST_CASE(slip39__construct__words__same_as_sentence)
{
    const auto sentence = vectors.at(single).mnemonics.front();
    const slip39 instance{ split(sentence) };
    BC_REQUIRE(instance);
    BC_REQUIRE_EQUAL(instance.sentence(), sentence);
}

BC_AUTO_TEST_CASE(slip39__construct__invalid_checksum__invalid)
{
    const slip39 instance{ vectors.at(invalid_checksum).mnemonics.front() };
    BC_REQUIRE(!instance);
}

BC_AUTO_TEST_CASE(slip39__construct__foreign_word__invalid)
{
    const slip39 instance{ "bogus enlarge academic academic agency result length solution fridge kidney coal piece deal husband erode duke ajar critical decision keyboard" };
    BC_REQUIRE(!instance);
}

BC_AUTO_TEST_CASE(slip39__construct__short_sentence__invalid)
{
    const slip39 instance{ "duckling enlarge academic" };
    BC_REQUIRE(!instance);
}

// verified vectors
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(slip39__to_secret__vectors__expected)
{
    for (const auto& vector: vectors)
    {
        const auto secret = slip39::to_secret(vector.shares(), passphrase);
        BC_REQUIRE_MESSAGE(encode_base16(secret) == vector.secret_, vector.description);
    }
}

BC_AUTO_TEST_CASE(slip39__to_key__vectors__expected)
{
    for (const auto& vector: vectors)
    {
        if (vector.valid())
            BC_REQUIRE_MESSAGE(slip39::to_key(vector.shares(), passphrase).encoded() == vector.hd_key_, vector.description);
    }
}

BC_AUTO_TEST_CASE(slip39__sentence__vectors__round_trips)
{
    for (const auto& vector: vectors)
    {
        if (vector.valid())
        {
            const auto shares = vector.shares();

            for (size_t index = 0; index < shares.size(); ++index)
                BC_REQUIRE_MESSAGE(shares.at(index).sentence() == vector.mnemonics.at(index), vector.description);
        }
    }
}

BC_AUTO_TEST_SUITE_END()
