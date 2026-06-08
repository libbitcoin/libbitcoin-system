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

static operations make_threshold_ops(uint8_t threshold, size_t keys)
{
    const auto xkey = to_chunk(ec_xonly{});

    operations ops{};
    ops.reserve(add1(add1(keys)));

    for (size_t key{}; key < keys; ++key)
    {
        ops.emplace_back(xkey, true);
        ops.emplace_back(is_zero(key) ? opcode::checksig : opcode::checksigadd);
    }

    // Uniform but non-minimal encoding.
    ops.emplace_back(to_chunk(threshold), true);
    ops.emplace_back(opcode::numequal);
    return ops;
}

BOOST_AUTO_TEST_CASE(script__extract_tapscript_threshold__match_2_of_3__true_expected)
{
    const chain::script script{ make_threshold_ops(2, 3) };

    size_t required{}, sigops{};
    BOOST_CHECK(script.extract_tapscript_threshold(required, sigops));
    BOOST_CHECK_EQUAL(required, 2u);
    BOOST_CHECK_EQUAL(sigops, 3u);
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

    // Requries more than are present (still valid and executed sigops).
    size_t required{}, sigops{};
    BOOST_CHECK(script.extract_tapscript_threshold(required, sigops));
    BOOST_CHECK_EQUAL(required, 3u);
    BOOST_CHECK_EQUAL(sigops, 1u);
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

    size_t required{}, sigops{};
    BOOST_CHECK(script.extract_tapscript_threshold(required, sigops));
    BOOST_CHECK_EQUAL(required, 9u);
    BOOST_CHECK_EQUAL(sigops, 1u);
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

    size_t required{}, sigops{};
    BOOST_CHECK(!script.extract_tapscript_threshold(required, sigops));
}

BOOST_AUTO_TEST_SUITE_END()
