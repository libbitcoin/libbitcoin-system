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

BC_AUTO_TEST_SUITE(script_tests)

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


BC_AUTO_TEST_CASE(script__bip16__valid)
{
    for (const auto& test: valid_bip16_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);
        BC_REQUIRE_MESSAGE(tx.is_valid(), name);

        // These are valid prior to and after BIP16 activation.
        BC_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) == error::script_success, name);
        BC_CHECK_MESSAGE(tx.connect({ flags::bip16_rule }, 0) == error::script_success, name);
        BC_CHECK_MESSAGE(tx.connect({ flags::all_rules }, 0) == error::script_success, name);
    }
}

BC_AUTO_TEST_CASE(script__bip16__invalidated)
{
    for (const auto& test: invalidated_bip16_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);
        BC_REQUIRE_MESSAGE(tx.is_valid(), name);

        // These are valid prior to BIP16 activation and invalid after.
        BC_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) == error::script_success, name);
        BC_CHECK_MESSAGE(tx.connect({ flags::bip16_rule }, 0) != error::script_success, name);
        BC_CHECK_MESSAGE(tx.connect({ flags::all_rules }, 0) != error::script_success, name);
    }
}

BC_AUTO_TEST_CASE(script__bip65__valid)
{
    for (const auto& test: valid_bip65_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);
        BC_REQUIRE_MESSAGE(tx.is_valid(), name);

        // These are valid prior to and after BIP65 activation.
        BC_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) == error::script_success, name);
        BC_CHECK_MESSAGE(tx.connect({ flags::bip65_rule }, 0) == error::script_success, name);
        BC_CHECK_MESSAGE(tx.connect({ flags::all_rules & ~flags::bip112_rule }, 0) == error::script_success, name);
    }
}

BC_AUTO_TEST_CASE(script__bip65__invalid)
{
    for (const auto& test: invalid_bip65_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);
        BC_REQUIRE_MESSAGE(tx.is_valid(), name);

        // These are invalid prior to and after BIP65 activation.
        BC_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) != error::script_success, name);
        BC_CHECK_MESSAGE(tx.connect({ flags::bip65_rule }, 0) != error::script_success, name);
        BC_CHECK_MESSAGE(tx.connect({ flags::all_rules & ~flags::bip112_rule }, 0) != error::script_success, name);
    }
}

BC_AUTO_TEST_CASE(script__bip65__invalidated)
{
    for (const auto& test: invalidated_bip65_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);
        BC_REQUIRE_MESSAGE(tx.is_valid(), name);

        // These are valid prior to BIP65 activation and invalid after.
        BC_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) == error::script_success, name);
        BC_CHECK_MESSAGE(tx.connect({ flags::bip65_rule }, 0) != error::script_success, name);
        BC_CHECK_MESSAGE(tx.connect({ flags::all_rules & ~flags::bip112_rule }, 0) != error::script_success, name);
    }
}

BC_AUTO_TEST_CASE(script__bip112__valid)
{
    for (const auto& test: valid_bip112_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);
        BC_REQUIRE_MESSAGE(tx.is_valid(), name);

        // These are valid prior to and after BIP112 activation.
        BC_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) == error::script_success, name);
        BC_CHECK_MESSAGE(tx.connect({ flags::bip112_rule }, 0) == error::script_success, name);
        BC_CHECK_MESSAGE(tx.connect({ flags::all_rules }, 0) == error::script_success, name);
    }
}

BC_AUTO_TEST_CASE(script__bip112__invalidated)
{
    for (const auto& test: invalidated_bip112_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);
        BC_REQUIRE_MESSAGE(tx.is_valid(), name);

        // These are valid prior to BIP112 activation and invalid after.
        BC_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) == error::script_success, name);
        BC_CHECK_MESSAGE(tx.connect({ flags::bip112_rule }, 0) != error::script_success, name);
        BC_CHECK_MESSAGE(tx.connect({ flags::all_rules }, 0) != error::script_success, name);
    }
}

BC_AUTO_TEST_CASE(script__number__valid)
{
    for (const auto& test: valid_number_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);
        BC_REQUIRE_MESSAGE(tx.is_valid(), name);

        // These are always valid.
        BC_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) == error::script_success, name);
        BC_CHECK_MESSAGE(tx.connect({ flags::all_rules }, 0) == error::script_success, name);
    }
}

