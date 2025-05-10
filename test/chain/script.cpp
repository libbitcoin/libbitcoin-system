/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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

// TODO:
//
//=/==/!=/[]
//from_string
//to_string
//size
//front/back/begin/end
//serialized_size
//operations
//to_payments_key
//is_enabled
//is_push_only
//is_relaxed_push
//is_coinbase_pattern
//is_commitment_pattern
//is_witness_program_pattern
//
//is_pay_op_return_pattern
//is_pay_null_data_pattern
//is_pay_multisig_pattern
//is_pay_public_key_pattern
//is_pay_key_hash_pattern
//is_pay_script_hash_pattern
//is_pay_witness_pattern
//is_pay_witness_key_hash_pattern
//is_pay_witness_script_hash_pattern
//
//is_sign_multisig_pattern
//is_sign_public_key_pattern
//is_sign_key_hash_pattern
//is_sign_script_hash_pattern
//
//to_pay_null_data_pattern
//to_pay_public_key_pattern
//to_pay_key_hash_pattern
//to_pay_script_hash_pattern
//to_pay_multisig_pattern
//to_pay_multisig_pattern
//to_pay_witness_pattern
//to_pay_witness_key_hash_pattern
//to_pay_witness_script_hash_pattern
//
//sigops
//is_oversized
//is_unspendable
//is_pay_to_witness
//is_pay_to_script_hash

BOOST_AUTO_TEST_SUITE(script_tests)

namespace json = boost::json;
using namespace system::chain;
using namespace system::machine;

static const std::string script_return = "return";
static const std::string script_return_empty = "return []";
static const std::string script_return_80 = "return [0001020304050607080900010203040506070809000102030405060708090001020304050607080900010203040506070809000102030405060708090001020304050607080900010203040506070809]";
static const std::string script_return_81 = "return [0001020304050607080900010203040506070809000102030405060708090001020304050607080900010203040506070809000102030405060708090001020304050607080900010203040506070809FF]";

static const std::string script_0_of_3_multisig = "0 [03dcfd9e580de35d8c2060d76dbf9e5561fe20febd2e64380e860a4d59f15ac864] [02440e0304bf8d32b2012994393c6a477acf238dd6adb4c3cef5bfa72f30c9861c] [03624505c6cc3967352cce480d8550490dd68519cd019066a4c302fdfb7d1c9934] 3 checkmultisig";
static const std::string script_1_of_3_multisig = "1 [03dcfd9e580de35d8c2060d76dbf9e5561fe20febd2e64380e860a4d59f15ac864] [02440e0304bf8d32b2012994393c6a477acf238dd6adb4c3cef5bfa72f30c9861c] [03624505c6cc3967352cce480d8550490dd68519cd019066a4c302fdfb7d1c9934] 3 checkmultisig";
static const std::string script_2_of_3_multisig = "2 [03dcfd9e580de35d8c2060d76dbf9e5561fe20febd2e64380e860a4d59f15ac864] [02440e0304bf8d32b2012994393c6a477acf238dd6adb4c3cef5bfa72f30c9861c] [03624505c6cc3967352cce480d8550490dd68519cd019066a4c302fdfb7d1c9934] 3 checkmultisig";
static const std::string script_3_of_3_multisig = "3 [03dcfd9e580de35d8c2060d76dbf9e5561fe20febd2e64380e860a4d59f15ac864] [02440e0304bf8d32b2012994393c6a477acf238dd6adb4c3cef5bfa72f30c9861c] [03624505c6cc3967352cce480d8550490dd68519cd019066a4c302fdfb7d1c9934] 3 checkmultisig";
static const std::string script_4_of_3_multisig = "4 [03dcfd9e580de35d8c2060d76dbf9e5561fe20febd2e64380e860a4d59f15ac864] [02440e0304bf8d32b2012994393c6a477acf238dd6adb4c3cef5bfa72f30c9861c] [03624505c6cc3967352cce480d8550490dd68519cd019066a4c302fdfb7d1c9934] 3 checkmultisig";

static const std::string script_16_of_16_multisig =
    "16 "
    "[03dcfd9e580de35d8c2060d76dbf9e5561fe20febd2e64380e860a4d59f15ac864] "
    "[02440e0304bf8d32b2012994393c6a477acf238dd6adb4c3cef5bfa72f30c9861c] "
    "[03624505c6cc3967352cce480d8550490dd68519cd019066a4c302fdfb7d1c9934] "
    "[03dcfd9e580de35d8c2060d76dbf9e5561fe20febd2e64380e860a4d59f15ac864] "
    "[02440e0304bf8d32b2012994393c6a477acf238dd6adb4c3cef5bfa72f30c9861c] "
    "[03624505c6cc3967352cce480d8550490dd68519cd019066a4c302fdfb7d1c9934] "
    "[03dcfd9e580de35d8c2060d76dbf9e5561fe20febd2e64380e860a4d59f15ac864] "
    "[02440e0304bf8d32b2012994393c6a477acf238dd6adb4c3cef5bfa72f30c9861c] "
    "[03624505c6cc3967352cce480d8550490dd68519cd019066a4c302fdfb7d1c9934] "
    "[03dcfd9e580de35d8c2060d76dbf9e5561fe20febd2e64380e860a4d59f15ac864] "
    "[02440e0304bf8d32b2012994393c6a477acf238dd6adb4c3cef5bfa72f30c9861c] "
    "[03624505c6cc3967352cce480d8550490dd68519cd019066a4c302fdfb7d1c9934] "
    "[03dcfd9e580de35d8c2060d76dbf9e5561fe20febd2e64380e860a4d59f15ac864] "
    "[02440e0304bf8d32b2012994393c6a477acf238dd6adb4c3cef5bfa72f30c9861c] "
    "[03624505c6cc3967352cce480d8550490dd68519cd019066a4c302fdfb7d1c9934] "
    "[03dcfd9e580de35d8c2060d76dbf9e5561fe20febd2e64380e860a4d59f15ac864] "
    "16 checkmultisig";

static const std::string script_17_of_17_multisig =
    "[17] "
    "[03dcfd9e580de35d8c2060d76dbf9e5561fe20febd2e64380e860a4d59f15ac864] "
    "[02440e0304bf8d32b2012994393c6a477acf238dd6adb4c3cef5bfa72f30c9861c] "
    "[03624505c6cc3967352cce480d8550490dd68519cd019066a4c302fdfb7d1c9934] "
    "[03dcfd9e580de35d8c2060d76dbf9e5561fe20febd2e64380e860a4d59f15ac864] "
    "[02440e0304bf8d32b2012994393c6a477acf238dd6adb4c3cef5bfa72f30c9861c] "
    "[03624505c6cc3967352cce480d8550490dd68519cd019066a4c302fdfb7d1c9934] "
    "[03dcfd9e580de35d8c2060d76dbf9e5561fe20febd2e64380e860a4d59f15ac864] "
    "[02440e0304bf8d32b2012994393c6a477acf238dd6adb4c3cef5bfa72f30c9861c] "
    "[03624505c6cc3967352cce480d8550490dd68519cd019066a4c302fdfb7d1c9934] "
    "[03dcfd9e580de35d8c2060d76dbf9e5561fe20febd2e64380e860a4d59f15ac864] "
    "[02440e0304bf8d32b2012994393c6a477acf238dd6adb4c3cef5bfa72f30c9861c] "
    "[03624505c6cc3967352cce480d8550490dd68519cd019066a4c302fdfb7d1c9934] "
    "[03dcfd9e580de35d8c2060d76dbf9e5561fe20febd2e64380e860a4d59f15ac864] "
    "[02440e0304bf8d32b2012994393c6a477acf238dd6adb4c3cef5bfa72f30c9861c] "
    "[03624505c6cc3967352cce480d8550490dd68519cd019066a4c302fdfb7d1c9934] "
    "[03dcfd9e580de35d8c2060d76dbf9e5561fe20febd2e64380e860a4d59f15ac864] "
    "[02440e0304bf8d32b2012994393c6a477acf238dd6adb4c3cef5bfa72f30c9861c] "
    "16 checkmultisig";

