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
#include "../test.hpp"

BC_AUTO_TEST_SUITE(shamir_tests)

constexpr auto secret128 = base16_array("bb54aac4b89dc868ba37d9cc21b2cece");
constexpr auto secret256 = base16_array("c36d0cb0fe8ee3f0e5c00d9b11fa5a6e4bbd2d5aa2dc0a3e4e56c0b8a5c3e5d7");

// entropy_size

BC_AUTO_TEST_CASE(shamir__entropy_size__one__zero)
{
    BC_REQUIRE_EQUAL(shamir128::entropy_size(1), 0u);
    BC_REQUIRE_EQUAL(shamir256::entropy_size(1), 0u);
}

BC_AUTO_TEST_CASE(shamir__entropy_size__two__digest_random_only)
{
    BC_REQUIRE_EQUAL(shamir128::entropy_size(2), 12u);
    BC_REQUIRE_EQUAL(shamir256::entropy_size(2), 28u);
}

BC_AUTO_TEST_CASE(shamir__entropy_size__three__random_share_and_digest_random)
{
    BC_REQUIRE_EQUAL(shamir128::entropy_size(3), 28u);
    BC_REQUIRE_EQUAL(shamir256::entropy_size(3), 60u);
}

// split

BC_AUTO_TEST_CASE(shamir__split__single__secret)
{
    shamir128::shares_t shares{};
    BC_REQUIRE(shamir128::split(shares, secret128, 1, 1, {}));
    BC_REQUIRE_EQUAL(shares.size(), 1u);
    BC_REQUIRE_EQUAL(shares.front().index, 0u);
    BC_REQUIRE_EQUAL(shares.front().value, secret128);
}

BC_AUTO_TEST_CASE(shamir__split__zero_threshold__false)
{
    shamir128::shares_t shares{};
    BC_REQUIRE(!shamir128::split(shares, secret128, 0, 1, {}));
}

BC_AUTO_TEST_CASE(shamir__split__threshold_above_count__false)
{
    const data_chunk entropy(shamir128::entropy_size(3), 0x42_u8);
    shamir128::shares_t shares{};
    BC_REQUIRE(!shamir128::split(shares, secret128, 3, 2, entropy));
}

BC_AUTO_TEST_CASE(shamir__split__single_threshold_multiple_shares__secrets)
{
    shamir128::shares_t shares{};
    BC_REQUIRE(shamir128::split(shares, secret128, 1, 2, {}));
    BC_REQUIRE_EQUAL(shares.size(), 2u);
    BC_REQUIRE_EQUAL(shares.at(0).index, 0u);
    BC_REQUIRE_EQUAL(shares.at(0).value, secret128);
    BC_REQUIRE_EQUAL(shares.at(1).index, 1u);
    BC_REQUIRE_EQUAL(shares.at(1).value, secret128);
}

BC_AUTO_TEST_CASE(shamir__split__wrong_entropy_size__false)
{
    const data_chunk entropy(sub1(shamir128::entropy_size(2)), 0x42_u8);
    shamir128::shares_t shares{};
    BC_REQUIRE(!shamir128::split(shares, secret128, 2, 3, entropy));
}

BC_AUTO_TEST_CASE(shamir__split__threshold__coordinates_from_zero)
{
    const data_chunk entropy(shamir128::entropy_size(3), 0x42_u8);
    shamir128::shares_t shares{};
    BC_REQUIRE(shamir128::split(shares, secret128, 3, 5, entropy));
    BC_REQUIRE_EQUAL(shares.size(), 5u);
    BC_REQUIRE_EQUAL(shares.at(0).index, 0u);
    BC_REQUIRE_EQUAL(shares.at(4).index, 4u);
    BC_REQUIRE_NE(shares.at(0).value, secret128);
}

// recover

BC_AUTO_TEST_CASE(shamir__recover__single__secret)
{
    shamir128::shares_t shares{};
    BC_REQUIRE(shamir128::split(shares, secret128, 1, 1, {}));

    shamir128::secret_t out{};
    BC_REQUIRE(shamir128::recover(out, shares, 1));
    BC_REQUIRE_EQUAL(out, secret128);
}

BC_AUTO_TEST_CASE(shamir__recover__any_threshold_subset__secret)
{
    const data_chunk entropy(shamir128::entropy_size(3), 0x42_u8);
    shamir128::shares_t shares{};
    BC_REQUIRE(shamir128::split(shares, secret128, 3, 5, entropy));

    const shamir128::shares_t low{ shares.at(0), shares.at(1), shares.at(2) };
    const shamir128::shares_t high{ shares.at(4), shares.at(3), shares.at(2) };
    const shamir128::shares_t mixed{ shares.at(3), shares.at(0), shares.at(4) };

    shamir128::secret_t out{};
    BC_REQUIRE(shamir128::recover(out, low, 3));
    BC_REQUIRE_EQUAL(out, secret128);
    BC_REQUIRE(shamir128::recover(out, high, 3));
    BC_REQUIRE_EQUAL(out, secret128);
    BC_REQUIRE(shamir128::recover(out, mixed, 3));
    BC_REQUIRE_EQUAL(out, secret128);
}

BC_AUTO_TEST_CASE(shamir__recover__256_bit__secret)
{
    const data_chunk entropy(shamir256::entropy_size(2), 0x17_u8);
    shamir256::shares_t shares{};
    BC_REQUIRE(shamir256::split(shares, secret256, 2, 3, entropy));

    const shamir256::shares_t subset{ shares.at(2), shares.at(0) };
    shamir256::secret_t out{};
    BC_REQUIRE(shamir256::recover(out, subset, 2));
    BC_REQUIRE_EQUAL(out, secret256);
}

