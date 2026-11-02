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

BC_AUTO_TEST_SUITE(base_32_tests)

// RFC 4648 section 10 test vectors.

BC_AUTO_TEST_CASE(base_32__encode_base32__empty__empty)
{
    BC_REQUIRE_EQUAL(encode_base32(data_chunk{}), "");
}

BC_AUTO_TEST_CASE(base_32__encode_base32__f__expected)
{
    BC_REQUIRE_EQUAL(encode_base32(to_chunk("f")), "MY======");
}

BC_AUTO_TEST_CASE(base_32__encode_base32__fo__expected)
{
    BC_REQUIRE_EQUAL(encode_base32(to_chunk("fo")), "MZXQ====");
}

BC_AUTO_TEST_CASE(base_32__encode_base32__foo__expected)
{
    BC_REQUIRE_EQUAL(encode_base32(to_chunk("foo")), "MZXW6===");
}

BC_AUTO_TEST_CASE(base_32__encode_base32__foob__expected)
{
    BC_REQUIRE_EQUAL(encode_base32(to_chunk("foob")), "MZXW6YQ=");
}

BC_AUTO_TEST_CASE(base_32__encode_base32__fooba__expected)
{
    BC_REQUIRE_EQUAL(encode_base32(to_chunk("fooba")), "MZXW6YTB");
}

BC_AUTO_TEST_CASE(base_32__encode_base32__foobar__expected)
{
    BC_REQUIRE_EQUAL(encode_base32(to_chunk("foobar")), "MZXW6YTBOI======");
}

BC_AUTO_TEST_CASE(base_32__decode_base32__empty__empty)
{
    data_chunk out;
    BC_REQUIRE(decode_base32(out, ""));
    BC_REQUIRE(out.empty());
}

BC_AUTO_TEST_CASE(base_32__decode_base32__padded__expected)
{
    data_chunk out;
    BC_REQUIRE(decode_base32(out, "MZXW6YTBOI======"));
    BC_REQUIRE(out == to_chunk("foobar"));
}

BC_AUTO_TEST_CASE(base_32__decode_base32__unpadded__expected)
{
    data_chunk out;
    BC_REQUIRE(decode_base32(out, "MZXW6YTBOI"));
    BC_REQUIRE(out == to_chunk("foobar"));
}

BC_AUTO_TEST_CASE(base_32__decode_base32__lower_case__expected)
{
    data_chunk out;
    BC_REQUIRE(decode_base32(out, "mzxw6ytboi"));
    BC_REQUIRE(out == to_chunk("foobar"));
}

BC_AUTO_TEST_CASE(base_32__decode_base32__unaligned__expected)
{
    data_chunk out;
    BC_REQUIRE(decode_base32(out, "MZXW6YQ="));
    BC_REQUIRE(out == to_chunk("foob"));
}

BC_AUTO_TEST_CASE(base_32__decode_base32__invalid_character__false)
{
    data_chunk out;
    BC_REQUIRE(!decode_base32(out, "MZXW6YTB0I"));
    BC_REQUIRE(!decode_base32(out, "MZXW6YTB1I"));
    BC_REQUIRE(!decode_base32(out, "MZXW6YTB8I"));
    BC_REQUIRE(!decode_base32(out, "MZXW6Y-BOI"));
}

BC_AUTO_TEST_CASE(base_32__decode_base32__invalid_length__false)
{
    data_chunk out;
    BC_REQUIRE(!decode_base32(out, "M"));
    BC_REQUIRE(!decode_base32(out, "MZX"));
    BC_REQUIRE(!decode_base32(out, "MZXW6Y"));
}

BC_AUTO_TEST_CASE(base_32__decode_base32__invalid_padding__false)
{
    data_chunk out;
    BC_REQUIRE(!decode_base32(out, "MY====="));
    BC_REQUIRE(!decode_base32(out, "MY======="));
    BC_REQUIRE(!decode_base32(out, "MZXW6YTB========"));
    BC_REQUIRE(!decode_base32(out, "MY==MZXQ"));
}

BC_AUTO_TEST_CASE(base_32__decode_base32__nonzero_trailing_bits__false)
{
    // Two characters carry ten bits, the trailing two must be zero.
    data_chunk out;
    BC_REQUIRE(!decode_base32(out, "MZ"));
}

BC_AUTO_TEST_CASE(base_32__round_trip__thirty_two_bytes__expected)
{
    // The i2p b32 payload size, 52 characters unpadded.
    const data_chunk data(32, 0xab);
    const auto encoded = encode_base32(data);
    BC_REQUIRE_EQUAL(encoded.size(), 56u);
    BC_REQUIRE_EQUAL(encoded.substr(52), "====");

    data_chunk out;
    BC_REQUIRE(decode_base32(out, encoded.substr(0, 52)));
    BC_REQUIRE(out == data);
}

BC_AUTO_TEST_CASE(base_32__round_trip__thirty_five_bytes__expected)
{
    // The onion v3 payload size, 56 characters, no padding.
    const data_chunk data(35, 0xcd);
    const auto encoded = encode_base32(data);
    BC_REQUIRE_EQUAL(encoded.size(), 56u);
    BC_REQUIRE_EQUAL(encoded.find('='), std::string::npos);

    data_chunk out;
    BC_REQUIRE(decode_base32(out, encoded));
    BC_REQUIRE(out == data);
}

BC_AUTO_TEST_SUITE_END()
