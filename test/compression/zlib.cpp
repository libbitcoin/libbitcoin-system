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

BC_AUTO_TEST_SUITE(zlib_tests)

// zlib streams produced by zlib.
const auto fox = to_chunk(std::string{ "The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog. The quick brown fox jumps over the lazy dog. " });
const auto fox_zlib = base16_chunk("789c0bc94855282ccd4cce56482aca2fcf5348cbaf50c82acd2d2856c82f4b2d5228014ae72456552aa4e4a7eb29840c0ec500fa60409d");

BC_AUTO_TEST_CASE(zlib_deflate__empty__expected)
{
    BC_REQUIRE_EQUAL(zlib_deflate(data_chunk{}), base16_chunk("789c030000000001"));
}

BC_AUTO_TEST_CASE(zlib_inflate__empty_stream__empty)
{
    data_chunk out{ 0x42 };
    BC_REQUIRE(zlib_inflate(out, base16_chunk("789c030000000001")));
    BC_REQUIRE(out.empty());
}

BC_AUTO_TEST_CASE(zlib_inflate__fox__expected)
{
    data_chunk out{};
    BC_REQUIRE(zlib_inflate(out, fox_zlib));
    BC_REQUIRE_EQUAL(out, fox);
}

BC_AUTO_TEST_CASE(zlib_inflate__round_trip__expected)
{
    const data_chunk data(70000, 'z');
    data_chunk out{};
    BC_REQUIRE(zlib_inflate(out, zlib_deflate(data)));
    BC_REQUIRE_EQUAL(out, data);
}

BC_AUTO_TEST_CASE(zlib_inflate__short__false)
{
    data_chunk out{};
    BC_REQUIRE(!zlib_inflate(out, base16_chunk("789c0300000000")));
}

BC_AUTO_TEST_CASE(zlib_inflate__bad_check__false)
{
    data_chunk out{};
    BC_REQUIRE(!zlib_inflate(out, base16_chunk("789c030000000002")));
}

BC_AUTO_TEST_CASE(zlib_inflate__bad_header_check__false)
{
    data_chunk out{};
    BC_REQUIRE(!zlib_inflate(out, base16_chunk("789d030000000001")));
}

BC_AUTO_TEST_CASE(zlib_inflate__not_deflate__false)
{
    data_chunk out{};
    BC_REQUIRE(!zlib_inflate(out, base16_chunk("7918030000000001")));
}

BC_AUTO_TEST_CASE(zlib_inflate__preset_dictionary__false)
{
    data_chunk out{};
    BC_REQUIRE(!zlib_inflate(out, base16_chunk("78bb030000000001")));
}

BC_AUTO_TEST_CASE(zlib_inflate__trailing_data__false)
{
    data_chunk out{};
    BC_REQUIRE(!zlib_inflate(out, base16_chunk("789c03000000000100")));
}

BC_AUTO_TEST_CASE(zlib_inflate__limit_exceeded__false)
{
    data_chunk out{};
    BC_REQUIRE(!zlib_inflate(out, fox_zlib, sub1(fox.size())));
}

BC_AUTO_TEST_SUITE_END()
