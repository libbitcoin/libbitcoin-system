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

BOOST_AUTO_TEST_SUITE(interpreter_tests)

using namespace system::chain;
using namespace system::machine;

// Isolated operation handlers (via accessor).
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(interpreter__op_add__two_and_three__five)
{
    const signatures capture{};
    const auto tx = accessor_transaction({});
    interpreter_accessor<contiguous_stack> accessor{ tx, tx.inputs_ptr()->begin(), flags::all_rules, capture };
    accessor.push_signed64(2);
    accessor.push_signed64(3);
    BOOST_REQUIRE_EQUAL(code{ accessor.op_add() }, error::op_success);
    BOOST_REQUIRE_EQUAL(accessor.stack_size(), 1u);

    int32_t value{};
    BOOST_REQUIRE(accessor.pop_signed32(value));
    BOOST_REQUIRE_EQUAL(value, 5);
}

BOOST_AUTO_TEST_CASE(interpreter__op_add__insufficient_stack__op_add)
{
    const signatures capture{};
    const auto tx = accessor_transaction({});
    interpreter_accessor<contiguous_stack> accessor{ tx, tx.inputs_ptr()->begin(), flags::all_rules, capture };
    accessor.push_signed64(2);
    BOOST_REQUIRE_EQUAL(code{ accessor.op_add() }, error::op_add);
}

BOOST_AUTO_TEST_CASE(interpreter__op_dup__chunk__duplicated)
{
    const signatures capture{};
    const auto tx = accessor_transaction({});
    interpreter_accessor<contiguous_stack> accessor{ tx, tx.inputs_ptr()->begin(), flags::all_rules, capture };
    accessor.push_chunk(data_chunk{ 0x2a });
    BOOST_REQUIRE_EQUAL(code{ accessor.op_dup() }, error::op_success);
    BOOST_REQUIRE_EQUAL(accessor.stack_size(), 2u);

    const auto top = accessor.pop_chunk_();
    const auto next = accessor.pop_chunk_();
    BOOST_REQUIRE_EQUAL(*top, *next);
    BOOST_REQUIRE_EQUAL(*top, data_chunk{ 0x2a });
}

BOOST_AUTO_TEST_CASE(interpreter__op_drop__empty_stack__op_drop)
{
    const signatures capture{};
    const auto tx = accessor_transaction({});
    interpreter_accessor<contiguous_stack> accessor{ tx, tx.inputs_ptr()->begin(), flags::all_rules, capture };
    BOOST_REQUIRE_EQUAL(code{ accessor.op_drop() }, error::op_drop);
}

BOOST_AUTO_TEST_CASE(interpreter__op_if__false__negative_scope)
{
    const signatures capture{};
    const auto tx = accessor_transaction({});
    interpreter_accessor<contiguous_stack> accessor{ tx, tx.inputs_ptr()->begin(), flags::all_rules, capture };
    accessor.push_bool(false);
    BOOST_REQUIRE_EQUAL(code{ accessor.op_if() }, error::op_success);
    BOOST_REQUIRE(!accessor.is_success());
    BOOST_REQUIRE_EQUAL(code{ accessor.op_endif() }, error::op_success);
    BOOST_REQUIRE(accessor.is_balanced());
}

BOOST_AUTO_TEST_CASE(interpreter__op_endif__balanced__op_endif)
{
    const signatures capture{};
    const auto tx = accessor_transaction({});
    interpreter_accessor<contiguous_stack> accessor{ tx, tx.inputs_ptr()->begin(), flags::all_rules, capture };
    BOOST_REQUIRE_EQUAL(code{ accessor.op_endif() }, error::op_endif);
}

// Tapscript execution.
// ----------------------------------------------------------------------------

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

namespace {

// <sig> <0> <key> OP_CHECKSIGADD <1> OP_NUMEQUAL.
// Success pushes number+1, so the script is true only when the sigop passed.
script to_checksigadd_script(const data_chunk& key, const data_chunk& endorsement) NOEXCEPT
{
    operations ops{};
    ops.emplace_back(endorsement, false);
    ops.emplace_back(opcode::push_size_0);
    ops.emplace_back(key, false);
    ops.emplace_back(opcode::checksigadd);
    ops.emplace_back(opcode::push_positive_1);
    ops.emplace_back(opcode::numequal);
    return script{ std::move(ops) };
}

// 64 bytes plus an undefined sighash byte (zero must be implicit) [bip341].
data_chunk to_undefined_sighash_endorsement() NOEXCEPT
{
    data_chunk endorsement(add1(ec_signature_size), 0x11);
    endorsement.back() = 0x04;
    return endorsement;
}

} // namespace

// An endorsement of neither 64 nor 65 bytes is not a schnorr signature, but
// with an unknown key type it is never parsed, so the sigop passes.
BOOST_AUTO_TEST_CASE(interpreter__run__checksigadd_unknown_key_unparsable_endorsement__success)
{
    const data_chunk endorsement(10, 0x11);
    const auto leaf = to_checksigadd_script(unknown_key, endorsement);
    BOOST_REQUIRE_EQUAL(run_tapscript(leaf), error::script_success);
}

// Likewise an undefined sighash byte does not encumber an unknown key type.
BOOST_AUTO_TEST_CASE(interpreter__run__checksigadd_unknown_key_undefined_sighash__success)
{
    const auto endorsement = to_undefined_sighash_endorsement();
    const auto leaf = to_checksigadd_script(unknown_key, endorsement);
    BOOST_REQUIRE_EQUAL(run_tapscript(leaf), error::script_success);
}

