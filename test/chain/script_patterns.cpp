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

BOOST_AUTO_TEST_SUITE(script_patterns_tests)

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

// pay_witness

// The version byte is the opcode that pushes the version value, so version 1
// serializes as 0x51 (op_1), not 0x01 (a one byte data push) [bip141].
static const auto witness_v0_key_hash = base16_chunk("0014a85b2107f791b26a84e7586c28cec7cb61202ed3");
static const auto witness_v0_script_hash = base16_chunk("0020a85b2107f791b26a84e7586c28cec7cb61202ed3d01944d832500f363782d675");
static const auto witness_v1_taproot = base16_chunk("5120a85b2107f791b26a84e7586c28cec7cb61202ed3d01944d832500f363782d675");
static const auto push_size_1_version = base16_chunk("010020a85b2107f791b26a84e7586c28cec7cb61202ed3d01944d832500f363782d675");

BOOST_AUTO_TEST_CASE(script__output_pattern__witness_v1_taproot__pay_witness_v1_taproot)
{
    const script instance(witness_v1_taproot, false);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.version() == script_version::taproot);
    BOOST_REQUIRE(instance.output_pattern() == chain::script_pattern::pay_witness_v1_taproot);
}

static const auto witness_v1_anchor = base16_chunk("51024e73");
static const auto witness_v2_anchor = base16_chunk("52024e73");
static const auto witness_v1_key_hash = base16_chunk("5114a85b2107f791b26a84e7586c28cec7cb61202ed3");
static const auto witness_v0_program_22 = base16_chunk("0016a85b2107f791b26a84e7586c28cec7cb61202ed3d019");

BOOST_AUTO_TEST_CASE(script__output_pattern__witness_v1_anchor__pay_anchor)
{
    const script instance(witness_v1_anchor, false);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(script::is_pay_anchor_pattern(instance.ops()));
    BOOST_REQUIRE(instance.output_pattern() == chain::script_pattern::pay_anchor);
}

BOOST_AUTO_TEST_CASE(script__output_pattern__witness_v2_anchor__pay_witness_unknown)
{
    const script instance(witness_v2_anchor, false);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(!script::is_pay_anchor_pattern(instance.ops()));
    BOOST_REQUIRE(instance.output_pattern() == chain::script_pattern::pay_witness_unknown);
}

BOOST_AUTO_TEST_CASE(script__output_pattern__witness_v1_key_hash__pay_witness_unknown)
{
    const script instance(witness_v1_key_hash, false);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.output_pattern() == chain::script_pattern::pay_witness_unknown);
}

BOOST_AUTO_TEST_CASE(script__output_pattern__witness_v0_program_22__non_standard)
{
    const script instance(witness_v0_program_22, false);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.output_pattern() == chain::script_pattern::non_standard);
}

BOOST_AUTO_TEST_CASE(script__output_pattern__witness_v0_key_hash__pay_witness_key_hash)
{
    const script instance(witness_v0_key_hash, false);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.version() == script_version::segwit);
    BOOST_REQUIRE(instance.output_pattern() == chain::script_pattern::pay_witness_key_hash);
}

BOOST_AUTO_TEST_CASE(script__output_pattern__witness_v0_script_hash__pay_witness_script_hash)
{
    const script instance(witness_v0_script_hash, false);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.version() == script_version::segwit);
    BOOST_REQUIRE(instance.output_pattern() == chain::script_pattern::pay_witness_script_hash);
}

// A one byte data push consumes the following byte, so this is not a program.
BOOST_AUTO_TEST_CASE(script__output_pattern__push_size_1_version__non_standard)
{
    const script instance(push_size_1_version, false);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.version() == script_version::unversioned);
    BOOST_REQUIRE(!script::is_pay_witness_taproot_pattern(instance.ops()));
    BOOST_REQUIRE(instance.output_pattern() == chain::script_pattern::non_standard);
}

BOOST_AUTO_TEST_CASE(script__to_pay_witness_taproot_pattern__round_trip__taproot)
{
    const script instance(script::to_pay_witness_taproot_pattern(hash_digest{}));
    BOOST_REQUIRE(script::is_pay_witness_taproot_pattern(instance.ops()));
    BOOST_REQUIRE(instance.version() == script_version::taproot);
    BOOST_REQUIRE(instance.output_pattern() == chain::script_pattern::pay_witness_v1_taproot);
}

BOOST_AUTO_TEST_CASE(script__to_pay_witness_key_hash_pattern__round_trip__segwit)
{
    const script instance(script::to_pay_witness_key_hash_pattern(short_hash{}));
    BOOST_REQUIRE(script::is_pay_witness_key_hash_pattern(instance.ops()));
    BOOST_REQUIRE(instance.version() == script_version::segwit);
    BOOST_REQUIRE(instance.output_pattern() == chain::script_pattern::pay_witness_key_hash);
}

