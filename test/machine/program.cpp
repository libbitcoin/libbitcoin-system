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

BOOST_AUTO_TEST_SUITE(program_tests)

using namespace system::chain;
using namespace system::machine;

// initialize

BOOST_AUTO_TEST_CASE(program__initialize__empty_input_script__script_success)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BOOST_REQUIRE(machine->initialize() == error::script_success);
}

BOOST_AUTO_TEST_CASE(program__initialize__underflow_input_script__invalid_script)
{
    const script underflow(base16_chunk("4c"), false);
    BOOST_REQUIRE(underflow.is_underflow());

    machine_accessor<contiguous_stack> machine{ underflow, flags::all_rules };
    BOOST_REQUIRE(machine->initialize() == error::invalid_script);
}

BOOST_AUTO_TEST_CASE(program__initialize__oversized_input_script__invalid_script_size)
{
    const script oversized{ operations{ operation{ data_chunk(10'001, 0x00), false } } };
    BOOST_REQUIRE(oversized.is_oversized());

    machine_accessor<contiguous_stack> machine{ oversized, flags::all_rules };
    BOOST_REQUIRE(machine->initialize() == error::invalid_script_size);
}

BOOST_AUTO_TEST_CASE(program__initialize__prefail_input_script__prefail_script)
{
    const script prefail{ operations{ operation{ opcode::op_verif } } };
    BOOST_REQUIRE(prefail.is_prefail());

    machine_accessor<contiguous_stack> machine{ prefail, flags::all_rules };
    BOOST_REQUIRE(machine->initialize() == error::prefail_script);
}

// primary stack typing

BOOST_AUTO_TEST_CASE(program__push_signed64__pop_signed32__expected)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(42);
    BOOST_REQUIRE_EQUAL(machine->stack_size(), 1u);

    int32_t value{};
    BOOST_REQUIRE(machine->pop_signed32(value));
    BOOST_REQUIRE_EQUAL(value, 42);
    BOOST_REQUIRE(machine->is_stack_empty());
}

BOOST_AUTO_TEST_CASE(program__pop_signed32__five_byte_value__false)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(add1<int64_t>(max_int32));

    int32_t value{};
    BOOST_REQUIRE(!machine->pop_signed32(value));
}

BOOST_AUTO_TEST_CASE(program__push_bool__peek_bool___expected)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_bool(true);
    BOOST_REQUIRE(machine->peek_bool_());
}

BOOST_AUTO_TEST_CASE(program__push_chunk__peek_size__expected)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{ 0x01, 0x02, 0x03 });
    BOOST_REQUIRE_EQUAL(machine->peek_size(), 3u);
}

BOOST_AUTO_TEST_CASE(program__push_length__value__expected)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_length(3);

    int32_t value{};
    BOOST_REQUIRE(machine->pop_signed32(value));
    BOOST_REQUIRE_EQUAL(value, 3);
}

BOOST_AUTO_TEST_CASE(program__pop_bool___nonminimal_relaxed__true)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{ 0x02 });

    bool value{};
    BOOST_REQUIRE(machine->pop_bool_(value, false));
    BOOST_REQUIRE(value);
}

BOOST_AUTO_TEST_CASE(program__pop_bool___nonminimal_minimal__false)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{ 0x02 });

    bool value{};
    BOOST_REQUIRE(!machine->pop_bool_(value, true));
}

BOOST_AUTO_TEST_CASE(program__pop_bool___minimal_true__true)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{ 0x01 });

    bool value{};
    BOOST_REQUIRE(machine->pop_bool_(value, true));
    BOOST_REQUIRE(value);
}

BOOST_AUTO_TEST_CASE(program__pop_bool___minimal_empty__false_value)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{});

    bool value{ true };
    BOOST_REQUIRE(machine->pop_bool_(value, true));
    BOOST_REQUIRE(!value);
}

BOOST_AUTO_TEST_CASE(program__pop_strict_bool___empty__false)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{});
    BOOST_REQUIRE(!machine->pop_strict_bool_());
}

BOOST_AUTO_TEST_CASE(program__pop_strict_bool___zero_byte__true)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{ 0x00 });
    BOOST_REQUIRE(machine->pop_strict_bool_());
}

