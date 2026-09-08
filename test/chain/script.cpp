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
#include <sstream>
#include "script.hpp"

BOOST_AUTO_TEST_SUITE(script_tests)

using namespace system::chain;
using namespace system::machine;

// Data-driven tests.
// -----------------------------------------------------------------------------

class transaction_accessor
  : public transaction
{
public:
    using transaction::transaction;

    code connect(const context& ctx) const NOEXCEPT
    {
        return transaction::connect(ctx);
    }

    code connect(const context& ctx, uint32_t index) const NOEXCEPT
    {
        return interpreter<contiguous_stack>::connect(ctx, *this, index);
    }
};

transaction_accessor test_tx(const script_test& test)
{
    const script in{ test.input };
    const script out{ test.output };

    if (!in.is_valid() || !out.is_valid())
        return {};

    const transaction_accessor tx
    {
        test.version,
        inputs
        {
            {
                point{},
                in,
                test.input_sequence
            }
        },
        outputs{},
        test.locktime
    };

    tx.inputs_ptr()->front()->prevout = to_shared(output{ {}, out });
    return tx;
}

std::string test_name(const script_test& test)
{
    std::stringstream out;
    out << "input: \"" << test.input << "\" "
        << "prevout: \"" << test.output << "\" "
        << "("
            << test.input_sequence << ", "
            << test.locktime << ", "
            << test.version
        << ") "
        << "name: " << test.description;
    return out.str();
}


BOOST_AUTO_TEST_CASE(script__bip16__valid)
{
    for (const auto& test: valid_bip16_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);
        BOOST_REQUIRE_MESSAGE(tx.is_valid(), name);

        // These are valid prior to and after BIP16 activation.
        BOOST_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) == error::script_success, name);
        BOOST_CHECK_MESSAGE(tx.connect({ flags::bip16_rule }, 0) == error::script_success, name);
        BOOST_CHECK_MESSAGE(tx.connect({ flags::all_rules }, 0) == error::script_success, name);
    }
}

BOOST_AUTO_TEST_CASE(script__bip16__invalidated)
{
    for (const auto& test: invalidated_bip16_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);
        BOOST_REQUIRE_MESSAGE(tx.is_valid(), name);

        // These are valid prior to BIP16 activation and invalid after.
        BOOST_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) == error::script_success, name);
        BOOST_CHECK_MESSAGE(tx.connect({ flags::bip16_rule }, 0) != error::script_success, name);
        BOOST_CHECK_MESSAGE(tx.connect({ flags::all_rules }, 0) != error::script_success, name);
    }
}

BOOST_AUTO_TEST_CASE(script__bip65__valid)
{
    for (const auto& test: valid_bip65_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);
        BOOST_REQUIRE_MESSAGE(tx.is_valid(), name);

        // These are valid prior to and after BIP65 activation.
        BOOST_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) == error::script_success, name);
        BOOST_CHECK_MESSAGE(tx.connect({ flags::bip65_rule }, 0) == error::script_success, name);
        BOOST_CHECK_MESSAGE(tx.connect({ flags::all_rules & ~flags::bip112_rule }, 0) == error::script_success, name);
    }
}

BOOST_AUTO_TEST_CASE(script__bip65__invalid)
{
    for (const auto& test: invalid_bip65_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);
        BOOST_REQUIRE_MESSAGE(tx.is_valid(), name);

        // These are invalid prior to and after BIP65 activation.
        BOOST_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) != error::script_success, name);
        BOOST_CHECK_MESSAGE(tx.connect({ flags::bip65_rule }, 0) != error::script_success, name);
        BOOST_CHECK_MESSAGE(tx.connect({ flags::all_rules & ~flags::bip112_rule }, 0) != error::script_success, name);
    }
}

BOOST_AUTO_TEST_CASE(script__bip65__invalidated)
{
    for (const auto& test: invalidated_bip65_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);
        BOOST_REQUIRE_MESSAGE(tx.is_valid(), name);

        // These are valid prior to BIP65 activation and invalid after.
        BOOST_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) == error::script_success, name);
        BOOST_CHECK_MESSAGE(tx.connect({ flags::bip65_rule }, 0) != error::script_success, name);
        BOOST_CHECK_MESSAGE(tx.connect({ flags::all_rules & ~flags::bip112_rule }, 0) != error::script_success, name);
    }
}

BOOST_AUTO_TEST_CASE(script__bip112__valid)
{
    for (const auto& test: valid_bip112_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);
        BOOST_REQUIRE_MESSAGE(tx.is_valid(), name);

        // These are valid prior to and after BIP112 activation.
        BOOST_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) == error::script_success, name);
        BOOST_CHECK_MESSAGE(tx.connect({ flags::bip112_rule }, 0) == error::script_success, name);
        BOOST_CHECK_MESSAGE(tx.connect({ flags::all_rules }, 0) == error::script_success, name);
    }
}

