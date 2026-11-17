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

BC_AUTO_TEST_SUITE(sponge_tests)

// SHAKE256 (FIPS 202 examples, otherwise hashlib.shake_256).

constexpr auto empty32 = base16_array("46b9dd2b0ba88d13233b3feb743eeb243fcd52ea62b81b82b50c27646ed5762f");
constexpr auto abc64 = base16_array("483366601360a8771c6863080cc4114d8db44530f8f1e1ee4f94ea37e78b5739d5a15bef186a5386c75744c0527e1faa9f8726e462a12a4feb06bd8801e751e4");
constexpr auto long32 = base16_array("4d8c2dd2435a0128eefbb8c36f6f87133a7911e18d979ee1ae6be5d4fd2e3329");
constexpr auto block32 = base16_array("ea947b835fec1f9b0a7eabba901deb7881fd9999a1cbd5ccbb5a9afab7f6fe70");
constexpr auto space32 = base16_array("4a6c0970c326babfaeef17f91988d1b4c5e95ed584c21b55b9f92e0d3671ddf9");
constexpr auto million32 = base16_array("3578a7a4ca9137569cdf76ed617d31bb994fca9c1bbf8b184013de8234dfd13a");
constexpr auto a300_100 = base16_array("542e4338ca18e43776834307a2b6158f70c14f22400081b2dc345cc54a6298cd125383fc7cc5e05937618e0474a71308d82cbd6bacd48a6d69f540bd72f2b19e41f006067f6f782d5f8bf1fe6660fe84a6b2f402aa545efb76ff39cd85030ad8a31bc3ae");
constexpr auto empty300 = base16_array(
    "46b9dd2b0ba88d13233b3feb743eeb243fcd52ea62b81b82b50c27646ed5762f"
    "d75dc4ddd8c0f200cb05019d67b592f6fc821c49479ab48640292eacb3b7c4be"
    "141e96616fb13957692cc7edd0b45ae3dc07223c8e92937bef84bc0eab862853"
    "349ec75546f58fb7c2775c38462c5010d846c185c15111e595522a6bcd16cf86"
    "f3d122109e3b1fdd943b6aec468a2d621a7c06c6a957c62b54dafc3be87567d6"
    "77231395f6147293b68ceab7a9e0c58d864e8efde4e1b9a46cbe854713672f5c"
    "aaae314ed9083dab4b099f8e300f01b8650f1f4b1d8fcf3f3cb53fb8e9eb2ea2"
    "03bdc970f50ae55428a91f7f53ac266b28419c3778a15fd248d339ede785fb7f"
    "5a1aaa96d313eacc890936c173cdcd0fab882c45755feb3aed96d477ff96390b"
    "f9a66d1368b208e21f7c10d0");

constexpr std_array<uint8_t, 3> abc{ 'a', 'b', 'c' };
constexpr auto long_message = to_array(
    "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");

// flush

BC_AUTO_TEST_CASE(sponge__flush__sha3_256_abc__simple_hash)
{
    sponge<sha3_256> hasher{};
    hasher.write(abc);
    BC_REQUIRE_EQUAL(hasher.flush(), sha3_256::simple_hash(abc));
}

BC_AUTO_TEST_CASE(sponge__flush__shake_256_empty__expected)
{
    sponge<shake_256> xof{};
    BC_REQUIRE_EQUAL(xof.flush(), empty32);
}

BC_AUTO_TEST_CASE(sponge__flush__shake_256_448_bit_message__expected)
{
    sponge<shake_256> xof{};
    xof.write(long_message);
    BC_REQUIRE_EQUAL(xof.flush(), long32);
}

BC_AUTO_TEST_CASE(sponge__flush__after_squeeze__continues_output)
{
    data_array<4> head{};
    sponge<shake_256> xof{};
    xof.squeeze(head);
    const auto next = xof.flush();
    const auto& expected_head = array_cast<uint8_t, 4>(empty300);
    const auto& expected_next = array_cast<uint8_t, 32, 4>(empty300);

    BC_REQUIRE_EQUAL(head, expected_head);
    BC_REQUIRE_EQUAL(next, expected_next);
}

// squeeze

BC_AUTO_TEST_CASE(sponge__squeeze__abc__expected)
{
    data_array<64> out{};
    sponge<shake_256> xof{};
    xof.write(abc);
    xof.squeeze(out);
    BC_REQUIRE_EQUAL(out, abc64);
}

BC_AUTO_TEST_CASE(sponge__squeeze__three_blocks__expected)
{
    data_array<300> out{};
    sponge<shake_256> xof{};
    xof.squeeze(out);
    BC_REQUIRE_EQUAL(out, empty300);
}

BC_AUTO_TEST_CASE(sponge__squeeze__split__matches_whole)
{
    data_array<300> out{};
    const byte_span span{ out };
    sponge<shake_256> xof{};
    xof.squeeze(span.first(one));
    xof.squeeze(span.subspan(one, sub1(shake_256::block_bytes)));
    xof.squeeze(span.subspan(shake_256::block_bytes));
    BC_REQUIRE_EQUAL(out, empty300);
}

// write

BC_AUTO_TEST_CASE(sponge__write__rate_block__expected)
{
    constexpr shake_256::block_t block{};
    sponge<shake_256> xof{};
    xof.write(block);
    BC_REQUIRE_EQUAL(xof.flush(), block32);
}

BC_AUTO_TEST_CASE(sponge__write__space__expected)
{
    constexpr shake_256::bytes_t<shake_256::space> tail{};
    sponge<shake_256> xof{};
    xof.write(tail);
    BC_REQUIRE_EQUAL(xof.flush(), space32);
}

BC_AUTO_TEST_CASE(sponge__write__partial_writes__matches_whole)
{
    const const_byte_span span{ abc };
    data_array<64> out{};
    sponge<shake_256> xof{};
    xof.write(span.first(one));
    xof.write(span.subspan(one));
    xof.squeeze(out);
    BC_REQUIRE_EQUAL(out, abc64);
}

BC_AUTO_TEST_CASE(sponge__write__split__matches_whole)
{
    const data_chunk data(300, 'a');
    const const_byte_span span{ data };
    data_array<100> out{};
    sponge<shake_256> xof{};
    xof.write(span.first(one));
    xof.write(span.subspan(one, sub1(shake_256::block_bytes)));
    xof.write(span.subspan(shake_256::block_bytes));
    xof.squeeze(out);
    BC_REQUIRE_EQUAL(out, a300_100);
}

BC_AUTO_TEST_CASE(sponge__write__one_million_a__expected)
{
    const data_chunk data(1'000'000, 'a');
    sponge<shake_256> xof{};
    xof.write(data);
    BC_REQUIRE_EQUAL(xof.flush(), million32);
}

BC_AUTO_TEST_SUITE_END()
