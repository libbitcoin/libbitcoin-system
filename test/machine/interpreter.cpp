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

BOOST_AUTO_TEST_SUITE(interpreter_tests)

using namespace system::chain;
using namespace system::machine;

BOOST_AUTO_TEST_CASE(interpreter_test)
{
    BOOST_REQUIRE(true);
}

namespace {

constexpr auto taproot_rules = flags::bip141_rule | flags::bip143_rule | flags::bip341_rule | flags::bip342_rule;
constexpr uint64_t prevout_value = 42u;

// A non-empty signature charges the budget. A key that is neither empty nor
// 32 bytes is an unknown (upgradable) type, for which signature validation is
// considered successful, so no valid signature is required here [bip342].
const data_chunk dummy_signature{ 0x01 };
const data_chunk unknown_key(add1(ec_xonly_size), 0x02);

// <sig> <key> OP_CHECKSIGVERIFY per sigop (37 bytes each), then OP_1.
script to_budget_script(size_t sigops) NOEXCEPT
{
    operations ops{};
    for (size_t sigop{}; sigop < sigops; ++sigop)
    {
        ops.emplace_back(dummy_signature, false);
        ops.emplace_back(unknown_key, false);
        ops.emplace_back(opcode::checksigverify);
    }

    ops.emplace_back(opcode::push_positive_1);
    return script{ std::move(ops) };
}

// The tx spending the tapscript. The signature hash does not cover the witness
// (apart from an annex), so a tx built with an empty witness produces the same
// hashes as one carrying the signatures over which those hashes are taken.
transaction to_spending_transaction(const chunk_cptrs& stack) NOEXCEPT
{
    // A non-null point, so the tx is not taken for a coinbase.
    const point outpoint{ one_hash, 0u };
    const witness spender{ stack };
    const chain::inputs ins{ input{ outpoint, script{}, spender, 0xffffffff } };
    const chain::outputs outs{ output{ prevout_value, script{} } };
    const transaction tx{ 1u, ins, outs, 0u };

    const auto prevout = to_shared<output>(output{ prevout_value, script{} });
    tx.inputs_ptr()->front()->prevout = prevout;
    return tx;
}

// Run a tapscript exactly as connect_witness does: the input carries the full
// witness, while the program is handed the stack with the control block and
// leaf script already popped. Elements are the witness stack-elements, in
// witness order (the last is the top of the execution stack).
code run_tapscript(const script& leaf, const chunk_cptrs& elements={}) NOEXCEPT
{
    // Witness of the input: [elements] <script> <control>.
    const data_chunk control(add1(ec_xonly_size), tapscript_version);
    auto stack = elements;
    stack.push_back(to_shared<data_chunk>(leaf.to_data(false)));
    stack.push_back(to_shared<data_chunk>(control));

    const auto tx = to_spending_transaction(stack);
    const auto in = tx.inputs_ptr()->begin();

    // Execution stack is the witness less the control block and leaf script.
    const auto execution = std::make_shared<chunk_cptrs>(elements);
    const auto tapleaf = to_shared(taproot::leaf_hash(tapscript_version, leaf));
    const auto leaf_ptr = to_shared<script>(leaf);
    const signatures capture{};
    constexpr auto version = script_version::taproot;

    interpreter<contiguous_stack> program{ tx, in, leaf_ptr, taproot_rules, version, execution, tapleaf, capture };
    return program.run();
}

code run_budget_script(size_t sigops) NOEXCEPT
{
    return run_tapscript(to_budget_script(sigops));
}

} // namespace

// Two sigops against a 75 byte leaf script. The full witness serializes to 111
// bytes, so the budget is 162 and two sigops (100) fit. Deriving the budget
// from the empty execution stack instead yields 51, which stops at one sigop.
BOOST_AUTO_TEST_CASE(interpreter__run__tapscript_budget_from_full_witness__success)
{
    BOOST_REQUIRE_EQUAL(run_budget_script(2), error::script_success);
}

// Six sigops (300) still fit a 223 byte leaf script, whose witness serializes
// to 259 bytes for a budget of 310.
BOOST_AUTO_TEST_CASE(interpreter__run__tapscript_budget_at_limit__success)
{
    BOOST_REQUIRE_EQUAL(run_budget_script(6), error::script_success);
}

// Seven sigops (350) exceed the 349 budget of a 260 byte leaf script, whose
// witness serializes to 298 bytes. The budget remains enforced.
BOOST_AUTO_TEST_CASE(interpreter__run__tapscript_budget_exceeded__op_check_sig_budget)
{
    BOOST_REQUIRE_EQUAL(run_budget_script(7), error::op_check_sig_budget);
}

BOOST_AUTO_TEST_SUITE_END()