BC_AUTO_TEST_CASE(script__number__invalid)
{
    for (const auto& test: invalid_number_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);
        BC_REQUIRE_MESSAGE(tx.is_valid(), name);

        // These are always invalid.
        BC_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) != error::script_success, name);
        BC_CHECK_MESSAGE(tx.connect({ flags::all_rules }, 0) != error::script_success, name);
    }
}

BC_AUTO_TEST_CASE(script__multisig__valid)
{
    for (const auto& test: valid_multisig_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);
        BC_REQUIRE_MESSAGE(tx.is_valid(), name);

        // These are always valid.
        // These are scripts potentially affected by bip66 (but should not be).
        BC_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) == error::script_success, name);
        BC_CHECK_MESSAGE(tx.connect({ flags::bip66_rule }, 0) == error::script_success, name);
        BC_CHECK_MESSAGE(tx.connect({ flags::all_rules }, 0) == error::script_success, name);
    }
}

BC_AUTO_TEST_CASE(script__multisig__bip147_invalidated)
{
    for (const auto& test: invalidated_bip147_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);
        BC_REQUIRE_MESSAGE(tx.is_valid(), name);

        // These are valid prior to BIP147 activation and invalid after.
        BC_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) == error::script_success, name);
        BC_CHECK_MESSAGE(tx.connect({ flags::bip147_rule }, 0) != error::script_success, name);
        BC_CHECK_MESSAGE(tx.connect({ flags::all_rules }, 0) != error::script_success, name);
    }
}

BC_AUTO_TEST_CASE(script__multisig__invalid)
{
    for (const auto& test: invalid_multisig_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);
        BC_REQUIRE_MESSAGE(tx.is_valid(), name);

        // These are always invalid.
        // These are scripts potentially affected by bip66 (but should not be).
        BC_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) != error::script_success, name);
        BC_CHECK_MESSAGE(tx.connect({ flags::bip66_rule }, 0) != error::script_success, name);
        BC_CHECK_MESSAGE(tx.connect({ flags::all_rules }, 0) != error::script_success, name);
    }
}

BC_AUTO_TEST_CASE(script__multisig__bip66_invalid_der)
{
    for (const auto& test: invalid_bip66_multisig_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);

        BC_REQUIRE_MESSAGE(tx.is_valid(), name);
        BC_CHECK_MESSAGE(tx.connect({ flags::bip66_rule }, 0) == error::op_check_multisig_decode_signature, name);
    }
}

BC_AUTO_TEST_CASE(script__multisig__soft_errors__push_false)
{
    for (const auto& test: valid_soft_error_multisig_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);

        BC_REQUIRE_MESSAGE(tx.is_valid(), name);
        BC_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) == error::stack_false, name);
    }
}

// Keys and endorsements are both evaluated from the top of the stack.
constexpr auto multisig_not_key_a = "03dcfd9e580de35d8c2060d76dbf9e5561fe20febd2e64380e860a4d59f15ac864";
constexpr auto multisig_not_key_b = "02440e0304bf8d32b2012994393c6a477acf238dd6adb4c3cef5bfa72f30c9861c";

// The spend is not a coinbase, so that it can be endorsed.
static transaction_accessor multisig_not_spend(const std::string& input_script, const std::string& output_script)
{
    const transaction_accessor tx{ 0, inputs{ { point{ one_hash, 0 }, script{ input_script }, 0 } }, outputs{}, 0 };
    tx.inputs_ptr()->front()->prevout = to_shared(output{ {}, script{ output_script } });
    return tx;
}

// 3-of-3 with the signed key second or top, scriptSig: 0 [bad] [bad] [sig].
static transaction_accessor multisig_not_tx(bool signed_key_on_top)
{
    const ec_secret secret = base16_hash("ce8f4b713ffdd2658900845251890f30371856be201cd1f5b3d970f793634333");
    ec_compressed point;
    if (!secret_to_public(point, secret))
        return {};

    const auto signed_key = encode_base16(point);
    const auto key2 = signed_key_on_top ? multisig_not_key_b : signed_key;
    const auto key3 = signed_key_on_top ? signed_key : multisig_not_key_b;
    const auto output = "3 [" + std::string{ multisig_not_key_a } + "] [" + key2 + "] [" + key3 + "] 3 checkmultisig not";

    endorsement top;
    const auto unsigned_tx = multisig_not_spend("", output);
    if (!unsigned_tx.create_endorsement(top, secret, script{ output }, 0, 0, coverage::hash_all, script_version::unversioned, flags::no_rules))
        return {};

    return multisig_not_spend("0 [0101] [0101] [" + encode_base16(top) + "]", output);
}

