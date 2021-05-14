/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(base_2048_tests)

// decode_base2048_list

BOOST_AUTO_TEST_CASE(base_2048__decode_base2048_list__vector0__expected)
{
    // []=>[]
    const auto encoded = base16_chunk("");
    const string_list decoded{};
    BOOST_REQUIRE_EQUAL(decode_base2048_list(encoded), decoded);
}

BOOST_AUTO_TEST_CASE(base_2048__decode_base2048_list__vector1__expected)
{
    // [00000000][00000000]=>
    // [00000000000]
    const auto encoded = base16_chunk("0000");
    const string_list decoded{ "abandon" };
    BOOST_REQUIRE_EQUAL(decode_base2048_list(encoded), decoded);
}

BOOST_AUTO_TEST_CASE(base_2048__decode_base2048_list__vector2__expected)
{
    // [00000000][00000000][00000100]=>
    // [00000000000][00000000001]
    const auto encoded = base16_chunk("000004");
    const string_list decoded{ "abandon", "ability" };
    BOOST_REQUIRE_EQUAL(decode_base2048_list(encoded), decoded);
}

// encode_base2048_list

BOOST_AUTO_TEST_CASE(base_2048__encode_base2048_list__vector0__expected)
{
    // []=>[]
    const string_list decoded{};
    const auto encoded = base16_chunk("");
    data_chunk out;
    BOOST_REQUIRE(encode_base2048_list(out, decoded));
    BOOST_REQUIRE_EQUAL(out, encoded);
}

BOOST_AUTO_TEST_CASE(base_2048__encode_base2048_list__vector1__expected)
{
    // [00000000000]=>
    // [00000000][00000000]
    const string_list decoded{ "abandon" };
    const auto encoded = base16_chunk("0000");
    data_chunk out;
    BOOST_REQUIRE(encode_base2048_list(out, decoded));
    BOOST_REQUIRE_EQUAL(out, encoded);
}

BOOST_AUTO_TEST_CASE(base_2048__encode_base2048_list__vector2__expected)
{
    // [00000000000][00000000001]=>
    // [00000000][00000000][00000100]
    const string_list decoded{ "abandon", "ability" };
    const auto encoded = base16_chunk("000004");
    data_chunk out;
    BOOST_REQUIRE(encode_base2048_list(out, decoded));
    BOOST_REQUIRE_EQUAL(out, encoded);
}

// base2048_unpack

BOOST_AUTO_TEST_CASE(base_2048__base2048_unpack__vector0__expected)
{
    // []=>[]
    const data_chunk expected{};
    const base2048_chunk packed{};
    data_chunk out;
    BOOST_REQUIRE_EQUAL(base2048_unpack(packed), expected);
}

BOOST_AUTO_TEST_CASE(base_2048__base2048_unpack__vector1__expected)
{
    // [11111111111]=>
    // [11111111][111ppppp]
    const base2048_chunk packed(1, 0x07ff);
    const data_chunk expected{ 0xff, 0xe0 };
    BOOST_REQUIRE_EQUAL(base2048_unpack(packed), expected);
}

BOOST_AUTO_TEST_CASE(base_2048__base2048_unpack__vector2__expected)
{
    // [11111111111][11111111111]=>
    // [11111111][11111111][111111pp]
    const base2048_chunk packed(2, 0x07ff);
    const data_chunk expected{ 0xff, 0xff, 0xfc };
    BOOST_REQUIRE_EQUAL(base2048_unpack(packed), expected);
}

BOOST_AUTO_TEST_CASE(base_2048__base2048_unpack__vector3__expected)
{
    // [11111111111][11111111111][11111111111]=>
    // [11111111][11111111][11111111][11111111][1ppppppp]
    const base2048_chunk packed(3, 0x07ff);
    const data_chunk expected{ 0xff, 0xff, 0xff, 0xff, 0x80 };
    BOOST_REQUIRE_EQUAL(base2048_unpack(packed), expected);
}

BOOST_AUTO_TEST_CASE(base_2048__base2048_unpack__vector4__expected)
{
    // [11111111111][11111111111][11111111111][11111111111]=>
    // [11111111][11111111][11111111][11111111][11111111][1111pppp]
    const base2048_chunk packed(4, 0x07ff);
    const data_chunk expected{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0 };
    BOOST_REQUIRE_EQUAL(base2048_unpack(packed), expected);
}

BOOST_AUTO_TEST_CASE(base_2048__base2048_unpack__vector5__expected)
{
    // [11111111111][11111111111][11111111111][11111111111][11111111111]=>
    // [11111111][11111111][11111111][11111111][11111111][11111111][1111111p]
    const base2048_chunk packed(5, 0x07ff);
    const data_chunk expected{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe };
    BOOST_REQUIRE_EQUAL(base2048_unpack(packed), expected);
}

BOOST_AUTO_TEST_CASE(base_2048__base2048_unpack__vector6__expected)
{
    // [11111111111][11111111111][11111111111][11111111111][11111111111][11111111111]=>
    // [11111111][11111111][11111111][11111111][11111111][11111111][11111111][11111111][11pppppp]
    const base2048_chunk packed(6, 0x07ff);
    const data_chunk expected{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0 };
    BOOST_REQUIRE_EQUAL(base2048_unpack(packed), expected);
}

