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

BOOST_AUTO_TEST_SUITE(interpreter_connect_tests)

using namespace system::chain;
using namespace system::machine;

// connect
// ----------------------------------------------------------------------------
// Signature verification is faked, isolating script and witness dispatch.

using mocked = mock_program<contiguous_stack>;
using connector = interpreter_accessor<contiguous_stack, mocked>;

static script unrunnable_script() NOEXCEPT
{
    const data_chunk data(add1(max_push_data_size), 0x00_u8);
    return script{ operations{ operation{ data, false } } };
}

static transaction connect_tx(const script& input_script,
    const script& prevout_script, const chain::witness& spender={}) NOEXCEPT
{
    const inputs ins
    {
        input{ point{ one_hash, 0 }, input_script, spender, max_input_sequence }
    };

    const outputs outs{ output{ 42, script{} } };
    const transaction tx{ 1, ins, outs, 0 };
    const output prevout{ 42, prevout_script };
    (*tx.inputs_ptr())[0]->prevout = to_shared(prevout);
    return tx;
}

static transaction unfunded_tx() NOEXCEPT
{
    const inputs ins
    {
        input{ point{ one_hash, 0 }, script{}, max_input_sequence }
    };

    const outputs outs{ output{ 42, script{} } };
    return transaction{ 1, ins, outs, 0 };
}

BOOST_AUTO_TEST_CASE(interpreter__connect__index_above_inputs__inputs_overflow)
{
    const auto tx = connect_tx(script{}, script{});
    BOOST_REQUIRE_EQUAL(connector::connect({ flags::all_rules }, tx, 1), error::inputs_overflow);
}

BOOST_AUTO_TEST_CASE(interpreter__connect__no_prevout__missing_previous_output)
{
    const auto tx = unfunded_tx();
    BOOST_REQUIRE_EQUAL(connector::connect({ flags::all_rules }, tx, 0), error::missing_previous_output);
}

BOOST_AUTO_TEST_CASE(interpreter__connect__input_script_error__returned)
{
    const auto tx = connect_tx(unrunnable_script(), script{});
    BOOST_REQUIRE_EQUAL(connector::connect({ flags::all_rules }, tx, 0), error::invalid_push_data_size);
}

BOOST_AUTO_TEST_CASE(interpreter__connect__output_script_error__returned)
{
    const auto tx = connect_tx(script{}, unrunnable_script());
    BOOST_REQUIRE_EQUAL(connector::connect({ flags::all_rules }, tx, 0), error::invalid_push_data_size);
}

BOOST_AUTO_TEST_CASE(interpreter__connect__empty_stack__stack_false)
{
    const auto tx = connect_tx(script{}, script{});
    BOOST_REQUIRE_EQUAL(connector::connect({ flags::all_rules }, tx, 0), error::stack_false);
}

BOOST_AUTO_TEST_CASE(interpreter__connect__true_stack__script_success)
{
    const script input_script{ operations{ operation{ opcode::push_positive_1 } } };
    const auto tx = connect_tx(input_script, script{});
    BOOST_REQUIRE_EQUAL(connector::connect({ flags::all_rules }, tx, 0), error::script_success);
}

BOOST_AUTO_TEST_CASE(interpreter__connect__non_witness_program_with_witness__unexpected_witness)
{
    const script input_script{ operations{ operation{ opcode::push_positive_1 } } };
    const chunk_cptrs stack{ to_shared<data_chunk>(data_chunk{ 0x01_u8 }) };
    const chain::witness spender{ stack };
    const auto tx = connect_tx(input_script, script{}, spender);
    BOOST_REQUIRE_EQUAL(connector::connect({ flags::all_rules }, tx, 0), error::unexpected_witness);
}

// The program remains on the stack, so it must be non-zero to reach the branch.
BOOST_AUTO_TEST_CASE(interpreter__connect__witness_program_dirty_input_script__dirty_witness)
{
    const data_chunk program(short_hash_size, 0x01_u8);
    const script prevout{ script::to_pay_witness_pattern(0, program) };
    const script input_script{ operations{ operation{ opcode::push_positive_1 } } };
    const auto tx = connect_tx(input_script, prevout);
    BOOST_REQUIRE_EQUAL(connector::connect({ flags::all_rules }, tx, 0), error::dirty_witness);
}

// connect_witness
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(interpreter__connect_witness__unversioned__unversioned_script)
{
    const auto tx = connect_tx(script{}, script{});
    const auto it = tx.inputs_ptr()->begin();
    const signatures capture{};
    const script prevout{};
    BOOST_REQUIRE_EQUAL(connector::connect_witness({ flags::all_rules }, tx, it, prevout, false, capture), error::unversioned_script);
}

