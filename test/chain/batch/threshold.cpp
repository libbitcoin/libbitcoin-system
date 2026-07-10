/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(threshold_tests)

using namespace system::chain;

BOOST_AUTO_TEST_CASE(threshold__is_satisfied__checksig__expected_only)
{
    // Tapscript multisig: all pairs must verify.
    BOOST_REQUIRE(threshold::is_satisfied(opcode::checksig, 5, 5, 0));
    BOOST_REQUIRE(!threshold::is_satisfied(opcode::checksig, 4, 5, 0));
}

BOOST_AUTO_TEST_CASE(threshold__is_satisfied__numequal__boundaries)
{
    BOOST_REQUIRE(threshold::is_satisfied(opcode::numequal, 5, 5, 0));
    BOOST_REQUIRE(!threshold::is_satisfied(opcode::numequal, 4, 5, 0));
    BOOST_REQUIRE(!threshold::is_satisfied(opcode::numequal, 6, 5, 0));
    BOOST_REQUIRE(threshold::is_satisfied(opcode::numequal, 0, 0, 0));
}

BOOST_AUTO_TEST_CASE(threshold__is_satisfied__numequalverify__always_false)
{
    // Verify variant leaves an empty stack; never clean-true [bip342].
    BOOST_REQUIRE(!threshold::is_satisfied(opcode::numequalverify, 5, 5, 0));
    BOOST_REQUIRE(!threshold::is_satisfied(opcode::numequalverify, 0, 0, 0));
}

BOOST_AUTO_TEST_CASE(threshold__is_satisfied__numnotequal__boundaries)
{
    BOOST_REQUIRE(threshold::is_satisfied(opcode::numnotequal, 4, 5, 0));
    BOOST_REQUIRE(threshold::is_satisfied(opcode::numnotequal, 6, 5, 0));
    BOOST_REQUIRE(!threshold::is_satisfied(opcode::numnotequal, 5, 5, 0));
}

BOOST_AUTO_TEST_CASE(threshold__is_satisfied__lessthan__boundaries)
{
    BOOST_REQUIRE(threshold::is_satisfied(opcode::lessthan, 3, 5, 0));
    BOOST_REQUIRE(!threshold::is_satisfied(opcode::lessthan, 5, 5, 0));
    BOOST_REQUIRE(threshold::is_satisfied(opcode::lessthan, 0, 1, 0));
    BOOST_REQUIRE(!threshold::is_satisfied(opcode::lessthan, 0, 0, 0));
}

BOOST_AUTO_TEST_CASE(threshold__is_satisfied__greaterthan__boundaries)
{
    BOOST_REQUIRE(threshold::is_satisfied(opcode::greaterthan, 7, 5, 0));
    BOOST_REQUIRE(!threshold::is_satisfied(opcode::greaterthan, 5, 5, 0));
    BOOST_REQUIRE(threshold::is_satisfied(opcode::greaterthan, 1, 0, 0));
}

BOOST_AUTO_TEST_CASE(threshold__is_satisfied__lessthanorequal__boundaries)
{
    BOOST_REQUIRE(threshold::is_satisfied(opcode::lessthanorequal, 5, 5, 0));
    BOOST_REQUIRE(threshold::is_satisfied(opcode::lessthanorequal, 3, 5, 0));
    BOOST_REQUIRE(!threshold::is_satisfied(opcode::lessthanorequal, 7, 5, 0));
    BOOST_REQUIRE(threshold::is_satisfied(opcode::lessthanorequal, 0, 0, 0));
}

BOOST_AUTO_TEST_CASE(threshold__is_satisfied__greaterthanorequal__boundaries)
{
    BOOST_REQUIRE(threshold::is_satisfied(opcode::greaterthanorequal, 5, 5, 0));
    BOOST_REQUIRE(threshold::is_satisfied(opcode::greaterthanorequal, 7, 5, 0));
    BOOST_REQUIRE(!threshold::is_satisfied(opcode::greaterthanorequal, 3, 5, 0));
    BOOST_REQUIRE(threshold::is_satisfied(opcode::greaterthanorequal, 0, 0, 0));
}

BOOST_AUTO_TEST_CASE(threshold__is_satisfied__within__half_open)
{
    // min <= x < max [op_within].
    BOOST_REQUIRE(threshold::is_satisfied(opcode::within, 5, 3, 7));
    BOOST_REQUIRE(threshold::is_satisfied(opcode::within, 3, 3, 7));
    BOOST_REQUIRE(threshold::is_satisfied(opcode::within, 6, 3, 7));
    BOOST_REQUIRE(!threshold::is_satisfied(opcode::within, 7, 3, 7));
    BOOST_REQUIRE(!threshold::is_satisfied(opcode::within, 2, 3, 7));
    BOOST_REQUIRE(!threshold::is_satisfied(opcode::within, 8, 3, 7));
    BOOST_REQUIRE(threshold::is_satisfied(opcode::within, 0, 0, 1));
    BOOST_REQUIRE(!threshold::is_satisfied(opcode::within, 1, 0, 1));
    BOOST_REQUIRE(!threshold::is_satisfied(opcode::within, 5, 5, 5));
    BOOST_REQUIRE(!threshold::is_satisfied(opcode::within, 5, 7, 3));
}

BOOST_AUTO_TEST_CASE(threshold__is_satisfied__non_threshold__always_false)
{
    // op_xor is the extract_tapscript_threshold sentinel.
    BOOST_REQUIRE(!threshold::is_satisfied(opcode::op_xor, 5, 5, 5));
    BOOST_REQUIRE(!threshold::is_satisfied(opcode::checksigadd, 5, 5, 5));
}

BOOST_AUTO_TEST_SUITE_END()
