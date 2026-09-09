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
#include "accessor.hpp"

BOOST_AUTO_TEST_SUITE(program_verify_tests)

using namespace system::chain;
using namespace system::machine;

// subscript
// ----------------------------------------------------------------------------

using verify_accessor = interpreter_accessor<contiguous_stack>;

static transaction funded_tx() NOEXCEPT
{
    const inputs ins
    {
        input{ point{ one_hash, 0 }, script{}, max_input_sequence }
    };

    const outputs outs{ output{ 42, script{} } };
    const transaction tx{ 1, ins, outs, 0 };
    const output prevout{ 42, script{} };
    (*tx.inputs_ptr())[0]->prevout = to_shared(prevout);
    return tx;
}

// Op stripping is not applied to bip141 v0 scripts [bip143].
BOOST_AUTO_TEST_CASE(program__subscript__segwit_bip143__unstripped)
{
    const operations ops{ operation{ opcode::codeseparator } };
    const auto separator = to_shared<script>(script{ ops });
    const auto tx = funded_tx();
    const auto it = tx.inputs_ptr()->begin();
    const auto stack = std::make_shared<chunk_cptrs>();
    const chain::signatures capture{};
    const verify_accessor out{ tx, it, separator, flags::bip143_rule, script_version::segwit, stack, capture };

    const data_chunk endorsement{ 0x01_u8 };
    const chunk_xptr single{ endorsement };
    BOOST_REQUIRE(out.subscript(single) == separator);
    BOOST_REQUIRE(out.subscript(chunk_xptrs{ single }) == separator);
}

BOOST_AUTO_TEST_CASE(program__subscript__codeseparator__stripped)
{
    const operations ops{ operation{ opcode::codeseparator }, operation{ opcode::dup } };
    const auto tx = accessor_transaction(script{ ops }, max_input_sequence, 0, 1);
    const auto it = tx.inputs_ptr()->begin();
    const chain::signatures capture{};
    const verify_accessor in{ tx, it, flags::no_rules, capture };

    const data_chunk endorsement{ 0x01_u8 };
    const chunk_xptr single{ endorsement };
    BOOST_REQUIRE_EQUAL(in.subscript(single)->ops().size(), one);
    BOOST_REQUIRE_EQUAL(in.subscript(chunk_xptrs{ single })->ops().size(), one);
}

// The subscripted sighash cannot fail, so the cache is set unconditionally.
BOOST_AUTO_TEST_CASE(program__set_hash__subscript__cached)
{
    const chain::signatures capture{};
    const auto tx = accessor_transaction(script{}, max_input_sequence, 0, 1);
    const auto it = tx.inputs_ptr()->begin();
    const verify_accessor in{ tx, it, flags::no_rules, capture };
    const script subscript{ operations{ operation{ opcode::checksig } } };
    BOOST_REQUIRE(!in.cached(coverage::hash_all));
    in.set_hash(subscript, coverage::hash_all);
    BOOST_REQUIRE(in.cached(coverage::hash_all));
}

BOOST_AUTO_TEST_SUITE_END()