BOOST_AUTO_TEST_CASE(interpreter__connect_witness__reserved_version__script_success)
{
    const data_chunk program(hash_size, 0x00_u8);
    const script prevout{ script::to_pay_witness_pattern(2, program) };
    const auto tx = connect_tx(script{}, prevout);
    const auto it = tx.inputs_ptr()->begin();
    const signatures capture{};
    BOOST_REQUIRE_EQUAL(connector::connect_witness({ flags::all_rules }, tx, it, prevout, false, capture), error::script_success);
}

BOOST_AUTO_TEST_CASE(interpreter__connect_witness__taproot_embedded__script_success)
{
    const data_chunk program(hash_size, 0x00_u8);
    const script prevout{ script::to_pay_witness_pattern(1, program) };
    const auto tx = connect_tx(script{}, prevout);
    const auto it = tx.inputs_ptr()->begin();
    const signatures capture{};
    BOOST_REQUIRE_EQUAL(connector::connect_witness({ flags::all_rules }, tx, it, prevout, true, capture), error::script_success);
}

BOOST_AUTO_TEST_CASE(interpreter__connect_witness__taproot_bip341_off__script_success)
{
    const data_chunk program(hash_size, 0x00_u8);
    const script prevout{ script::to_pay_witness_pattern(1, program) };
    const auto tx = connect_tx(script{}, prevout);
    const auto it = tx.inputs_ptr()->begin();
    const signatures capture{};
    BOOST_REQUIRE_EQUAL(connector::connect_witness({ flags::bip141_rule }, tx, it, prevout, false, capture), error::script_success);
}

BOOST_AUTO_TEST_CASE(interpreter__connect_witness__taproot_empty_witness__invalid_witness)
{
    const data_chunk program(hash_size, 0x00_u8);
    const script prevout{ script::to_pay_witness_pattern(1, program) };
    const auto tx = connect_tx(script{}, prevout);
    const auto it = tx.inputs_ptr()->begin();
    const signatures capture{};
    BOOST_REQUIRE_EQUAL(connector::connect_witness({ flags::all_rules }, tx, it, prevout, false, capture), error::invalid_witness);
}

BOOST_AUTO_TEST_CASE(interpreter__connect_witness__segwit_empty_witness__invalid_witness)
{
    const data_chunk program(short_hash_size, 0x00_u8);
    const script prevout{ script::to_pay_witness_pattern(0, program) };
    const auto tx = connect_tx(script{}, prevout);
    const auto it = tx.inputs_ptr()->begin();
    const signatures capture{};
    BOOST_REQUIRE_EQUAL(connector::connect_witness({ flags::all_rules }, tx, it, prevout, false, capture), error::invalid_witness);
}

static script witness_script() NOEXCEPT
{
    return script{ operations{ operation{ opcode::push_positive_1 } } };
}

static script false_witness_script() NOEXCEPT
{
    return script{ operations{ operation{ opcode::push_size_0 } } };
}

static data_chunk witness_program(const script& script) NOEXCEPT
{
    return to_chunk(sha256_hash(script.to_data(false)));
}

BOOST_AUTO_TEST_CASE(interpreter__connect_witness__segwit_true_script__script_success)
{
    const script prevout{ script::to_pay_witness_pattern(0, witness_program(witness_script())) };
    const chunk_cptrs stack
    {
        to_shared<data_chunk>(witness_script().to_data(false))
    };

    const chain::witness spender{ stack };
    const auto tx = connect_tx(script{}, prevout, spender);
    const auto it = tx.inputs_ptr()->begin();
    const signatures capture{};
    BOOST_REQUIRE_EQUAL(connector::connect_witness({ flags::all_rules }, tx, it, prevout, false, capture), error::script_success);
}

BOOST_AUTO_TEST_CASE(interpreter__connect_witness__taproot_key_path__script_success)
{
    const data_chunk program(hash_size, 0x02_u8);
    const script prevout{ script::to_pay_witness_pattern(1, program) };
    const chunk_cptrs stack
    {
        to_shared<data_chunk>(data_chunk(ec_signature_size, 0x11_u8))
    };

    const chain::witness spender{ stack };
    const auto tx = connect_tx(script{}, prevout, spender);
    const auto it = tx.inputs_ptr()->begin();
    const signatures capture{};
    BOOST_REQUIRE_EQUAL(connector::connect_witness({ flags::all_rules }, tx, it, prevout, false, capture), error::script_success);
}

BOOST_AUTO_TEST_CASE(interpreter__connect__pay_to_script_hash__script_success)
{
    const script embedded{ operations{ operation{ opcode::push_positive_1 } } };
    const auto data = embedded.to_data(false);
    const script prevout{ script::to_pay_script_hash_pattern(bitcoin_short_hash(data)) };
    const script input_script{ operations{ operation{ data, false } } };
    const auto tx = connect_tx(input_script, prevout);
    BOOST_REQUIRE_EQUAL(connector::connect({ flags::all_rules }, tx, 0), error::script_success);
}