BOOST_AUTO_TEST_CASE(script__bip112__invalidated)
{
    for (const auto& test: invalidated_bip112_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);
        BOOST_REQUIRE_MESSAGE(tx.is_valid(), name);

        // These are valid prior to BIP112 activation and invalid after.
        BOOST_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) == error::script_success, name);
        BOOST_CHECK_MESSAGE(tx.connect({ flags::bip112_rule }, 0) != error::script_success, name);
        BOOST_CHECK_MESSAGE(tx.connect({ flags::all_rules }, 0) != error::script_success, name);
    }
}

BOOST_AUTO_TEST_CASE(script__number__valid)
{
    for (const auto& test: valid_number_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);
        BOOST_REQUIRE_MESSAGE(tx.is_valid(), name);

        // These are always valid.
        BOOST_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) == error::script_success, name);
        BOOST_CHECK_MESSAGE(tx.connect({ flags::all_rules }, 0) == error::script_success, name);
    }
}

BOOST_AUTO_TEST_CASE(script__number__invalid)
{
    for (const auto& test: invalid_number_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);
        BOOST_REQUIRE_MESSAGE(tx.is_valid(), name);

        // These are always invalid.
        BOOST_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) != error::script_success, name);
        BOOST_CHECK_MESSAGE(tx.connect({ flags::all_rules }, 0) != error::script_success, name);
    }
}

BOOST_AUTO_TEST_CASE(script__multisig__valid)
{
    for (const auto& test: valid_multisig_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);
        BOOST_REQUIRE_MESSAGE(tx.is_valid(), name);

        // These are always valid.
        // These are scripts potentially affected by bip66 (but should not be).
        BOOST_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) == error::script_success, name);
        BOOST_CHECK_MESSAGE(tx.connect({ flags::bip66_rule }, 0) == error::script_success, name);
        BOOST_CHECK_MESSAGE(tx.connect({ flags::all_rules }, 0) == error::script_success, name);
    }
}

BOOST_AUTO_TEST_CASE(script__multisig__bip147_invalidated)
{
    for (const auto& test: invalidated_bip147_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);
        BOOST_REQUIRE_MESSAGE(tx.is_valid(), name);

        // These are valid prior to BIP147 activation and invalid after.
        BOOST_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) == error::script_success, name);
        BOOST_CHECK_MESSAGE(tx.connect({ flags::bip147_rule }, 0) != error::script_success, name);
        BOOST_CHECK_MESSAGE(tx.connect({ flags::all_rules }, 0) != error::script_success, name);
    }
}

BOOST_AUTO_TEST_CASE(script__multisig__invalid)
{
    for (const auto& test: invalid_multisig_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);
        BOOST_REQUIRE_MESSAGE(tx.is_valid(), name);

        // These are always invalid.
        // These are scripts potentially affected by bip66 (but should not be).
        BOOST_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) != error::script_success, name);
        BOOST_CHECK_MESSAGE(tx.connect({ flags::bip66_rule }, 0) != error::script_success, name);
        BOOST_CHECK_MESSAGE(tx.connect({ flags::all_rules }, 0) != error::script_success, name);
    }
}

BOOST_AUTO_TEST_CASE(script__multisig__bip66_invalid_der)
{
    for (const auto& test: invalid_bip66_multisig_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);

        BOOST_REQUIRE_MESSAGE(tx.is_valid(), name);
        BOOST_CHECK_MESSAGE(tx.connect({ flags::bip66_rule }, 0) == error::op_check_multisig_decode_signature, name);
    }
}

BOOST_AUTO_TEST_CASE(script__multisig__soft_errors__push_false)
{
    for (const auto& test: valid_soft_error_multisig_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);

        BOOST_REQUIRE_MESSAGE(tx.is_valid(), name);
        BOOST_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) == error::stack_false, name);
    }
}

BOOST_AUTO_TEST_CASE(script__checksigverify__empty_key_without_bip342__empty_endorsement)
{
    const script_test test
    {
        "",
        "0 0 checksigverify",
        "empty legacy checksigverify key is not a tapscript empty-key error"
    };
    const auto tx = test_tx(test);
    const auto name = test_name(test);
    BOOST_REQUIRE_MESSAGE(tx.is_valid(), name);

    BOOST_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) == error::op_check_sig_verify2, name);
    BOOST_CHECK_MESSAGE(tx.connect({ flags::all_rules }, 0) == error::op_check_sig_verify2, name);
}

BOOST_AUTO_TEST_CASE(script__context_free__valid)
{
    for (const auto& test: valid_context_free_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);
        BOOST_REQUIRE_MESSAGE(tx.is_valid(), name);

        // These are always valid.
        BOOST_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) == error::script_success, name);
        BOOST_CHECK_MESSAGE(tx.connect({ flags::all_rules }, 0) == error::script_success, name);
    }
}

BOOST_AUTO_TEST_CASE(script__context_free__invalid)
{
    for (const auto& test: invalid_context_free_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);
        BOOST_REQUIRE_MESSAGE(tx.is_valid(), name);

        // These are always invalid.
        BOOST_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) != error::script_success, name);
        BOOST_CHECK_MESSAGE(tx.connect({ flags::all_rules }, 0) != error::script_success, name);
    }
}