// An empty key is invalid for any endorsement [bip342].
BOOST_AUTO_TEST_CASE(interpreter__run__checksigadd_empty_key__op_check_sig_add2)
{
    const data_chunk key{};
    const data_chunk endorsement(10, 0x11);
    const auto leaf = to_checksigadd_script(key, endorsement);
    BOOST_REQUIRE_EQUAL(run_tapscript(leaf), error::op_check_sig_add2);
}

// A 32 byte key remains subject to endorsement parsing.
BOOST_AUTO_TEST_CASE(interpreter__run__checksigadd_xonly_key_unparsable_endorsement__op_check_sig_add4)
{
    const data_chunk key(ec_xonly_size, 0x02);
    const data_chunk endorsement(10, 0x11);
    const auto leaf = to_checksigadd_script(key, endorsement);
    BOOST_REQUIRE_EQUAL(run_tapscript(leaf), error::op_check_sig_add4);
}

// A 32 byte key remains subject to the defined sighash type rule.
BOOST_AUTO_TEST_CASE(interpreter__run__checksigadd_xonly_key_undefined_sighash__op_check_sig_add4)
{
    const data_chunk key(ec_xonly_size, 0x02);
    const auto endorsement = to_undefined_sighash_endorsement();
    const auto leaf = to_checksigadd_script(key, endorsement);
    BOOST_REQUIRE_EQUAL(run_tapscript(leaf), error::op_check_sig_add4);
}

// A 32 byte key remains subject to signature verification.
BOOST_AUTO_TEST_CASE(interpreter__run__checksigadd_xonly_key_invalid_signature__op_check_sig_add6)
{
    const data_chunk key(ec_xonly_size, 0x02);
    const data_chunk endorsement(ec_signature_size, 0x11);
    const auto leaf = to_checksigadd_script(key, endorsement);
    BOOST_REQUIRE_EQUAL(run_tapscript(leaf), error::op_check_sig_add6);
}

namespace {

// OP_0 <key> OP_CHECKSIGADD OP_DROP OP_CODESEPARATOR OP_0 <key> OP_CHECKSIGADD
// Both sigops take their signature from the witness. The second signs over the
// codeseparator position of 4, the first over none (0xffffffff). The offset is
// the op following the codeseparator, from which that position is derived.
constexpr size_t after_codeseparator = 5;

script to_codeseparator_script(const ec_xonly& key) NOEXCEPT
{
    const data_chunk point{ key.begin(), key.end() };

    operations ops{};
    ops.emplace_back(opcode::push_size_0);
    ops.emplace_back(point, false);
    ops.emplace_back(opcode::checksigadd);
    ops.emplace_back(opcode::drop);
    ops.emplace_back(opcode::codeseparator);
    ops.emplace_back(opcode::push_size_0);
    ops.emplace_back(point, false);
    ops.emplace_back(opcode::checksigadd);
    return script{ std::move(ops) };
}

} // namespace

BOOST_AUTO_TEST_CASE(interpreter__run__tapscript_codeseparator_signature_hash__success)
{
    // Any valid secret, with its bip340 x-only point.
    const auto secret = base16_hash("b7e151628aed2a6abf7158809cf4f3c762e7160f38b4da56a784d9045190cfef");
    ec_compressed compressed{};
    BOOST_REQUIRE(secret_to_public(compressed, secret));

    const auto x_only = std::next(compressed.data());
    const auto key = unsafe_array_cast<uint8_t, ec_xonly_size>(x_only);
    const auto leaf = to_codeseparator_script(key);
    const auto tapleaf = to_shared(taproot::leaf_hash(tapscript_version, leaf));
    constexpr auto version = script_version::taproot;
    constexpr auto sighash = coverage::hash_default;

    // The signature hash is taken over a tx without the signatures, which it
    // does not cover, and is identical to that of the tx that carries them.
    const auto tx = to_spending_transaction({});
    const auto in = tx.inputs_ptr()->begin();

    // The first sigop executes before any codeseparator.
    hash_digest first{};
    leaf.clear_offset();
    BOOST_REQUIRE(tx.signature_hash(first, in, leaf, prevout_value, tapleaf, version, sighash, taproot_rules));

    // The second executes after the codeseparator, which the interpreter
    // records by advancing the offset to the op following it.
    hash_digest second{};
    leaf.offset = std::next(leaf.ops().begin(), after_codeseparator);
    BOOST_REQUIRE(tx.signature_hash(second, in, leaf, prevout_value, tapleaf, version, sighash, taproot_rules));
    leaf.clear_offset();

    // The codeseparator position is the only difference between them.
    BOOST_REQUIRE_NE(first, second);

    // Sign each with the implied SIGHASH_DEFAULT (64 byte, no sighash byte).
    ec_signature first_signature{};
    ec_signature second_signature{};
    BOOST_REQUIRE(schnorr::sign(first_signature, secret, first, one_hash));
    BOOST_REQUIRE(schnorr::sign(second_signature, secret, second, one_hash));

    // Witness elements, the last of which is the top of the execution stack.
    const data_chunk second_chunk{ second_signature.begin(), second_signature.end() };
    const data_chunk first_chunk{ first_signature.begin(), first_signature.end() };
    const chunk_cptrs elements{ to_shared<data_chunk>(second_chunk), to_shared<data_chunk>(first_chunk) };

    // Both signatures verify only if the second sigop takes its own hash. A
    // cache retained across the codeseparator hands it the first instead.
    BOOST_REQUIRE_EQUAL(run_tapscript(leaf, elements), error::script_success);
}

BOOST_AUTO_TEST_SUITE_END()