BOOST_AUTO_TEST_CASE(program__pop_binary32__two_values__expected_order)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    machine->push_signed64(2);

    int32_t left{};
    int32_t right{};
    BOOST_REQUIRE(machine->pop_binary32(left, right));
    BOOST_REQUIRE_EQUAL(left, 1);
    BOOST_REQUIRE_EQUAL(right, 2);
}

BOOST_AUTO_TEST_CASE(program__pop_ternary32__three_values__expected_order)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    machine->push_signed64(2);
    machine->push_signed64(3);

    int32_t upper{};
    int32_t lower{};
    int32_t value{};
    BOOST_REQUIRE(machine->pop_ternary32(upper, lower, value));
    BOOST_REQUIRE_EQUAL(upper, 3);
    BOOST_REQUIRE_EQUAL(lower, 2);
    BOOST_REQUIRE_EQUAL(value, 1);
}

BOOST_AUTO_TEST_CASE(program__pop_index32__valid_index__expected)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(7);
    machine->push_signed64(0);

    size_t index{};
    BOOST_REQUIRE(machine->pop_index32(index));
    BOOST_REQUIRE_EQUAL(index, 0u);
}

BOOST_AUTO_TEST_CASE(program__pop_index32__index_at_size__false)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(7);
    machine->push_signed64(1);

    size_t index{};
    BOOST_REQUIRE(!machine->pop_index32(index));
}

BOOST_AUTO_TEST_CASE(program__peek_unsigned32__negative__false)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(-1);

    uint32_t value{};
    BOOST_REQUIRE(!machine->peek_unsigned32(value));
}

BOOST_AUTO_TEST_CASE(program__peek_unsigned32__above_32_bits__narrowed)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(0x100000001);

    uint32_t value{};
    BOOST_REQUIRE(machine->peek_unsigned32(value));
    BOOST_REQUIRE_EQUAL(value, 1u);
}

BOOST_AUTO_TEST_CASE(program__peek_unsigned40__negative__false)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(-1);

    uint64_t value{};
    BOOST_REQUIRE(!machine->peek_unsigned40(value));
}

BOOST_AUTO_TEST_CASE(program__peek_unsigned40__five_byte_value__expected)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(add1<int64_t>(max_uint32));

    uint64_t value{};
    BOOST_REQUIRE(machine->peek_unsigned40(value));
    BOOST_REQUIRE_EQUAL(value, add1<uint64_t>(max_uint32));
}

BOOST_AUTO_TEST_CASE(program__drop___pushed__empty)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_signed64(1);
    BOOST_REQUIRE(!machine->is_stack_empty());

    machine->drop_();
    BOOST_REQUIRE(machine->is_stack_empty());
}

// equal_chunks

BOOST_AUTO_TEST_CASE(program__equal_chunks__bool_and_integer__true)
{
    const stack_variant left{ true };
    const stack_variant right{ int64_t{ 1 } };
    BOOST_REQUIRE(interpreter_accessor<contiguous_stack>::equal_chunks(left, right));
}

BOOST_AUTO_TEST_CASE(program__equal_chunks__integer_and_chunk__true)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_chunk(data_chunk{ 0x00, 0x01 });
    const stack_variant left{ int64_t{ 0x0100 } };
    const auto right = machine->pop_();
    BOOST_REQUIRE(interpreter_accessor<contiguous_stack>::equal_chunks(left, right));
}

// alternate stack

BOOST_AUTO_TEST_CASE(program__push_alternate__pop_alternate___expected)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BOOST_REQUIRE(machine->is_alternate_empty());

    machine->push_alternate(stack_variant{ true });
    BOOST_REQUIRE(!machine->is_alternate_empty());

    const auto variant = machine->pop_alternate_();
    BOOST_REQUIRE(variant == stack_variant{ true });
    BOOST_REQUIRE(machine->is_alternate_empty());
}

// conditional stack

BOOST_AUTO_TEST_CASE(program__begin_if__negative__not_success)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BOOST_REQUIRE(machine->is_balanced());
    BOOST_REQUIRE(machine->is_success());

    machine->begin_if(false);
    BOOST_REQUIRE(!machine->is_balanced());
    BOOST_REQUIRE(!machine->is_success());

    machine->else_if_();
    BOOST_REQUIRE(!machine->is_balanced());
    BOOST_REQUIRE(machine->is_success());

    machine->end_if_();
    BOOST_REQUIRE(machine->is_balanced());
    BOOST_REQUIRE(machine->is_success());
}