BOOST_AUTO_TEST_CASE(interpreter__connect__pay_to_witness__script_success)
{
    const script prevout{ script::to_pay_witness_pattern(0, witness_program(witness_script())) };
    const chunk_cptrs stack
    {
        to_shared<data_chunk>(witness_script().to_data(false))
    };

    const chain::witness spender{ stack };
    const auto tx = connect_tx(script{}, prevout, spender);
    BOOST_REQUIRE_EQUAL(connector::connect({ flags::all_rules }, tx, 0), error::script_success);
}

BOOST_AUTO_TEST_CASE(interpreter__connect__pay_to_script_hash_false_embedded__stack_false)
{
    const script embedded{ operations{ operation{ opcode::push_size_0 } } };
    const auto data = embedded.to_data(false);
    const script prevout{ script::to_pay_script_hash_pattern(bitcoin_short_hash(data)) };
    const script input_script{ operations{ operation{ data, false } } };
    const auto tx = connect_tx(input_script, prevout);
    BOOST_REQUIRE_EQUAL(connector::connect({ flags::all_rules }, tx, 0), error::stack_false);
}

BOOST_AUTO_TEST_CASE(interpreter__connect__pay_to_witness_false_script__stack_false)
{
    const auto program = witness_program(false_witness_script());
    const script prevout{ script::to_pay_witness_pattern(0, program) };
    const chunk_cptrs stack
    {
        to_shared<data_chunk>(false_witness_script().to_data(false))
    };

    const chain::witness spender{ stack };
    const auto tx = connect_tx(script{}, prevout, spender);
    BOOST_REQUIRE_EQUAL(connector::connect({ flags::all_rules }, tx, 0), error::stack_false);
}

BOOST_AUTO_TEST_CASE(interpreter__connect_witness__segwit_script_error__returned)
{
    const auto program = witness_program(unrunnable_script());
    const script prevout{ script::to_pay_witness_pattern(0, program) };
    const chunk_cptrs stack
    {
        to_shared<data_chunk>(unrunnable_script().to_data(false))
    };

    const chain::witness spender{ stack };
    const auto tx = connect_tx(script{}, prevout, spender);
    const auto it = tx.inputs_ptr()->begin();
    const signatures capture{};
    BOOST_REQUIRE_EQUAL(connector::connect_witness({ flags::all_rules }, tx, it, prevout, false, capture), error::invalid_push_data_size);
}

BOOST_AUTO_TEST_CASE(interpreter__connect_witness__taproot_invalid_sighash__op_check_sig_schnorr1)
{
    const data_chunk program(hash_size, 0x02_u8);
    const script prevout{ script::to_pay_witness_pattern(1, program) };
    data_chunk endorsement(add1(ec_signature_size), 0x11_u8);
    endorsement.back() = coverage::invalid;
    const chunk_cptrs stack{ to_shared<data_chunk>(endorsement) };
    const chain::witness spender{ stack };
    const auto tx = connect_tx(script{}, prevout, spender);
    const auto it = tx.inputs_ptr()->begin();
    const signatures capture{};
    BOOST_REQUIRE_EQUAL(connector::connect_witness({ flags::all_rules }, tx, it, prevout, false, capture), error::op_check_sig_schnorr1);
}

// connect_embedded
// ----------------------------------------------------------------------------
// The embedded script is popped from the input script evaluation stack.

BOOST_AUTO_TEST_CASE(interpreter__connect_embedded__non_push_input_script__invalid_script_embed)
{
    const script input_script{ operations{ operation{ opcode::dup } } };
    const auto tx = connect_tx(input_script, script{});
    const auto it = tx.inputs_ptr()->begin();
    const signatures capture{};
    connector in{ tx, it, flags::all_rules, capture };
    BOOST_REQUIRE_EQUAL(connector::connect_embedded({ flags::all_rules }, tx, it, in, capture), error::invalid_script_embed);
}

BOOST_AUTO_TEST_CASE(interpreter__connect_embedded__empty_embedded__stack_false)
{
    const script input_script{ operations{ operation{ opcode::push_size_0 } } };
    const auto tx = connect_tx(input_script, script{});
    const auto it = tx.inputs_ptr()->begin();
    const signatures capture{};
    connector in{ tx, it, flags::all_rules, capture };
    in.push_chunk(data_chunk{});
    BOOST_REQUIRE_EQUAL(connector::connect_embedded({ flags::all_rules }, tx, it, in, capture), error::stack_false);
}