// Test helpers.
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

    tx.inputs_ptr()->front()->prevout = to_shared(output{ 0u, out });
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

// Serialization tests.
// -----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(script__from_data__testnet_119058_invalid_op_codes__success)
{
    const auto data = base16_chunk("0130323066643366303435313438356531306633383837363437356630643265396130393739343332353534313766653139316438623963623230653430643863333030326431373463336539306366323433393231383761313037623634373337633937333135633932393264653431373731636565613062323563633534353732653302ae");

    const script instance(data, false);
    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(script__from_data__parse__success)
{
    const auto data = base16_chunk("3045022100ff1fc58dbd608e5e05846a8e6b45a46ad49878aef6879ad1a7cf4c5a7f853683022074a6a10f6053ab3cddc5620d169c7374cd42c1416c51b9744db2c8d9febfb84d01");

    const script instance(data, false);
    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(script__from_data__to_data__roundtrips)
{
    const auto normal_output_script = base16_chunk("76a91406ccef231c2db72526df9338894ccf9355e8f12188ac");

    const script instance(normal_output_script, false);
    BOOST_REQUIRE(instance.is_valid());

    const auto roundtrip = instance.to_data(false);
    BOOST_REQUIRE_EQUAL(roundtrip, normal_output_script);
}

BOOST_AUTO_TEST_CASE(script__from_data__to_data_weird__roundtrips)
{
    const auto weird_raw_script = base16_chunk(
        "0c49206c69656b20636174732e483045022100c7387f64e1f4"
        "cf654cae3b28a15f7572106d6c1319ddcdc878e636ccb83845"
        "e30220050ebf440160a4c0db5623e0cb1562f46401a7ff5b87"
        "7aa03415ae134e8c71c901534d4f0176519c6375522103b124"
        "c48bbff7ebe16e7bd2b2f2b561aa53791da678a73d2777cc1c"
        "a4619ab6f72103ad6bb76e00d124f07a22680e39debd4dc4bd"
        "b1aa4b893720dd05af3c50560fdd52af67529c63552103b124"
        "c48bbff7ebe16e7bd2b2f2b561aa53791da678a73d2777cc1c"
        "a4619ab6f721025098a1d5a338592bf1e015468ec5a8fafc1f"
        "c9217feb5cb33597f3613a2165e9210360cfabc01d52eaaeb3"
        "976a5de05ff0cfa76d0af42d3d7e1b4c233ee8a00655ed2103"
        "f571540c81fd9dbf9622ca00cfe95762143f2eab6b65150365"
        "bb34ac533160432102bc2b4be1bca32b9d97e2d6fb255504f4"
        "bc96e01aaca6e29bfa3f8bea65d8865855af672103ad6bb76e"
        "00d124f07a22680e39debd4dc4bdb1aa4b893720dd05af3c50"
        "560fddada820a4d933888318a23c28fb5fc67aca8530524e20"
        "74b1d185dbf5b4db4ddb0642848868685174519c6351670068");

    const script weird(weird_raw_script, false);
    BOOST_REQUIRE(weird.is_valid());

    const auto roundtrip = weird.to_data(false);
    BOOST_REQUIRE_EQUAL(roundtrip, weird_raw_script);
}

BOOST_AUTO_TEST_CASE(script__factory_chunk_test)
{
    const auto raw = base16_chunk("76a914fc7b44566256621affb1541cc9d59f08336d276b88ac");
    const script instance(raw, false);
    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(script__factory_fast_test)
{
    const auto raw = base16_chunk("76a914fc7b44566256621affb1541cc9d59f08336d276b88ac");
    stream::in::fast istream(raw);
    const script instance(istream, false);
    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(script__factory_stream_test)
{
    const auto raw = base16_chunk("76a914fc7b44566256621affb1541cc9d59f08336d276b88ac");
    stream::in::copy istream(raw);
    const script instance(istream, false);
    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(script__factory_reader_test)
{
    const auto raw = base16_chunk("76a914fc7b44566256621affb1541cc9d59f08336d276b88ac");
    read::bytes::copy source(raw);
    const script instance(source, false);
    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(script__from_data__first_byte_invalid_wire_code__success)
{
    const auto raw = to_chunk(base16_array(
        "bb566a54e38193e381aee4b896e7958ce381afe496e4babae381abe38288e381"
        "a3e381a6e7ac91e9a194e38292e5a5aae3828fe3828ce3828be7bea9e58b99e3"
        "8292e8a8ade38191e381a6e381afe38184e381aae38184"));

    const script instance(raw, false);
    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(script__from_data__internal_invalid_wire_code__success)
{
    const auto raw = to_chunk(base16_array(
        "566a54e38193e381aee4b896e7958ce381afe4bb96e4babae381abe38288e381"
        "a3e381a6e7ac91e9a194e38292e5a5aae3828fe3828ce3828be7bea9e58b99e3"
        "8292e8a8ade38191e381a6e381afe38184e381aae38184"));

    const script instance(raw, false);
    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(script__from_data__block_163120_tx_0_script_0__success)
{
    const auto raw = base16_array("2cfabe6d6da181809e4eb72d7b9a8e565331015cb25daf652c81fb7373188db65eee7a45cb0100000000000000");
    // OP_RETURN_250 OP_RETURN_190 OP_2DROP OP_2DROP OP_LESSTHANOREQUAL OP_RIGHT OP_LEFT OP_NUMNOTEQUAL OP_PUSHDATA4 <push past end>
    // blockstream.info/tx/11dbdd3f87bf0eec51a519a9a8ac84ff9dcfb9d08e44f4f6c4101f94afd490a0

    const script instance(raw, true);
    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(script__from_string__empty__success)
{
    const script instance(std::string{});
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.ops().empty());
}

BOOST_AUTO_TEST_CASE(script__from_string__two_of_three_multisig__success)
{
    const script instance(script_2_of_3_multisig);
    const auto& ops = instance.ops();
    BOOST_REQUIRE_EQUAL(ops.size(), 6u);
    BOOST_REQUIRE(ops[0] == opcode::push_positive_2);
    BOOST_REQUIRE_EQUAL(ops[1].to_string(flags::no_rules), "[03dcfd9e580de35d8c2060d76dbf9e5561fe20febd2e64380e860a4d59f15ac864]");
    BOOST_REQUIRE_EQUAL(ops[2].to_string(flags::no_rules), "[02440e0304bf8d32b2012994393c6a477acf238dd6adb4c3cef5bfa72f30c9861c]");
    BOOST_REQUIRE_EQUAL(ops[3].to_string(flags::no_rules), "[03624505c6cc3967352cce480d8550490dd68519cd019066a4c302fdfb7d1c9934]");
    BOOST_REQUIRE(ops[4] == opcode::push_positive_3);
    BOOST_REQUIRE(ops[5] == opcode::checkmultisig);
}

BOOST_AUTO_TEST_CASE(script__empty__default__true)
{
    const script instance;
    BOOST_REQUIRE(instance.ops().empty());
}

BOOST_AUTO_TEST_CASE(script__empty__no_operations__true)
{
    const script instance(operations{});
    BOOST_REQUIRE(instance.ops().empty());
}

BOOST_AUTO_TEST_CASE(script__empty__non_empty__false)
{
    const script instance(script::to_pay_null_data_pattern(data_chunk{ 42u }));
    BOOST_REQUIRE(!instance.ops().empty());
}

// Pattern matching tests.
// -----------------------------------------------------------------------------

// pay_null_data

BOOST_AUTO_TEST_CASE(script__pattern__pay_null_data_return_only__non_standard)
{
    const script instance(script_return);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.output_pattern() == chain::script_pattern::non_standard);
    BOOST_REQUIRE(instance.input_pattern() == chain::script_pattern::non_standard);
    BOOST_REQUIRE(instance.pattern() == chain::script_pattern::non_standard);
}

BOOST_AUTO_TEST_CASE(script__pattern__pay_null_data_empty__null_data)
{
    const script instance(script_return_empty);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.output_pattern() == chain::script_pattern::pay_null_data);
    BOOST_REQUIRE(instance.input_pattern() == chain::script_pattern::non_standard);
    BOOST_REQUIRE(instance.pattern() == chain::script_pattern::pay_null_data);
}

BOOST_AUTO_TEST_CASE(script__pattern__pay_null_data_80_bytes__null_data)
{
    const script instance(script_return_80);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.output_pattern() == chain::script_pattern::pay_null_data);
    BOOST_REQUIRE(instance.input_pattern() == chain::script_pattern::non_standard);
    BOOST_REQUIRE(instance.pattern() == chain::script_pattern::pay_null_data);
}

BOOST_AUTO_TEST_CASE(script__pattern__pay_null_data_81_bytes__non_standard)
{
    const script instance(script_return_81);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.output_pattern() == chain::script_pattern::non_standard);
    BOOST_REQUIRE(instance.input_pattern() == chain::script_pattern::non_standard);
    BOOST_REQUIRE(instance.pattern() == chain::script_pattern::non_standard);
}

// pay_multisig

BOOST_AUTO_TEST_CASE(script__pattern__0_of_3_multisig__non_standard)
{
    const script instance(script_0_of_3_multisig);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.output_pattern() == chain::script_pattern::non_standard);
    BOOST_REQUIRE(instance.input_pattern() == chain::script_pattern::non_standard);
    BOOST_REQUIRE(instance.pattern() == chain::script_pattern::non_standard);
}

BOOST_AUTO_TEST_CASE(script__pattern__1_of_3_multisig__pay_multisig)
{
    const script instance(script_1_of_3_multisig);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.output_pattern() == chain::script_pattern::pay_multisig);
    BOOST_REQUIRE(instance.input_pattern() == chain::script_pattern::non_standard);
    BOOST_REQUIRE(instance.pattern() == chain::script_pattern::pay_multisig);
}

BOOST_AUTO_TEST_CASE(script__pattern__2_of_3_multisig__pay_multisig)
{
    const script instance(script_2_of_3_multisig);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.output_pattern() == chain::script_pattern::pay_multisig);
    BOOST_REQUIRE(instance.input_pattern() == chain::script_pattern::non_standard);
    BOOST_REQUIRE(instance.pattern() == chain::script_pattern::pay_multisig);
}

BOOST_AUTO_TEST_CASE(script__pattern__3_of_3_multisig__pay_multisig)
{
    const script instance(script_3_of_3_multisig);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.output_pattern() == chain::script_pattern::pay_multisig);
    BOOST_REQUIRE(instance.input_pattern() == chain::script_pattern::non_standard);
    BOOST_REQUIRE(instance.pattern() == chain::script_pattern::pay_multisig);
}