BOOST_AUTO_TEST_CASE(program__begin_if__nested_negative__success_after_both_closed)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->begin_if(false);
    machine->begin_if(false);
    BOOST_REQUIRE(!machine->is_success());

    machine->end_if_();
    BOOST_REQUIRE(!machine->is_success());

    machine->end_if_();
    BOOST_REQUIRE(machine->is_success());
    BOOST_REQUIRE(machine->is_balanced());
}

BOOST_AUTO_TEST_CASE(program__if___negative_scope_unconditional_op__false)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->begin_if(false);
    const operation nop{ opcode::nop };
    BOOST_REQUIRE(!machine->if_(nop));
}

BOOST_AUTO_TEST_CASE(program__if___negative_scope_conditional_op__true)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->begin_if(false);
    const operation conditional{ opcode::if_ };
    BOOST_REQUIRE(machine->if_(conditional));
}

// stack limits

BOOST_AUTO_TEST_CASE(program__is_stack_overflow__empty__false)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BOOST_REQUIRE(!machine->is_stack_overflow());
}

BOOST_AUTO_TEST_CASE(program__stack_nonempty__mixed_elements__counts_nonempty)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_bool(true);
    machine->push_bool(false);
    machine->push_signed64(0);
    machine->push_signed64(42);
    machine->push_chunk(data_chunk{});
    machine->push_chunk(data_chunk{ 0x01 });
    BOOST_REQUIRE_EQUAL(machine->stack_size(), 6u);
    BOOST_REQUIRE_EQUAL(machine->stack_nonempty(), 3u);
}

// The primary and alternate stacks share one limit [bip342].
BOOST_AUTO_TEST_CASE(program__is_stack_overflow__alternate_counted__true)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BOOST_REQUIRE(machine->ops_increment(0));
    machine->push_bool(true);
    machine->push_alternate(stack_variant{ true });
    BOOST_REQUIRE(!machine->is_stack_overflow());
}

// accumulators

BOOST_AUTO_TEST_CASE(program__ops_increment__op_at_limit__expected)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BOOST_REQUIRE(machine->ops_increment(200));

    const operation nop{ opcode::nop };
    BOOST_REQUIRE(machine->ops_increment(nop));
    BOOST_REQUIRE(!machine->ops_increment(nop));
}

BOOST_AUTO_TEST_CASE(program__ops_increment__push_op_not_counted__true)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BOOST_REQUIRE(machine->ops_increment(201));

    const operation push{ data_chunk{ 0x01 }, false };
    BOOST_REQUIRE(machine->ops_increment(push));
}

BOOST_AUTO_TEST_CASE(program__ops_increment__keys_beyond_limit__false)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BOOST_REQUIRE(machine->ops_increment(201));
    BOOST_REQUIRE(!machine->ops_increment(1));
}

// is_true

BOOST_AUTO_TEST_CASE(program__is_true__empty_stack__false)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    BOOST_REQUIRE(!machine->is_true(true));
    BOOST_REQUIRE(!machine->is_true(false));
}

BOOST_AUTO_TEST_CASE(program__is_true__single_true__true)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_bool(true);
    BOOST_REQUIRE(machine->is_true(true));
    BOOST_REQUIRE(machine->is_true(false));
}

BOOST_AUTO_TEST_CASE(program__is_true__single_false__false)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_bool(false);
    BOOST_REQUIRE(!machine->is_true(true));
    BOOST_REQUIRE(!machine->is_true(false));
}

// BIP62: the clean stack rule requires that exactly one element remains.
BOOST_AUTO_TEST_CASE(program__is_true__dirty_stack__clean_only_false)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_bool(true);
    machine->push_bool(true);
    BOOST_REQUIRE(!machine->is_true(true));
    BOOST_REQUIRE(machine->is_true(false));
}

// The top element is evaluated, so a dirty stack with a false top is false.
BOOST_AUTO_TEST_CASE(program__is_true__dirty_stack_false_top__false)
{
    machine_accessor<contiguous_stack> machine{ {}, flags::all_rules };
    machine->push_bool(true);
    machine->push_bool(false);
    BOOST_REQUIRE(!machine->is_true(true));
    BOOST_REQUIRE(!machine->is_true(false));
}