BOOST_AUTO_TEST_CASE(interpreter__connect_embedded__embedded_script_error__returned)
{
    const script input_script{ operations{ operation{ opcode::push_size_0 } } };
    const auto tx = connect_tx(input_script, script{});
    const auto it = tx.inputs_ptr()->begin();
    const signatures capture{};
    connector in{ tx, it, flags::all_rules, capture };
    in.push_chunk(unrunnable_script().to_data(false));
    BOOST_REQUIRE_EQUAL(connector::connect_embedded({ flags::all_rules }, tx, it, in, capture), error::invalid_push_data_size);
}

BOOST_AUTO_TEST_CASE(interpreter__connect_embedded__true_embedded__script_success)
{
    const script embedded{ operations{ operation{ opcode::push_positive_1 } } };
    const auto data = embedded.to_data(false);
    const script input_script{ operations{ operation{ data, false } } };
    const auto tx = connect_tx(input_script, script{});
    const auto it = tx.inputs_ptr()->begin();
    const signatures capture{};
    connector in{ tx, it, flags::all_rules, capture };
    in.push_chunk(data_chunk{ data });
    BOOST_REQUIRE_EQUAL(connector::connect_embedded({ flags::all_rules }, tx, it, in, capture), error::script_success);
}

BOOST_AUTO_TEST_CASE(interpreter__connect_embedded__non_witness_embedded_with_witness__unexpected_witness)
{
    const script embedded{ operations{ operation{ opcode::push_positive_1 } } };
    const auto data = embedded.to_data(false);
    const script input_script{ operations{ operation{ data, false } } };
    const chunk_cptrs stack{ to_shared<data_chunk>(data_chunk{ 0x01_u8 }) };
    const chain::witness spender{ stack };
    const auto tx = connect_tx(input_script, script{}, spender);
    const auto it = tx.inputs_ptr()->begin();
    const signatures capture{};
    connector in{ tx, it, flags::all_rules, capture };
    in.push_chunk(data_chunk{ data });
    BOOST_REQUIRE_EQUAL(connector::connect_embedded({ flags::all_rules }, tx, it, in, capture), error::unexpected_witness);
}

BOOST_AUTO_TEST_CASE(interpreter__connect_embedded__embedded_witness__script_success)
{
    const script embedded{ script::to_pay_witness_pattern(0, witness_program(witness_script())) };
    const auto data = embedded.to_data(false);
    const script input_script{ operations{ operation{ data, false } } };
    const chunk_cptrs stack
    {
        to_shared<data_chunk>(witness_script().to_data(false))
    };

    const chain::witness spender{ stack };
    const auto tx = connect_tx(input_script, script{}, spender);
    const auto it = tx.inputs_ptr()->begin();
    const signatures capture{};
    connector in{ tx, it, flags::all_rules, capture };
    in.push_chunk(data_chunk{ data });
    BOOST_REQUIRE_EQUAL(connector::connect_embedded({ flags::all_rules }, tx, it, in, capture), error::script_success);
}

BOOST_AUTO_TEST_CASE(interpreter__connect_embedded__embedded_witness_false_script__stack_false)
{
    const auto program = witness_program(false_witness_script());
    const script embedded{ script::to_pay_witness_pattern(0, program) };
    const auto data = embedded.to_data(false);
    const script input_script{ operations{ operation{ data, false } } };
    const chunk_cptrs stack
    {
        to_shared<data_chunk>(false_witness_script().to_data(false))
    };

    const chain::witness spender{ stack };
    const auto tx = connect_tx(input_script, script{}, spender);
    const auto it = tx.inputs_ptr()->begin();
    const signatures capture{};
    connector in{ tx, it, flags::all_rules, capture };
    in.push_chunk(data_chunk{ data });
    BOOST_REQUIRE_EQUAL(connector::connect_embedded({ flags::all_rules }, tx, it, in, capture), error::stack_false);
}

BOOST_AUTO_TEST_CASE(interpreter__connect_embedded__dirty_embedded_witness__dirty_embed)
{
    const data_chunk program(short_hash_size, 0x01_u8);
    const script embedded{ script::to_pay_witness_pattern(0, program) };
    const auto data = embedded.to_data(false);
    const operations ops
    {
        operation{ opcode::push_size_0 },
        operation{ data, false }
    };

    const auto tx = connect_tx(script{ ops }, script{});
    const auto it = tx.inputs_ptr()->begin();
    const signatures capture{};
    connector in{ tx, it, flags::all_rules, capture };
    in.push_chunk(data_chunk{ data });
    BOOST_REQUIRE_EQUAL(connector::connect_embedded({ flags::all_rules }, tx, it, in, capture), error::dirty_embed);
}

BOOST_AUTO_TEST_SUITE_END()