BOOST_AUTO_TEST_CASE(script__pattern__4_of_3_multisig__non_standard)
{
    const script instance(script_4_of_3_multisig);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.output_pattern() == chain::script_pattern::non_standard);
    BOOST_REQUIRE(instance.input_pattern() == chain::script_pattern::non_standard);
    BOOST_REQUIRE(instance.pattern() == chain::script_pattern::non_standard);
}

BOOST_AUTO_TEST_CASE(script__pattern__16_of_16_multisig__pay_multisig)
{
    const script instance(script_16_of_16_multisig);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.output_pattern() == chain::script_pattern::pay_multisig);
    BOOST_REQUIRE(instance.input_pattern() == chain::script_pattern::non_standard);
    BOOST_REQUIRE(instance.pattern() == chain::script_pattern::pay_multisig);
}

BOOST_AUTO_TEST_CASE(script__pattern__17_of_17_multisig__non_standard)
{
    const script instance(script_17_of_17_multisig);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.output_pattern() == chain::script_pattern::non_standard);
    BOOST_REQUIRE(instance.input_pattern() == chain::script_pattern::non_standard);
    BOOST_REQUIRE(instance.pattern() == chain::script_pattern::non_standard);
}

// Data-driven tests.
// -----------------------------------------------------------------------------

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