// Batch capture fallbacks (protected surface).
// ----------------------------------------------------------------------------
// Batching is gated on the running script being an output script of a
// batchable pattern; every other route falls through to inline verification.

using batch_accessor = interpreter_accessor<contiguous_stack>;

constexpr auto batch_secret = base16_array("0000000000000000000000000000000000000000000000000000000000000001");

static data_chunk batch_key() NOEXCEPT
{
    ec_compressed public_key{};
    secret_to_public(public_key, batch_secret);
    return to_chunk(public_key);
}

static script batch_p2pk() NOEXCEPT
{
    const operations ops
    {
        operation{ batch_key(), false },
        operation{ opcode::checksig }
    };
    return script{ ops };
}

// The key is supplied by the input, so its size is not constrained here.
static script batch_p2kh() NOEXCEPT
{
    const operations ops
    {
        operation{ opcode::dup },
        operation{ opcode::hash160 },
        operation{ data_chunk(short_hash_size, 0x42), true },
        operation{ opcode::equalverify },
        operation{ opcode::checksig }
    };

    return script{ ops };
}

static script batch_multisig() NOEXCEPT
{
    const auto positive1 = operation::opcode_from_positive(1_u8);
    const operations ops
    {
        operation{ positive1 },
        operation{ batch_key(), false },
        operation{ positive1 },
        operation{ opcode::checkmultisig }
    };

    return script{ ops };
}

// A point of neither compressed nor uncompressed size verifies inline.
BOOST_AUTO_TEST_CASE(program__verify_ecdsa_signature__uncompressible_point__false)
{
    const chain::signatures capture{ true };
    const auto tx = accessor_transaction(script{}, max_input_sequence, 0, 1);
    const auto it = tx.inputs_ptr()->begin();
    const batch_accessor in{ tx, it, flags::no_rules, capture };
    const batch_accessor out{ in, to_shared<script>(batch_p2kh()) };

    const data_chunk point(short_hash_size, 0x00);
    const hash_digest hash{};
    const ec_signature signature{};
    BOOST_REQUIRE(!out.verify_ecdsa_signature(point, hash, signature, true));
    BOOST_REQUIRE(!capture.batched.load());
}

BOOST_AUTO_TEST_CASE(program__verify_ecdsa_signature__unbatchable_script__false)
{
    const chain::signatures capture{ true };
    const auto tx = accessor_transaction(script{}, max_input_sequence, 0, 1);
    const auto it = tx.inputs_ptr()->begin();
    const batch_accessor in{ tx, it, flags::no_rules, capture };
    const batch_accessor out{ in, to_shared<script>(script{ operations{ operation{ opcode::checksig } } }) };

    const auto point = batch_key();
    const hash_digest hash{};
    const ec_signature signature{};
    BOOST_REQUIRE(!out.verify_ecdsa_signature(point, hash, signature, true));
    BOOST_REQUIRE(!capture.batched.load());
}

// An input script is never batchable.
BOOST_AUTO_TEST_CASE(program__verify_ecdsa_signature__input_script__false)
{
    const chain::signatures capture{ true };
    const auto tx = accessor_transaction(batch_p2pk(), max_input_sequence, 0, 1);
    const auto it = tx.inputs_ptr()->begin();
    const batch_accessor in{ tx, it, flags::no_rules, capture };

    const auto point = batch_key();
    const hash_digest hash{};
    const ec_signature signature{};
    BOOST_REQUIRE(!in.verify_ecdsa_signature(point, hash, signature, true));
    BOOST_REQUIRE(!capture.batched.load());
}

BOOST_AUTO_TEST_CASE(program__verify_schnorr_signature__unbatchable_script__false)
{
    const chain::signatures capture{ true };
    const auto tx = accessor_transaction(script{}, max_input_sequence, 0, 1);
    const auto it = tx.inputs_ptr()->begin();
    const batch_accessor in{ tx, it, flags::no_rules, capture };
    const batch_accessor out{ in, to_shared<script>(batch_p2pk()) };

    const data_chunk point(ec_xonly_size, 0x02);
    const hash_digest hash{};
    const ec_signature signature{};
    BOOST_REQUIRE(!out.verify_schnorr_signature(point, hash, signature));
    BOOST_REQUIRE(!capture.batched.load());
}

