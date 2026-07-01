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
#include "../test.hpp"

BOOST_AUTO_TEST_SUITE(threshold_tests)

using namespace system;
using namespace system::chain;

BOOST_AUTO_TEST_CASE(threshold__construction__default__unknown_empty)
{
    threshold group{};
    BOOST_CHECK(group.tuples.empty());
    BOOST_CHECK_EQUAL(group.category, threshold::category_t::unknown);
    BOOST_CHECK_EQUAL(group.minimum, 0u);
    BOOST_CHECK_EQUAL(group.maximum, 0u);
    BOOST_CHECK_EQUAL(group.expected, 0u);
}

BOOST_AUTO_TEST_CASE(threshold__push_tuple__under_expected__false)
{
    threshold group{};
    group.expected = 2;
    hash_digest digest{};
    ec_signature sig{};
    ec_xonly point{};

    BOOST_CHECK(!group.push_tuple(digest, cref<ec_xonly>(point), cref<ec_signature>(sig)));
    BOOST_CHECK_EQUAL(group.tuples.size(), 1u);
}

BOOST_AUTO_TEST_CASE(threshold__push_tuple__exactly_expected__true)
{
    threshold group{};
    group.expected = 1;
    hash_digest digest{};
    ec_signature sig{};
    ec_xonly point{};

    BOOST_CHECK(group.push_tuple(digest, cref<ec_xonly>(point), cref<ec_signature>(sig)));
    BOOST_CHECK_EQUAL(group.tuples.size(), 1u);
}

// to_category

BOOST_AUTO_TEST_CASE(threshold__to_category__checksig__single)
{
    // checksig is not a threshold script code but is recorded with
    // single signatures archived in the multisig table.
    BOOST_CHECK(threshold::to_category(opcode::checksig) == threshold::category_t::single);
}

BOOST_AUTO_TEST_CASE(threshold__to_category__numequal__equal)
{
    BOOST_CHECK(threshold::to_category(opcode::numequal) == threshold::category_t::equal);
}

BOOST_AUTO_TEST_CASE(threshold__to_category__numequalverify__equal)
{
    BOOST_CHECK(threshold::to_category(opcode::numequalverify) == threshold::category_t::equal);
}

BOOST_AUTO_TEST_CASE(threshold__to_category__numnotequal__inequal)
{
    BOOST_CHECK(threshold::to_category(opcode::numnotequal) == threshold::category_t::inequal);
}

BOOST_AUTO_TEST_CASE(threshold__to_category__lessthan__lesser)
{
    BOOST_CHECK(threshold::to_category(opcode::lessthan) == threshold::category_t::lesser);
}

BOOST_AUTO_TEST_CASE(threshold__to_category__greaterthan__greater)
{
    BOOST_CHECK(threshold::to_category(opcode::greaterthan) == threshold::category_t::greater);
}

BOOST_AUTO_TEST_CASE(threshold__to_category__lessthanorequal__not_greater)
{
    BOOST_CHECK(threshold::to_category(opcode::lessthanorequal) == threshold::category_t::not_greater);
}

BOOST_AUTO_TEST_CASE(threshold__to_category__greaterthanorequal__not_lesser)
{
    BOOST_CHECK(threshold::to_category(opcode::greaterthanorequal) == threshold::category_t::not_lesser);
}

BOOST_AUTO_TEST_CASE(threshold__to_category__within__between)
{
    BOOST_CHECK(threshold::to_category(opcode::within) == threshold::category_t::between);
}

BOOST_AUTO_TEST_CASE(threshold__to_category__non_threashold__unknown)
{
    BOOST_CHECK(threshold::to_category(opcode::add) == threshold::category_t::unknown);
    BOOST_CHECK(threshold::to_category(opcode::boolor) == threshold::category_t::unknown);
    BOOST_CHECK(threshold::to_category(opcode::op_xor) == threshold::category_t::unknown);
}

BOOST_AUTO_TEST_SUITE_END()
