/**
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <boost/iostreams/stream.hpp>
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

data_chunk valid_raw_operation = to_chunk(base16_literal(
    "0900ff11ee22bb33aa44"));

BOOST_AUTO_TEST_SUITE(operation_tests)

BOOST_AUTO_TEST_CASE(operation__constructor_1__always__returns_default_initialized)
{
    chain::operation instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(operation__constructor_2__valid_input__returns_input_initialized)
{
    const auto code = chain::opcode::reserved;
    const auto data = to_chunk(base16_literal(
        "000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"));

    chain::operation instance(code, data);
    BOOST_REQUIRE_EQUAL(true, instance.is_valid());
    BOOST_REQUIRE(code == instance.code());
    BOOST_REQUIRE(data == instance.data());
}

BOOST_AUTO_TEST_CASE(operation__constructor_3__valid_input__returns_input_initialized)
{
    const auto code = chain::opcode::reserved;
    const auto data = to_chunk(base16_literal(
        "000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"));

    auto dup_data = data;
    chain::operation instance(code, std::move(dup_data));
    BOOST_REQUIRE_EQUAL(true, instance.is_valid());
    BOOST_REQUIRE(code == instance.code());
    BOOST_REQUIRE(data == instance.data());
}

BOOST_AUTO_TEST_CASE(operation__constructor_4__valid_input__returns_input_initialized)
{
    const chain::operation expected(chain::opcode::reserved,
        to_chunk(base16_literal("23156214")));

    chain::operation instance(expected);
    BOOST_REQUIRE_EQUAL(true, instance.is_valid());
    BOOST_REQUIRE(expected == instance);
}

BOOST_AUTO_TEST_CASE(operation__constructor_5__valid_input__returns_input_initialized)
{
    chain::operation expected(chain::opcode::reserved,
        to_chunk(base16_literal("23156214")));

    chain::operation instance(std::move(expected));
    BOOST_REQUIRE_EQUAL(true, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(operation__from_data__insufficient_bytes__failure)
{
    data_chunk data(0);
    chain::operation instance;

    BOOST_REQUIRE_EQUAL(false, instance.from_data(data));
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(operation__from_data__roundtrip_reserved__success)
{
    const auto code = chain::opcode::reserved;
    const auto data = to_chunk(base16_literal(""));
    const auto raw_operation = to_chunk(base16_literal("50"));
    chain::operation instance;

    BOOST_REQUIRE_EQUAL(true, instance.from_data(raw_operation));
    BOOST_REQUIRE_EQUAL(true, instance.is_valid());
    BOOST_REQUIRE(raw_operation == instance.to_data());
    chain::operation duplicate;
    BOOST_REQUIRE_EQUAL(true, duplicate.from_data(instance.to_data()));
    BOOST_REQUIRE(instance == duplicate);
    BOOST_REQUIRE(code == instance.code());
    BOOST_REQUIRE(data == instance.data());
}

BOOST_AUTO_TEST_CASE(operation__from_data__roundtrip_pushdata1__success)
{
    const auto code = chain::opcode::pushdata1;
    const auto data = to_chunk(base16_literal("0102030405"));
    const auto raw_operation = to_chunk(base16_literal("4c050102030405"));
    chain::operation instance;

    BOOST_REQUIRE_EQUAL(true, instance.from_data(raw_operation));
    BOOST_REQUIRE_EQUAL(true, instance.is_valid());
    BOOST_REQUIRE(raw_operation == instance.to_data());
    chain::operation duplicate;
    BOOST_REQUIRE_EQUAL(true, duplicate.from_data(instance.to_data()));
    BOOST_REQUIRE(instance == duplicate);
    BOOST_REQUIRE(code == instance.code());
    BOOST_REQUIRE(data == instance.data());
}

BOOST_AUTO_TEST_CASE(operation__from_data__roundtrip_pushdata2__success)
{
    const auto code = chain::opcode::pushdata2;
    const auto data = to_chunk(base16_literal(
        "000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"));

    chain::operation initial{ code, data };

    BOOST_REQUIRE(initial.is_valid());
    BOOST_REQUIRE(code == initial.code());
    BOOST_REQUIRE(data == initial.data());

    chain::operation operation;

    BOOST_REQUIRE(operation != initial);
    BOOST_REQUIRE(operation.from_data(initial.to_data()));
    BOOST_REQUIRE(operation.is_valid());
    BOOST_REQUIRE(operation == initial);
}

BOOST_AUTO_TEST_CASE(operation__from_data__roundtrip_pushdata4__success)
{
    const auto code = chain::opcode::pushdata4;
    const auto data = to_chunk(base16_literal("0102030405"));
    const auto raw_operation = to_chunk(base16_literal("4e050000000102030405"));
    chain::operation instance;

    BOOST_REQUIRE_EQUAL(true, instance.from_data(raw_operation));
    BOOST_REQUIRE_EQUAL(true, instance.is_valid());
    BOOST_REQUIRE(raw_operation == instance.to_data());
    chain::operation duplicate;
    BOOST_REQUIRE_EQUAL(true, duplicate.from_data(instance.to_data()));
    BOOST_REQUIRE(instance == duplicate);
    BOOST_REQUIRE(code == instance.code());
    BOOST_REQUIRE(data == instance.data());
}

BOOST_AUTO_TEST_CASE(operation__factory_from_data_1__roundtrip__success)
{
    auto operation = chain::operation::factory_from_data(valid_raw_operation);

    BOOST_REQUIRE(operation.is_valid());
    data_chunk output = operation.to_data();
    BOOST_REQUIRE(output == valid_raw_operation);
}

BOOST_AUTO_TEST_CASE(operation__factory_from_data_2__roundtrip__success)
{
    data_source istream(valid_raw_operation);
    auto operation = chain::operation::factory_from_data(istream);

    BOOST_REQUIRE(operation.is_valid());
    data_chunk output = operation.to_data();
    BOOST_REQUIRE(output == valid_raw_operation);
}

BOOST_AUTO_TEST_CASE(operation__factory_from_data_3__roundtrip__success)
{
    data_source istream(valid_raw_operation);
    istream_reader source(istream);
    auto operation = chain::operation::factory_from_data(source);

    BOOST_REQUIRE(operation.is_valid());
    data_chunk output = operation.to_data();
    BOOST_REQUIRE(output == valid_raw_operation);
}

BOOST_AUTO_TEST_CASE(operation__code__roundtrip__success)
{
    const auto value = chain::opcode::reserved;
    chain::operation instance;
    BOOST_REQUIRE_EQUAL(false, value == instance.code());
    instance.set_code(value);
    BOOST_REQUIRE_EQUAL(true, value == instance.code());
}

BOOST_AUTO_TEST_CASE(operation__data_setter_1__roundtrip__success)
{
     const data_chunk value = to_chunk(base16_literal(
         "4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"));

    chain::operation instance;
    BOOST_REQUIRE(value != instance.data());
    instance.set_data(value);
    BOOST_REQUIRE(value == instance.data());
}

BOOST_AUTO_TEST_CASE(operation__data_setter_2__roundtrip__success)
{
    const data_chunk value = to_chunk(base16_literal(
        "4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"));

    data_chunk dup_value = value;
    chain::operation instance;
    BOOST_REQUIRE(value != instance.data());
    instance.set_data(std::move(dup_value));
    BOOST_REQUIRE(value == instance.data());
}

//BOOST_AUTO_TEST_CASE(operation__is_pay_multisig_pattern__checkmultisig)

BOOST_AUTO_TEST_CASE(operation__operator_assign_equals_1__always__matches_equivalent)
{
    chain::operation expected;
    BOOST_REQUIRE(expected.from_data(valid_raw_operation));
    chain::operation instance;
    chain::operation value;
    BOOST_REQUIRE(value.from_data(valid_raw_operation));
    instance = std::move(value);
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(operation__operator_assign_equals_2__always__matches_equivalent)
{
    chain::operation expected;
    BOOST_REQUIRE(expected.from_data(valid_raw_operation));
    chain::operation instance;
    instance = expected;
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(operation__operator_boolean_equals__duplicates__returns_true)
{
    chain::operation alpha;
    chain::operation beta;
    BOOST_REQUIRE(alpha.from_data(valid_raw_operation));
    BOOST_REQUIRE(beta.from_data(valid_raw_operation));
    BOOST_REQUIRE(alpha == beta);
}

BOOST_AUTO_TEST_CASE(operation__operator_boolean_equals__differs__returns_false)
{
    chain::operation alpha;
    chain::operation beta;
    BOOST_REQUIRE(alpha.from_data(valid_raw_operation));
    BOOST_REQUIRE_EQUAL(false, alpha == beta);
}

BOOST_AUTO_TEST_CASE(operation__operator_boolean_not_equals__duplicates__returns_false)
{
    chain::operation alpha;
    chain::operation beta;
    BOOST_REQUIRE(alpha.from_data(valid_raw_operation));
    BOOST_REQUIRE(beta.from_data(valid_raw_operation));
    BOOST_REQUIRE_EQUAL(false, alpha != beta);
}

BOOST_AUTO_TEST_CASE(operation__operator_boolean_not_equals__differs__returns_true)
{
    chain::operation alpha;
    chain::operation beta;
    BOOST_REQUIRE(alpha.from_data(valid_raw_operation));
    BOOST_REQUIRE(alpha != beta);
}

BOOST_AUTO_TEST_SUITE_END()