BOOST_AUTO_TEST_CASE(base_2048__base2048_unpack__vector7__expected)
{
    // [11111111111][11111111111][11111111111][11111111111][11111111111][11111111111][11111111111]=>
    // [11111111][11111111][11111111][11111111][11111111][11111111][11111111][11111111][11111111][11111ppp]
    const base2048_chunk packed(7, 0x07ff);
    const data_chunk expected{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8 };
    BOOST_REQUIRE_EQUAL(base2048_unpack(packed), expected);
}

// Minimum unpadded (non-zero) size.
BOOST_AUTO_TEST_CASE(base_2048__base2048_unpack__vector8__expected)
{
    // [11111111111][11111111111][11111111111][11111111111][11111111111][11111111111][11111111111][11111111111]=>
    // [11111111][11111111][11111111][11111111][11111111][11111111][11111111][11111111][11111111][11111111][11111111]
    const base2048_chunk packed(8, 0x07ff);
    const data_chunk expected((packed.size() * 11) / 8, 0xff);
    BOOST_REQUIRE_EQUAL(base2048_unpack(packed), expected);
}

// base2048_pack

BOOST_AUTO_TEST_CASE(base_2048__base2048_pack__vector0__expected)
{
    // []=>[]
    const data_chunk unpacked{};
    const base2048_chunk expected{};
    data_chunk out;
    BOOST_REQUIRE_EQUAL(base2048_pack(unpacked), expected);
}

BOOST_AUTO_TEST_CASE(base_2048__base2048_pack__vector1__expected)
{
    // [11111111][111xxxxx]=>
    // [11111111111][xxxxxpppppp]
    const data_chunk unpacked{ 0xff, 0xe0 };
    const base2048_chunk expected(1, 0x07ff);
    BOOST_REQUIRE_EQUAL(base2048_pack(unpacked), expected);
}

BOOST_AUTO_TEST_CASE(base_2048__base2048_pack__vector2__expected)
{
    // [11111111][11111111][111111xx]=>
    // [11111111111][11111111111][xxppppppppp]
    const data_chunk unpacked{ 0xff, 0xff, 0xfc };
    const base2048_chunk expected(2, 0x07ff);
    BOOST_REQUIRE_EQUAL(base2048_pack(unpacked), expected);
}

BOOST_AUTO_TEST_CASE(base_2048__base2048_pack__vector3__expected)
{
    // [11111111][11111111][11111111][11111111][1xxxxxxx]=>
    // [11111111111][11111111111][11111111111][xxxxxxxpppp]
    const data_chunk unpacked{ 0xff, 0xff, 0xff, 0xff, 0x80 };
    const base2048_chunk expected(3, 0x07ff);
    BOOST_REQUIRE_EQUAL(base2048_pack(unpacked), expected);
}

BOOST_AUTO_TEST_CASE(base_2048__base2048_pack__vector4__expected)
{
    // [11111111][11111111][11111111][11111111][11111111][1111xxxx]=>
    // [11111111111][11111111111][11111111111][11111111111][xxxxppppppp]
    const data_chunk unpacked{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0 };
    const base2048_chunk expected(4, 0x07ff);
    BOOST_REQUIRE_EQUAL(base2048_pack(unpacked), expected);
}

BOOST_AUTO_TEST_CASE(base_2048__base2048_pack__vector5__expected)
{
    // [11111111][11111111][11111111][11111111][11111111][11111111][1111111x]=>
    // [11111111111][11111111111][11111111111][11111111111][11111111111][xpppppppppp]
    const data_chunk unpacked{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe };
    const base2048_chunk expected(5, 0x07ff);
    BOOST_REQUIRE_EQUAL(base2048_pack(unpacked), expected);
}

BOOST_AUTO_TEST_CASE(base_2048__base2048_pack__vector6__expected)
{
    // [11111111][11111111][11111111][11111111][11111111][11111111][11111111][11111111][11xxxxxx]=>
    // [11111111111][11111111111][11111111111][11111111111][11111111111][11111111111][xxxxxxppppp]
    const data_chunk unpacked{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0 };
    const base2048_chunk expected(6, 0x07ff);
    BOOST_REQUIRE_EQUAL(base2048_pack(unpacked), expected);
}

BOOST_AUTO_TEST_CASE(base_2048__base2048_pack__vector7__expected)
{
    // [11111111][11111111][11111111][11111111][11111111][11111111][11111111][11111111][11111111][11111ppp]=>
    // [11111111111][11111111111][11111111111][11111111111][11111111111][11111111111][11111111111][xxxpppppppp]
    const data_chunk unpacked{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8 };
    const base2048_chunk expected(7, 0x07ff);
    BOOST_REQUIRE_EQUAL(base2048_pack(unpacked), expected);
}

// Minimum unpadded (non-zero) size.
BOOST_AUTO_TEST_CASE(base_2048__base2048_pack__vector8__expected)
{
    // [11111111][11111111][11111111][11111111][11111111][11111111][11111111][11111111][11111111][11111111][11111111]=>
    // [11111111111][11111111111][11111111111][11111111111][11111111111][11111111111][11111111111][11111111111]
    const base2048_chunk expected(8, 0x07ff);
    const data_chunk unpacked((expected.size() * 11) / 8, 0xff);
    BOOST_REQUIRE_EQUAL(base2048_pack(unpacked), expected);
}

BOOST_AUTO_TEST_SUITE_END()
