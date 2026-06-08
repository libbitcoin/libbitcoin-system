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

// Taproot changes signature requirements, not expected in these tests.
constexpr auto all_but_taproot = flags::all_rules & ~flags::bip342_rule;

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
        BOOST_CHECK_MESSAGE(tx.connect({ all_but_taproot }, 0) == error::script_success, name);
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
        BOOST_CHECK_MESSAGE(tx.connect({ all_but_taproot }, 0) != error::script_success, name);
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
        ////BOOST_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) == error::script_success, name);
        BOOST_CHECK_MESSAGE(tx.connect({ flags::bip65_rule }, 0) == error::script_success, name);
        ////BOOST_CHECK_MESSAGE(tx.connect({ all_but_taproot & ~flags::bip112_rule }, 0) == error::script_success, name);
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
        BOOST_CHECK_MESSAGE(tx.connect({ all_but_taproot & ~flags::bip112_rule }, 0) != error::script_success, name);
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
        BOOST_CHECK_MESSAGE(tx.connect({ all_but_taproot & ~flags::bip112_rule }, 0) != error::script_success, name);
    }
}

// TODO: add bip112 test cases.

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

        // One test fails under bip147 due to alternating results on the stack.
        BOOST_CHECK_MESSAGE(tx.connect({ all_but_taproot & ~flags::bip147_rule }, 0) == error::script_success, name);
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
        BOOST_CHECK_MESSAGE(tx.connect({ all_but_taproot }, 0) != error::script_success, name);
    }
}

BOOST_AUTO_TEST_CASE(script__multisig__bip66_invalid_der)
{
    for (const auto& test: invalid_bip66_multisig_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);

        BOOST_REQUIRE_MESSAGE(tx.is_valid(), name);
        BOOST_CHECK_MESSAGE(tx.connect({ flags::bip66_rule }, 0) == error::op_check_multisig_parse_signature, name);
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
    BOOST_CHECK_MESSAGE(tx.connect({ all_but_taproot }, 0) == error::op_check_sig_verify2, name);
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
        BOOST_CHECK_MESSAGE(tx.connect({ all_but_taproot }, 0) == error::script_success, name);
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
        BOOST_CHECK_MESSAGE(tx.connect({ all_but_taproot }, 0) != error::script_success, name);
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

BOOST_AUTO_TEST_SUITE_END()