// Ad-hoc tx.connect tests.
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(script__verify__testnet_block_23428_multisig_tx__success)
{
    //// DEBUG [blockchain] Verify failed[23428] : stack false (consensus patch bug).
    //// libconsensus : false
    //// forks : 1073742031
    //// outpoint : 18327d91dcf0ded31212b0da3efd472b98766d9f21f887a65d04ddfe35a01eca : 1
    //// script : a9144aba54e2541475f91659ccdbb13ce0b490778c7f87
    //// value : 100000000
    //// inpoint : d5f973b43e0b7df603264a98d04ea21dfc47b07a679e539ea695bbed23304435 : 1
    //// transaction : 0100000002c0cd5346700d18a937575424eb84888bdc277bdbfade39b3bb9a4ce31fd4455101000000fdf60100483045022100f681bb660ef85bb191e337450f2ba3493c37b90a8622864d932cec5b40a74428022007cab269d846b7e63899b8e7082bdea94375d4b1c197b7c50365823c9ffd935e01483045022100b4d3be95b088c8ef176b25c9cee0b16ac7f91c10ccf645ab7421ad3de1d8aba802205c6b3bd9df0b19271abefc47997ce7bd113a6f069674003c821c01440d49a48b0148304502207fad219634211fb614cef1654bdb956a9ed751a352e47c2b64d4ac8459e05ec2022100bd4ae53e76f266938f2ec09d1b2d515eaf5e21b990e5c7df0779ca61f24a10de014830450221009f2dd7fa5eafdf9f660e764750aabffd86628bb19501d49007551151f6409266022019fad776d46c6896a849bf7fcacefa73da6d5db22680c7bacdd055d8d8547858014ccf542102d7dafdc7f5d63bc1e5210a93a93c57e96acfb123df06ca02318be689791fa634210204affb8e9fd6228d370aed6b8fff2fc33bbe9603e2933ae3b92b11a67d7e7d3d210269fa9a07b38c01440ecabc74481fdd2ede1591e293a1108ba1ac511d85e40ca62102f24bda0faab218a98c5975cb7eb035b37020ec3758e454d95f382e1f274da2112102795716e51a5539961872b559f2e938a29862565c1d0c70ed6a748adb8541c82b2103828209539e87cc72694e0d397a00ae1c1b3aa3aa7931df3bb72c71172758ed7756aeffffffffca1ea035fedd045da687f8219f6d76982b47fd3edab01212d3def0dc917d321801000000fdf40100473044022050cf9d0bf024af1780af7ce91a8cac62fd54a3df96cc1eb27889a58aaf82f09e02205f85c010faa5978963f569cfa6bd7202363841ad82bab0a6044c1092140ba49001483045022100f3f5076e1f233acf3fd2bb1188da82f3259224ee29a50af287b707c71503543f02202e82db849e59f8eb836ec6c55dab6a3d61b6511e9d25e550901d5534276432320148304502210094ff0cd6c74dd756a07334c2b76373dd4fb8f5ef7c1da7e09a168d54cf79a7770220114337de0ac0edd7871c079b796ad422d2d5e50d350d3b9f7371b0f1bd66fe7a01473044022022bc92872b6c680da40aa6388e28ef396c7ffa410317ae574c1f31836c85b28602203b43a7d2cdcc2ba1afaf53c1a0c0b474933581b0e359aed788ad7ad819260dfb014ccf542102d7dafdc7f5d63bc1e5210a93a93c57e96acfb123df06ca02318be689791fa634210204affb8e9fd6228d370aed6b8fff2fc33bbe9603e2933ae3b92b11a67d7e7d3d210269fa9a07b38c01440ecabc74481fdd2ede1591e293a1108ba1ac511d85e40ca62102f24bda0faab218a98c5975cb7eb035b37020ec3758e454d95f382e1f274da2112102795716e51a5539961872b559f2e938a29862565c1d0c70ed6a748adb8541c82b2103828209539e87cc72694e0d397a00ae1c1b3aa3aa7931df3bb72c71172758ed7756aeffffffff02605af405000000001976a914bb6754a948265de730c60fbd745aeb5868ea921e88ac00e1f5050000000017a9144aba54e2541475f91659ccdbb13ce0b490778c7f8700000000

    // Isolate rules from integer values (1073742031u/1100 1110).
    // Flag bit 30 was removed as a "checkpointed" indicator (why was it set?).
    // Flag bit 0 was reversed from easy to difficult, for mainnet consistency.
    constexpr auto forks =
        ////flags::difficult |
        flags::retarget |
        flags::bip16_rule |
        flags::bip30_rule |
        ////flags::bip34_rule |
        ////flags::bip66_rule |
        flags::bip65_rule | // bip65 should be superseded by bip90.
        flags::bip90_rule;

    constexpr auto index = 1u;
    const auto decoded_tx = base16_chunk("0100000002c0cd5346700d18a937575424eb84888bdc277bdbfade39b3bb9a4ce31fd4455101000000fdf60100483045022100f681bb660ef85bb191e337450f2ba3493c37b90a8622864d932cec5b40a74428022007cab269d846b7e63899b8e7082bdea94375d4b1c197b7c50365823c9ffd935e01483045022100b4d3be95b088c8ef176b25c9cee0b16ac7f91c10ccf645ab7421ad3de1d8aba802205c6b3bd9df0b19271abefc47997ce7bd113a6f069674003c821c01440d49a48b0148304502207fad219634211fb614cef1654bdb956a9ed751a352e47c2b64d4ac8459e05ec2022100bd4ae53e76f266938f2ec09d1b2d515eaf5e21b990e5c7df0779ca61f24a10de014830450221009f2dd7fa5eafdf9f660e764750aabffd86628bb19501d49007551151f6409266022019fad776d46c6896a849bf7fcacefa73da6d5db22680c7bacdd055d8d8547858014ccf542102d7dafdc7f5d63bc1e5210a93a93c57e96acfb123df06ca02318be689791fa634210204affb8e9fd6228d370aed6b8fff2fc33bbe9603e2933ae3b92b11a67d7e7d3d210269fa9a07b38c01440ecabc74481fdd2ede1591e293a1108ba1ac511d85e40ca62102f24bda0faab218a98c5975cb7eb035b37020ec3758e454d95f382e1f274da2112102795716e51a5539961872b559f2e938a29862565c1d0c70ed6a748adb8541c82b2103828209539e87cc72694e0d397a00ae1c1b3aa3aa7931df3bb72c71172758ed7756aeffffffffca1ea035fedd045da687f8219f6d76982b47fd3edab01212d3def0dc917d321801000000fdf40100473044022050cf9d0bf024af1780af7ce91a8cac62fd54a3df96cc1eb27889a58aaf82f09e02205f85c010faa5978963f569cfa6bd7202363841ad82bab0a6044c1092140ba49001483045022100f3f5076e1f233acf3fd2bb1188da82f3259224ee29a50af287b707c71503543f02202e82db849e59f8eb836ec6c55dab6a3d61b6511e9d25e550901d5534276432320148304502210094ff0cd6c74dd756a07334c2b76373dd4fb8f5ef7c1da7e09a168d54cf79a7770220114337de0ac0edd7871c079b796ad422d2d5e50d350d3b9f7371b0f1bd66fe7a01473044022022bc92872b6c680da40aa6388e28ef396c7ffa410317ae574c1f31836c85b28602203b43a7d2cdcc2ba1afaf53c1a0c0b474933581b0e359aed788ad7ad819260dfb014ccf542102d7dafdc7f5d63bc1e5210a93a93c57e96acfb123df06ca02318be689791fa634210204affb8e9fd6228d370aed6b8fff2fc33bbe9603e2933ae3b92b11a67d7e7d3d210269fa9a07b38c01440ecabc74481fdd2ede1591e293a1108ba1ac511d85e40ca62102f24bda0faab218a98c5975cb7eb035b37020ec3758e454d95f382e1f274da2112102795716e51a5539961872b559f2e938a29862565c1d0c70ed6a748adb8541c82b2103828209539e87cc72694e0d397a00ae1c1b3aa3aa7931df3bb72c71172758ed7756aeffffffff02605af405000000001976a914bb6754a948265de730c60fbd745aeb5868ea921e88ac00e1f5050000000017a9144aba54e2541475f91659ccdbb13ce0b490778c7f8700000000");
    const transaction_accessor tx(decoded_tx, true);
    BOOST_REQUIRE(tx.is_valid());
    BOOST_REQUIRE_GT(tx.inputs(), index);
    BOOST_REQUIRE_GT(tx.inputs_ptr()->size(), index);

    constexpr auto value = 100000000u;
    (*tx.inputs_ptr())[index]->prevout = to_shared(output{ value, { base16_chunk("a9144aba54e2541475f91659ccdbb13ce0b490778c7f87"), false } });
    BOOST_REQUIRE_EQUAL(tx.connect({ forks }, index), error::script_success);
}