BOOST_AUTO_TEST_CASE(script__to_pay_witness_script_hash_pattern__round_trip__segwit)
{
    const script instance(script::to_pay_witness_script_hash_pattern(hash_digest{}));
    BOOST_REQUIRE(script::is_pay_witness_script_hash_pattern(instance.ops()));
    BOOST_REQUIRE(instance.version() == script_version::segwit);
    BOOST_REQUIRE(instance.output_pattern() == chain::script_pattern::pay_witness_script_hash);
}

// is_nominal_push_pattern

// A 22 byte push is nominally encoded by push_size_22 (0x16).
BOOST_AUTO_TEST_CASE(script__is_nominal_push_pattern__push_size_22__true)
{
    const script instance(base16_chunk("16001479091972186c449eb1ded22b78e40d009bdf0089"), false);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_CHECK(script::is_nominal_push_pattern(instance.ops()));
}

BOOST_AUTO_TEST_CASE(script__is_nominal_push_pattern__push_one_size_22__false)
{
    const script instance(base16_chunk("4c16001479091972186c449eb1ded22b78e40d009bdf0089"), false);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_CHECK(!script::is_nominal_push_pattern(instance.ops()));
}

BOOST_AUTO_TEST_CASE(script__is_nominal_push_pattern__push_two_size_22__false)
{
    const script instance(base16_chunk("4d1600001479091972186c449eb1ded22b78e40d009bdf0089"), false);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_CHECK(!script::is_nominal_push_pattern(instance.ops()));
}

// A 76 byte push is nominally encoded by push_one_size (0x4c).
BOOST_AUTO_TEST_CASE(script__is_nominal_push_pattern__push_one_size_76__true)
{
    const script instance(base16_chunk("4c4c01020304050607080102030405060708010203040506070801020304050607080102030405060708010203040506070801020304050607080102030405060708010203040506070801020304"), false);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_CHECK(script::is_nominal_push_pattern(instance.ops()));
}

BOOST_AUTO_TEST_CASE(script__is_nominal_push_pattern__push_two_size_76__false)
{
    const script instance(base16_chunk("4d4c0001020304050607080102030405060708010203040506070801020304050607080102030405060708010203040506070801020304050607080102030405060708010203040506070801020304"), false);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_CHECK(!script::is_nominal_push_pattern(instance.ops()));
}

BOOST_AUTO_TEST_CASE(script__is_nominal_push_pattern__two_pushes__false)
{
    const script instance(base16_chunk("0016001479091972186c449eb1ded22b78e40d009bdf0089"), false);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_CHECK(!script::is_nominal_push_pattern(instance.ops()));
}

// version_value

BOOST_AUTO_TEST_CASE(script__version_value__not_witness_program__unversioned)
{
    const script instance(script_return);
    constexpr auto unversioned = to_value(script_version::unversioned);
    BOOST_REQUIRE_EQUAL(instance.version_value(), unversioned);
    BOOST_REQUIRE(instance.version() == script_version::unversioned);
}

BOOST_AUTO_TEST_CASE(script__version_value__segwit_program__zero)
{
    const script instance{ script::to_pay_witness_pattern(0, null_hash) };
    BOOST_REQUIRE_EQUAL(instance.version_value(), 0u);
    BOOST_REQUIRE(instance.version() == script_version::segwit);
}

BOOST_AUTO_TEST_CASE(script__version_value__taproot_program__one)
{
    const script instance{ script::to_pay_witness_pattern(1, null_hash) };
    BOOST_REQUIRE_EQUAL(instance.version_value(), 1u);
    BOOST_REQUIRE(instance.version() == script_version::taproot);
}

// The reserved versions are distinguished by value but not by version.
BOOST_AUTO_TEST_CASE(script__version_value__reserved_program__two)
{
    const script instance{ script::to_pay_witness_pattern(2, null_hash) };
    BOOST_REQUIRE_EQUAL(instance.version_value(), 2u);
    BOOST_REQUIRE(instance.version() == script_version::reserved);
}

BOOST_AUTO_TEST_CASE(script__version_value__maximum_program__sixteen)
{
    const script instance{ script::to_pay_witness_pattern(16, null_hash) };
    BOOST_REQUIRE_EQUAL(instance.version_value(), 16u);
    BOOST_REQUIRE(instance.version() == script_version::reserved);
}

// public_key

BOOST_AUTO_TEST_CASE(script__public_key__not_pay_public_key__empty)
{
    const script instance(script_return);
    BOOST_REQUIRE(instance.public_key()->empty());
}

BOOST_AUTO_TEST_CASE(script__public_key__pay_public_key__key)
{
    const data_chunk key(33, 0x02);
    const script instance{ script::to_pay_public_key_pattern(key) };
    BOOST_REQUIRE(*instance.public_key() == key);
}

