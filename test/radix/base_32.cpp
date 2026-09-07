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

BOOST_AUTO_TEST_SUITE(base_32_tests)

// RFC 4648 section 10 test vectors.

BOOST_AUTO_TEST_CASE(base_32__encode_base32__empty__empty)
{
    BOOST_REQUIRE_EQUAL(encode_base32(data_chunk{}), "");
}

BOOST_AUTO_TEST_CASE(base_32__encode_base32__f__expected)
{
    BOOST_REQUIRE_EQUAL(encode_base32(to_chunk("f")), "MY======");
}

BOOST_AUTO_TEST_CASE(base_32__encode_base32__fo__expected)
{
    BOOST_REQUIRE_EQUAL(encode_base32(to_chunk("fo")), "MZXQ====");
}

BOOST_AUTO_TEST_CASE(base_32__encode_base32__foo__expected)
{
    BOOST_REQUIRE_EQUAL(encode_base32(to_chunk("foo")), "MZXW6===");
}

BOOST_AUTO_TEST_CASE(base_32__encode_base32__foob__expected)
{
    BOOST_REQUIRE_EQUAL(encode_base32(to_chunk("foob")), "MZXW6YQ=");
}

BOOST_AUTO_TEST_CASE(base_32__encode_base32__fooba__expected)
{
    BOOST_REQUIRE_EQUAL(encode_base32(to_chunk("fooba")), "MZXW6YTB");
}

BOOST_AUTO_TEST_CASE(base_32__encode_base32__foobar__expected)
{
    BOOST_REQUIRE_EQUAL(encode_base32(to_chunk("foobar")), "MZXW6YTBOI======");
}

BOOST_AUTO_TEST_CASE(base_32__decode_base32__empty__empty)
{
    data_chunk out;
    BOOST_REQUIRE(decode_base32(out, ""));
    BOOST_REQUIRE(out.empty());
}

BOOST_AUTO_TEST_CASE(base_32__decode_base32__padded__expected)
{
    data_chunk out;
    BOOST_REQUIRE(decode_base32(out, "MZXW6YTBOI======"));
    BOOST_REQUIRE(out == to_chunk("foobar"));
}

BOOST_AUTO_TEST_CASE(base_32__decode_base32__unpadded__expected)
{
    data_chunk out;
    BOOST_REQUIRE(decode_base32(out, "MZXW6YTBOI"));
    BOOST_REQUIRE(out == to_chunk("foobar"));
}

BOOST_AUTO_TEST_CASE(base_32__decode_base32__lower_case__expected)
{
    data_chunk out;
    BOOST_REQUIRE(decode_base32(out, "mzxw6ytboi"));
    BOOST_REQUIRE(out == to_chunk("foobar"));
}

BOOST_AUTO_TEST_CASE(base_32__decode_base32__unaligned__expected)
{
    data_chunk out;
    BOOST_REQUIRE(decode_base32(out, "MZXW6YQ="));
    BOOST_REQUIRE(out == to_chunk("foob"));
}

BOOST_AUTO_TEST_CASE(base_32__decode_base32__invalid_character__false)
{
    data_chunk out;
    BOOST_REQUIRE(!decode_base32(out, "MZXW6YTB0I"));
    BOOST_REQUIRE(!decode_base32(out, "MZXW6YTB1I"));
    BOOST_REQUIRE(!decode_base32(out, "MZXW6YTB8I"));
    BOOST_REQUIRE(!decode_base32(out, "MZXW6Y-BOI"));
}

BOOST_AUTO_TEST_CASE(base_32__decode_base32__invalid_length__false)
{
    data_chunk out;
    BOOST_REQUIRE(!decode_base32(out, "M"));
    BOOST_REQUIRE(!decode_base32(out, "MZX"));
    BOOST_REQUIRE(!decode_base32(out, "MZXW6Y"));
}

BOOST_AUTO_TEST_CASE(base_32__decode_base32__invalid_padding__false)
{
    data_chunk out;
    BOOST_REQUIRE(!decode_base32(out, "MY====="));
    BOOST_REQUIRE(!decode_base32(out, "MY======="));
    BOOST_REQUIRE(!decode_base32(out, "MZXW6YTB========"));
    BOOST_REQUIRE(!decode_base32(out, "MY==MZXQ"));
}

BOOST_AUTO_TEST_CASE(base_32__decode_base32__nonzero_trailing_bits__false)
{
    // Two characters carry ten bits, the trailing two must be zero.
    data_chunk out;
    BOOST_REQUIRE(!decode_base32(out, "MZ"));
}

BOOST_AUTO_TEST_CASE(base_32__round_trip__thirty_two_bytes__expected)
{
    // The i2p b32 payload size, 52 characters unpadded.
    const data_chunk data(32, 0xab);
    const auto encoded = encode_base32(data);
    BOOST_REQUIRE_EQUAL(encoded.size(), 56u);
    BOOST_REQUIRE_EQUAL(encoded.substr(52), "====");

    data_chunk out;
    BOOST_REQUIRE(decode_base32(out, encoded.substr(0, 52)));
    BOOST_REQUIRE(out == data);
}

BOOST_AUTO_TEST_CASE(base_32__round_trip__thirty_five_bytes__expected)
{
    // The onion v3 payload size, 56 characters, no padding.
    const data_chunk data(35, 0xcd);
    const auto encoded = encode_base32(data);
    BOOST_REQUIRE_EQUAL(encoded.size(), 56u);
    BOOST_REQUIRE_EQUAL(encoded.find('='), std::string::npos);

    data_chunk out;
    BOOST_REQUIRE(decode_base32(out, encoded));
    BOOST_REQUIRE(out == data);
}

BOOST_AUTO_TEST_SUITE_END()
