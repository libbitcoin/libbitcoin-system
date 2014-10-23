/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(format_tests)

// encode_hex/decode_hex
// TODO: these should be tested for correctness, not just round-tripping.

BOOST_AUTO_TEST_CASE(encode_test_round_trips)
{
    const auto& hex_str = "10a7fd15cb45bda9e90e19a15f";
    BOOST_REQUIRE_EQUAL(encode_hex(decode_hex(hex_str)), hex_str);
}

BOOST_AUTO_TEST_CASE(encode_test_padded_round_trips_value)
{
    const auto& unpadded_hex = "10a7fd15cb45bda9e90e19a15f";
    const auto& padded_hex = "  \n\t10a7fd15cb45bda9e90e19a15f\n  \t";
    BOOST_REQUIRE_EQUAL(encode_hex(decode_hex(padded_hex)), unpadded_hex);
}

// from_little_endian/from_big_endian/to_little_endian/to_big_endian

BOOST_AUTO_TEST_CASE(endian_test)
{
    auto le = to_little_endian<uint32_t>(123456789);
    BOOST_REQUIRE(from_little_endian<uint32_t>(le.begin()) == 123456789);

    auto be = to_big_endian<uint32_t>(123456789);
    BOOST_REQUIRE(from_big_endian<uint32_t>(be.begin()) == 123456789);

    std::reverse(le.begin(), le.end());
    BOOST_REQUIRE(from_big_endian<uint32_t>(le.begin()) == 123456789);

    auto byte = to_little_endian<uint8_t>(0xff);
    BOOST_REQUIRE(from_big_endian<uint8_t>(byte.begin()) == 0xff);

    auto quad = to_little_endian<uint64_t>(0x1122334455667788);
    BOOST_REQUIRE(from_little_endian<uint64_t>(quad.begin()) ==
        0x1122334455667788);
}

// btc_to_satoshi

BOOST_AUTO_TEST_CASE(btc_to_satoshi_0_btc_test)
{
    uint64_t satoshi;
    BOOST_REQUIRE(btc_to_satoshi(satoshi, "0"));
    BOOST_REQUIRE_EQUAL(satoshi, 0);
}

BOOST_AUTO_TEST_CASE(btc_to_satoshi_42_btc_test)
{
    uint64_t satoshi;
    uint64_t expected = 42 * coin_price(1);
    BOOST_REQUIRE(btc_to_satoshi(satoshi, "42"));
    BOOST_REQUIRE_EQUAL(satoshi, expected);
}

BOOST_AUTO_TEST_CASE(btc_to_satoshi_max_money_test)
{
    uint64_t satoshi;
    uint64_t expected = max_money();
    BOOST_REQUIRE(btc_to_satoshi(satoshi, "20999999.9769"));
    BOOST_REQUIRE_EQUAL(satoshi, expected);
}

BOOST_AUTO_TEST_CASE(btc_to_satoshi_overflow_precision_test)
{
    uint64_t satoshi;
    BOOST_REQUIRE(!btc_to_satoshi(satoshi, "0.999999999"));
}

BOOST_AUTO_TEST_CASE(btc_to_satoshi_no_leading_zero_test)
{
    uint64_t satoshi;
    uint64_t expected = coin_price(1) / 2;
    BOOST_REQUIRE(btc_to_satoshi(satoshi, ".5"));
    BOOST_REQUIRE_EQUAL(satoshi, expected);
}

BOOST_AUTO_TEST_CASE(btc_to_satoshi_trailing_zero_test)
{
    uint64_t satoshi;
    uint64_t expected = coin_price(1) / 2;
    BOOST_REQUIRE(btc_to_satoshi(satoshi, "0.50"));
    BOOST_REQUIRE_EQUAL(satoshi, expected);
}

BOOST_AUTO_TEST_CASE(btc_to_satoshi_offset_zero_test)
{
    uint64_t satoshi;
    uint64_t expected = coin_price(1) / 20;
    BOOST_REQUIRE(btc_to_satoshi(satoshi, "0.05"));
    BOOST_REQUIRE_EQUAL(satoshi, expected);
}

BOOST_AUTO_TEST_CASE(btc_to_satoshi_max_int64_test)
{
    uint64_t satoshi;
    uint64_t expected = max_uint64;
    BOOST_REQUIRE(btc_to_satoshi(satoshi, "184467440737.09551615"));
    BOOST_REQUIRE_EQUAL(satoshi, expected);
}

BOOST_AUTO_TEST_CASE(btc_to_satoshi_overflow_int64_test)
{
    uint64_t satoshi;
    BOOST_REQUIRE(!btc_to_satoshi(satoshi, "184467440737.09551616"));
}

// satoshi_to_btc

BOOST_AUTO_TEST_CASE(satoshi_to_btc_0_satoshi_test)
{
    BOOST_REQUIRE_EQUAL(satoshi_to_btc(0), "0");
}

BOOST_AUTO_TEST_CASE(satoshi_to_btc_42_satoshi_test)
{
    uint64_t satoshi = 42 * coin_price(1);
    BOOST_REQUIRE_EQUAL(satoshi_to_btc(satoshi), "42");
}

BOOST_AUTO_TEST_CASE(satoshi_to_btc_max_money_test)
{
    uint64_t satoshi = max_money();
    BOOST_REQUIRE_EQUAL(satoshi_to_btc(satoshi), "20999999.9769");
}

BOOST_AUTO_TEST_CASE(satoshi_to_btc_overflow_max_money_test)
{
    uint64_t satoshi = max_money() + 1;
    BOOST_REQUIRE_EQUAL(satoshi_to_btc(satoshi), "20999999.97690001");
}

BOOST_AUTO_TEST_CASE(satoshi_to_btc_max_int64_test)
{
    uint64_t satoshi = max_uint64;
    BOOST_REQUIRE_EQUAL(satoshi_to_btc(satoshi), "184467440737.09551615");
}

BOOST_AUTO_TEST_SUITE_END()