BC_AUTO_TEST_CASE(shamir__recover__too_few_shares__false)
{
    const data_chunk entropy(shamir128::entropy_size(3), 0x42_u8);
    shamir128::shares_t shares{};
    BC_REQUIRE(shamir128::split(shares, secret128, 3, 5, entropy));

    const shamir128::shares_t subset{ shares.at(0), shares.at(1) };
    shamir128::secret_t out{};
    BC_REQUIRE(!shamir128::recover(out, subset, 3));
}

BC_AUTO_TEST_CASE(shamir__recover__repeated_coordinate__false)
{
    const data_chunk entropy(shamir128::entropy_size(2), 0x42_u8);
    shamir128::shares_t shares{};
    BC_REQUIRE(shamir128::split(shares, secret128, 2, 3, entropy));

    const shamir128::shares_t repeated{ shares.at(1), shares.at(1) };
    shamir128::secret_t out{};
    BC_REQUIRE(!shamir128::recover(out, repeated, 2));
}

BC_AUTO_TEST_CASE(shamir__recover__corrupted_share__false)
{
    const data_chunk entropy(shamir128::entropy_size(2), 0x42_u8);
    shamir128::shares_t shares{};
    BC_REQUIRE(shamir128::split(shares, secret128, 2, 3, entropy));

    auto corrupted = shares;
    corrupted.resize(2);
    corrupted.front().value.front() = bit_not(corrupted.front().value.front());

    shamir128::secret_t out{};
    BC_REQUIRE(!shamir128::recover(out, corrupted, 2));
}

// encrypt/decrypt

BC_AUTO_TEST_CASE(shamir__decrypt__encrypted__secret)
{
    const auto encrypted = shamir128::encrypt(secret128, "passphrase", 0, 1234, false);
    BC_REQUIRE_NE(encrypted, secret128);
    BC_REQUIRE_EQUAL(shamir128::decrypt(encrypted, "passphrase", 0, 1234, false), secret128);
}

BC_AUTO_TEST_CASE(shamir__decrypt__wrong_passphrase__not_secret)
{
    const auto encrypted = shamir128::encrypt(secret128, "passphrase", 0, 1234, false);
    BC_REQUIRE_NE(shamir128::decrypt(encrypted, "other", 0, 1234, false), secret128);
}

BC_AUTO_TEST_CASE(shamir__encrypt__extendable__differs)
{
    const auto bound = shamir128::encrypt(secret128, "", 0, 1234, false);
    const auto extendable = shamir128::encrypt(secret128, "", 0, 1234, true);
    BC_REQUIRE_NE(bound, extendable);
    BC_REQUIRE_EQUAL(shamir128::decrypt(extendable, "", 0, 1234, true), secret128);
}

BC_AUTO_TEST_CASE(shamir__encrypt__identifier__binds_unless_extendable)
{
    BC_REQUIRE_NE(shamir128::encrypt(secret128, "", 0, 1234, false), shamir128::encrypt(secret128, "", 0, 4321, false));
    BC_REQUIRE_EQUAL(shamir128::encrypt(secret128, "", 0, 1234, true), shamir128::encrypt(secret128, "", 0, 4321, true));
}

BC_AUTO_TEST_CASE(shamir__encrypt__exponent__differs)
{
    BC_REQUIRE_NE(shamir128::encrypt(secret128, "", 0, 1234, false), shamir128::encrypt(secret128, "", 1, 1234, false));
}

BC_AUTO_TEST_CASE(shamir__decrypt__256_bit__secret)
{
    const auto encrypted = shamir256::encrypt(secret256, "passphrase", 1, 7470, true);
    BC_REQUIRE_EQUAL(shamir256::decrypt(encrypted, "passphrase", 1, 7470, true), secret256);
}

// rs1024

BC_AUTO_TEST_CASE(rs1024__verify__checksummed__true)
{
    shamir::rs1024::words_t words{ 1, 2, 3, 4, 5 };
    const auto checksum = shamir::rs1024::checksum(words, false);
    words.insert(words.end(), checksum.cbegin(), checksum.cend());
    BC_REQUIRE(shamir::rs1024::verify(words, false));
}

BC_AUTO_TEST_CASE(rs1024__verify__other_customization__false)
{
    shamir::rs1024::words_t words{ 1, 2, 3, 4, 5 };
    const auto checksum = shamir::rs1024::checksum(words, false);
    words.insert(words.end(), checksum.cbegin(), checksum.cend());
    BC_REQUIRE(!shamir::rs1024::verify(words, true));
}

BC_AUTO_TEST_CASE(rs1024__verify__extendable_checksummed__true)
{
    shamir::rs1024::words_t words{ 1, 2, 3, 4, 5 };
    const auto checksum = shamir::rs1024::checksum(words, true);
    words.insert(words.end(), checksum.cbegin(), checksum.cend());
    BC_REQUIRE(shamir::rs1024::verify(words, true));
    BC_REQUIRE(!shamir::rs1024::verify(words, false));
}

BC_AUTO_TEST_CASE(rs1024__verify__mutated_word__false)
{
    shamir::rs1024::words_t words{ 1, 2, 3, 4, 5 };
    const auto checksum = shamir::rs1024::checksum(words, false);
    words.insert(words.end(), checksum.cbegin(), checksum.cend());
    words.front() = add1(words.front());
    BC_REQUIRE(!shamir::rs1024::verify(words, false));
}

BC_AUTO_TEST_CASE(rs1024__checksum__always__three_words)
{
    const shamir::rs1024::words_t words{ 1, 2, 3, 4, 5 };
    const auto checksum = shamir::rs1024::checksum(words, false);
    BC_REQUIRE_EQUAL(checksum.size(), 3u);
}

BC_AUTO_TEST_SUITE_END()