BC_AUTO_TEST_CASE(script__multisig_not__top_endorsement_fails_top_key__invalid_der_not_decoded)
{
    const auto tx = multisig_not_tx(false);
    BC_REQUIRE(tx.is_valid());

    // Three endorsements remain against two keys, so the op is false.
    BC_CHECK(tx.connect({ flags::no_rules }, 0) == error::script_success);
    BC_CHECK(tx.connect({ flags::bip66_rule }, 0) == error::script_success);
    BC_CHECK(tx.connect({ flags::all_rules }, 0) == error::script_success);
}

BC_AUTO_TEST_CASE(script__multisig_not__top_endorsement_verifies_top_key__invalid_der_decoded)
{
    const auto tx = multisig_not_tx(true);
    BC_REQUIRE(tx.is_valid());

    // Two endorsements remain against two keys, so the next is decoded.
    BC_CHECK(tx.connect({ flags::no_rules }, 0) == error::script_success);
    BC_CHECK(tx.connect({ flags::bip66_rule }, 0) == error::op_check_multisig_decode_signature);
    BC_CHECK(tx.connect({ flags::all_rules }, 0) == error::op_check_multisig_decode_signature);
}

BC_AUTO_TEST_CASE(script__checksigverify__empty_key_without_bip342__empty_endorsement)
{
    const script_test test
    {
        "",
        "0 0 checksigverify",
        "empty legacy checksigverify key is not a tapscript empty-key error"
    };
    const auto tx = test_tx(test);
    const auto name = test_name(test);
    BC_REQUIRE_MESSAGE(tx.is_valid(), name);

    BC_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) == error::op_check_sig_verify2, name);
    BC_CHECK_MESSAGE(tx.connect({ flags::all_rules }, 0) == error::op_check_sig_verify2, name);
}

BC_AUTO_TEST_CASE(script__context_free__valid)
{
    for (const auto& test: valid_context_free_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);
        BC_REQUIRE_MESSAGE(tx.is_valid(), name);

        // These are always valid.
        BC_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) == error::script_success, name);
        BC_CHECK_MESSAGE(tx.connect({ flags::all_rules }, 0) == error::script_success, name);
    }
}

BC_AUTO_TEST_CASE(script__context_free__invalid)
{
    for (const auto& test: invalid_context_free_scripts)
    {
        const auto tx = test_tx(test);
        const auto name = test_name(test);
        BC_REQUIRE_MESSAGE(tx.is_valid(), name);

        // These are always invalid.
        BC_CHECK_MESSAGE(tx.connect({ flags::no_rules }, 0) != error::script_success, name);
        BC_CHECK_MESSAGE(tx.connect({ flags::all_rules }, 0) != error::script_success, name);
    }
}

BC_AUTO_TEST_CASE(script__parse__not_invalid)
{
    for (const auto& test: not_invalid_parse_scripts)
    {
        BC_CHECK_MESSAGE(test_tx(test).is_valid(), test_name(test));
    }
}

BC_AUTO_TEST_CASE(script__parse_syntax__invalid)
{
    for (const auto& test: invalid_syntax_scripts)
    {
        BC_CHECK_MESSAGE(!test_tx(test).is_valid(), test_name(test));
    }
}

BC_AUTO_TEST_CASE(script__parse_push_not_overflow__valid)
{
    for (const auto& test: valid_push_data_scripts)
    {
        BC_CHECK_MESSAGE(test_tx(test).is_valid(), test_name(test));
    }
}

BC_AUTO_TEST_CASE(script__parse_push_overflow__invalid)
{
    for (const auto& test: invalid_overflowed_push_data_scripts)
    {
        BC_CHECK_MESSAGE(!test_tx(test).is_valid(), test_name(test));
    }
}

// state predicates
// -----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(script__is_prefail__invalid_opcode__true)
{
    const script instance{ operations{ operation{ opcode::op_cat } } };
    BC_REQUIRE(instance.is_prefail());
    BC_REQUIRE(!instance.is_underflow());
}

BC_AUTO_TEST_CASE(script__is_prefail__valid_opcodes__false)
{
    const script instance{ operations{ operation{ opcode::checksig } } };
    BC_REQUIRE(!instance.is_prefail());
}