// A group must carry at least one signature.
BOOST_AUTO_TEST_CASE(program__try_batch_multisig_verification__no_endorsements__false)
{
    const chain::signatures capture{ true };
    const auto tx = accessor_transaction(script{}, max_input_sequence, 0, 1);
    const auto it = tx.inputs_ptr()->begin();
    const batch_accessor in{ tx, it, flags::no_rules, capture };
    const batch_accessor out{ in, to_shared<script>(batch_multisig()) };

    const auto key = batch_key();
    const chunk_xptrs points{ chunk_xptr{ key } };
    const chunk_xptrs endorsements{};
    BOOST_REQUIRE(!out.try_batch_multisig_verification(points, endorsements));
}

BOOST_AUTO_TEST_CASE(program__try_batch_multisig_verification__empty_endorsement__false)
{
    const chain::signatures capture{ true };
    const auto tx = accessor_transaction(script{}, max_input_sequence, 0, 1);
    const auto it = tx.inputs_ptr()->begin();
    const batch_accessor in{ tx, it, flags::no_rules, capture };
    const batch_accessor out{ in, to_shared<script>(batch_multisig()) };

    const auto key = batch_key();
    const data_chunk empty{};
    const chunk_xptrs points{ chunk_xptr{ key } };
    const chunk_xptrs endorsements{ chunk_xptr{ empty } };
    BOOST_REQUIRE(!out.try_batch_multisig_verification(points, endorsements));
}

// A decodable endorsement, so the uniform sighash check is reached.
static data_chunk batch_endorsement(uint8_t sighash) NOEXCEPT
{
    ec_signature signature{};
    ecdsa::sign(signature, batch_secret, hash_digest{});
    der_signature der{};
    ecdsa::encode_signature(der, signature);

    auto endorsement = to_chunk(der);
    endorsement.push_back(sighash);
    return endorsement;
}

// One digest per group, so the sighash byte must be uniform.
BOOST_AUTO_TEST_CASE(program__try_batch_multisig_verification__mixed_sighash__false)
{
    const chain::signatures capture{ true };
    const auto tx = accessor_transaction(script{}, max_input_sequence, 0, 1);
    const auto it = tx.inputs_ptr()->begin();
    const batch_accessor in{ tx, it, flags::no_rules, capture };
    const batch_accessor out{ in, to_shared<script>(batch_multisig()) };

    const auto key = batch_key();
    const auto all = batch_endorsement(coverage::hash_all);
    const auto none = batch_endorsement(coverage::hash_none);
    const chunk_xptrs points{ chunk_xptr{ key }, chunk_xptr{ key } };
    const chunk_xptrs endorsements{ chunk_xptr{ all }, chunk_xptr{ none } };
    BOOST_REQUIRE(!out.try_batch_multisig_verification(points, endorsements));
}

BOOST_AUTO_TEST_CASE(program__try_batch_multisig_verification__unbatchable_script__false)
{
    const chain::signatures capture{ true };
    const auto tx = accessor_transaction(script{}, max_input_sequence, 0, 1);
    const auto it = tx.inputs_ptr()->begin();
    const batch_accessor in{ tx, it, flags::no_rules, capture };
    const batch_accessor out{ in, to_shared<script>(batch_p2pk()) };

    const auto key = batch_key();
    const auto endorsement = batch_endorsement(coverage::hash_all);
    const chunk_xptrs points{ chunk_xptr{ key } };
    const chunk_xptrs endorsements{ chunk_xptr{ endorsement } };
    BOOST_REQUIRE(!out.try_batch_multisig_verification(points, endorsements));
}

BOOST_AUTO_TEST_CASE(program__try_batch_multisig_verification__undecodable_endorsement__false)
{
    const chain::signatures capture{ true };
    const auto tx = accessor_transaction(script{}, max_input_sequence, 0, 1);
    const auto it = tx.inputs_ptr()->begin();
    const batch_accessor in{ tx, it, flags::no_rules, capture };
    const batch_accessor out{ in, to_shared<script>(batch_multisig()) };

    const auto key = batch_key();
    const data_chunk garbage(ec_signature_size, 0xff);
    const chunk_xptrs points{ chunk_xptr{ key } };
    const chunk_xptrs endorsements{ chunk_xptr{ garbage } };
    BOOST_REQUIRE(!out.try_batch_multisig_verification(points, endorsements));
}

BOOST_AUTO_TEST_SUITE_END()
