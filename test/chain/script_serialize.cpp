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

BOOST_AUTO_TEST_SUITE(script_tests)

using namespace system::chain;
using namespace system::machine;

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
    constexpr auto script_2_of_3_multisig = "2 [03dcfd9e580de35d8c2060d76dbf9e5561fe20febd2e64380e860a4d59f15ac864] [02440e0304bf8d32b2012994393c6a477acf238dd6adb4c3cef5bfa72f30c9861c] [03624505c6cc3967352cce480d8550490dd68519cd019066a4c302fdfb7d1c9934] 3 checkmultisig";
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

BOOST_AUTO_TEST_SUITE_END()