BC_AUTO_TEST_CASE(script__is_underflow__truncated_push__true)
{
    const script instance(base16_chunk("4c"), false);
    BC_REQUIRE(instance.is_prefail());
    BC_REQUIRE(instance.is_underflow());
}

BC_AUTO_TEST_CASE(script__is_prevalid__success_opcode__true)
{
    const script instance{ operations{ operation{ opcode::reserved_80 } } };
    BC_REQUIRE(instance.is_prevalid());
}

BC_AUTO_TEST_CASE(script__is_prevalid__no_success_opcode__false)
{
    const script instance{ operations{ operation{ opcode::checksig } } };
    BC_REQUIRE(!instance.is_prevalid());
}

BC_AUTO_TEST_CASE(script__is_roller__roll__true)
{
    const script instance{ operations{ operation{ opcode::roll } } };
    BC_REQUIRE(instance.is_roller());
}

BC_AUTO_TEST_CASE(script__is_roller__pick__false)
{
    const script instance{ operations{ operation{ opcode::pick } } };
    BC_REQUIRE(!instance.is_roller());
}

BC_AUTO_TEST_CASE(script__is_oversized__at_limit__false)
{
    const script instance{ operations{ operation{ data_chunk(max_script_size - 3u, 0x00), false } } };
    BC_REQUIRE_EQUAL(instance.serialized_size(false), max_script_size);
    BC_REQUIRE(!instance.is_oversized());
}

BC_AUTO_TEST_CASE(script__is_oversized__above_limit__true)
{
    const script instance{ operations{ operation{ data_chunk(max_script_size - 2u, 0x00), false } } };
    BC_REQUIRE(instance.is_oversized());
}

BC_AUTO_TEST_CASE(script__is_unspendable__op_return__true)
{
    const script instance{ operations{ operation{ opcode::op_return } } };
    BC_REQUIRE(instance.is_unspendable());
}

BC_AUTO_TEST_CASE(script__is_unspendable__leading_invalid__true)
{
    const script instance{ operations{ operation{ opcode::op_cat } } };
    BC_REQUIRE(instance.is_unspendable());
}

BC_AUTO_TEST_CASE(script__is_unspendable__empty__false)
{
    const script instance{};
    BC_REQUIRE(!instance.is_unspendable());
}

BC_AUTO_TEST_CASE(script__is_unspendable__spendable__false)
{
    const script instance{ operations{ operation{ opcode::checksig } } };
    BC_REQUIRE(!instance.is_unspendable());
}

// A default initialized or failed parse script hashes to null_hash.
BC_AUTO_TEST_CASE(script__hash__default__null_hash)
{
    const script instance{};
    BC_REQUIRE(!instance.is_valid());
    BC_REQUIRE_EQUAL(instance.hash(), null_hash);
}

BC_AUTO_TEST_CASE(script__hash__valid_empty__sha256_of_empty)
{
    const script instance(data_chunk{}, false);
    BC_REQUIRE(instance.is_valid());
    BC_REQUIRE_EQUAL(instance.hash(), sha256_hash(data_chunk{}));
}

BC_AUTO_TEST_CASE(script__hash__checksig__expected)
{
    const script instance{ operations{ operation{ opcode::checksig } } };
    BC_REQUIRE_EQUAL(instance.hash(), sha256_hash(base16_chunk("ac")));
}

BC_AUTO_TEST_CASE(script__inequality__different__true)
{
    const script instance{ operations{ { opcode::pick } } };
    const script other{ operations{ { opcode::roll } } };
    BC_REQUIRE(instance != other);
    BC_REQUIRE(!(instance != instance));
}

// Operation counting traps a previously-invalidated stream.
BC_AUTO_TEST_CASE(script__construct__invalid_source__invalid)
{
    const data_chunk empty{};
    read::bytes::copy source(empty);
    source.invalidate();
    const script instance{ source, true };
    BC_REQUIRE(!instance.is_valid());
}

BC_AUTO_TEST_CASE(script__to_data__ostream__matches_chunk)
{
    const script instance{ operations{ { opcode::pick }, { opcode::roll } } };
    std::ostringstream stream{};
    instance.to_data(stream, true);
    const auto text = stream.str();
    const data_chunk data(text.cbegin(), text.cend());
    BC_REQUIRE_EQUAL(data, instance.to_data(true));
}

BC_AUTO_TEST_SUITE_END()
