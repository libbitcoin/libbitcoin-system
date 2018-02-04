/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(base_32_tests)

bool decode_test(const std::string& in)
{
    std::string hr_part;
    data_chunk values;
    return decode_base32(hr_part, values, in);
}

void encode_decode_test(const std::string& hr, const data_chunk& values)
{
    const std::string encoded = encode_base32(hr, values);
    std::string hr_decoded;
    data_chunk values_decoded;
    BOOST_REQUIRE(decode_base32(hr_decoded, values_decoded, encoded));
    BOOST_REQUIRE_EQUAL(hr, hr_decoded);
    BOOST_REQUIRE_EQUAL(values, values_decoded);
}

BOOST_AUTO_TEST_CASE(encode_base32__encode__expected)
{
    BOOST_REQUIRE_EQUAL(encode_base32("a", data_chunk()), std::string("a12uel5l"));
    BOOST_REQUIRE_EQUAL(encode_base32("an83characterlonghumanreadablepartthatcontainsthenumber1andtheexcludedcharactersbio", data_chunk()), 
        std::string("an83characterlonghumanreadablepartthatcontainsthenumber1andtheexcludedcharactersbio1tt5tgs"));
}

BOOST_AUTO_TEST_CASE(decode_base32__decode__true)
{
    // Valid inputs, decoding must be successful
    BOOST_REQUIRE(decode_test("a12uel5l"));
    BOOST_REQUIRE(decode_test("A12UEL5L"));
    BOOST_REQUIRE(decode_test("an83characterlonghumanreadablepartthatcontainsthenumber1andtheexcludedcharactersbio1tt5tgs"));
    BOOST_REQUIRE(decode_test("abcdef1qpzry9x8gf2tvdw0s3jn54khce6mua7lmqqqxw"));
    BOOST_REQUIRE(decode_test("11qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqc8247j"));
    BOOST_REQUIRE(decode_test("split1checkupstagehandshakeupstreamerranterredcaperred2y9e3w"));
    BOOST_REQUIRE(decode_test("?1ezyfcl"));
}

BOOST_AUTO_TEST_CASE(decode_base32__decode__false)
{
    // Invalid inputs must be recognized by decoder
    BOOST_REQUIRE(!decode_test("\x20""1nwldj5"));
    BOOST_REQUIRE(!decode_test("\x7F""1axkwrx"));
    BOOST_REQUIRE(!decode_test("\x80""1eym55h"));
    BOOST_REQUIRE(!decode_test("an84characterslonghumanreadablepartthatcontainsthenumber1andtheexcludedcharactersbio1569pvx"));
    BOOST_REQUIRE(!decode_test("pzry9x0s0muk"));
    BOOST_REQUIRE(!decode_test("1pzry9x0s0muk"));
    BOOST_REQUIRE(!decode_test("x1b4n0q5v"));
    BOOST_REQUIRE(!decode_test("li1dgmt3"));
    BOOST_REQUIRE(!decode_test("de1lg7wt\xFF"));
    BOOST_REQUIRE(!decode_test("A1G7SGD8"));
    BOOST_REQUIRE(!decode_test("10a06t8"));
    BOOST_REQUIRE(!decode_test("1qzzfhee"));
}

BOOST_AUTO_TEST_CASE(encode_decode_base32__true)
{
    // Encode and decode, input must match output
    encode_decode_test("a", {});
    encode_decode_test("an83characterlonghumanreadablepartthatcontainsthenumber1andtheexcludedcharactersbio", {});
    encode_decode_test("test", {0,1,2,3,4,5,6,7,8,9,10});
    encode_decode_test("someothertest42", {12,12,12,0,0,1,1,0,1,1,1,0});
    encode_decode_test("1startswith1endswith1", {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31});
}

BOOST_AUTO_TEST_SUITE_END()
