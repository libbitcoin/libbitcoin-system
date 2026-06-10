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
#include <sstream>
#include "script.hpp"

BOOST_AUTO_TEST_SUITE(script_tests)

using namespace system::chain;
using namespace system::machine;

// extract_tapscript_threshold

BOOST_AUTO_TEST_CASE(script__extract_tapscript_threshold__match_2_of_3__true_expected)
{
    const chain::script script{ make_tapscript_threshold_ops(2, 3) };

    size_t min{}, max{};
    const auto condition = script.extract_tapscript_threshold(min, max);
    BOOST_CHECK(!operation::is_invalid(condition));
    BOOST_CHECK_EQUAL(min, 2u);
    BOOST_CHECK_EQUAL(max, 2u);
}

BOOST_AUTO_TEST_CASE(script__extract_tapscript_threshold__match_within__true_expected)
{
    const auto xkey = to_chunk(ec_xonly{});

    operations ops{};
    ops.emplace_back(xkey, true);
    ops.emplace_back(opcode::checksig);
    ops.emplace_back(xkey, true);
    ops.emplace_back(opcode::checksigadd);
    ops.emplace_back(xkey, true);
    ops.emplace_back(opcode::checksigadd);
    ops.emplace_back(opcode::push_positive_1);
    ops.emplace_back(opcode::push_positive_3);
    ops.emplace_back(opcode::within);
    const chain::script script{ ops };

    size_t min{}, max{};
    const auto condition = script.extract_tapscript_threshold(min, max);
    BOOST_CHECK(condition == opcode::within);
    BOOST_CHECK_EQUAL(min, 1u);
    BOOST_CHECK_EQUAL(max, 3u);
}

BOOST_AUTO_TEST_CASE(script__extract_tapscript_threshold__match_numequal__true_expected)
{
    const auto xkey = to_chunk(ec_xonly{});

    operations ops{};
    ops.emplace_back(xkey, true);
    ops.emplace_back(opcode::checksig);
    ops.emplace_back(opcode::push_positive_3);
    ops.emplace_back(opcode::numequal);
    const chain::script script{ ops };

    size_t min{}, max{};
    const auto condition = script.extract_tapscript_threshold(min, max);
    BOOST_CHECK(condition == opcode::numequal);
    BOOST_CHECK_EQUAL(min, 3u);
    BOOST_CHECK_EQUAL(max, 3u);
}

BOOST_AUTO_TEST_CASE(script__extract_tapscript_threshold__match_numequalverify__true_expected)
{
    const auto xkey = to_chunk(ec_xonly{});

    operations ops{};
    ops.emplace_back(xkey, true);
    ops.emplace_back(opcode::checksig);
    ops.emplace_back(opcode::push_positive_9);
    ops.emplace_back(opcode::numequalverify);
    const chain::script script{ ops };

    size_t min{}, max{};
    const auto condition = script.extract_tapscript_threshold(min, max);
    BOOST_CHECK(condition == opcode::numequalverify);
    BOOST_CHECK_EQUAL(min, 9u);
    BOOST_CHECK_EQUAL(max, 9u);
}

BOOST_AUTO_TEST_CASE(script__extract_tapscript_threshold__mismatch_final__false)
{
    const auto xkey = to_chunk(ec_xonly{});

    operations ops{};
    ops.emplace_back(xkey, true);
    ops.emplace_back(opcode::checksig);
    ops.emplace_back(opcode::push_positive_16);
    ops.emplace_back(opcode::equal);
    const chain::script script{ ops };

    size_t min{}, max{};
    const auto condition = script.extract_tapscript_threshold(min, max);
    BOOST_CHECK(operation::is_invalid(condition));
}

// Multisig (pure k-of-k) pattern tests

BOOST_AUTO_TEST_CASE(script__extract_tapscript_threshold__multisig_1_of_1__true_expected)
{
    const chain::script script{ make_tapscript_multisig_ops(1) };

    size_t min{}, max{};
    const auto condition = script.extract_tapscript_threshold(min, max);
    BOOST_CHECK(condition == opcode::checksig);
    BOOST_CHECK_EQUAL(min, 1u);
    BOOST_CHECK_EQUAL(max, 1u);
}

BOOST_AUTO_TEST_CASE(script__extract_tapscript_threshold__multisig_2_of_2__true_expected)
{
    const chain::script script{ make_tapscript_multisig_ops(2) };

    size_t min{}, max{};
    const auto condition = script.extract_tapscript_threshold(min, max);
    BOOST_CHECK(condition == opcode::checksig);
    BOOST_CHECK_EQUAL(min, 2u);
    BOOST_CHECK_EQUAL(max, 2u);
}

BOOST_AUTO_TEST_CASE(script__extract_tapscript_threshold__multisig_3_of_3__true_expected)
{
    const chain::script script{ make_tapscript_multisig_ops(3) };

    size_t min{}, max{};
    const auto condition = script.extract_tapscript_threshold(min, max);
    BOOST_CHECK(condition == opcode::checksig);
    BOOST_CHECK_EQUAL(min, 3u);
    BOOST_CHECK_EQUAL(max, 3u);
}

BOOST_AUTO_TEST_SUITE_END()