BOOST_AUTO_TEST_CASE(script__parse__not_invalid)
{
    for (const auto& test: not_invalid_parse_scripts)
    {
        BOOST_CHECK_MESSAGE(test_tx(test).is_valid(), test_name(test));
    }
}

BOOST_AUTO_TEST_CASE(script__parse_syntax__invalid)
{
    for (const auto& test: invalid_syntax_scripts)
    {
        BOOST_CHECK_MESSAGE(!test_tx(test).is_valid(), test_name(test));
    }
}

BOOST_AUTO_TEST_CASE(script__parse_push_not_overflow__valid)
{
    for (const auto& test: valid_push_data_scripts)
    {
        BOOST_CHECK_MESSAGE(test_tx(test).is_valid(), test_name(test));
    }
}

BOOST_AUTO_TEST_CASE(script__parse_push_overflow__invalid)
{
    for (const auto& test: invalid_overflowed_push_data_scripts)
    {
        BOOST_CHECK_MESSAGE(!test_tx(test).is_valid(), test_name(test));
    }
}

// state predicates
// -----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(script__is_prefail__invalid_opcode__true)
{
    const script instance{ operations{ operation{ opcode::op_cat } } };
    BOOST_REQUIRE(instance.is_prefail());
    BOOST_REQUIRE(!instance.is_underflow());
}

BOOST_AUTO_TEST_CASE(script__is_prefail__valid_opcodes__false)
{
    const script instance{ operations{ operation{ opcode::checksig } } };
    BOOST_REQUIRE(!instance.is_prefail());
}

BOOST_AUTO_TEST_CASE(script__is_underflow__truncated_push__true)
{
    const script instance(base16_chunk("4c"), false);
    BOOST_REQUIRE(instance.is_prefail());
    BOOST_REQUIRE(instance.is_underflow());
}

BOOST_AUTO_TEST_CASE(script__is_prevalid__success_opcode__true)
{
    const script instance{ operations{ operation{ opcode::reserved_80 } } };
    BOOST_REQUIRE(instance.is_prevalid());
}

BOOST_AUTO_TEST_CASE(script__is_prevalid__no_success_opcode__false)
{
    const script instance{ operations{ operation{ opcode::checksig } } };
    BOOST_REQUIRE(!instance.is_prevalid());
}

BOOST_AUTO_TEST_CASE(script__is_roller__roll__true)
{
    const script instance{ operations{ operation{ opcode::roll } } };
    BOOST_REQUIRE(instance.is_roller());
}

BOOST_AUTO_TEST_CASE(script__is_roller__pick__false)
{
    const script instance{ operations{ operation{ opcode::pick } } };
    BOOST_REQUIRE(!instance.is_roller());
}

BOOST_AUTO_TEST_CASE(script__is_oversized__at_limit__false)
{
    const script instance{ operations{ operation{ data_chunk(max_script_size - 3u, 0x00), false } } };
    BOOST_REQUIRE_EQUAL(instance.serialized_size(false), max_script_size);
    BOOST_REQUIRE(!instance.is_oversized());
}

BOOST_AUTO_TEST_CASE(script__is_oversized__above_limit__true)
{
    const script instance{ operations{ operation{ data_chunk(max_script_size - 2u, 0x00), false } } };
    BOOST_REQUIRE(instance.is_oversized());
}

BOOST_AUTO_TEST_CASE(script__is_unspendable__op_return__true)
{
    const script instance{ operations{ operation{ opcode::op_return } } };
    BOOST_REQUIRE(instance.is_unspendable());
}

BOOST_AUTO_TEST_CASE(script__is_unspendable__leading_invalid__true)
{
    const script instance{ operations{ operation{ opcode::op_cat } } };
    BOOST_REQUIRE(instance.is_unspendable());
}

BOOST_AUTO_TEST_CASE(script__is_unspendable__empty__false)
{
    const script instance{};
    BOOST_REQUIRE(!instance.is_unspendable());
}

BOOST_AUTO_TEST_CASE(script__is_unspendable__spendable__false)
{
    const script instance{ operations{ operation{ opcode::checksig } } };
    BOOST_REQUIRE(!instance.is_unspendable());
}

// A default initialized or failed parse script hashes to null_hash.
BOOST_AUTO_TEST_CASE(script__hash__default__null_hash)
{
    const script instance{};
    BOOST_REQUIRE(!instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.hash(), null_hash);
}

BOOST_AUTO_TEST_CASE(script__hash__valid_empty__sha256_of_empty)
{
    const script instance(data_chunk{}, false);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.hash(), sha256_hash(data_chunk{}));
}

BOOST_AUTO_TEST_CASE(script__hash__checksig__expected)
{
    const script instance{ operations{ operation{ opcode::checksig } } };
    BOOST_REQUIRE_EQUAL(instance.hash(), sha256_hash(base16_chunk("ac")));
}

BOOST_AUTO_TEST_SUITE_END()