BOOST_AUTO_TEST_CASE(script__verify__block_290329_tx__success)
{
    //// DEBUG [blockchain] Verify failed [290329] : stack false (find and delete).
    //// libconsensus : false
    //// forks        : 1073742030
    //// outpoint     : ab9805c6d57d7070d9a42c5176e47bb705023e6b67249fb6760880548298e742:0
    //// script       : a914d8dacdadb7462ae15cd906f1878706d0da8660e687
    //// inpoint      : 5df1375ffe61ac35ca178ebb0cab9ea26dedbd0e96005dfcee7e379fa513232f:1
    //// transaction  : 0100000002f9cbafc519425637ba4227f8d0a0b7160b4e65168193d5af39747891de98b5b5000000006b4830450221008dd619c563e527c47d9bd53534a770b102e40faa87f61433580e04e271ef2f960220029886434e18122b53d5decd25f1f4acb2480659fea20aabd856987ba3c3907e0121022b78b756e2258af13779c1a1f37ea6800259716ca4b7f0b87610e0bf3ab52a01ffffffff42e7988254800876b69f24676b3e0205b77be476512ca4d970707dd5c60598ab00000000fd260100483045022015bd0139bcccf990a6af6ec5c1c52ed8222e03a0d51c334df139968525d2fcd20221009f9efe325476eb64c3958e4713e9eefe49bf1d820ed58d2112721b134e2a1a53034930460221008431bdfa72bc67f9d41fe72e94c88fb8f359ffa30b33c72c121c5a877d922e1002210089ef5fc22dd8bfc6bf9ffdb01a9862d27687d424d1fefbab9e9c7176844a187a014c9052483045022015bd0139bcccf990a6af6ec5c1c52ed8222e03a0d51c334df139968525d2fcd20221009f9efe325476eb64c3958e4713e9eefe49bf1d820ed58d2112721b134e2a1a5303210378d430274f8c5ec1321338151e9f27f4c676a008bdf8638d07c0b6be9ab35c71210378d430274f8c5ec1321338151e9f27f4c676a008bdf8638d07c0b6be9ab35c7153aeffffffff01a08601000000000017a914d8dacdadb7462ae15cd906f1878706d0da8660e68700000000

    // Isolate rules from integer values (1073742031u/1100 1110).
    // Flag bit 30 was removed as a "checkpointed" indicator (why was it set?).
    // Flag bit 0 was reversed from easy to difficult, for mainnet consistency.
    constexpr auto forks =
        ////flags::difficult |
        flags::retarget |
        flags::bip16_rule |
        flags::bip30_rule |
        ////flags::bip34_rule |
        ////flags::bip66_rule |
        flags::bip65_rule | // bip65 should be superseded by bip90.
        flags::bip90_rule;

    constexpr auto index = 1u;
    const auto decoded_tx = base16_chunk("0100000002f9cbafc519425637ba4227f8d0a0b7160b4e65168193d5af39747891de98b5b5000000006b4830450221008dd619c563e527c47d9bd53534a770b102e40faa87f61433580e04e271ef2f960220029886434e18122b53d5decd25f1f4acb2480659fea20aabd856987ba3c3907e0121022b78b756e2258af13779c1a1f37ea6800259716ca4b7f0b87610e0bf3ab52a01ffffffff42e7988254800876b69f24676b3e0205b77be476512ca4d970707dd5c60598ab00000000fd260100483045022015bd0139bcccf990a6af6ec5c1c52ed8222e03a0d51c334df139968525d2fcd20221009f9efe325476eb64c3958e4713e9eefe49bf1d820ed58d2112721b134e2a1a53034930460221008431bdfa72bc67f9d41fe72e94c88fb8f359ffa30b33c72c121c5a877d922e1002210089ef5fc22dd8bfc6bf9ffdb01a9862d27687d424d1fefbab9e9c7176844a187a014c9052483045022015bd0139bcccf990a6af6ec5c1c52ed8222e03a0d51c334df139968525d2fcd20221009f9efe325476eb64c3958e4713e9eefe49bf1d820ed58d2112721b134e2a1a5303210378d430274f8c5ec1321338151e9f27f4c676a008bdf8638d07c0b6be9ab35c71210378d430274f8c5ec1321338151e9f27f4c676a008bdf8638d07c0b6be9ab35c7153aeffffffff01a08601000000000017a914d8dacdadb7462ae15cd906f1878706d0da8660e68700000000");
    const transaction_accessor tx(decoded_tx, true);
    BOOST_REQUIRE(tx.is_valid());
    BOOST_REQUIRE_GT(tx.inputs_ptr()->size(), index);

    constexpr auto value = 0u;
    (*tx.inputs_ptr())[index]->prevout = to_shared(output{ value, { base16_chunk("a914d8dacdadb7462ae15cd906f1878706d0da8660e687"), false } });
    BOOST_REQUIRE_EQUAL(tx.connect({ forks }, index), error::script_success);
}

BOOST_AUTO_TEST_CASE(script__verify__block_438513_tx__success)
{
    //// DEBUG [blockchain] Input validation failed (stack false).
    //// libconsensus : false
    //// forks        : 62
    //// outpoint     : 8e51d775e0896e03149d585c0655b3001da0c55068b0885139ac6ec34cf76ba0:0
    //// script       : a914faa558780a5767f9e3be14992a578fc1cbcf483087
    //// inpoint      : 6b7f50afb8448c39f4714a73d2b181d3e3233e84670bdfda8f141db668226c54:0
    //// transaction  : 0100000001a06bf74cc36eac395188b06850c5a01d00b355065c589d14036e89e075d7518e000000009d483045022100ba555ac17a084e2a1b621c2171fa563bc4fb75cd5c0968153f44ba7203cb876f022036626f4579de16e3ad160df01f649ffb8dbf47b504ee56dc3ad7260af24ca0db0101004c50632102768e47607c52e581595711e27faffa7cb646b4f481fe269bd49691b2fbc12106ad6704355e2658b1756821028a5af8284a12848d69a25a0ac5cea20be905848eb645fd03d3b065df88a9117cacfeffffff0158920100000000001976a9149d86f66406d316d44d58cbf90d71179dd8162dd388ac355e2658

    // Isolate rules from integer values (62u/0011 0011).
    // Flag bit 0 was reversed from easy to difficult, for mainnet consistency.
    constexpr auto forks =
        flags::difficult |
        flags::retarget |
        ////flags::bip16_rule | // This looks wrong.
        ////flags::bip30_rule | // This is moot but looks wrong.
        flags::bip34_rule |
        flags::bip66_rule;

    constexpr auto index = 0u;
    const auto decoded_tx = base16_chunk("0100000001a06bf74cc36eac395188b06850c5a01d00b355065c589d14036e89e075d7518e000000009d483045022100ba555ac17a084e2a1b621c2171fa563bc4fb75cd5c0968153f44ba7203cb876f022036626f4579de16e3ad160df01f649ffb8dbf47b504ee56dc3ad7260af24ca0db0101004c50632102768e47607c52e581595711e27faffa7cb646b4f481fe269bd49691b2fbc12106ad6704355e2658b1756821028a5af8284a12848d69a25a0ac5cea20be905848eb645fd03d3b065df88a9117cacfeffffff0158920100000000001976a9149d86f66406d316d44d58cbf90d71179dd8162dd388ac355e2658");
    const transaction_accessor tx(decoded_tx, true);
    BOOST_REQUIRE(tx.is_valid());
    BOOST_REQUIRE_GT(tx.inputs_ptr()->size(), index);

    constexpr auto value = 0u;
    (*tx.inputs_ptr())[index]->prevout = to_shared(output{ value, { base16_chunk("a914faa558780a5767f9e3be14992a578fc1cbcf483087"), false } });
    BOOST_REQUIRE_EQUAL(tx.connect({ forks }, index), error::script_success);
}

BOOST_AUTO_TEST_CASE(script__verify__block_481824_tx__success)
{
    //// DEBUG [blockchain] Verify failed [481824] (stack false).
    //// libconsensus : false
    //// forks        : 1073758206
    //// outpoint     : 42f7d0545ef45bd3b9cfee6b170cf6314a3bd8b3f09b610eeb436d92993ad440:1
    //// script       : a9142928f43af18d2d60e8a843540d8086b30534133987
    //// value        : 100200000
    //// inpoint      : c586389e5e4b3acb9d6c8be1c19ae8ab2795397633176f5a6442a261bbdefc3a:0
    //// transaction  : 0200000000010140d43a99926d43eb0e619bf0b3d83b4a31f60c176beecfb9d35bf45e54d0f7420100000017160014a4b4ca48de0b3fffc15404a1acdc8dbaae226955ffffffff0100e1f5050000000017a9144a1154d50b03292b3024370901711946cb7cccc387024830450221008604ef8f6d8afa892dee0f31259b6ce02dd70c545cfcfed8148179971876c54a022076d771d6e91bed212783c9b06e0de600fab2d518fad6f15a2b191d7fbd262a3e0121039d25ab79f41f75ceaf882411fd41fa670a4c672c23ffaf0e361a969cde0692e800000000

    // Isolate rules from integer values (1073758206u/0011 1111 1111 1111).
    // Flag bit 30 was removed as a "checkpointed" indicator (why was it set?).
    // Flag bit 0 was reversed from easy to difficult, for mainnet consistency.
    constexpr auto forks =
        flags::difficult |
        flags::retarget |
        flags::bip16_rule |
        flags::bip30_rule |
        flags::bip34_rule |
        flags::bip66_rule |
        flags::bip65_rule |
        flags::bip90_rule |
        flags::bip68_rule |
        flags::bip112_rule |
        flags::bip113_rule |
        flags::bip141_rule |
        flags::bip143_rule |
        flags::bip147_rule;

    constexpr auto index = 0u;
    const auto decoded_tx = base16_chunk("0200000000010140d43a99926d43eb0e619bf0b3d83b4a31f60c176beecfb9d35bf45e54d0f7420100000017160014a4b4ca48de0b3fffc15404a1acdc8dbaae226955ffffffff0100e1f5050000000017a9144a1154d50b03292b3024370901711946cb7cccc387024830450221008604ef8f6d8afa892dee0f31259b6ce02dd70c545cfcfed8148179971876c54a022076d771d6e91bed212783c9b06e0de600fab2d518fad6f15a2b191d7fbd262a3e0121039d25ab79f41f75ceaf882411fd41fa670a4c672c23ffaf0e361a969cde0692e800000000");
    const transaction_accessor tx(decoded_tx, true);
    BOOST_REQUIRE(tx.is_valid());
    BOOST_REQUIRE_GT(tx.inputs_ptr()->size(), index);

    constexpr auto value = 100200000u;
    (*tx.inputs_ptr())[index]->prevout = to_shared(output{ value, { base16_chunk("a9142928f43af18d2d60e8a843540d8086b30534133987"), false } });
    BOOST_REQUIRE_EQUAL(tx.connect({ forks }, index), error::script_success);
}

