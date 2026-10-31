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

BC_AUTO_TEST_SUITE(pem_tests)

using namespace x509;

BC_AUTO_TEST_CASE(pem__encode_pem__short__one_line)
{
    const auto text = encode_pem("TEST", base16_chunk("000102"));
    BC_REQUIRE_EQUAL(text, "-----BEGIN TEST-----\nAAEC\n-----END TEST-----\n");
}

BC_AUTO_TEST_CASE(pem__encode_pem__long__sixty_four_character_lines)
{
    const data_chunk data(60, 0xff);
    const auto text = encode_pem("TEST", data);
    const std::string line(64, '/');
    BC_REQUIRE_EQUAL(text, "-----BEGIN TEST-----\n" + line + "\n" + "////////////////\n-----END TEST-----\n");
}

BC_AUTO_TEST_CASE(pem__decode_pem__encoded__round_trip)
{
    const data_chunk data(100, 0x2a);
    pems blocks{};
    BC_REQUIRE(decode_pem(blocks, encode_pem("DATA", data)));
    BC_REQUIRE_EQUAL(blocks.size(), 1u);
    BC_REQUIRE_EQUAL(blocks.front().label, "DATA");
    BC_REQUIRE_EQUAL(blocks.front().data, data);
}

BC_AUTO_TEST_CASE(pem__decode_pem__empty__no_blocks)
{
    pems blocks{ { "X", {} } };
    BC_REQUIRE(decode_pem(blocks, ""));
    BC_REQUIRE(blocks.empty());
}

BC_AUTO_TEST_CASE(pem__decode_pem__explanatory_text_and_crlf__blocks)
{
    const std::string text{ "subject=bs\r\n-----BEGIN A-----\r\nAAEC\r\n-----END A-----\r\n\r\ntrailing\n-----BEGIN B-----\nAw==  \n-----END B-----" };
    pems blocks{};
    BC_REQUIRE(decode_pem(blocks, text));
    BC_REQUIRE_EQUAL(blocks.size(), 2u);
    BC_REQUIRE_EQUAL(blocks[0].label, "A");
    BC_REQUIRE_EQUAL(blocks[0].data, base16_chunk("000102"));
    BC_REQUIRE_EQUAL(blocks[1].label, "B");
    BC_REQUIRE_EQUAL(blocks[1].data, base16_chunk("03"));
}

BC_AUTO_TEST_CASE(pem__decode_pem__fixture_chain__two_certificates)
{
    pems blocks{};
    BC_REQUIRE(decode_pem(blocks, fixture::server_pem + fixture::intermediate_pem));
    BC_REQUIRE_EQUAL(blocks.size(), 2u);
    BC_REQUIRE_EQUAL(blocks[0].label, "CERTIFICATE");
    BC_REQUIRE_EQUAL(blocks[1].label, "CERTIFICATE");
}

BC_AUTO_TEST_CASE(pem__decode_pem__boundary_without_label__ignored)
{
    pems blocks{};
    BC_REQUIRE(decode_pem(blocks, "-----BEGIN -----\nAAEC\n"));
    BC_REQUIRE(blocks.empty());
}

BC_AUTO_TEST_CASE(pem__decode_pem__unterminated__false)
{
    pems blocks{};
    BC_REQUIRE(!decode_pem(blocks, "-----BEGIN A-----\nAAEC\n"));
}

BC_AUTO_TEST_CASE(pem__decode_pem__mismatched_label__false)
{
    pems blocks{};
    BC_REQUIRE(!decode_pem(blocks, "-----BEGIN A-----\nAAEC\n-----END B-----\n"));
}

BC_AUTO_TEST_CASE(pem__decode_pem__invalid_base64__false)
{
    pems blocks{};
    BC_REQUIRE(!decode_pem(blocks, "-----BEGIN A-----\nA*EC\n-----END A-----\n"));
}

BC_AUTO_TEST_CASE(pem__decode_pem__empty_body__false)
{
    pems blocks{};
    BC_REQUIRE(!decode_pem(blocks, "-----BEGIN A-----\n-----END A-----\n"));
}

BC_AUTO_TEST_CASE(pem__decode_pem__encapsulated_header__false)
{
    pems blocks{};
    BC_REQUIRE(!decode_pem(blocks, "-----BEGIN A-----\nProc-Type: 4,ENCRYPTED\nAAEC\n-----END A-----\n"));
}

BC_AUTO_TEST_SUITE_END()
