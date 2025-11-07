/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#include "../../test.hpp"
#include <sstream>

BOOST_AUTO_TEST_SUITE(hex_reader_tests)

constexpr auto header_size = chain::header::serialized_size();

static const auto& genesis() NOEXCEPT
{
    static auto block = settings(chain::selection::mainnet).genesis_block;
    return block;
}

static std::string expected_base16() NOEXCEPT
{
    const auto expected = []() NOEXCEPT
    {
        data_chunk data(header_size);
        stream::out::fast ostream{ data };
        write::bytes::fast writer{ ostream };
        genesis().header().to_data(writer);
        return encode_base16(data);
    };

    static auto hex = expected();
    return hex;
}

BOOST_AUTO_TEST_CASE(hex_reader__stream__genesis_header__expected)
{
    std::istringstream hex{ expected_base16() };
    read::base16::istream hexer{ hex };
    chain::header header{ hexer };
    BOOST_REQUIRE(hexer);
    BOOST_REQUIRE(header == genesis().header());
}

BOOST_AUTO_TEST_CASE(hex_reader__fast__genesis_header_expected)
{
    const auto hex = to_chunk(expected_base16());
    stream::in::fast stream{ hex };
    read::base16::fast hexer{ stream };
    chain::header header{ hexer };
    BOOST_REQUIRE(hexer);
    BOOST_REQUIRE(header == genesis().header());
}

BOOST_AUTO_TEST_CASE(hex_reader__copy__genesis_header__expected)
{
    const auto hex = to_chunk(expected_base16());
    read::base16::copy hexer{ hex };
    chain::header header{ hexer };
    BOOST_REQUIRE(hexer);
    BOOST_REQUIRE(header == genesis().header());
}

// verify seek using full tx parse (scripts use position/skip/rewind/peek).

static const data_slice hex
{
    "0100000000000000000000000000000000000000000000000000000000000000"
    "000000003ba3edfd7a7b12b27ac72c3e67768f617fc81bc3888a51323a9fb8aa"
    "4b1e5e4a29ab5f49ffff001d1dac2b7c01010000000100000000000000000000"
    "00000000000000000000000000000000000000000000ffffffff4d04ffff001d"
    "0104455468652054696d65732030332f4a616e2f32303039204368616e63656c"
    "6c6f72206f6e206272696e6b206f66207365636f6e64206261696c6f75742066"
    "6f722062616e6b73ffffffff0100f2052a01000000434104678afdb0fe554827"
    "1967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4"
    "f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5fac00000000"
};

BOOST_AUTO_TEST_CASE(hex_reader__fast_stream__genesis_block__expected)
{
    stream::in::fast source{ hex };
    read::base16::fast hexer{ source };
    const chain::block block{ hexer, true };
    BOOST_REQUIRE(block == genesis());
}

BOOST_AUTO_TEST_CASE(hex_reader__copy_stream__genesis_block__expected)
{
    read::base16::copy hexer{ hex };
    const chain::block block{ hexer, true };
    BOOST_REQUIRE(block == genesis());
}

BOOST_AUTO_TEST_CASE(hex_reader__istream_stream__genesis_block__expected)
{
    std::istringstream stream{ hex.to_string() };
    read::base16::istream hexer{ stream };
    const chain::block block{ hexer, true };
    BOOST_REQUIRE(block == genesis());
}

BOOST_AUTO_TEST_SUITE_END()