BOOST_AUTO_TEST_CASE(script__verify__testnet_block_892321_tx_missing_witness__invalid_witness)
{
    //// DEBUG [blockchain] Verify failed [892321] : invalid witness.
    //// libconsensus : false
    //// forks        : 1073758207
    //// outpoint     : fca5e8f5d6ce5201f45230268dfe6cbf8472932e63a001216e9002993acd08f5:0
    //// script       : 0020925fe0a6cde95bdc7a21b08925c246cae17005f8a013efffdb5e5cb7b7f8d0c2
    //// value        : 194445
    //// inpoint      : feecd68efcbbf1b81a99ca7b58410aa1477a46b0a03e4cfcea11b9dc538eb713:0
    //// transaction  : 0200000001f508cd3a9902906e2101a0632e937284bf6cfe8d263052f40152ced6f5e8a5fc0000000000ffffffff0105e4020000000000160014b6aa463696df9140b1191fa2cc1891cf9b5da6d900000000

    // Isolate rules from integer values (1073758207u/0011 1111 1111 1110).
    // Flag bit 30 was removed as a "checkpointed" indicator (why was it set?).
    // Flag bit 0 was reversed from easy to difficult, for mainnet consistency.
    constexpr auto forks =
        ////flags::difficult |
        flags::retarget |
        flags::bip16_rule |
        flags::bip30_rule |
        flags::bip34_rule |
        flags::bip66_rule |
        flags::bip65_rule |
        flags::bip90_rule |
        flags::bip68_rule |
        flags::bip112_rule |
        flags::bip113_rule |
        flags::bip141_rule |
        flags::bip143_rule |
        flags::bip147_rule;

    constexpr auto index = 0u;
    const auto decoded_tx = base16_chunk("0200000001f508cd3a9902906e2101a0632e937284bf6cfe8d263052f40152ced6f5e8a5fc0000000000ffffffff0105e4020000000000160014b6aa463696df9140b1191fa2cc1891cf9b5da6d900000000");
    const transaction_accessor tx(decoded_tx, true);
    BOOST_REQUIRE(tx.is_valid());
    BOOST_REQUIRE_GT(tx.inputs_ptr()->size(), index);
    BOOST_REQUIRE((*tx.inputs_ptr())[index]->witness().stack().empty());

    constexpr auto value = 194445u;
    (*tx.inputs_ptr())[index]->prevout = to_shared(output{ value, { base16_chunk("0020925fe0a6cde95bdc7a21b08925c246cae17005f8a013efffdb5e5cb7b7f8d0c2"), false } });

    // This is correctly invalid.
    BOOST_REQUIRE_EQUAL(tx.connect({ forks }, index), error::invalid_witness);
}

// bip143 test cases.
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(script__verify__bip143_native_p2wpkh_tx__success)
{
    const auto decoded_tx = base16_chunk("01000000000102fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f00000000494830450221008b9d1dc26ba6a9cb62127b02742fa9d754cd3bebf337f7a55d114c8e5cdd30be022040529b194ba3f9281a99f2b1c0a19c0489bc22ede944ccf4ecbab4cc618ef3ed01eeffffffef51e1b804cc89d182d279655c3aa89e815b1b309fe287d9b2b55d57b90ec68a0100000000ffffffff02202cb206000000001976a9148280b37df378db99f66f85c95a783a76ac7a6d5988ac9093510d000000001976a9143bde42dbee7e4dbe6a21b2d50ce2f0167faa815988ac000247304402203609e17b84f6a7d30c80bfa610b5b4542f32a8a0d5447a12fb1366d7f01cc44a0220573a954c4518331561406f90300e8f3358f51928d43c212a8caed02de67eebee0121025476c2e83188368da1ff3e292e7acafcdb3566bb0ad253f62fc70f07aeee635711000000");
    const transaction_accessor tx(decoded_tx, true);
    BOOST_REQUIRE(tx.is_valid());
    BOOST_REQUIRE_EQUAL(tx.inputs(), 2u);
    BOOST_REQUIRE_EQUAL(tx.inputs_ptr()->size(), 2u);
    BOOST_REQUIRE((*tx.inputs_ptr())[0]->witness().stack().empty());
    BOOST_REQUIRE(!(*tx.inputs_ptr())[1]->witness().stack().empty());

    constexpr auto value0 = 625000000u;
    (*tx.inputs_ptr())[0]->prevout = to_shared(output{ value0, { base16_chunk("2103c9f4836b9a4f77fc0d81f7bcb01b7f1b35916864b9476c241ce9fc198bd25432ac"), false } });

    constexpr auto value1 = 600000000u;
    (*tx.inputs_ptr())[1]->prevout = to_shared(output{ value1, { base16_chunk("00141d0f172a0ecb48aee1be1f2687d2963ae33f71a1"), false } });

    // ordinary P2PK (no rules required).
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::no_rules }, 0), error::script_success);

    // P2WPKH witness program.
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip141_rule | flags::bip143_rule }, 1), error::script_success);

    // Other scenarios:

    // extra rules (okay).
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip141_rule | flags::bip143_rule }, 0), error::script_success);

    // missing bip143 (invalid sighash).
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip141_rule }, 1), error::stack_false);

    // missing bip141 (witness not allowed).
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip143_rule }, 1), error::unexpected_witness);

    // missing bip141 (witness not allowed).
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::no_rules }, 1), error::unexpected_witness);
}

BOOST_AUTO_TEST_CASE(script__verify__bip143_p2sh_p2wpkh_tx__success)
{
    const auto decoded_tx = base16_chunk("01000000000101db6b1b20aa0fd7b23880be2ecbd4a98130974cf4748fb66092ac4d3ceb1a5477010000001716001479091972186c449eb1ded22b78e40d009bdf0089feffffff02b8b4eb0b000000001976a914a457b684d7f0d539a46a45bbc043f35b59d0d96388ac0008af2f000000001976a914fd270b1ee6abcaea97fea7ad0402e8bd8ad6d77c88ac02473044022047ac8e878352d3ebbde1c94ce3a10d057c24175747116f8288e5d794d12d482f0220217f36a485cae903c713331d877c1f64677e3622ad4010726870540656fe9dcb012103ad1d8e89212f0b92c74d23bb710c00662ad1470198ac48c43f7d6f93a2a2687392040000");
    const transaction_accessor tx(decoded_tx, true);
    BOOST_REQUIRE(tx.is_valid());
    BOOST_REQUIRE_EQUAL(tx.inputs_ptr()->size(), 1u);
    BOOST_REQUIRE(!(*tx.inputs_ptr())[0]->witness().stack().empty());

    constexpr auto value = 1000000000u;
    (*tx.inputs_ptr())[0]->prevout = to_shared(output{ value, { base16_chunk("a9144733f37cf4db86fbc2efed2500b4f4e49f31202387"), false } });

    // P2SH-P2WPKH witness program.
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip16_rule | flags::bip141_rule | flags::bip143_rule }, 0), error::script_success);

    // Other scenarios:

    // missing bip16 (required for p2sh, embedded witness not consumed).
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip141_rule | flags::bip143_rule }, 0), error::unexpected_witness);

    // missing bip141 (witness not allowed).
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip16_rule | flags::bip143_rule }, 0), error::unexpected_witness);

    // missing bip143 (invalid sighash).
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip16_rule | flags::bip141_rule }, 0), error::stack_false);
}

