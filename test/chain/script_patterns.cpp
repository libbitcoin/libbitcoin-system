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

// Pattern matching tests.
// -----------------------------------------------------------------------------

// pay_null_data

static const std::string script_return = "return";
static const std::string script_return_empty = "return []";
static const std::string script_return_80 = "return [0001020304050607080900010203040506070809000102030405060708090001020304050607080900010203040506070809000102030405060708090001020304050607080900010203040506070809]";
static const std::string script_return_81 = "return [0001020304050607080900010203040506070809000102030405060708090001020304050607080900010203040506070809000102030405060708090001020304050607080900010203040506070809FF]";

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

static const std::string script_20_of_20_multisig =
    "[14] "
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
    "[03624505c6cc3967352cce480d8550490dd68519cd019066a4c302fdfb7d1c9934] "
    "[03dcfd9e580de35d8c2060d76dbf9e5561fe20febd2e64380e860a4d59f15ac864] "
    "[02440e0304bf8d32b2012994393c6a477acf238dd6adb4c3cef5bfa72f30c9861c] "
    "[14] checkmultisig";

static const std::string script_20_of_21_multisig =
    "[14] "
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
    "[03624505c6cc3967352cce480d8550490dd68519cd019066a4c302fdfb7d1c9934] "
    "[03dcfd9e580de35d8c2060d76dbf9e5561fe20febd2e64380e860a4d59f15ac864] "
    "[02440e0304bf8d32b2012994393c6a477acf238dd6adb4c3cef5bfa72f30c9861c] "
    "[03624505c6cc3967352cce480d8550490dd68519cd019066a4c302fdfb7d1c9934] "
    "[15] checkmultisig";

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

BOOST_AUTO_TEST_CASE(script__pattern__20_of_20_multisig__pay_multisig)
{
    const script instance(script_20_of_20_multisig);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.output_pattern() == chain::script_pattern::pay_multisig);
    BOOST_REQUIRE(instance.input_pattern() == chain::script_pattern::non_standard);
    BOOST_REQUIRE(instance.pattern() == chain::script_pattern::pay_multisig);
}

BOOST_AUTO_TEST_CASE(script__pattern__20_of_21_multisig__non_standard)
{
    const script instance(script_20_of_21_multisig);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.output_pattern() == chain::script_pattern::non_standard);
    BOOST_REQUIRE(instance.input_pattern() == chain::script_pattern::non_standard);
    BOOST_REQUIRE(instance.pattern() == chain::script_pattern::non_standard);
}

