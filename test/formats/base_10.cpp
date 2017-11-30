/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(base_10_tests)

#define TEST_AMOUNT(NAME, EXPECTED, ...) \
BOOST_AUTO_TEST_CASE(parse_amount_##NAME##_test) \
{ \
    uint64_t result, expected = EXPECTED; \
    BOOST_REQUIRE(decode_base10(result, __VA_ARGS__)); \
    BOOST_REQUIRE_EQUAL(result, expected); \
}

#define TEST_AMOUNT_NEGATIVE(NAME, ...) \
BOOST_AUTO_TEST_CASE(parse_amount_##NAME##_test) \
{ \
    uint64_t result; \
    BOOST_REQUIRE(!decode_base10(result, __VA_ARGS__)); \
}

#define TEST_FORMAT(NAME, EXPECTED, ...) \
BOOST_AUTO_TEST_CASE(format_amount_##NAME##_test) \
{ \
    std::string expected = EXPECTED; \
    std::string result = encode_base10(__VA_ARGS__); \
    BOOST_REQUIRE_EQUAL(result, expected); \
}

// Limits:
TEST_AMOUNT(zero, 0, "0")
TEST_AMOUNT(max_uint64, max_uint64, "18446744073709551615")

// Max money (mainnet, testnet):
TEST_AMOUNT(max_money_retarget, max_money(), "20999999.9769", btc_decimal_places)
TEST_AMOUNT(overflow_max_money_retarget, max_money() + 1, "20999999.97690001", btc_decimal_places)

// Max money (regtest):
TEST_AMOUNT(max_money, max_money(false), "14999.99998350", btc_decimal_places)
TEST_AMOUNT(overflow_max_money, max_money(false) + 1, "14999.99998351", btc_decimal_places)

// Decimal points:
TEST_AMOUNT(pure_integer, 42, "42.0", 0)
TEST_AMOUNT(no_decimal, 1000000, "10", mbtc_decimal_places)
TEST_AMOUNT(normal_decimal, 420000, "4.2", mbtc_decimal_places)
TEST_AMOUNT(leading_decimal, 50000, ".5", mbtc_decimal_places)
TEST_AMOUNT(trailing_decial, 500000, "5.", mbtc_decimal_places)
TEST_AMOUNT(extra_zeros, 1002000, "010.020", mbtc_decimal_places)
TEST_AMOUNT(harmless_zeros, 1, "0.0000100", mbtc_decimal_places)
TEST_AMOUNT(decimal_point_only, 0, ".")

// Rounding:
TEST_AMOUNT(pure_integer_rounding, 1, ".1", 0, false)
TEST_AMOUNT(rounding, 11, "0.101", ubtc_decimal_places, false)
TEST_AMOUNT(rounding_carry, 1000, "9.991", ubtc_decimal_places, false)
TEST_AMOUNT(zero_past_max, max_uint64, "18446744073709551615.0")

// Format errors:
TEST_AMOUNT_NEGATIVE(lexical_cast_fail, "0.-1")
TEST_AMOUNT_NEGATIVE(extra_decimal, "0.0.0")
TEST_AMOUNT_NEGATIVE(bad_characters, "0x0ff")

// Numeric errors:
TEST_AMOUNT_NEGATIVE(overflow, "18446744073709551616")
TEST_AMOUNT_NEGATIVE(rounding_overflow, "18446744073709551615.1", 0, false)
TEST_AMOUNT_NEGATIVE(fractional_amount, "0.999999999", btc_decimal_places)

// Limits:
TEST_FORMAT(zero, "0", 0)
TEST_FORMAT(max_uint64, "18446744073709551615", max_uint64)

// Max money (mainnet, testnet):
TEST_FORMAT(max_money_retarget, "20999999.9769", max_money(), btc_decimal_places)
TEST_FORMAT(overflow_max_money_retarget, "20999999.97690001", max_money() + 1, btc_decimal_places)

// Max money (regtest):
TEST_FORMAT(max_money, "14999.9999835", max_money(false), btc_decimal_places)
TEST_FORMAT(overflow_max_money, "14999.99998351", max_money(false) + 1, btc_decimal_places)

// Decimal points:
TEST_FORMAT(pure_integer, "42", 42, 0)
TEST_FORMAT(no_decimal, "10", 1000000, mbtc_decimal_places)
TEST_FORMAT(normal_decimal, "4.2", 420000, mbtc_decimal_places)
TEST_FORMAT(leading_zero, "0.42", 42000, mbtc_decimal_places)
TEST_FORMAT(internal_leading_zero, "0.042", 4200, mbtc_decimal_places)

BOOST_AUTO_TEST_SUITE_END()