BOOST_AUTO_TEST_CASE(script__verify__bip143_native_p2wsh_1_tx__success)
{
    const auto decoded_tx = base16_chunk("01000000000102fe3dc9208094f3ffd12645477b3dc56f60ec4fa8e6f5d67c565d1c6b9216b36e000000004847304402200af4e47c9b9629dbecc21f73af989bdaa911f7e6f6c2e9394588a3aa68f81e9902204f3fcf6ade7e5abb1295b6774c8e0abd94ae62217367096bc02ee5e435b67da201ffffffff0815cf020f013ed6cf91d29f4202e8a58726b1ac6c79da47c23d1bee0a6925f80000000000ffffffff0100f2052a010000001976a914a30741f8145e5acadf23f751864167f32e0963f788ac000347304402200de66acf4527789bfda55fc5459e214fa6083f936b430a762c629656216805ac0220396f550692cd347171cbc1ef1f51e15282e837bb2b30860dc77c8f78bc8501e503473044022027dc95ad6b740fe5129e7e62a75dd00f291a2aeb1200b84b09d9e3789406b6c002201a9ecd315dd6a0e632ab20bbb98948bc0c6fb204f2c286963bb48517a7058e27034721026dccc749adc2a9d0d89497ac511f760f45c47dc5ed9cf352a58ac706453880aeadab210255a9626aebf5e29c0e6538428ba0d1dcf6ca98ffdf086aa8ced5e0d0215ea465ac00000000");
    const transaction_accessor tx(decoded_tx, true);
    BOOST_REQUIRE(tx.is_valid());
    BOOST_REQUIRE_EQUAL(tx.inputs_ptr()->size(), 2u);
    BOOST_REQUIRE((*tx.inputs_ptr())[0]->witness().stack().empty());
    BOOST_REQUIRE(!(*tx.inputs_ptr())[1]->witness().stack().empty());

    constexpr auto value0 = 156250000u;
    (*tx.inputs_ptr())[0]->prevout = to_shared(output{ value0, { base16_chunk("21036d5c20fa14fb2f635474c1dc4ef5909d4568e5569b79fc94d3448486e14685f8ac"), false } });

    constexpr auto value1 = 4900000000u;
    (*tx.inputs_ptr())[1]->prevout = to_shared(output{ value1, { base16_chunk("00205d1b56b63d714eebe542309525f484b7e9d6f686b3781b6f61ef925d66d6f6a0"), false } });

    // ordinary P2PK (no rules required).
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::no_rules }, 0), error::script_success);

    // native P2WSH witness program.
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip141_rule | flags::bip143_rule }, 1), error::script_success);

    // Other scenarios:

    // extra rules (okay).
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip16_rule | flags::bip141_rule | flags::bip143_rule }, 0), error::script_success);

    // missing bip143 (code-separator treatment).
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip141_rule }, 1), error::op_check_sig_verify8);

    // missing bip141 (witness not allowed).
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip143_rule }, 1), error::unexpected_witness);
}

BOOST_AUTO_TEST_CASE(script__verify__bip143_native_p2wsh_2_tx__success)
{
    const auto decoded_tx = base16_chunk("01000000000102e9b542c5176808107ff1df906f46bb1f2583b16112b95ee5380665ba7fcfc0010000000000ffffffff80e68831516392fcd100d186b3c2c7b95c80b53c77e77c35ba03a66b429a2a1b0000000000ffffffff0280969800000000001976a914de4b231626ef508c9a74a8517e6783c0546d6b2888ac80969800000000001976a9146648a8cd4531e1ec47f35916de8e259237294d1e88ac02483045022100f6a10b8604e6dc910194b79ccfc93e1bc0ec7c03453caaa8987f7d6c3413566002206216229ede9b4d6ec2d325be245c5b508ff0339bf1794078e20bfe0babc7ffe683270063ab68210392972e2eb617b2388771abe27235fd5ac44af8e61693261550447a4c3e39da98ac024730440220032521802a76ad7bf74d0e2c218b72cf0cbc867066e2e53db905ba37f130397e02207709e2188ed7f08f4c952d9d13986da504502b8c3be59617e043552f506c46ff83275163ab68210392972e2eb617b2388771abe27235fd5ac44af8e61693261550447a4c3e39da98ac00000000");
    const transaction_accessor tx(decoded_tx, true);
    BOOST_REQUIRE(tx.is_valid());
    BOOST_REQUIRE_EQUAL(tx.inputs_ptr()->size(), 2u);
    BOOST_REQUIRE(!(*tx.inputs_ptr())[0]->witness().stack().empty());
    BOOST_REQUIRE(!(*tx.inputs_ptr())[1]->witness().stack().empty());

    constexpr auto value0 = 16777215u;
    (*tx.inputs_ptr())[0]->prevout = to_shared(output{ value0, { base16_chunk("0020ba468eea561b26301e4cf69fa34bde4ad60c81e70f059f045ca9a79931004a4d"), false } });

    constexpr auto value1 = 16777215u;
    (*tx.inputs_ptr())[1]->prevout = to_shared(output{ value1, { base16_chunk("0020d9bbfbe56af7c4b7f960a70d7ea107156913d9e5a26b0a71429df5e097ca6537"), false } });

    // native P2WSH witness program.
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip141_rule | flags::bip143_rule }, 0), error::script_success);

    // native P2WSH witness program.
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip141_rule | flags::bip143_rule }, 1), error::script_success);

    // all inputs
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip141_rule | flags::bip143_rule }), error::transaction_success);

    // Other scenarios:

    // extra rules (okay).
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip16_rule | flags::bip141_rule | flags::bip143_rule }, 0), error::script_success);

    // extra rules (okay).
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip16_rule | flags::bip141_rule | flags::bip143_rule }, 1), error::script_success);

    // missing bip143 (code-separator treatment).
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip141_rule }, 0), error::stack_false);

    // missing bip143 (code-separator treatment).
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip141_rule }, 1), error::stack_false);

    // missing bip141 (witness not allowed).
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip143_rule }, 0), error::unexpected_witness);

    // missing bip141 (witness not allowed).
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip143_rule }, 1), error::unexpected_witness);
}