// is_pay_tapscript_threshold_pattern

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_threshold_pattern__match_1_of_1__true)
{
    const auto ops = make_tapscript_threshold_ops(1, 1);
    BOOST_CHECK(script::is_pay_tapscript_threshold_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_threshold_pattern__match_2_of_2__true)
{
    const auto ops = make_tapscript_threshold_ops(2, 2);
    BOOST_CHECK(script::is_pay_tapscript_threshold_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_threshold_pattern__match_2_of_3__true)
{
    const auto ops = make_tapscript_threshold_ops(2, 3);
    BOOST_CHECK(script::is_pay_tapscript_threshold_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_threshold_pattern__match_0_of_1__true)
{
    const auto xkey = to_chunk(ec_xonly{});

    operations ops{};
    ops.emplace_back(xkey, true);
    ops.emplace_back(opcode::checksig);
    ops.emplace_back(opcode::push_size_0);
    ops.emplace_back(opcode::numequal);
    BOOST_CHECK(script::is_pay_tapscript_threshold_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_threshold_pattern__within_0_to_1__true)
{
    const auto xkey = to_chunk(ec_xonly{});

    operations ops{};
    ops.emplace_back(xkey, true);
    ops.emplace_back(opcode::checksig);
    ops.emplace_back(xkey, true);
    ops.emplace_back(opcode::checksigadd);
    ops.emplace_back(opcode::push_size_0);
    ops.emplace_back(opcode::push_size_1);
    ops.emplace_back(opcode::within);
    BOOST_CHECK(script::is_pay_tapscript_threshold_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_threshold_pattern__odd_length__false)
{
    auto ops = make_tapscript_threshold_ops(2, 2);
    ops.pop_back();
    BOOST_CHECK(!script::is_pay_tapscript_threshold_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_threshold_pattern__wrong_final_opcode__false)
{
    auto ops = make_tapscript_threshold_ops(2, 2);
    ops.back() = operation(opcode::equal);
    BOOST_CHECK(!script::is_pay_tapscript_threshold_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_threshold_pattern__negative_threshold__false)
{
    auto ops = make_tapscript_threshold_ops(2, 2);
    ops[ops.size() - 2] = operation(opcode::push_negative_1);
    BOOST_CHECK(!script::is_pay_tapscript_threshold_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_threshold_pattern__wrong_order__false)
{
    const auto xkey = to_chunk(ec_xonly{});

    operations ops{};
    ops.emplace_back(xkey, true);
    ops.emplace_back(opcode::checksig);
    ops.emplace_back(opcode::checksigadd);
    ops.emplace_back(xkey, true);
    ops.emplace_back(opcode::push_size_2);
    ops.emplace_back(opcode::numequal);
    BOOST_CHECK(!script::is_pay_tapscript_threshold_pattern(ops));
}

// is_pay_tapscript_multisig_pattern

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_multisig_pattern__match_1_of_1__true)
{
    const auto ops = make_tapscript_multisig_ops(1);
    BOOST_CHECK(script::is_pay_tapscript_multisig_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_multisig_pattern__match_2_of_2__true)
{
    const auto ops = make_tapscript_multisig_ops(2);
    BOOST_CHECK(script::is_pay_tapscript_multisig_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_multisig_pattern__match_3_of_3__true)
{
    const auto ops = make_tapscript_multisig_ops(3);
    BOOST_CHECK(script::is_pay_tapscript_multisig_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_multisig_pattern__odd_length__false)
{
    auto ops = make_tapscript_multisig_ops(2);
    ops.pop_back();
    BOOST_CHECK(!script::is_pay_tapscript_multisig_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_multisig_pattern__wrong_final_opcode__false)
{
    auto ops = make_tapscript_multisig_ops(2);
    ops.back() = operation(opcode::checksigverify);
    BOOST_CHECK(!script::is_pay_tapscript_multisig_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_multisig_pattern__wrong_intermediate_opcode__false)
{
    const auto xkey = to_chunk(ec_xonly{});

    operations ops{};
    ops.emplace_back(xkey, true);
    ops.emplace_back(opcode::checksig);
    ops.emplace_back(xkey, true);
    ops.emplace_back(opcode::checksig);
    BOOST_CHECK(!script::is_pay_tapscript_multisig_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_multisig_pattern__wrong_key_size__false)
{
    const auto short_key = to_chunk(short_hash{});

    operations ops{};
    ops.emplace_back(short_key, true);
    ops.emplace_back(opcode::checksigverify);
    ops.emplace_back(short_key, true);
    ops.emplace_back(opcode::checksig);
    BOOST_CHECK(!script::is_pay_tapscript_multisig_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_multisig_pattern__wrong_order__false)
{
    const auto xkey = to_chunk(ec_xonly{});

    operations ops{};
    ops.emplace_back(xkey, true);
    ops.emplace_back(opcode::checksig);
    ops.emplace_back(xkey, true);
    ops.emplace_back(opcode::checksigverify);
    BOOST_CHECK(!script::is_pay_tapscript_multisig_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_multisig_pattern__empty__false)
{
    operations ops{};
    BOOST_CHECK(!script::is_pay_tapscript_multisig_pattern(ops));
}

// is_pay_tapscript_inscription_pattern

// <pubkey> OP_CHECKSIG
// OP_0
// OP_IF
//   OP_PUSHBYTES_3 "ord"   ; 0x6f7264
//   OP_PUSHBYTES_1 0x01
//   OP_PUSHBYTES_... "<MIME type>"
//   OP_0
//   [data chunk(s) ≤ 520 bytes each]
// OP_ENDIF

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_inscription_pattern__match_minimal__true)
{
    const auto xkey = to_chunk(ec_xonly{});

    operations ops{};
    ops.emplace_back(xkey, true);
    ops.emplace_back(opcode::checksig);
    ops.emplace_back(opcode::push_size_0);
    ops.emplace_back(opcode::if_);
    ops.emplace_back(opcode::endif);

    BOOST_CHECK(script::is_pay_tapscript_inscription_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_inscription_pattern__match_normal__true)
{
    const auto xkey = to_chunk(ec_xonly{});

    operations ops{};
    ops.emplace_back(xkey, true);
    ops.emplace_back(opcode::checksig);
    ops.emplace_back(opcode::push_size_0);
    ops.emplace_back(opcode::if_);
    ops.emplace_back(to_chunk(std::string("ord")), true);
    ops.emplace_back(opcode::push_positive_1);
    ops.emplace_back(to_chunk(std::string("text/plain")), true);
    ops.emplace_back(opcode::push_size_0);
    ops.emplace_back(to_chunk(std::string("hello")), true);
    ops.emplace_back(opcode::endif);

    BOOST_CHECK(script::is_pay_tapscript_inscription_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_inscription_pattern__too_small__false)
{
    const auto xkey = to_chunk(ec_xonly{});

    operations ops{};
    ops.emplace_back(xkey, true);
    ops.emplace_back(opcode::checksig);
    ops.emplace_back(opcode::push_size_0);
    ops.emplace_back(opcode::if_);

    BOOST_CHECK(!script::is_pay_tapscript_inscription_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_inscription_pattern__wrong_start__false)
{
    const auto xkey = to_chunk(ec_xonly{});

    operations ops{};
    ops.emplace_back(opcode::push_size_0);
    ops.emplace_back(opcode::checksig);
    ops.emplace_back(opcode::push_size_0);
    ops.emplace_back(opcode::if_);
    ops.emplace_back(opcode::endif);

    BOOST_CHECK(!script::is_pay_tapscript_inscription_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_inscription_pattern__no_if_envelope__false)
{
    const auto xkey = to_chunk(ec_xonly{});

    operations ops{};
    ops.emplace_back(xkey, true);
    ops.emplace_back(opcode::checksig);
    ops.emplace_back(opcode::push_size_0);
    ops.emplace_back(opcode::endif);

    BOOST_CHECK(!script::is_pay_tapscript_inscription_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_inscription_pattern__wrong_final_opcode__false)
{
    const auto xkey = to_chunk(ec_xonly{});

    operations ops{};
    ops.emplace_back(xkey, true);
    ops.emplace_back(opcode::checksig);
    ops.emplace_back(opcode::push_size_0);
    ops.emplace_back(opcode::if_);
    ops.emplace_back(opcode::push_size_0); // not endif

    BOOST_CHECK(!script::is_pay_tapscript_inscription_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_inscription_pattern__empty__false)
{
    operations ops{};
    BOOST_CHECK(!script::is_pay_tapscript_inscription_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_inscription_pattern__wrong_key_size__false)
{
    const auto short_key = to_chunk(short_hash{});

    operations ops{};
    ops.emplace_back(short_key, true);
    ops.emplace_back(opcode::checksig);
    ops.emplace_back(opcode::push_size_0);
    ops.emplace_back(opcode::if_);
    ops.emplace_back(opcode::endif);

    BOOST_CHECK(!script::is_pay_tapscript_inscription_pattern(ops));
}

BOOST_AUTO_TEST_SUITE_END()
