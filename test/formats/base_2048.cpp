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

#ifdef UNDEFINED

// decode_base2048

BOOST_AUTO_TEST_CASE(base_2048__decode_base2048__vector00__expected)
{
    const auto encoded = "";
    const auto decoded = to_chunk(base16_literal(""));
    data_chunk out;
    BOOST_REQUIRE(decode_base2048(out, encoded));
    BOOST_REQUIRE_EQUAL(out, decoded);
}

BOOST_AUTO_TEST_CASE(base_2048__decode_base2048__vector01__expected)
{
    const auto encoded = "pu";
    const auto decoded = to_chunk(base16_literal("0f"));
    data_chunk out;
    BOOST_REQUIRE(decode_base2048(out, encoded));
    BOOST_REQUIRE_EQUAL(out, decoded);
}

BOOST_AUTO_TEST_CASE(base_2048__decode_base2048__vector02__expected)
{
    const auto encoded = "qyhs";
    const auto decoded = to_chunk(base16_literal("012f"));
    data_chunk out;
    BOOST_REQUIRE(decode_base2048(out, encoded));
    BOOST_REQUIRE_EQUAL(out, decoded);
}

BOOST_AUTO_TEST_CASE(base_2048__decode_base2048__vector03__expected)
{
    const auto encoded = "qy357";
    const auto decoded = to_chunk(base16_literal("01234f"));
    data_chunk out;
    BOOST_REQUIRE(decode_base2048(out, encoded));
    BOOST_REQUIRE_EQUAL(out, decoded);
}

BOOST_AUTO_TEST_CASE(base_2048__decode_base2048__vector04__expected)
{
    const auto encoded = "qy352mc";
    const auto decoded = to_chunk(base16_literal("0123456f"));
    data_chunk out;
    BOOST_REQUIRE(decode_base2048(out, encoded));
    BOOST_REQUIRE_EQUAL(out, decoded);
}

// Minimum unpadded (non-zero) size.
BOOST_AUTO_TEST_CASE(base_2048__decode_base2048__vector05__expected)
{
    const auto encoded = "qy352eu0";
    const auto decoded = to_chunk(base16_literal("012345678f"));
    data_chunk out;
    BOOST_REQUIRE(decode_base2048(out, encoded));
    BOOST_REQUIRE_EQUAL(out, decoded);
}

// encode_base2048

BOOST_AUTO_TEST_CASE(base_2048__encode_base2048__vector00__expected)
{
    const auto encoded = "";
    const auto decoded = to_chunk(base16_literal(""));
    BOOST_REQUIRE_EQUAL(encode_base2048(decoded), encoded);
}

BOOST_AUTO_TEST_CASE(base_2048__encode_base2048__vector01__expected)
{
    const auto encoded = "pu";
    const auto decoded = to_chunk(base16_literal("0f"));
    BOOST_REQUIRE_EQUAL(encode_base2048(decoded), encoded);
}

BOOST_AUTO_TEST_CASE(base_2048__encode_base2048__vector02__expected)
{
    const auto encoded = "qyhs";
    const auto decoded = to_chunk(base16_literal("012f"));
    BOOST_REQUIRE_EQUAL(encode_base2048(decoded), encoded);
}

BOOST_AUTO_TEST_CASE(base_2048__encode_base2048__vector03__expected)
{
    const auto encoded = "qy357";
    const auto decoded = to_chunk(base16_literal("01234f"));
    BOOST_REQUIRE_EQUAL(encode_base2048(decoded), encoded);
}

BOOST_AUTO_TEST_CASE(base_2048__encode_base2048__vector04__expected)
{
    const auto encoded = "qy352mc";
    const auto decoded = to_chunk(base16_literal("0123456f"));
    BOOST_REQUIRE_EQUAL(encode_base2048(decoded), encoded);
}

// Minimum unpadded (non-zero) size.
BOOST_AUTO_TEST_CASE(base_2048__encode_base2048__vector05__expected)
{
    const auto encoded = "qy352eu0";
    const auto decoded = to_chunk(base16_literal("012345678f"));
    BOOST_REQUIRE_EQUAL(encode_base2048(decoded), encoded);
}

// base2048_compact

BOOST_AUTO_TEST_CASE(base_2048__base2048_compact__vector00__expected)
{
    // []=>[]
    const base2048_chunk expanded{};
    const data_chunk expected{};
    data_chunk out;
    BOOST_REQUIRE_EQUAL(base2048_compact(expanded), expected);
}

