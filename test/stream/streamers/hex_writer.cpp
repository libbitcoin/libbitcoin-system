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

BOOST_AUTO_TEST_SUITE(hex_writer_tests)

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

BOOST_AUTO_TEST_CASE(hex_writer__stream__genesis_block__expected)
{
    std::ostringstream hex{};
    write::base16::ostream hexer{ hex };
    genesis().header().to_data(hexer);
    hexer.flush();
    BOOST_REQUIRE(hexer);

    const auto expected = expected_base16();
    BOOST_REQUIRE_EQUAL(hex.str(), expected);
}

BOOST_AUTO_TEST_CASE(hex_writer__fast__genesis_block__expected)
{
    data_chunk hex(header_size * two);
    stream::out::fast stream{ hex };
    write::base16::fast hexer{ stream };
    genesis().header().to_data(hexer);
    ////hexer.flush();
    BOOST_REQUIRE(hexer);
    BOOST_REQUIRE_EQUAL(to_string(hex), expected_base16());
}

BOOST_AUTO_TEST_CASE(hex_writer__copy__genesis_block__expected)
{
    data_chunk hex(header_size * two);
    write::base16::copy hexer{ hex };
    genesis().header().to_data(hexer);
    ////hexer.flush();
    BOOST_REQUIRE(hexer);
    BOOST_REQUIRE_EQUAL(to_string(hex), expected_base16());
}

BOOST_AUTO_TEST_CASE(hex_writer__text__genesis_block__expected)
{
    std::string hex{};
    write::base16::text hexer{ hex };
    genesis().header().to_data(hexer);
    hexer.flush();
    BOOST_REQUIRE(hexer);
    BOOST_REQUIRE_EQUAL(hex, expected_base16());
}

BOOST_AUTO_TEST_CASE(hex_writer__data__genesis_block__expected)
{
    data_chunk hex{};
    write::base16::data hexer{ hex };
    genesis().header().to_data(hexer);
    hexer.flush();
    BOOST_REQUIRE(hexer);
    BOOST_REQUIRE_EQUAL(to_string(hex), expected_base16());
}

BOOST_AUTO_TEST_SUITE_END()