// This example shows how SINGLE|ANYONECANPAY does not commit to the input index (swap input indexes).
BOOST_AUTO_TEST_CASE(script__verify__bip143_native_p2wsh_3_tx__success)
{
    const auto decoded_tx = base16_chunk("0100000000010280e68831516392fcd100d186b3c2c7b95c80b53c77e77c35ba03a66b429a2a1b0000000000ffffffffe9b542c5176808107ff1df906f46bb1f2583b16112b95ee5380665ba7fcfc0010000000000ffffffff0280969800000000001976a9146648a8cd4531e1ec47f35916de8e259237294d1e88ac80969800000000001976a914de4b231626ef508c9a74a8517e6783c0546d6b2888ac024730440220032521802a76ad7bf74d0e2c218b72cf0cbc867066e2e53db905ba37f130397e02207709e2188ed7f08f4c952d9d13986da504502b8c3be59617e043552f506c46ff83275163ab68210392972e2eb617b2388771abe27235fd5ac44af8e61693261550447a4c3e39da98ac02483045022100f6a10b8604e6dc910194b79ccfc93e1bc0ec7c03453caaa8987f7d6c3413566002206216229ede9b4d6ec2d325be245c5b508ff0339bf1794078e20bfe0babc7ffe683270063ab68210392972e2eb617b2388771abe27235fd5ac44af8e61693261550447a4c3e39da98ac00000000");
    const transaction_accessor tx(decoded_tx, true);
    BOOST_REQUIRE_EQUAL(tx.inputs_ptr()->size(), 2u);
    BOOST_REQUIRE(!(*tx.inputs_ptr())[0]->witness().stack().empty());
    BOOST_REQUIRE(!(*tx.inputs_ptr())[1]->witness().stack().empty());

    constexpr auto value0 = 16777215u;
    (*tx.inputs_ptr())[0]->prevout = to_shared(output{ value0, { base16_chunk("0020d9bbfbe56af7c4b7f960a70d7ea107156913d9e5a26b0a71429df5e097ca6537"), false } });

    constexpr auto value1 = 16777215u;
    (*tx.inputs_ptr())[1]->prevout = to_shared(output{ value1, { base16_chunk("0020ba468eea561b26301e4cf69fa34bde4ad60c81e70f059f045ca9a79931004a4d"), false } });

    // native P2WSH witness program.
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip141_rule | flags::bip143_rule }, 0), error::script_success);

    // native P2WSH witness program.
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip141_rule | flags::bip143_rule }, 1), error::script_success);

    // all inputs
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip141_rule | flags::bip143_rule }), error::transaction_success);
}

BOOST_AUTO_TEST_CASE(script__verify__bip143_p2sh_p2wsh_tx__success)
{
    const auto decoded_tx = base16_chunk("0100000000010136641869ca081e70f394c6948e8af409e18b619df2ed74aa106c1ca29787b96e0100000023220020a16b5755f7f6f96dbd65f5f0d6ab9418b89af4b1f14a1bb8a09062c35f0dcb54ffffffff0200e9a435000000001976a914389ffce9cd9ae88dcc0631e88a821ffdbe9bfe2688acc0832f05000000001976a9147480a33f950689af511e6e84c138dbbd3c3ee41588ac080047304402206ac44d672dac41f9b00e28f4df20c52eeb087207e8d758d76d92c6fab3b73e2b0220367750dbbe19290069cba53d096f44530e4f98acaa594810388cf7409a1870ce01473044022068c7946a43232757cbdf9176f009a928e1cd9a1a8c212f15c1e11ac9f2925d9002205b75f937ff2f9f3c1246e547e54f62e027f64eefa2695578cc6432cdabce271502473044022059ebf56d98010a932cf8ecfec54c48e6139ed6adb0728c09cbe1e4fa0915302e022007cd986c8fa870ff5d2b3a89139c9fe7e499259875357e20fcbb15571c76795403483045022100fbefd94bd0a488d50b79102b5dad4ab6ced30c4069f1eaa69a4b5a763414067e02203156c6a5c9cf88f91265f5a942e96213afae16d83321c8b31bb342142a14d16381483045022100a5263ea0553ba89221984bd7f0b13613db16e7a70c549a86de0cc0444141a407022005c360ef0ae5a5d4f9f2f87a56c1546cc8268cab08c73501d6b3be2e1e1a8a08824730440220525406a1482936d5a21888260dc165497a90a15669636d8edca6b9fe490d309c022032af0c646a34a44d1f4576bf6a4a74b67940f8faa84c7df9abe12a01a11e2b4783cf56210307b8ae49ac90a048e9b53357a2354b3334e9c8bee813ecb98e99a7e07e8c3ba32103b28f0c28bfab54554ae8c658ac5c3e0ce6e79ad336331f78c428dd43eea8449b21034b8113d703413d57761b8b9781957b8c0ac1dfe69f492580ca4195f50376ba4a21033400f6afecb833092a9a21cfdf1ed1376e58c5d1f47de74683123987e967a8f42103a6d48b1131e94ba04d9737d61acdaa1322008af9602b3b14862c07a1789aac162102d8b661b0b3302ee2f162b09e07a55ad5dfbe673a9f01d9f0c19617681024306b56ae00000000");
    const transaction_accessor tx(decoded_tx, true);
    BOOST_REQUIRE(tx.is_valid());
    BOOST_REQUIRE_EQUAL(tx.inputs_ptr()->size(), 1u);
    BOOST_REQUIRE(!(*tx.inputs_ptr())[0]->witness().stack().empty());

    constexpr auto value0 = 987654321u;
    (*tx.inputs_ptr())[0]->prevout = to_shared(output{ value0, { base16_chunk("a9149993a429037b5d912407a71c252019287b8d27a587"), false } });

    // P2SH-P2WSH 6-of-6 multisig witness program.
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip16_rule | flags::bip141_rule | flags::bip143_rule }, 0), error::script_success);

    // Other scenarios:

    // missing bip16 (required for p2sh, embedded witness not consumed).
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip141_rule | flags::bip143_rule }, 0), error::unexpected_witness);

    // missing bip141 (witness not allowed).
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip16_rule | flags::bip143_rule }, 0), error::unexpected_witness);

    // missing bip143 (invalid sighash).
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip16_rule | flags::bip141_rule  }, 0), error::stack_false);
}

BOOST_AUTO_TEST_CASE(script__verify__bip143_no_find_and_delete_tx__success)
{
    const auto decoded_tx = base16_chunk("0100000000010169c12106097dc2e0526493ef67f21269fe888ef05c7a3a5dacab38e1ac8387f14c1d000000ffffffff01010000000000000000034830450220487fb382c4974de3f7d834c1b617fe15860828c7f96454490edd6d891556dcc9022100baf95feb48f845d5bfc9882eb6aeefa1bc3790e39f59eaa46ff7f15ae626c53e012102a9781d66b61fb5a7ef00ac5ad5bc6ffc78be7b44a566e3c87870e1079368df4c4aad4830450220487fb382c4974de3f7d834c1b617fe15860828c7f96454490edd6d891556dcc9022100baf95feb48f845d5bfc9882eb6aeefa1bc3790e39f59eaa46ff7f15ae626c53e0100000000");
    const transaction_accessor tx(decoded_tx, true);
    BOOST_REQUIRE(tx.is_valid());
    BOOST_REQUIRE_EQUAL(tx.inputs_ptr()->size(), 1u);
    BOOST_REQUIRE(!(*tx.inputs_ptr())[0]->witness().stack().empty());

    constexpr auto value0 = 200000u;
    (*tx.inputs_ptr())[0]->prevout = to_shared(output{ value0, { base16_chunk("00209e1be07558ea5cc8e02ed1d80c0911048afad949affa36d5c3951e3159dbea19"), false } });

    // P2WSH witness program.
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip141_rule | flags::bip143_rule | flags::bip147_rule }, 0), error::script_success);

    // Other scenarios:

    // missing bip141 (witness not allowed).
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip16_rule | flags::bip143_rule }, 0), error::unexpected_witness);

    // missing bip143 (find-and-delete treatment).
    BOOST_REQUIRE_EQUAL(tx.connect({ flags::bip16_rule | flags::bip141_rule }, 0), error::op_check_sig_verify8);
}

// json
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(script__json__conversions__expected)
{
    const std::string text
    {
        "\"pick roll return\""
    };

    const chain::script instance
    {
        chain::operations
        {
            { opcode::pick },
            { opcode::roll },
            { opcode::op_return }
        }
    };

    const auto value = json::value_from(instance);

    // cannot parse a value, must be an object.
    ////BOOST_REQUIRE(json::parse(text) == value);
    BOOST_REQUIRE_EQUAL(json::serialize(value), text);

    BOOST_REQUIRE(json::value_from(instance) == value);
    BOOST_REQUIRE(json::value_to<chain::script>(value) == instance);
}

BOOST_AUTO_TEST_SUITE_END()