BOOST_AUTO_TEST_CASE(base_2048__base2048_compact__vector01__expected)
{
    // [11111][111pp]=>
    // [11111111]
    const base2048_chunk expanded{ 0x1f, 0x1c };
    const data_chunk expected(1, 0xff);
    BOOST_REQUIRE_EQUAL(base2048_compact(expanded), expected);
}

BOOST_AUTO_TEST_CASE(base_2048__base2048_compact__vector02__expected)
{
    // [11111][11111][11111][1pppp]=>
    // [11111111][11111111]
    const base2048_chunk expanded{ 0x1f, 0x1f, 0x1f, 0x10 };
    const data_chunk expected(2, 0xff);
    BOOST_REQUIRE_EQUAL(base2048_compact(expanded), expected);
}

BOOST_AUTO_TEST_CASE(base_2048__base2048_compact__vector03__expected)
{
    // [11111][11111][11111][11111][1111p]=>
    // [11111111][11111111][11111111]
    const base2048_chunk expanded{ 0x1f, 0x1f, 0x1f, 0x1f, 0x1e };
    const data_chunk expected(3, 0xff);
    BOOST_REQUIRE_EQUAL(base2048_compact(expanded), expected);
}

BOOST_AUTO_TEST_CASE(base_2048__base2048_compact__vector04__expected)
{
    // [11111][11111][11111][11111][11111][11111][11ppp]=>
    // [11111111][11111111][11111111][11111111]
    const base2048_chunk expanded{ 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x18 };
    const data_chunk expected(4, 0xff);
    BOOST_REQUIRE_EQUAL(base2048_compact(expanded), expected);
}

// Minimum unpadded (non-zero) size.
BOOST_AUTO_TEST_CASE(base_2048__base2048_compact__vector05__expected)
{
    // [11111][11111][11111][11111][11111][11111][11111][11111]=>
    // [11111111][11111111][11111111][11111111][11111111]
    const base2048_chunk expanded(8, 0x1f);
    const data_chunk expected((expanded.size() * 5) / 8, 0xff);
    BOOST_REQUIRE_EQUAL(base2048_compact(expanded), expected);
}

// base2048_expand

BOOST_AUTO_TEST_CASE(base_2048__base2048_expand__vector00__expected)
{
    // []=>[]
    const data_chunk compacted{};
    const base2048_chunk expected{};
    BOOST_REQUIRE_EQUAL(base2048_expand(compacted), expected);
}

BOOST_AUTO_TEST_CASE(base_2048__base2048_expand__vector01__expected)
{
    // [11111111]=>
    // [11111][111pp]
    const data_chunk compacted(1, 0xff);
    const base2048_chunk expected{ 0x1f, 0x1c };
    BOOST_REQUIRE_EQUAL(base2048_expand(compacted), expected);
}

BOOST_AUTO_TEST_CASE(base_2048__base2048_expand__vector02__expected)
{
    // [11111111][11111111]=>
    // [11111][11111][11111][1pppp]
    const data_chunk compacted(2, 0xff);
    const base2048_chunk expected{ 0x1f, 0x1f, 0x1f, 0x10 };
    BOOST_REQUIRE_EQUAL(base2048_expand(compacted), expected);
}

BOOST_AUTO_TEST_CASE(base_2048__base2048_expand__vector03__expected)
{
    // [11111111][11111111][11111111]=>
    // [11111][11111][11111][11111][1111p]
    const data_chunk compacted(3, 0xff);
    const base2048_chunk expected{ 0x1f, 0x1f, 0x1f, 0x1f, 0x1e };
    BOOST_REQUIRE_EQUAL(base2048_expand(compacted), expected);
}

BOOST_AUTO_TEST_CASE(base_2048__base2048_expand__vector04__expected)
{
    // [11111111][11111111][11111111][11111111]=>
    // [11111][11111][11111][11111][11111][11111][11ppp]
    const data_chunk compacted(4, 0xff);
    const base2048_chunk expected{ 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x18 };
    BOOST_REQUIRE_EQUAL(base2048_expand(compacted), expected);
}

// Minimum unpadded (non-zero) size.
BOOST_AUTO_TEST_CASE(base_2048__base2048_expand__vector05__expected)
{
    // [11111111][11111111][11111111][11111111][11111111]=>
    // [11111][11111][11111][11111][11111][11111][11111][11111]
    const data_chunk compacted(5, 0xff);
    const base2048_chunk expected((compacted.size() * 8) / 5, 0x1f);
    BOOST_REQUIRE_EQUAL(base2048_expand(compacted), expected);
}

#endif

BOOST_AUTO_TEST_SUITE_END()