// multisig_required/multisig_keys

BOOST_AUTO_TEST_CASE(script__multisig_required__not_multisig__zero_and_empty)
{
    const script instance(script_return);
    BOOST_REQUIRE_EQUAL(instance.multisig_required(), 0u);
    BOOST_REQUIRE(instance.multisig_keys().empty());
}

BOOST_AUTO_TEST_CASE(script__multisig_required__two_of_three__two_and_keys)
{
    // The pattern builder requires valid public keys (compressed prefixes).
    const data_chunk key1(33, 0x02);
    const data_chunk key2(33, 0x03);
    auto key3 = key1;
    key3.back() = 0x04;
    const data_stack points{ key1, key2, key3 };
    const script instance{ script::to_pay_multisig_pattern(2, points) };
    BOOST_REQUIRE_EQUAL(instance.multisig_required(), 2u);

    const auto keys = instance.multisig_keys();
    BOOST_REQUIRE_EQUAL(keys.size(), 3u);
    BOOST_REQUIRE(*keys.front() == key1);
    BOOST_REQUIRE(*keys.back() == key3);
}

// endorsements

BOOST_AUTO_TEST_CASE(script__endorsements__not_sign_multisig__empty)
{
    const script instance(script_return);
    BOOST_REQUIRE(instance.endorsements().empty());
}

BOOST_AUTO_TEST_CASE(script__endorsements__two_endorsements__both)
{
    const data_chunk sig1(71, 0x30);
    const data_chunk sig2(72, 0x30);
    const operations ops
    {
        { opcode::push_size_0 },
        { data_chunk{ sig1 }, false },
        { data_chunk{ sig2 }, false }
    };

    const script instance{ ops };
    const auto sigs = instance.endorsements();
    BOOST_REQUIRE_EQUAL(sigs.size(), 2u);
    BOOST_REQUIRE(*sigs.front() == sig1);
    BOOST_REQUIRE(*sigs.back() == sig2);
}

BOOST_AUTO_TEST_CASE(script__is_nominal_push_pattern__empty__false)
{
    BOOST_CHECK(!script::is_nominal_push_pattern(operations{}));
}

BOOST_AUTO_TEST_CASE(script__is_nominal_push_pattern__non_push__false)
{
    const operations ops{ operation{ opcode::checksig } };
    BOOST_CHECK(!script::is_nominal_push_pattern(ops));
}

// is_push_only_pattern/is_relaxed_push_pattern

BOOST_AUTO_TEST_CASE(script__is_push_only_pattern__empty__true)
{
    BOOST_CHECK(script::is_push_only_pattern(operations{}));
}

