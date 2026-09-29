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
#include "fixtures.hpp"

BOOST_AUTO_TEST_SUITE(pem_tests)

using namespace x509;

BOOST_AUTO_TEST_CASE(pem__encode_pem__short__one_line)
{
    const auto text = encode_pem("TEST", base16_chunk("000102"));
    BOOST_REQUIRE_EQUAL(text, "-----BEGIN TEST-----\nAAEC\n-----END TEST-----\n");
}

BOOST_AUTO_TEST_CASE(pem__encode_pem__long__sixty_four_character_lines)
{
    const data_chunk data(60, 0xff);
    const auto text = encode_pem("TEST", data);
    const std::string line(64, '/');
    BOOST_REQUIRE_EQUAL(text, "-----BEGIN TEST-----\n" + line + "\n" + "////////////////\n-----END TEST-----\n");
}

BOOST_AUTO_TEST_CASE(pem__decode_pem__encoded__round_trip)
{
    const data_chunk data(100, 0x2a);
    pems blocks{};
    BOOST_REQUIRE(decode_pem(blocks, encode_pem("DATA", data)));
    BOOST_REQUIRE_EQUAL(blocks.size(), 1u);
    BOOST_REQUIRE_EQUAL(blocks.front().label, "DATA");
    BOOST_REQUIRE_EQUAL(blocks.front().data, data);
}

BOOST_AUTO_TEST_CASE(pem__decode_pem__empty__no_blocks)
{
    pems blocks{ { "X", {} } };
    BOOST_REQUIRE(decode_pem(blocks, ""));
    BOOST_REQUIRE(blocks.empty());
}

BOOST_AUTO_TEST_CASE(pem__decode_pem__explanatory_text_and_crlf__blocks)
{
    const std::string text{ "subject=bs\r\n-----BEGIN A-----\r\nAAEC\r\n-----END A-----\r\n\r\ntrailing\n-----BEGIN B-----\nAw==  \n-----END B-----" };
    pems blocks{};
    BOOST_REQUIRE(decode_pem(blocks, text));
    BOOST_REQUIRE_EQUAL(blocks.size(), 2u);
    BOOST_REQUIRE_EQUAL(blocks[0].label, "A");
    BOOST_REQUIRE_EQUAL(blocks[0].data, base16_chunk("000102"));
    BOOST_REQUIRE_EQUAL(blocks[1].label, "B");
    BOOST_REQUIRE_EQUAL(blocks[1].data, base16_chunk("03"));
}

BOOST_AUTO_TEST_CASE(pem__decode_pem__fixture_chain__two_certificates)
{
    pems blocks{};
    BOOST_REQUIRE(decode_pem(blocks, fixture::server_pem + fixture::intermediate_pem));
    BOOST_REQUIRE_EQUAL(blocks.size(), 2u);
    BOOST_REQUIRE_EQUAL(blocks[0].label, "CERTIFICATE");
    BOOST_REQUIRE_EQUAL(blocks[1].label, "CERTIFICATE");
}

BOOST_AUTO_TEST_CASE(pem__decode_pem__boundary_without_label__ignored)
{
    pems blocks{};
    BOOST_REQUIRE(decode_pem(blocks, "-----BEGIN -----\nAAEC\n"));
    BOOST_REQUIRE(blocks.empty());
}

BOOST_AUTO_TEST_CASE(pem__decode_pem__unterminated__false)
{
    pems blocks{};
    BOOST_REQUIRE(!decode_pem(blocks, "-----BEGIN A-----\nAAEC\n"));
}

BOOST_AUTO_TEST_CASE(pem__decode_pem__mismatched_label__false)
{
    pems blocks{};
    BOOST_REQUIRE(!decode_pem(blocks, "-----BEGIN A-----\nAAEC\n-----END B-----\n"));
}

BOOST_AUTO_TEST_CASE(pem__decode_pem__invalid_base64__false)
{
    pems blocks{};
    BOOST_REQUIRE(!decode_pem(blocks, "-----BEGIN A-----\nA*EC\n-----END A-----\n"));
}

BOOST_AUTO_TEST_CASE(pem__decode_pem__empty_body__false)
{
    pems blocks{};
    BOOST_REQUIRE(!decode_pem(blocks, "-----BEGIN A-----\n-----END A-----\n"));
}

BOOST_AUTO_TEST_CASE(pem__decode_pem__encapsulated_header__false)
{
    pems blocks{};
    BOOST_REQUIRE(!decode_pem(blocks, "-----BEGIN A-----\nProc-Type: 4,ENCRYPTED\nAAEC\n-----END A-----\n"));
}

BOOST_AUTO_TEST_SUITE_END()