BOOST_AUTO_TEST_CASE(script__is_push_only_pattern__pushes__true)
{
    const operations ops{ operation{ data_chunk{ 0x01 }, false }, operation{ opcode::push_positive_1 } };
    BOOST_CHECK(script::is_push_only_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_push_only_pattern__non_push__false)
{
    const operations ops{ operation{ data_chunk{ 0x01 }, false }, operation{ opcode::checksig } };
    BOOST_CHECK(!script::is_push_only_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_relaxed_push_pattern__pushes__true)
{
    const operations ops{ operation{ data_chunk{ 0x01 }, false }, operation{ opcode::push_positive_16 } };
    BOOST_CHECK(script::is_relaxed_push_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_relaxed_push_pattern__non_push__false)
{
    const operations ops{ operation{ opcode::checksig } };
    BOOST_CHECK(!script::is_relaxed_push_pattern(ops));
}

// is_pay_script_hash_pattern

BOOST_AUTO_TEST_CASE(script__is_pay_script_hash_pattern__expected__true)
{
    const auto ops = script::to_pay_script_hash_pattern(short_hash{});
    BOOST_CHECK(script::is_pay_script_hash_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_script_hash_pattern__wrong_hash_size__false)
{
    const operations ops
    {
        operation{ opcode::hash160 },
        operation{ data_chunk(32, 0x00), false },
        operation{ opcode::equal }
    };

    BOOST_CHECK(!script::is_pay_script_hash_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_script_hash_pattern__missing_equal__false)
{
    const operations ops
    {
        operation{ opcode::hash160 },
        operation{ data_chunk(20, 0x00), false }
    };

    BOOST_CHECK(!script::is_pay_script_hash_pattern(ops));
}

// is_pay_witness_pattern

BOOST_AUTO_TEST_CASE(script__is_pay_witness_pattern__minimum_program__true)
{
    const auto ops = script::to_pay_witness_pattern(0_u8, data_chunk(2, 0x00));
    BOOST_CHECK(script::is_pay_witness_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_witness_pattern__maximum_program__true)
{
    const auto ops = script::to_pay_witness_pattern(16_u8, data_chunk(40, 0x00));
    BOOST_CHECK(script::is_pay_witness_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_witness_pattern__undersized_program__false)
{
    const auto ops = script::to_pay_witness_pattern(0_u8, data_chunk(1, 0x00));
    BOOST_CHECK(!script::is_pay_witness_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_witness_pattern__oversized_program__false)
{
    const auto ops = script::to_pay_witness_pattern(0_u8, data_chunk(41, 0x00));
    BOOST_CHECK(!script::is_pay_witness_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_witness_pattern__non_numeric_version__false)
{
    const operations ops
    {
        operation{ opcode::checksig },
        operation{ data_chunk(20, 0x00), false }
    };

    BOOST_CHECK(!script::is_pay_witness_pattern(ops));
}

// is_commitment_pattern

BOOST_AUTO_TEST_CASE(script__is_commitment_pattern__expected__true)
{
    const script commitment(base16_chunk("6a24aa21a9ed0102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20"), false);
    BOOST_CHECK(script::is_commitment_pattern(commitment.ops()));
}

BOOST_AUTO_TEST_CASE(script__is_commitment_pattern__wrong_head__false)
{
    const script commitment(base16_chunk("6a24aa21a9ee0102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20"), false);
    BOOST_CHECK(!script::is_commitment_pattern(commitment.ops()));
}

BOOST_AUTO_TEST_CASE(script__is_commitment_pattern__wrong_push_size__false)
{
    const script commitment(base16_chunk("6a23aa21a9ed0102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f"), false);
    BOOST_CHECK(!script::is_commitment_pattern(commitment.ops()));
}

// is_pay_null_data_pattern

BOOST_AUTO_TEST_CASE(script__is_pay_null_data_pattern__pushed_data__true)
{
    const operations ops{ operation{ opcode::op_return }, operation{ data_chunk{ 0x2a }, false } };
    BOOST_CHECK(script::is_pay_null_data_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_null_data_pattern__small_integer_opcode__true)
{
    const operations ops{ operation{ opcode::op_return }, operation{ opcode::push_positive_1 } };
    BOOST_CHECK(script::is_pay_null_data_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_null_data_pattern__non_minimal_push__false)
{
    const script instance(base16_chunk("6a4c0101"), false);
    BOOST_REQUIRE(instance.ops()[1].code() == opcode::push_one_size);
    BOOST_CHECK(!script::is_pay_null_data_pattern(instance.ops()));
}

// version/version_value/witness_program

BOOST_AUTO_TEST_CASE(script__version__non_witness__unversioned)
{
    const script instance{ operations{ operation{ opcode::checksig } } };
    BOOST_REQUIRE(instance.version() == script_version::unversioned);
    BOOST_REQUIRE(instance.witness_program()->empty());
}

BOOST_AUTO_TEST_CASE(script__version__version_zero_program__segwit)
{
    const auto program = data_chunk(20, 0x42);
    const script instance{ script::to_pay_witness_pattern(0_u8, program) };
    BOOST_REQUIRE(instance.version() == script_version::segwit);
    BOOST_REQUIRE_EQUAL(instance.version_value(), 0_u8);
    BOOST_REQUIRE_EQUAL(*instance.witness_program(), program);
}

BOOST_AUTO_TEST_CASE(script__version__version_one_program__taproot)
{
    const auto program = data_chunk(32, 0x42);
    const script instance{ script::to_pay_witness_pattern(1_u8, program) };
    BOOST_REQUIRE(instance.version() == script_version::taproot);
    BOOST_REQUIRE_EQUAL(instance.version_value(), 1_u8);
}

BOOST_AUTO_TEST_CASE(script__version__version_two_program__reserved)
{
    const auto program = data_chunk(32, 0x42);
    const script instance{ script::to_pay_witness_pattern(2_u8, program) };
    BOOST_REQUIRE(instance.version() == script_version::reserved);
    BOOST_REQUIRE_EQUAL(instance.version_value(), 2_u8);
}

BOOST_AUTO_TEST_CASE(script__version__version_sixteen_program__reserved)
{
    const auto program = data_chunk(32, 0x42);
    const script instance{ script::to_pay_witness_pattern(16_u8, program) };
    BOOST_REQUIRE(instance.version() == script_version::reserved);
}

// is_pay_to_witness/is_pay_to_script_hash are flag gated.

BOOST_AUTO_TEST_CASE(script__is_pay_to_witness__bip141_gated__expected)
{
    const script instance{ script::to_pay_witness_pattern(0_u8, data_chunk(20, 0x42)) };
    BOOST_REQUIRE(instance.is_pay_to_witness(flags::bip141_rule));
    BOOST_REQUIRE(!instance.is_pay_to_witness(flags::no_rules));
}

BOOST_AUTO_TEST_CASE(script__is_pay_to_script_hash__bip16_gated__expected)
{
    const script instance{ script::to_pay_script_hash_pattern(short_hash{}) };
    BOOST_REQUIRE(instance.is_pay_to_script_hash(flags::bip16_rule));
    BOOST_REQUIRE(!instance.is_pay_to_script_hash(flags::no_rules));
}

// Predicates not surfaced by pattern/input_pattern/output_pattern.
// -----------------------------------------------------------------------------

static const auto pattern_signature = data_chunk(70, 0x42);
static const auto pattern_short = data_chunk(8, 0x42);
static const auto pattern_xonly = data_chunk(ec_xonly_size, 0x42);
static const auto pattern_hash20 = data_chunk(short_hash_size, 0x42);
static const auto pattern_hash32 = data_chunk(hash_size, 0x42);
static const auto pattern_anchor = base16_chunk("4e73");
static const auto pattern_compressed = build_chunk(
{
    base16_chunk("03"),
    data_chunk(sub1(ec_compressed_size), 0x42)
});
static const auto pattern_uncompressed = build_chunk(
{
    base16_chunk("04"),
    data_chunk(sub1(ec_uncompressed_size), 0x42)
});

// is_pay_op_return_pattern

BOOST_AUTO_TEST_CASE(script__is_pay_op_return_pattern__empty__false)
{
    BOOST_REQUIRE(!script::is_pay_op_return_pattern(operations{}));
}

BOOST_AUTO_TEST_CASE(script__is_pay_op_return_pattern__return_only__true)
{
    const operations ops{ { opcode::op_return } };
    BOOST_REQUIRE(script::is_pay_op_return_pattern(ops));
}

// Unlike pay_null_data this is unbounded in data size and operation count.
BOOST_AUTO_TEST_CASE(script__is_pay_op_return_pattern__return_and_trailing__true)
{
    const operations ops
    {
        { opcode::op_return },
        { pattern_hash32, true },
        { opcode::checksig }
    };

    BOOST_REQUIRE(script::is_pay_op_return_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_op_return_pattern__trailing_return__false)
{
    const operations ops{ { pattern_hash32, true }, { opcode::op_return } };
    BOOST_REQUIRE(!script::is_pay_op_return_pattern(ops));
}

// is_pay_witness_unknown_pattern

BOOST_AUTO_TEST_CASE(script__is_pay_witness_unknown_pattern__version_two__true)
{
    const auto version = operation::opcode_from_nonnegative(2_u8);
    const operations ops{ { version }, { pattern_hash32, true } };
    BOOST_REQUIRE(script::is_pay_witness_unknown_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_witness_unknown_pattern__version_zero__false)
{
    const auto version = operation::opcode_from_nonnegative(0_u8);
    const operations ops{ { version }, { pattern_hash20, true } };
    BOOST_REQUIRE(!script::is_pay_witness_unknown_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_witness_unknown_pattern__taproot__false)
{
    const auto version = operation::opcode_from_nonnegative(1_u8);
    const operations ops{ { version }, { pattern_hash32, true } };
    BOOST_REQUIRE(script::is_pay_witness_taproot_pattern(ops));
    BOOST_REQUIRE(!script::is_pay_witness_unknown_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_witness_unknown_pattern__anchor__false)
{
    const auto version = operation::opcode_from_nonnegative(1_u8);
    const operations ops{ { version }, { pattern_anchor, true } };
    BOOST_REQUIRE(script::is_pay_anchor_pattern(ops));
    BOOST_REQUIRE(!script::is_pay_witness_unknown_pattern(ops));
}

// is_pay_taproot_key_path_pattern

BOOST_AUTO_TEST_CASE(script__is_pay_taproot_key_path_pattern__checksig__true)
{
    const operations ops{ { opcode::checksig } };
    BOOST_REQUIRE(script::is_pay_taproot_key_path_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_taproot_key_path_pattern__two_operations__false)
{
    const operations ops{ { opcode::checksig }, { opcode::checksig } };
    BOOST_REQUIRE(!script::is_pay_taproot_key_path_pattern(ops));
}

// is_pay_tapscript_single_pattern

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_single_pattern__xonly_key__true)
{
    const operations ops{ { pattern_xonly, true }, { opcode::checksig } };
    BOOST_REQUIRE(script::is_pay_tapscript_single_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_single_pattern__compressed_key__false)
{
    const operations ops{ { pattern_compressed, true }, { opcode::checksig } };
    BOOST_REQUIRE(!script::is_pay_tapscript_single_pattern(ops));
}

// is_pay_tapscript_timelock_pattern

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_timelock_pattern__checklocktimeverify__true)
{
    const operations ops
    {
        { opcode::push_positive_1 },
        { opcode::checklocktimeverify },
        { opcode::drop },
        { pattern_xonly, true },
        { opcode::checksig }
    };

    BOOST_REQUIRE(script::is_pay_tapscript_timelock_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_timelock_pattern__checksequenceverify__true)
{
    const operations ops
    {
        { opcode::push_positive_1 },
        { opcode::checksequenceverify },
        { opcode::drop },
        { pattern_xonly, true },
        { opcode::checksig }
    };

    BOOST_REQUIRE(script::is_pay_tapscript_timelock_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_tapscript_timelock_pattern__not_timelock__false)
{
    const operations ops
    {
        { opcode::push_positive_1 },
        { opcode::nop },
        { opcode::drop },
        { pattern_xonly, true },
        { opcode::checksig }
    };

    BOOST_REQUIRE(!script::is_pay_tapscript_timelock_pattern(ops));
}

// is_pay_key_hash_pattern

BOOST_AUTO_TEST_CASE(script__is_pay_key_hash_pattern__short_hash__true)
{
    const operations ops
    {
        { opcode::dup },
        { opcode::hash160 },
        { pattern_hash20, true },
        { opcode::equalverify },
        { opcode::checksig }
    };

    BOOST_REQUIRE(script::is_pay_key_hash_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_key_hash_pattern__long_hash__false)
{
    const operations ops
    {
        { opcode::dup },
        { opcode::hash160 },
        { pattern_hash32, true },
        { opcode::equalverify },
        { opcode::checksig }
    };

    BOOST_REQUIRE(!script::is_pay_key_hash_pattern(ops));
}

// is_pay_public_key_pattern

BOOST_AUTO_TEST_CASE(script__is_pay_public_key_pattern__compressed__true)
{
    const operations ops{ { pattern_compressed, true }, { opcode::checksig } };
    BOOST_REQUIRE(script::is_pay_public_key_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_public_key_pattern__uncompressed__true)
{
    const operations ops{ { pattern_uncompressed, true }, { opcode::checksig } };
    BOOST_REQUIRE(script::is_pay_public_key_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_public_key_pattern__xonly__false)
{
    const operations ops{ { pattern_xonly, true }, { opcode::checksig } };
    BOOST_REQUIRE(!script::is_pay_public_key_pattern(ops));
}

// is_pay_multisig_standard_pattern

BOOST_AUTO_TEST_CASE(script__is_pay_multisig_standard_pattern__one_of_one__true)
{
    const operations ops
    {
        { operation::opcode_from_positive(1_u8) },
        { pattern_compressed, true },
        { operation::opcode_from_positive(1_u8) },
        { opcode::checkmultisig }
    };

    BOOST_REQUIRE(script::is_pay_multisig_standard_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_multisig_standard_pattern__two_of_three__true)
{
    const operations ops
    {
        { operation::opcode_from_positive(2_u8) },
        { pattern_compressed, true },
        { pattern_uncompressed, true },
        { pattern_compressed, true },
        { operation::opcode_from_positive(3_u8) },
        { opcode::checkmultisig }
    };

    BOOST_REQUIRE(script::is_pay_multisig_standard_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_multisig_standard_pattern__signatures_exceed_keys__false)
{
    const operations ops
    {
        { operation::opcode_from_positive(2_u8) },
        { pattern_compressed, true },
        { operation::opcode_from_positive(1_u8) },
        { opcode::checkmultisig }
    };

    BOOST_REQUIRE(!script::is_pay_multisig_standard_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_multisig_standard_pattern__invalid_key__false)
{
    const operations ops
    {
        { operation::opcode_from_positive(1_u8) },
        { pattern_xonly, true },
        { operation::opcode_from_positive(1_u8) },
        { opcode::checkmultisig }
    };

    BOOST_REQUIRE(!script::is_pay_multisig_standard_pattern(ops));
}

// Standard form requires the counts as op_1..op_16, not as data pushes.
BOOST_AUTO_TEST_CASE(script__is_pay_multisig_standard_pattern__non_minimal_count__false)
{
    const operations ops
    {
        { data_chunk{ 0x01 }, false },
        { pattern_compressed, true },
        { operation::opcode_from_positive(1_u8) },
        { opcode::checkmultisig }
    };

    BOOST_REQUIRE(!script::is_pay_multisig_standard_pattern(ops));
}

// is_pay_multisig_pattern

BOOST_AUTO_TEST_CASE(script__is_pay_multisig_pattern__one_of_one__true)
{
    const operations ops
    {
        { operation::opcode_from_positive(1_u8) },
        { pattern_compressed, true },
        { operation::opcode_from_positive(1_u8) },
        { opcode::checkmultisig }
    };

    BOOST_REQUIRE(script::is_pay_multisig_pattern(ops));
}

// Unlike the standard form this admits non-minimal counts.
BOOST_AUTO_TEST_CASE(script__is_pay_multisig_pattern__non_minimal_count__true)
{
    const operations ops
    {
        { data_chunk{ 0x01 }, false },
        { pattern_compressed, true },
        { operation::opcode_from_positive(1_u8) },
        { opcode::checkmultisig }
    };

    BOOST_REQUIRE(script::is_pay_multisig_pattern(ops));
}

// Unlike the standard form this admits invalid public key forms.
BOOST_AUTO_TEST_CASE(script__is_pay_multisig_pattern__invalid_key__true)
{
    const operations ops
    {
        { operation::opcode_from_positive(1_u8) },
        { pattern_xonly, true },
        { operation::opcode_from_positive(1_u8) },
        { opcode::checkmultisig }
    };

    BOOST_REQUIRE(script::is_pay_multisig_pattern(ops));
}

// Zero signatures verifies nothing, so it does not match.
BOOST_AUTO_TEST_CASE(script__is_pay_multisig_pattern__zero_signatures__false)
{
    const operations ops
    {
        { operation::opcode_from_nonnegative(0_u8) },
        { pattern_compressed, true },
        { operation::opcode_from_positive(1_u8) },
        { opcode::checkmultisig }
    };

    BOOST_REQUIRE(!script::is_pay_multisig_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_pay_multisig_pattern__count_mismatch__false)
{
    const operations ops
    {
        { operation::opcode_from_positive(1_u8) },
        { pattern_compressed, true },
        { pattern_compressed, true },
        { operation::opcode_from_positive(1_u8) },
        { opcode::checkmultisig }
    };

    BOOST_REQUIRE(!script::is_pay_multisig_pattern(ops));
}

// is_sign_public_key_pattern

BOOST_AUTO_TEST_CASE(script__is_sign_public_key_pattern__endorsement__true)
{
    const operations ops{ { pattern_signature, true } };
    BOOST_REQUIRE(script::is_sign_public_key_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_sign_public_key_pattern__undersized__false)
{
    const operations ops{ { pattern_short, true } };
    BOOST_REQUIRE(!script::is_sign_public_key_pattern(ops));
}

// is_sign_key_hash_pattern

BOOST_AUTO_TEST_CASE(script__is_sign_key_hash_pattern__endorsement_and_key__true)
{
    const operations ops
    {
        { pattern_signature, true },
        { pattern_compressed, true }
    };

    BOOST_REQUIRE(script::is_sign_key_hash_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_sign_key_hash_pattern__endorsement_only__false)
{
    const operations ops{ { pattern_signature, true } };
    BOOST_REQUIRE(!script::is_sign_key_hash_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_sign_key_hash_pattern__invalid_key__false)
{
    const operations ops
    {
        { pattern_signature, true },
        { pattern_hash20, true }
    };

    BOOST_REQUIRE(!script::is_sign_key_hash_pattern(ops));
}

// is_sign_multisig_pattern

BOOST_AUTO_TEST_CASE(script__is_sign_multisig_pattern__dummy_and_endorsement__true)
{
    const operations ops
    {
        { data_chunk{}, true },
        { pattern_signature, true }
    };

    BOOST_REQUIRE(script::is_sign_multisig_pattern(ops));
}

// The leading push must be the satoshi op_check_multisig dummy.
BOOST_AUTO_TEST_CASE(script__is_sign_multisig_pattern__missing_dummy__false)
{
    const operations ops
    {
        { pattern_signature, true },
        { pattern_signature, true }
    };

    BOOST_REQUIRE(!script::is_sign_multisig_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_sign_multisig_pattern__dummy_only__false)
{
    const operations ops{ { data_chunk{}, true } };
    BOOST_REQUIRE(!script::is_sign_multisig_pattern(ops));
}

// is_sign_script_hash_pattern

BOOST_AUTO_TEST_CASE(script__is_sign_script_hash_pattern__push_only__true)
{
    const operations ops
    {
        { pattern_signature, true },
        { pattern_hash20, true }
    };

    BOOST_REQUIRE(script::is_sign_script_hash_pattern(ops));
}

// The last push is the serialized script, so it cannot be empty.
BOOST_AUTO_TEST_CASE(script__is_sign_script_hash_pattern__empty_last_push__false)
{
    const operations ops
    {
        { pattern_signature, true },
        { data_chunk{}, true }
    };

    BOOST_REQUIRE(!script::is_sign_script_hash_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_sign_script_hash_pattern__not_push_only__false)
{
    const operations ops
    {
        { opcode::nop },
        { pattern_hash20, true }
    };

    BOOST_REQUIRE(!script::is_sign_script_hash_pattern(ops));
}

BOOST_AUTO_TEST_CASE(script__is_sign_script_hash_pattern__empty__false)
{
    BOOST_REQUIRE(!script::is_sign_script_hash_pattern(operations{}));
}

// is_coinbase_pattern
// -----------------------------------------------------------------------------
// BIP34 requires a minimally-encoded push of the block height.

BOOST_AUTO_TEST_CASE(script__is_coinbase_pattern__empty__false)
{
    BOOST_REQUIRE(!script::is_coinbase_pattern(operations{}, 1));
}

// Heights one through sixteen are minimally encoded as a single opcode.
BOOST_AUTO_TEST_CASE(script__is_coinbase_pattern__positive_opcode__true)
{
    const operations one{ { operation::opcode_from_positive(1_u8) } };
    const operations sixteen{ { operation::opcode_from_positive(16_u8) } };
    BOOST_REQUIRE(script::is_coinbase_pattern(one, 1));
    BOOST_REQUIRE(script::is_coinbase_pattern(sixteen, 16));
}

BOOST_AUTO_TEST_CASE(script__is_coinbase_pattern__wrong_height__false)
{
    const operations ops{ { operation::opcode_from_positive(1_u8) } };
    BOOST_REQUIRE(!script::is_coinbase_pattern(ops, 2));
}

// A data push of a value in [1..16] is not the minimal encoding.
BOOST_AUTO_TEST_CASE(script__is_coinbase_pattern__non_minimal_push__false)
{
    const operations ops{ { data_chunk{ 0x01 }, false } };
    BOOST_REQUIRE(!script::is_coinbase_pattern(ops, 1));
}

BOOST_AUTO_TEST_CASE(script__is_coinbase_pattern__one_byte_height__true)
{
    const operations ops{ { data_chunk{ 0x11 }, true } };
    BOOST_REQUIRE(script::is_coinbase_pattern(ops, 17));
}

// The sign byte is required, so 255 is a two byte push.
BOOST_AUTO_TEST_CASE(script__is_coinbase_pattern__signed_height__true)
{
    const operations ops{ { data_chunk{ 0xff, 0x00 }, true } };
    BOOST_REQUIRE(script::is_coinbase_pattern(ops, 255));
}

// Only the first operation is the commitment, the remainder is arbitrary.
BOOST_AUTO_TEST_CASE(script__is_coinbase_pattern__trailing_operations__true)
{
    const operations ops
    {
        { operation::opcode_from_positive(1_u8) },
        { pattern_hash32, true },
        { opcode::checksig }
    };

    BOOST_REQUIRE(script::is_coinbase_pattern(ops, 1));
}

// output_pattern
// -----------------------------------------------------------------------------

static const std::string script_pay_script_hash =
    "hash160 [0000000000000000000000000000000000000000] equal";
static const std::string script_pay_public_key =
    "[02abababababababababababababababababababababababababababababababab] checksig";

BOOST_AUTO_TEST_CASE(script__output_pattern__pay_script_hash__expected)
{
    const script instance(script_pay_script_hash);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.output_pattern() == chain::script_pattern::pay_script_hash);
}

BOOST_AUTO_TEST_CASE(script__output_pattern__pay_public_key__expected)
{
    const script instance(script_pay_public_key);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.output_pattern() == chain::script_pattern::pay_public_key);
}

// input_pattern
// -----------------------------------------------------------------------------

static const std::string endorsement = "[3044022001111111111111111111111111111111111111111111111111111111111111110220022222222222222222222222222222222222222222222222222222222222222201]";

BOOST_AUTO_TEST_CASE(script__input_pattern__sign_key_hash__expected)
{
    const std::string text = endorsement + " [02abababababababababababababababababababababababababababababababab]";
    const script instance(text);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.input_pattern() == chain::script_pattern::sign_key_hash);
}

BOOST_AUTO_TEST_CASE(script__input_pattern__push_only__sign_script_hash)
{
    const script instance("[0102] [0304]");
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.input_pattern() == chain::script_pattern::sign_script_hash);
}

// sigop counting
// -----------------------------------------------------------------------------

// Accurate counting reads the key count from the preceding positive opcode.
BOOST_AUTO_TEST_CASE(script__signature_operations__multisig_accurate__key_count)
{
    const script instance(script_1_of_3_multisig);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.signature_operations(true), 3u);
}

BOOST_AUTO_TEST_CASE(script__signature_operations__multisig_inaccurate__default)
{
    const script instance(script_1_of_3_multisig);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.signature_operations(false), multisig_default_sigops);
}

// The general sign_script_hash pattern subsumes these, so ordering decides.

BOOST_AUTO_TEST_CASE(script__input_pattern__lone_endorsement__sign_public_key)
{
    const script instance(endorsement);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.input_pattern() == chain::script_pattern::sign_public_key);
}

BOOST_AUTO_TEST_CASE(script__input_pattern__empty_push_and_endorsements__sign_multisig)
{
    const std::string text = "0 " + endorsement + " " + endorsement;
    const script instance(text);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.input_pattern() == chain::script_pattern::sign_multisig);
}

BOOST_AUTO_TEST_SUITE_END()
