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

BOOST_AUTO_TEST_SUITE(operation_tests)

namespace json = boost::json;
using namespace system::chain;

static const auto op_data = base16_chunk("0900ff11ee22bb33aa44");
static const auto expected_op = operation(op_data);
static const auto op_data_underflow = base16_chunk("0900ff11ee22bb33");
static const auto push_data_32 = base16_chunk(
    "000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");

// constructors
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(operation__constructor__default__one_invalid)
{
    const operation instance;
    BOOST_REQUIRE_EQUAL(instance.data().size(), one);
    BOOST_REQUIRE(instance.is_invalid());
}

BOOST_AUTO_TEST_CASE(operation__constructor__move__expected)
{
    const operation expected(push_data_32, false);
    auto copy = expected;
    const operation instance(std::move(copy));
    BOOST_REQUIRE(expected == instance);
}

BOOST_AUTO_TEST_CASE(operation__constructor__copy__expected)
{
    const operation expected(push_data_32, false);
    const operation instance(expected);
    BOOST_REQUIRE(expected == instance);
}

BOOST_AUTO_TEST_CASE(operation__constructor__opcode_checklocktimeverify__empty_valid)
{
    const operation instance(opcode::checklocktimeverify);
    BOOST_REQUIRE(instance.data().empty());
    BOOST_REQUIRE(!instance.is_invalid());
}

BOOST_AUTO_TEST_CASE(operation__constructor__opcode_xor__empty_invalid)
{
    const operation instance(opcode::op_xor);
    BOOST_REQUIRE(instance.data().empty());
    BOOST_REQUIRE(instance.is_invalid());
}

BOOST_AUTO_TEST_CASE(operation__constructor__opcode_push_two_size___empty)
{
    const operation instance(opcode::push_two_size);

    // If code is push data the data member will be inconsistent (empty).
    BOOST_REQUIRE(instance.data().empty());
    BOOST_REQUIRE(!instance.is_invalid());
}

BOOST_AUTO_TEST_CASE(operation__constructor__push_data_move__expected)
{
    auto copy = push_data_32;
    const operation instance(std::move(copy), false);
    BOOST_REQUIRE(instance.code() == opcode::push_size_32);
    BOOST_REQUIRE_EQUAL(instance.data(), push_data_32);
}

BOOST_AUTO_TEST_CASE(operation__constructor__push_data_move_minimal__expected)
{
    const operation instance(data_chunk{ 0x01 }, true);

    // minimal: push_positive_1 <= 0x51 (numbers::positive_1) on the stack.
    BOOST_REQUIRE(instance.code() == opcode::push_positive_1);
    BOOST_REQUIRE(instance.data().empty());
}

BOOST_AUTO_TEST_CASE(operation__constructor__push_data_move_non_minimal__expected)
{
    const data_chunk expected{ 0x51 };
    auto copy = expected;
    const operation instance(data_chunk{ 0x51 }, false);

    // non-minimal: [1.01] <= 0x51 on the stack.
    BOOST_REQUIRE(instance.code() == opcode::push_size_1);
    BOOST_REQUIRE_EQUAL(instance.data(), expected);
}

BOOST_AUTO_TEST_CASE(operation__constructor__push_data_copy__expected)
{
    const operation instance(push_data_32, false);
    BOOST_REQUIRE(instance.code() == opcode::push_size_32);
    BOOST_REQUIRE_EQUAL(instance.data(), push_data_32);
}

BOOST_AUTO_TEST_CASE(operation__constructor__push_data_copy_minimal__expected)
{
    const data_chunk expected{ 0x01 };
    const operation instance(expected, true);

    // minimal: push_positive_1 <= 0x51 (numbers::positive_1) on the stack.
    BOOST_REQUIRE(instance.code() == opcode::push_positive_1);
    BOOST_REQUIRE(instance.data().empty());
}

BOOST_AUTO_TEST_CASE(operation__constructor__push_data_copy_non_minimal__expected)
{
    const data_chunk expected{ 0x51 };
    const operation instance(expected, false);

    // non-minimal: [1.51] <= 0x51 on the stack.
    BOOST_REQUIRE(instance.code() == opcode::push_size_1);
    BOOST_REQUIRE_EQUAL(instance.data(), expected);
}

BOOST_AUTO_TEST_CASE(operation__constructor__op_data__expected)
{
    const operation instance(op_data);
    BOOST_REQUIRE_EQUAL(instance.to_data(), op_data);
}

BOOST_AUTO_TEST_CASE(operation__constructor__fast__expected)
{
    stream::in::fast istream(op_data);
    const operation instance(istream);
    BOOST_REQUIRE_EQUAL(instance.to_data(), op_data);
}

BOOST_AUTO_TEST_CASE(operation__constructor__stream__expected)
{
    stream::in::copy istream(op_data);
    const operation instance(istream);
    BOOST_REQUIRE_EQUAL(instance.to_data(), op_data);
}

BOOST_AUTO_TEST_CASE(operation__constructor__reader__expected)
{
    read::bytes::copy source(op_data);
    const operation instance(source);
    BOOST_REQUIRE_EQUAL(instance.to_data(), op_data);
}

// more constructor

BOOST_AUTO_TEST_CASE(operation__constructor__empty__invalid_not_underflow)
{
    operation instance;
    BOOST_REQUIRE(instance.is_invalid());
    BOOST_REQUIRE(!instance.is_underflow());
}

BOOST_AUTO_TEST_CASE(operation__constructor__push_size_0__expected)
{
    const auto data0 = base16_chunk("");
    const auto data = base16_chunk("00");
    operation instance(data);

    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.to_data(), data);

    operation duplicate(instance.to_data());
    BOOST_REQUIRE(duplicate.is_valid());
    BOOST_REQUIRE(instance == duplicate);

    BOOST_REQUIRE(instance.code() == opcode::push_size_0);
    BOOST_REQUIRE_EQUAL(instance.data(), data0);
}

BOOST_AUTO_TEST_CASE(operation__constructor__push_size_75__expected)
{
    const auto data75 = data_chunk(75, '.');
    const auto data = splice(base16_array("4b"), data75);
    operation instance(data);

    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.to_data(), data);

    operation duplicate(instance.to_data());
    BOOST_REQUIRE(duplicate.is_valid());
    BOOST_REQUIRE(instance == duplicate);

    BOOST_REQUIRE(instance.code() == opcode::push_size_75);
    BOOST_REQUIRE(instance.data() == data75);
}

BOOST_AUTO_TEST_CASE(operation__constructor__push_negative_1__expected)
{
    const auto op_79 = static_cast<uint8_t>(opcode::push_negative_1);
    const auto data = data_chunk{ op_79 };
    operation instance(data);

    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.to_data(), data);

    operation duplicate(instance.to_data());
    BOOST_REQUIRE(duplicate.is_valid());
    BOOST_REQUIRE(instance == duplicate);

    // The code is the data for numeric push codes.
    BOOST_REQUIRE(instance.code() == opcode::push_negative_1);
    BOOST_REQUIRE(instance.data().empty());
}

BOOST_AUTO_TEST_CASE(operation__constructor__push_positive_1__expected)
{
    const auto op_81 = static_cast<uint8_t>(opcode::push_positive_1);
    const auto data = data_chunk{ op_81 };
    operation instance(data);

    // This is read as an encoded operation, not as data.
    // Constructors read (unencoded) data and can select minimal encoding.
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.to_data(), data);

    operation duplicate(instance.to_data());
    BOOST_REQUIRE(duplicate.is_valid());
    BOOST_REQUIRE(instance == duplicate);

    // The code is the data for numeric push codes.
    BOOST_REQUIRE(instance.code() == opcode::push_positive_1);
    BOOST_REQUIRE(instance.data().empty());
}

BOOST_AUTO_TEST_CASE(operation__constructor__push_positive_16__expected)
{
    const auto op_96 = static_cast<uint8_t>(opcode::push_positive_16);
    const auto data = data_chunk{ op_96 };
    operation instance(data);

    // This is read as an encoded operation, not as data.
    // Constructors read (unencoded) data and can select minimal encoding.
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.to_data(), data);

    operation duplicate(instance.to_data());
    BOOST_REQUIRE(duplicate.is_valid());
    BOOST_REQUIRE(instance == duplicate);

    // The code is the data for numeric push codes.
    BOOST_REQUIRE(instance.code() == opcode::push_positive_16);
    BOOST_REQUIRE(instance.data().empty());
}

BOOST_AUTO_TEST_CASE(operation__constructor__push_one_size__expected)
{
    const auto data255 = data_chunk(255, '.');
    const auto data = splice(base16_array("4c" "ff"), data255);
    operation instance(data);

    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.to_data(), data);

    operation duplicate(instance.to_data());
    BOOST_REQUIRE(duplicate.is_valid());
    BOOST_REQUIRE(instance == duplicate);

    BOOST_REQUIRE(instance.code() == opcode::push_one_size);
    BOOST_REQUIRE_EQUAL(instance.data(), data255);
}

BOOST_AUTO_TEST_CASE(operation__constructor__push_two_size__expected)
{
    const auto data520 = data_chunk(520, '.');
    const auto data = splice(base16_array("4d" "0802"), data520);
    operation instance(data);

    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.to_data(), data);

    operation duplicate(instance.to_data());
    BOOST_REQUIRE(duplicate.is_valid());
    BOOST_REQUIRE(instance == duplicate);

    BOOST_REQUIRE(instance.code() == opcode::push_two_size);
    BOOST_REQUIRE_EQUAL(instance.data(), data520);
}

BOOST_AUTO_TEST_CASE(operation__constructor__push_four_size__expected)
{
    const auto data520 = data_chunk(520, '.');
    const auto data = splice(base16_array("4e" "08020000"), data520);
    operation instance(data);

    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.to_data(), data);

    operation duplicate(instance.to_data());
    BOOST_REQUIRE(duplicate.is_valid());
    BOOST_REQUIRE(instance == duplicate);

    BOOST_REQUIRE(instance.code() == opcode::push_four_size);
    BOOST_REQUIRE_EQUAL(instance.data(), data520);
}

BOOST_AUTO_TEST_CASE(operation__constructor__data__valid)
{
    operation instance(op_data);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance == expected_op);
}

BOOST_AUTO_TEST_CASE(operation__constructor__stream__valid)
{
    stream::in::copy stream(op_data);
    operation instance(stream);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance == expected_op);
}

BOOST_AUTO_TEST_CASE(operation__constructor__reader__valid)
{
    read::bytes::copy source(op_data);
    operation instance(source);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance == expected_op);
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__negative_1__push_negative_1_empty)
{
    operation value(std::string_view("-1"));
    BOOST_REQUIRE(value.code() == opcode::push_negative_1);
    BOOST_REQUIRE(value.data().empty());
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__0__push_size_0_empty)
{
    operation value(std::string_view("0"));
    BOOST_REQUIRE(value.code() == opcode::push_size_0);
    BOOST_REQUIRE(value.data().empty());
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__1__push_positive_1_empty)
{
    operation value(std::string_view("1"));
    BOOST_REQUIRE(value.code() == opcode::push_positive_1);
    BOOST_REQUIRE(value.data().empty());
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__16__push_positive_16_empty)
{
    operation value(std::string_view("16"));
    BOOST_REQUIRE(value.code() == opcode::push_positive_16);
    BOOST_REQUIRE(value.data().empty());
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__17__push_size_1_expected)
{
    const data_chunk expected{ 0x11 };
    operation value(std::string_view("17"));
    BOOST_REQUIRE(value.code() == opcode::push_size_1);
    BOOST_REQUIRE_EQUAL(value.data(), expected);
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__negative_2__push_size_1_expected)
{
    const data_chunk expected{ 0x82 };
    operation value(std::string_view("-2"));
    BOOST_REQUIRE(value.code() == opcode::push_size_1);
    BOOST_REQUIRE_EQUAL(value.data(), expected);
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__9223372036854775807__push_size_8_expected)
{
    const data_chunk expected{ { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f } };
    operation value(std::string_view("9223372036854775807"));
    BOOST_REQUIRE(value.code() == opcode::push_size_8);
    BOOST_REQUIRE_EQUAL(value.data(), expected);
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__negative_9223372036854775807__push_size_8_expected)
{
    const data_chunk expected{ { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } };
    operation value(std::string_view("-9223372036854775807"));
    BOOST_REQUIRE(value.code() == opcode::push_size_8);
    BOOST_REQUIRE_EQUAL(value.data(), expected);
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__string_empty__push_size_0_empty)
{
    const data_chunk expected{ 0x61 };
    operation value(std::string_view("''"));
    BOOST_REQUIRE(value.code() == opcode::push_size_0);
    BOOST_REQUIRE(value.data().empty());
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__string_a__push_size_1_expected_byte)
{
    const data_chunk expected{ 0x61 };
    operation value(std::string_view("'a'"));
    BOOST_REQUIRE(value.code() == opcode::push_size_1);
    BOOST_REQUIRE_EQUAL(value.data(), expected);
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__string_abc__push_size_3_expected_byte_order)
{
    const data_chunk expected{ { 0x61, 0x62, 0x63 } };
    operation value(std::string_view("'abc'"));
    BOOST_REQUIRE(value.code() == opcode::push_size_3);
    BOOST_REQUIRE_EQUAL(value.data(), expected);
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__negative_1_character__push_size_1_nominal_encoding)
{
    const data_chunk expected{ 0x4f };
    operation value(std::string_view("'O'"));
    BOOST_REQUIRE(value.code() == opcode::push_size_1);
    BOOST_REQUIRE_EQUAL(value.data(), expected);
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__empty_underflow__expected)
{
    operation value(std::string_view("<>"));
    BOOST_REQUIRE(value.is_invalid());
    BOOST_REQUIRE(value.is_underflow());
    BOOST_REQUIRE(value.data().empty());
    BOOST_REQUIRE(value.to_data().empty());
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__non_empty_underflow__expected)
{
    const data_chunk expected{ { 0x61, 0x62, 0x63 } };
    operation value(std::string_view("<616263>"));
    BOOST_REQUIRE(value.is_invalid());
    BOOST_REQUIRE(value.is_underflow());
    BOOST_REQUIRE_EQUAL(value.data(), expected);
    BOOST_REQUIRE_EQUAL(value.to_data(), expected);
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__push_0__push_size_0)
{
    operation value(std::string_view("push_0"));
    BOOST_REQUIRE(value.code() == opcode::push_size_0);
    BOOST_REQUIRE(value.data().empty());
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__push_1__empty)
{
    operation value(std::string_view("push_1"));
    BOOST_REQUIRE(value.code() == opcode::push_size_1);
    BOOST_REQUIRE(value.data().empty());
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__push_75__empty)
{
    operation value(std::string_view("push_75"));
    BOOST_REQUIRE(value.code() == opcode::push_size_75);
    BOOST_REQUIRE(value.data().empty());
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__push_one__push_one_size_empty)
{
    operation value(std::string_view("push_one"));
    BOOST_REQUIRE(value.code() == opcode::push_one_size);
    BOOST_REQUIRE(value.data().empty());
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__push_two__push_two_size_empty)
{
    operation value(std::string_view("push_two"));
    BOOST_REQUIRE(value.code() == opcode::push_two_size);
    BOOST_REQUIRE(value.data().empty());
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__push_four__push_four_size_empty)
{
    operation value(std::string_view("push_four"));
    BOOST_REQUIRE(value.code() == opcode::push_four_size);
    BOOST_REQUIRE(value.data().empty());
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__7__push_positive_7)
{
    operation value(std::string_view("7"));
    BOOST_REQUIRE(value.code() == opcode::push_positive_7);
    BOOST_REQUIRE(value.data().empty());
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__empty__push_size_0)
{
    operation value(std::string_view("[]"));
    BOOST_REQUIRE(value.code() == opcode::push_size_0);
    BOOST_REQUIRE(value.data().empty());
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__0x07__push_size_1)
{
    const data_chunk expected{ 0x07 };
    operation value(std::string_view("[07]"));
    BOOST_REQUIRE(value.code() == opcode::push_size_1);
    BOOST_REQUIRE_EQUAL(value.data(), expected);
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__0x42__push_size_1)
{
    const data_chunk expected{ 0x42 };
    operation value(std::string_view("[42]"));
    BOOST_REQUIRE(value.code() == opcode::push_size_1);
    BOOST_REQUIRE_EQUAL(value.data(), expected);
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__0x112233__push_size_3)
{
    const data_chunk expected{ { 0x11, 0x22, 0x33 } };
    operation value(std::string_view("[112233]"));
    BOOST_REQUIRE(value.code() == opcode::push_size_3);
    BOOST_REQUIRE_EQUAL(value.data(), expected);
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__0_0x112233__push_size_3)
{
    const data_chunk expected{ { 0x11, 0x22, 0x33 } };
    operation value(std::string_view("[0.112233]"));
    BOOST_REQUIRE(value.code() == opcode::push_size_3);
    BOOST_REQUIRE_EQUAL(value.data(), expected);
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__1_0x112233__push_one_size)
{
    const data_chunk expected{ { 0x11, 0x22, 0x33 } };
    operation value(std::string_view("[1.112233]"));
    BOOST_REQUIRE(value.code() == opcode::push_one_size);
    BOOST_REQUIRE_EQUAL(value.data(), expected);
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__2_0x112233__push_two_size)
{
    const data_chunk expected{ { 0x11, 0x22, 0x33 } };
    operation value(std::string_view("[2.112233]"));
    BOOST_REQUIRE(value.code() == opcode::push_two_size);
    BOOST_REQUIRE_EQUAL(value.data(), expected);
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__4_0x112233__push_four_size)
{
    const data_chunk expected{ { 0x11, 0x22, 0x33 } };
    operation value(std::string_view("[4.112233]"));
    BOOST_REQUIRE(value.code() == opcode::push_four_size);
    BOOST_REQUIRE_EQUAL(value.data(), expected);
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__5_0x112233__false)
{
    operation value(std::string_view("[5.112233]"));
    BOOST_REQUIRE(!value.is_valid());;
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__empty_0x112233__false)
{
    operation value(std::string_view("[.112233]"));
    BOOST_REQUIRE(!value.is_valid());
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__nop2__nop2_checklocktimeverify)
{
    operation value(std::string_view("nop2"));
    BOOST_REQUIRE(value.code() == opcode::nop2);
    BOOST_REQUIRE(value.code() == opcode::checklocktimeverify);
    BOOST_REQUIRE(value.data().empty());
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__checklocktimeverify__nop2_checklocktimeverify)
{
    operation value(std::string_view("checklocktimeverify"));
    BOOST_REQUIRE(value.code() == opcode::nop2);
    BOOST_REQUIRE(value.code() == opcode::checklocktimeverify);
    BOOST_REQUIRE(value.data().empty());
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__nop3__nop3_checksequenceverify)
{
    operation value(std::string_view("nop3"));
    BOOST_REQUIRE(value.code() == opcode::nop3);
    BOOST_REQUIRE(value.code() == opcode::checksequenceverify);
    BOOST_REQUIRE(value.data().empty());
}

BOOST_AUTO_TEST_CASE(operation__constructor_string__checklocktimeverify__nop3_checksequenceverify)
{
    operation value(std::string_view("checksequenceverify"));
    BOOST_REQUIRE(value.code() == opcode::nop3);
    BOOST_REQUIRE(value.code() == opcode::checksequenceverify);
    BOOST_REQUIRE(value.data().empty());
}

// operators
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(operation__assign__move__expected)
{
    const operation alpha(op_data);
    const auto beta = operation(op_data);
    BOOST_REQUIRE(beta == alpha);
}

BOOST_AUTO_TEST_CASE(operation__assign__copy__expected)
{
    const operation alpha(op_data);
    const auto beta = alpha;
    BOOST_REQUIRE(beta == alpha);
}

BOOST_AUTO_TEST_CASE(operation__equality__same__expected)
{
    const operation alpha(op_data);
    const operation beta(op_data);
    BOOST_REQUIRE(alpha == beta);
}

BOOST_AUTO_TEST_CASE(operation__equality__different__expected)
{
    const operation alpha;
    const operation beta(op_data);
    BOOST_REQUIRE(!(alpha == beta));
}

BOOST_AUTO_TEST_CASE(operation__inequality__same__expected)
{
    const operation alpha(op_data);
    const operation beta(op_data);
    BOOST_REQUIRE(!(alpha != beta));
}

BOOST_AUTO_TEST_CASE(operation__inequality__different__expected)
{
    const operation alpha;
    const operation beta(op_data);
    BOOST_REQUIRE(alpha != beta);
}

// to_data
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(operation__to_data__empty__underflow)
{
    const operation instance(data_chunk{});
    BOOST_REQUIRE(instance.is_invalid());
    BOOST_REQUIRE(instance.is_underflow());
    BOOST_REQUIRE(instance.data().empty());
    BOOST_REQUIRE(instance.to_data().empty());
}

BOOST_AUTO_TEST_CASE(operation__to_data__data__expected)
{
    const auto size = expected_op.to_data().size();
    BOOST_REQUIRE_EQUAL(size, expected_op.serialized_size());
}

BOOST_AUTO_TEST_CASE(operation__to_data__stream__expected)
{
    // Write operation to stream.
    std::stringstream iostream;
    expected_op.to_data(iostream);
    BOOST_REQUIRE(iostream);

    // Verify stream contents.
    const operation copy(iostream);
    BOOST_REQUIRE(iostream);
    BOOST_REQUIRE(!copy.is_invalid());
    BOOST_REQUIRE(!copy.is_underflow());
    BOOST_REQUIRE(copy == expected_op);
}

BOOST_AUTO_TEST_CASE(operation__to_data__writer__expected)
{
    // Write operation to stream.
    std::stringstream iostream;
    write::bytes::ostream out(iostream);
    expected_op.to_data(out);
    BOOST_REQUIRE(iostream);

    // Verify stream contents.
    const operation copy(iostream);
    BOOST_REQUIRE(iostream);
    BOOST_REQUIRE(!copy.is_invalid());
    BOOST_REQUIRE(!copy.is_underflow());
    BOOST_REQUIRE(copy == expected_op);
}

// to_string
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(operation__to_string__push_size_0__zero)
{
    const operation value(opcode::push_size_0);
    BOOST_REQUIRE_EQUAL(value.to_string(0), "zero");
}

BOOST_AUTO_TEST_CASE(operation__to_string__push_size_75__push_75)
{
    // Empty data allows the push code to serialize as an op code.
    const operation value(opcode::push_size_75);
    BOOST_REQUIRE_EQUAL(value.to_string(0), "push_75");
}

BOOST_AUTO_TEST_CASE(operation__to_string__push_positive_7__7)
{
    const operation value(opcode::push_positive_7);
    BOOST_REQUIRE_EQUAL(value.to_string(0), "7");
}

BOOST_AUTO_TEST_CASE(operation__to_string_minimal__0x07__7)
{
    const operation value(data_chunk{ 0x07 }, true);
    BOOST_REQUIRE_EQUAL(value.to_string(0), "7");
}

BOOST_AUTO_TEST_CASE(operation__to_string_nominal__empty__zero)
{
    const operation value(data_chunk{}, false);
    BOOST_REQUIRE_EQUAL(value.to_string(0), "zero");
}

BOOST_AUTO_TEST_CASE(operation__to_string_nominal__0x07__0x07)
{
    const operation value(data_chunk{ 0x07 }, false);
    BOOST_REQUIRE_EQUAL(value.to_string(0), "[07]");
}

BOOST_AUTO_TEST_CASE(operation__to_string__0x42__0x42)
{
    const operation value(data_chunk{ 0x42 }, true);
    BOOST_REQUIRE_EQUAL(value.to_string(0), "[42]");
}

BOOST_AUTO_TEST_CASE(operation__to_string__0x112233__0x112233)
{
    const operation value(data_chunk{ 0x11, 0x22, 0x33 }, true);
    BOOST_REQUIRE_EQUAL(value.to_string(0), "[112233]");
}

BOOST_AUTO_TEST_CASE(operation__to_string__push_size_3__0x112233__0x112233)
{
    const data_chunk encoded{ { 0x03, 0x11, 0x22, 0x33 } };
    operation value(encoded);
    BOOST_REQUIRE_EQUAL(value.to_string(0), "[112233]");
}

BOOST_AUTO_TEST_CASE(operation__to_string__push_one_size_0x112233__1_0x112233)
{
    const data_chunk encoded{ { 0x4c, 0x03, 0x11, 0x22, 0x33 } };
    operation value(encoded);
    BOOST_REQUIRE_EQUAL(value.to_string(0), "[1.112233]");
}

BOOST_AUTO_TEST_CASE(operation__to_string__push_two_size_0x112233__2_0x112233)
{
    const data_chunk encoded{ { 0x4d, 0x03, 0x00, 0x11, 0x22, 0x33 } };
    operation value(encoded);
    BOOST_REQUIRE_EQUAL(value.to_string(0), "[2.112233]");
}

BOOST_AUTO_TEST_CASE(operation__to_string__push_four_size_0x112233__4_0x112233)
{
    const data_chunk encoded{ { 0x4e, 0x03, 0x00, 0x00, 0x00, 0x11, 0x22, 0x33 } };
    operation value(encoded);
    BOOST_REQUIRE_EQUAL(value.to_string(0), "[4.112233]");
}

BOOST_AUTO_TEST_CASE(operation__to_string__empty_underflow__empty)
{
    const data_chunk encoded;
    operation value(encoded);
    BOOST_REQUIRE(value.is_invalid());
    BOOST_REQUIRE(value.is_underflow());
    BOOST_REQUIRE_EQUAL(value.to_string(0), "()");
}

// to_string (bitcoind)
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(operation__to_string_bitcoind__push_size_0__0)
{
    const operation value(opcode::push_size_0);
    BOOST_REQUIRE_EQUAL(value.to_string(0, true), "0");
}

BOOST_AUTO_TEST_CASE(operation__to_string_bitcoind__push_negative_1__negative_1)
{
    const operation value(opcode::push_negative_1);
    BOOST_REQUIRE_EQUAL(value.to_string(0, true), "-1");
}

BOOST_AUTO_TEST_CASE(operation__to_string_bitcoind__push_positive_7__7)
{
    const operation value(opcode::push_positive_7);
    BOOST_REQUIRE_EQUAL(value.to_string(0, true), "7");
}

BOOST_AUTO_TEST_CASE(operation__to_string_bitcoind__push_size_75_empty__0)
{
    const operation value(opcode::push_size_75);
    BOOST_REQUIRE_EQUAL(value.to_string(0, true), "0");
}

BOOST_AUTO_TEST_CASE(operation__to_string_bitcoind__0x42__66)
{
    const operation value(data_chunk{ 0x42 }, true);
    BOOST_REQUIRE_EQUAL(value.to_string(0, true), "66");
}

BOOST_AUTO_TEST_CASE(operation__to_string_bitcoind__0x81__negative_1)
{
    const operation value(data_chunk{ 0x81 }, true);
    BOOST_REQUIRE_EQUAL(value.to_string(0, true), "-1");
}

BOOST_AUTO_TEST_CASE(operation__to_string_bitcoind__0x112233__3351057)
{
    const operation value(data_chunk{ 0x11, 0x22, 0x33 }, true);
    BOOST_REQUIRE_EQUAL(value.to_string(0, true), "3351057");
}

BOOST_AUTO_TEST_CASE(operation__to_string_bitcoind__0x1122334455__hex)
{
    const operation value(data_chunk{ 0x11, 0x22, 0x33, 0x44, 0x55 }, true);
    BOOST_REQUIRE_EQUAL(value.to_string(0, true), "1122334455");
}

BOOST_AUTO_TEST_CASE(operation__to_string_bitcoind__push_one_size_0x1122334455__hex)
{
    const data_chunk encoded{ { 0x4c, 0x05, 0x11, 0x22, 0x33, 0x44, 0x55 } };
    const operation value(encoded);
    BOOST_REQUIRE_EQUAL(value.to_string(0, true), "1122334455");
}

BOOST_AUTO_TEST_CASE(operation__to_string_bitcoind__dup__op_dup)
{
    const operation value(opcode::dup);
    BOOST_REQUIRE_EQUAL(value.to_string(0, true), "OP_DUP");
}

BOOST_AUTO_TEST_CASE(operation__to_string_bitcoind__nonzero__op_0notequal)
{
    const operation value(opcode::nonzero);
    BOOST_REQUIRE_EQUAL(value.to_string(0, true), "OP_0NOTEQUAL");
}

BOOST_AUTO_TEST_CASE(operation__to_string_bitcoind__reserved_80__op_reserved)
{
    const operation value(opcode::reserved_80);
    BOOST_REQUIRE_EQUAL(value.to_string(0, true), "OP_RESERVED");
}

BOOST_AUTO_TEST_CASE(operation__to_string_bitcoind__reserved_137__op_reserved1)
{
    const operation value(opcode::reserved_137);
    BOOST_REQUIRE_EQUAL(value.to_string(0, true), "OP_RESERVED1");
}

BOOST_AUTO_TEST_CASE(operation__to_string_bitcoind__reserved_187__op_unknown)
{
    const operation value(opcode::reserved_187);
    BOOST_REQUIRE_EQUAL(value.to_string(0, true), "OP_UNKNOWN");
}

BOOST_AUTO_TEST_CASE(operation__to_string_bitcoind__reserved_255__op_invalidopcode)
{
    const operation value(opcode::reserved_255);
    BOOST_REQUIRE_EQUAL(value.to_string(0, true), "OP_INVALIDOPCODE");
}

BOOST_AUTO_TEST_CASE(operation__to_string_bitcoind__checklocktimeverify_inactive__op_nop2)
{
    const operation value(opcode::checklocktimeverify);
    BOOST_REQUIRE_EQUAL(value.to_string(chain::flags::no_rules, true), "OP_NOP2");
}

BOOST_AUTO_TEST_CASE(operation__to_string_bitcoind__checklocktimeverify_active__op_checklocktimeverify)
{
    const operation value(opcode::checklocktimeverify);
    BOOST_REQUIRE_EQUAL(value.to_string(chain::flags::bip65_rule, true), "OP_CHECKLOCKTIMEVERIFY");
}

BOOST_AUTO_TEST_CASE(operation__to_string_bitcoind__checksequenceverify_inactive__op_nop3)
{
    const operation value(opcode::checksequenceverify);
    BOOST_REQUIRE_EQUAL(value.to_string(chain::flags::no_rules, true), "OP_NOP3");
}

BOOST_AUTO_TEST_CASE(operation__to_string_bitcoind__checksequenceverify_active__op_checksequenceverify)
{
    const operation value(opcode::checksequenceverify);
    BOOST_REQUIRE_EQUAL(value.to_string(chain::flags::bip112_rule, true), "OP_CHECKSEQUENCEVERIFY");
}

BOOST_AUTO_TEST_CASE(operation__to_string_bitcoind__checksigadd_inactive__op_unknown)
{
    const operation value(opcode::checksigadd);
    BOOST_REQUIRE_EQUAL(value.to_string(chain::flags::no_rules, true), "OP_UNKNOWN");
}

BOOST_AUTO_TEST_CASE(operation__to_string_bitcoind__checksigadd_active__op_checksigadd)
{
    const operation value(opcode::checksigadd);
    BOOST_REQUIRE_EQUAL(value.to_string(chain::flags::bip342_rule, true), "OP_CHECKSIGADD");
}

BOOST_AUTO_TEST_CASE(operation__to_string_bitcoind__underflow__error)
{
    const data_chunk encoded{ { 0x4c, 0x03, 0x11 } };
    const operation value(encoded);
    BOOST_REQUIRE(value.is_underflow());
    BOOST_REQUIRE_EQUAL(value.to_string(0, true), "[error]");
}

BOOST_AUTO_TEST_CASE(operation__to_string__non_empty_underflow__empty)
{
    // Should be two bytes of push data.
    const data_chunk encoded{ { 0x02, 0xff } };
    operation value(encoded);
    BOOST_REQUIRE(value.is_invalid());
    BOOST_REQUIRE(value.is_underflow());
    BOOST_REQUIRE_EQUAL(value.to_string(0), "(02ff)");
}

BOOST_AUTO_TEST_CASE(operation__to_string__nop2_no_rules__nop2)
{
    const operation value(opcode::nop2);
    BOOST_REQUIRE_EQUAL(value.to_string(chain::flags::no_rules), "nop2");
}

BOOST_AUTO_TEST_CASE(operation__to_string__nop2_bip65_rule__checklocktimeverify)
{
    const operation value(opcode::nop2);
    BOOST_REQUIRE_EQUAL(value.to_string(chain::flags::bip65_rule), "checklocktimeverify");
}

BOOST_AUTO_TEST_CASE(operation__to_string__nop3_no_rules__nop3)
{
    const operation value(opcode::nop3);
    BOOST_REQUIRE_EQUAL(value.to_string(chain::flags::no_rules), "nop3");
}

BOOST_AUTO_TEST_CASE(operation__to_string__nop3_bip112_rule__checksequenceverify)
{
    const operation value(opcode::nop3);
    BOOST_REQUIRE_EQUAL(value.to_string(chain::flags::bip112_rule), "checksequenceverify");
}

// utilities (static)
// ----------------------------------------------------------------------------

////size_t serialized_size() const
////opcode opcode_from_size(size_t size)
////opcode minimal_opcode_from_data(const data_chunk& data)
////opcode nominal_opcode_from_data(const data_chunk& data)

// operation__is_positive__always__expected
static_assert(!operation::is_positive(opcode::push_size_1));
static_assert(!operation::is_positive(opcode::push_size_75));
static_assert(!operation::is_positive(opcode::push_one_size));
static_assert(!operation::is_positive(opcode::push_two_size));
static_assert(!operation::is_positive(opcode::push_four_size));
static_assert(!operation::is_positive(opcode::push_negative_1));
static_assert(!operation::is_positive(opcode::push_size_0));
static_assert(operation::is_positive(opcode::push_positive_1));
static_assert(operation::is_positive(opcode::push_positive_2));
static_assert(operation::is_positive(opcode::push_positive_3));
static_assert(operation::is_positive(opcode::push_positive_4));
static_assert(operation::is_positive(opcode::push_positive_5));
static_assert(operation::is_positive(opcode::push_positive_6));
static_assert(operation::is_positive(opcode::push_positive_7));
static_assert(operation::is_positive(opcode::push_positive_8));
static_assert(operation::is_positive(opcode::push_positive_9));
static_assert(operation::is_positive(opcode::push_positive_10));
static_assert(operation::is_positive(opcode::push_positive_11));
static_assert(operation::is_positive(opcode::push_positive_12));
static_assert(operation::is_positive(opcode::push_positive_13));
static_assert(operation::is_positive(opcode::push_positive_14));
static_assert(operation::is_positive(opcode::push_positive_15));
static_assert(operation::is_positive(opcode::push_positive_16));

// operation__is_nonnegative__always__expected
static_assert(!operation::is_nonnegative(opcode::push_size_1));
static_assert(!operation::is_nonnegative(opcode::push_size_75));
static_assert(!operation::is_nonnegative(opcode::push_one_size));
static_assert(!operation::is_nonnegative(opcode::push_two_size));
static_assert(!operation::is_nonnegative(opcode::push_four_size));
static_assert(!operation::is_nonnegative(opcode::push_negative_1));
static_assert(operation::is_nonnegative(opcode::push_size_0));
static_assert(operation::is_nonnegative(opcode::push_positive_1));
static_assert(operation::is_nonnegative(opcode::push_positive_2));
static_assert(operation::is_nonnegative(opcode::push_positive_3));
static_assert(operation::is_nonnegative(opcode::push_positive_4));
static_assert(operation::is_nonnegative(opcode::push_positive_5));
static_assert(operation::is_nonnegative(opcode::push_positive_6));
static_assert(operation::is_nonnegative(opcode::push_positive_7));
static_assert(operation::is_nonnegative(opcode::push_positive_8));
static_assert(operation::is_nonnegative(opcode::push_positive_9));
static_assert(operation::is_nonnegative(opcode::push_positive_10));
static_assert(operation::is_nonnegative(opcode::push_positive_11));
static_assert(operation::is_nonnegative(opcode::push_positive_12));
static_assert(operation::is_nonnegative(opcode::push_positive_13));
static_assert(operation::is_nonnegative(opcode::push_positive_14));
static_assert(operation::is_nonnegative(opcode::push_positive_15));
static_assert(operation::is_nonnegative(opcode::push_positive_16));

// operation__opcode_to_nonnegative__all_nonnegatives__expected
static_assert(operation::opcode_to_nonnegative(opcode::push_size_0) == 0u);
static_assert(operation::opcode_to_nonnegative(opcode::push_positive_1) == 1u);
static_assert(operation::opcode_to_nonnegative(opcode::push_positive_2) == 2u);
static_assert(operation::opcode_to_nonnegative(opcode::push_positive_3) == 3u);
static_assert(operation::opcode_to_nonnegative(opcode::push_positive_4) == 4u);
static_assert(operation::opcode_to_nonnegative(opcode::push_positive_5) == 5u);
static_assert(operation::opcode_to_nonnegative(opcode::push_positive_6) == 6u);
static_assert(operation::opcode_to_nonnegative(opcode::push_positive_7) == 7u);
static_assert(operation::opcode_to_nonnegative(opcode::push_positive_8) == 8u);
static_assert(operation::opcode_to_nonnegative(opcode::push_positive_9) == 9u);
static_assert(operation::opcode_to_nonnegative(opcode::push_positive_10) == 10u);
static_assert(operation::opcode_to_nonnegative(opcode::push_positive_11) == 11u);
static_assert(operation::opcode_to_nonnegative(opcode::push_positive_12) == 12u);
static_assert(operation::opcode_to_nonnegative(opcode::push_positive_13) == 13u);
static_assert(operation::opcode_to_nonnegative(opcode::push_positive_14) == 14u);
static_assert(operation::opcode_to_nonnegative(opcode::push_positive_15) == 15u);
static_assert(operation::opcode_to_nonnegative(opcode::push_positive_16) == 16u);

// operation__opcode_from_positive__all_positives__expected
static_assert(operation::opcode_from_positive(1u) == opcode::push_positive_1);
static_assert(operation::opcode_from_positive(2u) == opcode::push_positive_2);
static_assert(operation::opcode_from_positive(3u) == opcode::push_positive_3);
static_assert(operation::opcode_from_positive(4u) == opcode::push_positive_4);
static_assert(operation::opcode_from_positive(5u) == opcode::push_positive_5);
static_assert(operation::opcode_from_positive(6u) == opcode::push_positive_6);
static_assert(operation::opcode_from_positive(7u) == opcode::push_positive_7);
static_assert(operation::opcode_from_positive(8u) == opcode::push_positive_8);
static_assert(operation::opcode_from_positive(9u) == opcode::push_positive_9);
static_assert(operation::opcode_from_positive(10u) == opcode::push_positive_10);
static_assert(operation::opcode_from_positive(11u) == opcode::push_positive_11);
static_assert(operation::opcode_from_positive(12u) == opcode::push_positive_12);
static_assert(operation::opcode_from_positive(13u) == opcode::push_positive_13);
static_assert(operation::opcode_from_positive(14u) == opcode::push_positive_14);
static_assert(operation::opcode_from_positive(15u) == opcode::push_positive_15);
static_assert(operation::opcode_from_positive(16u) == opcode::push_positive_16);

// operation__opcode_from_nonnegative__all_nonnegatives__expected
static_assert(operation::opcode_from_nonnegative(0u) == opcode::push_size_0);
static_assert(operation::opcode_from_nonnegative(1u) == opcode::push_positive_1);
static_assert(operation::opcode_from_nonnegative(2u) == opcode::push_positive_2);
static_assert(operation::opcode_from_nonnegative(3u) == opcode::push_positive_3);
static_assert(operation::opcode_from_nonnegative(4u) == opcode::push_positive_4);
static_assert(operation::opcode_from_nonnegative(5u) == opcode::push_positive_5);
static_assert(operation::opcode_from_nonnegative(6u) == opcode::push_positive_6);
static_assert(operation::opcode_from_nonnegative(7u) == opcode::push_positive_7);
static_assert(operation::opcode_from_nonnegative(8u) == opcode::push_positive_8);
static_assert(operation::opcode_from_nonnegative(9u) == opcode::push_positive_9);
static_assert(operation::opcode_from_nonnegative(10u) == opcode::push_positive_10);
static_assert(operation::opcode_from_nonnegative(11u) == opcode::push_positive_11);
static_assert(operation::opcode_from_nonnegative(12u) == opcode::push_positive_12);
static_assert(operation::opcode_from_nonnegative(13u) == opcode::push_positive_13);
static_assert(operation::opcode_from_nonnegative(14u) == opcode::push_positive_14);
static_assert(operation::opcode_from_nonnegative(15u) == opcode::push_positive_15);
static_assert(operation::opcode_from_nonnegative(16u) == opcode::push_positive_16);

// operation__opcode_to_positive__all_positives__expected
static_assert(operation::opcode_to_positive(opcode::push_positive_1) == 1u);
static_assert(operation::opcode_to_positive(opcode::push_positive_2) == 2u);
static_assert(operation::opcode_to_positive(opcode::push_positive_3) == 3u);
static_assert(operation::opcode_to_positive(opcode::push_positive_4) == 4u);
static_assert(operation::opcode_to_positive(opcode::push_positive_5) == 5u);
static_assert(operation::opcode_to_positive(opcode::push_positive_6) == 6u);
static_assert(operation::opcode_to_positive(opcode::push_positive_7) == 7u);
static_assert(operation::opcode_to_positive(opcode::push_positive_8) == 8u);
static_assert(operation::opcode_to_positive(opcode::push_positive_9) == 9u);
static_assert(operation::opcode_to_positive(opcode::push_positive_10) == 10u);
static_assert(operation::opcode_to_positive(opcode::push_positive_11) == 11u);
static_assert(operation::opcode_to_positive(opcode::push_positive_12) == 12u);
static_assert(operation::opcode_to_positive(opcode::push_positive_13) == 13u);
static_assert(operation::opcode_to_positive(opcode::push_positive_14) == 14u);
static_assert(operation::opcode_to_positive(opcode::push_positive_15) == 15u);
static_assert(operation::opcode_to_positive(opcode::push_positive_16) == 16u);

// operation__is_relaxed_push__always__expected
static_assert(operation::is_relaxed_push(opcode::push_size_0));
static_assert(operation::is_relaxed_push(opcode::push_size_75));
static_assert(operation::is_relaxed_push(opcode::push_one_size));
static_assert(operation::is_relaxed_push(opcode::push_four_size));
static_assert(operation::is_relaxed_push(opcode::reserved_80));
static_assert(operation::is_relaxed_push(opcode::push_negative_1));
static_assert(operation::is_relaxed_push(opcode::push_positive_16));
static_assert(!operation::is_relaxed_push(opcode::nop));
static_assert(!operation::is_relaxed_push(opcode::checksig));

// operation__is_push__always__expected
static_assert(operation::is_push(opcode::push_size_0));
static_assert(operation::is_push(opcode::push_four_size));
static_assert(operation::is_push(opcode::push_negative_1));
static_assert(operation::is_push(opcode::push_positive_16));
static_assert(!operation::is_push(opcode::reserved_80));
static_assert(!operation::is_push(opcode::nop));

// operation__is_payload__always__expected
static_assert(operation::is_payload(opcode::push_size_1));
static_assert(operation::is_payload(opcode::push_size_75));
static_assert(operation::is_payload(opcode::push_one_size));
static_assert(operation::is_payload(opcode::push_four_size));
static_assert(!operation::is_payload(opcode::push_size_0));
static_assert(!operation::is_payload(opcode::push_negative_1));
static_assert(!operation::is_payload(opcode::push_positive_1));

// operation__is_number__always__expected
static_assert(operation::is_number(opcode::push_negative_1));
static_assert(operation::is_number(opcode::push_size_0));
static_assert(operation::is_number(opcode::push_positive_1));
static_assert(operation::is_number(opcode::push_positive_16));
static_assert(!operation::is_number(opcode::push_size_1));
static_assert(!operation::is_number(opcode::reserved_80));

// operation__is_timelock__always__expected
static_assert(operation::is_timelock(opcode::checklocktimeverify));
static_assert(operation::is_timelock(opcode::checksequenceverify));
static_assert(!operation::is_timelock(opcode::nop1));
static_assert(!operation::is_timelock(opcode::nop4));

// operation__is_roller__always__expected
static_assert(operation::is_roller(opcode::roll));
static_assert(!operation::is_roller(opcode::pick));

// operation__is_counted__always__expected
static_assert(operation::is_counted(opcode::nop));
static_assert(operation::is_counted(opcode::checksig));
static_assert(operation::is_counted(opcode::reserved_255));
static_assert(!operation::is_counted(opcode::push_positive_16));
static_assert(!operation::is_counted(opcode::push_size_0));

// operation__is_conditional__always__expected
static_assert(operation::is_conditional(opcode::if_));
static_assert(operation::is_conditional(opcode::notif));
static_assert(operation::is_conditional(opcode::else_));
static_assert(operation::is_conditional(opcode::endif));
static_assert(!operation::is_conditional(opcode::verify));
static_assert(!operation::is_conditional(opcode::op_return));

// operation__is_reserved__always__expected
static_assert(operation::is_reserved(opcode::op_ver));
static_assert(operation::is_reserved(opcode::op_return));
static_assert(operation::is_reserved(opcode::reserved_80));
static_assert(operation::is_reserved(opcode::reserved_137));
static_assert(operation::is_reserved(opcode::reserved_138));
static_assert(operation::is_reserved(opcode::reserved_186));
static_assert(operation::is_reserved(opcode::reserved_255));
static_assert(!operation::is_reserved(opcode::nop));
static_assert(!operation::is_reserved(opcode::checksig));

BOOST_AUTO_TEST_CASE(operation__is_success__bip342__expected)
{
    // 80
    BOOST_CHECK(!operation::is_success(opcode::push_negative_1));
    BOOST_CHECK( operation::is_success(opcode::reserved_80));
    BOOST_CHECK(!operation::is_success(opcode::push_positive_1));

    // 98
    BOOST_CHECK(!operation::is_success(opcode::nop));
    BOOST_CHECK( operation::is_success(opcode::op_ver));
    BOOST_CHECK(!operation::is_success(opcode::if_));

    // 126-129
    BOOST_CHECK(!operation::is_success(opcode::tuck));
    BOOST_CHECK( operation::is_success(opcode::op_cat));
    BOOST_CHECK( operation::is_success(opcode::op_substr));
    BOOST_CHECK( operation::is_success(opcode::op_left));
    BOOST_CHECK( operation::is_success(opcode::op_right));
    BOOST_CHECK(!operation::is_success(opcode::size));

    // 131-134
    BOOST_CHECK(!operation::is_success(opcode::size));
    BOOST_CHECK( operation::is_success(opcode::op_invert));
    BOOST_CHECK( operation::is_success(opcode::op_and));
    BOOST_CHECK( operation::is_success(opcode::op_or));
    BOOST_CHECK( operation::is_success(opcode::op_xor));
    BOOST_CHECK(!operation::is_success(opcode::equal));

    // 137-138
    BOOST_CHECK(!operation::is_success(opcode::equalverify));
    BOOST_CHECK( operation::is_success(opcode::reserved_137));
    BOOST_CHECK( operation::is_success(opcode::reserved_138));
    BOOST_CHECK(!operation::is_success(opcode::add1));

    // 141-142
    BOOST_CHECK(!operation::is_success(opcode::sub1));
    BOOST_CHECK( operation::is_success(opcode::op_mul2));
    BOOST_CHECK( operation::is_success(opcode::op_div2));
    BOOST_CHECK(!operation::is_success(opcode::negate));

    // 149-153
    BOOST_CHECK(!operation::is_success(opcode::sub));
    BOOST_CHECK( operation::is_success(opcode::op_mul));
    BOOST_CHECK( operation::is_success(opcode::op_div));
    BOOST_CHECK( operation::is_success(opcode::op_mod));
    BOOST_CHECK( operation::is_success(opcode::op_lshift));
    BOOST_CHECK( operation::is_success(opcode::op_rshift));
    BOOST_CHECK(!operation::is_success(opcode::booland));

    // 187-254
    BOOST_CHECK(!operation::is_success(opcode::reserved_186));
    BOOST_CHECK( operation::is_success(opcode::reserved_187));
    BOOST_CHECK( operation::is_success(opcode::reserved_254));
    BOOST_CHECK(!operation::is_success(opcode::reserved_255));
}

// is_threshold

BOOST_AUTO_TEST_CASE(operation__is_threshold__expected)
{
    // 156-158 (numeric in/equality family)
    BOOST_CHECK( operation::is_threshold(opcode::numequal));
    BOOST_CHECK( operation::is_threshold(opcode::numequalverify));
    BOOST_CHECK( operation::is_threshold(opcode::numnotequal));

    // 159-162 (numeric comparison family)
    BOOST_CHECK( operation::is_threshold(opcode::lessthan));
    BOOST_CHECK( operation::is_threshold(opcode::greaterthan));
    BOOST_CHECK( operation::is_threshold(opcode::lessthanorequal));
    BOOST_CHECK( operation::is_threshold(opcode::greaterthanorequal));
    BOOST_CHECK(!operation::is_threshold(opcode::equal));

    // 165 (range check)
    BOOST_CHECK( operation::is_threshold(opcode::within));
    BOOST_CHECK(!operation::is_threshold(opcode::add));
    BOOST_CHECK(!operation::is_threshold(opcode::reserved_255));
}

// is_timelock

BOOST_AUTO_TEST_CASE(operation__is_timelock__expected)
{
    // 177
    BOOST_CHECK(!operation::is_timelock(opcode::checksig));
    BOOST_CHECK( operation::is_timelock(opcode::checklocktimeverify));
    BOOST_CHECK(!operation::is_timelock(opcode::checksigverify));

    // 178
    BOOST_CHECK(!operation::is_timelock(opcode::checksigadd));
    BOOST_CHECK( operation::is_timelock(opcode::checksequenceverify));
    BOOST_CHECK(!operation::is_timelock(opcode::checkmultisig));
    BOOST_CHECK(!operation::is_timelock(opcode::reserved_255));
}

// utilities (member)
// ----------------------------------------------------------------------------

// The member predicates forward the operation code to the static overloads.

BOOST_AUTO_TEST_CASE(operation__member_code_predicates__push__expected)
{
    const operation instance{ data_chunk{ 0x2a }, false };
    BOOST_REQUIRE(instance.is_relaxed_push());
    BOOST_REQUIRE(instance.is_push());
    BOOST_REQUIRE(instance.is_payload());
    BOOST_REQUIRE(!instance.is_positive());
    BOOST_REQUIRE(!instance.is_nonnegative());
    BOOST_REQUIRE(!instance.is_number());
    BOOST_REQUIRE(!instance.is_roller());
    BOOST_REQUIRE(!instance.is_counted());
    BOOST_REQUIRE(!instance.is_success());
    BOOST_REQUIRE(!instance.is_invalid());
    BOOST_REQUIRE(!instance.is_conditional());
    BOOST_REQUIRE(!instance.is_reserved());
}

BOOST_AUTO_TEST_CASE(operation__member_code_predicates__small_integer__expected)
{
    const operation instance{ opcode::push_positive_1 };
    BOOST_REQUIRE(instance.is_relaxed_push());
    BOOST_REQUIRE(instance.is_push());
    BOOST_REQUIRE(!instance.is_payload());
    BOOST_REQUIRE(instance.is_positive());
    BOOST_REQUIRE(instance.is_nonnegative());
    BOOST_REQUIRE(instance.is_number());
    BOOST_REQUIRE(!instance.is_counted());
}

BOOST_AUTO_TEST_CASE(operation__member_code_predicates__roll__expected)
{
    const operation instance{ opcode::roll };
    BOOST_REQUIRE(instance.is_roller());
    BOOST_REQUIRE(instance.is_counted());
    BOOST_REQUIRE(!instance.is_push());
}

BOOST_AUTO_TEST_CASE(operation__member_code_predicates__conditional__expected)
{
    const operation instance{ opcode::if_ };
    BOOST_REQUIRE(instance.is_conditional());
    BOOST_REQUIRE(instance.is_counted());
}

BOOST_AUTO_TEST_CASE(operation__member_code_predicates__reserved__expected)
{
    const operation instance{ opcode::reserved_80 };
    BOOST_REQUIRE(instance.is_reserved());
    BOOST_REQUIRE(instance.is_relaxed_push());
    BOOST_REQUIRE(!instance.is_push());
}

BOOST_AUTO_TEST_CASE(operation__member_code_predicates__invalid__expected)
{
    const operation instance{ opcode::op_cat };
    BOOST_REQUIRE(instance.is_invalid());
    BOOST_REQUIRE(instance.is_success());
}

// is_underclaimed/is_oversized/is_underflow

BOOST_AUTO_TEST_CASE(operation__is_underclaimed__nominal_push__false)
{
    const operation instance{ data_chunk{ 0x2a }, false };
    BOOST_REQUIRE(!instance.is_underclaimed());
}

BOOST_AUTO_TEST_CASE(operation__is_underclaimed__truncated_push__true)
{
    const operation instance{ base16_chunk("03ffff") };
    BOOST_REQUIRE(instance.is_underclaimed());
}

BOOST_AUTO_TEST_CASE(operation__is_oversized__maximum_push__false)
{
    const operation instance{ data_chunk(max_push_data_size, 0x00), false };
    BOOST_REQUIRE(!instance.is_oversized());
}

BOOST_AUTO_TEST_CASE(operation__is_oversized__above_maximum_push__true)
{
    const operation instance{ data_chunk(add1(max_push_data_size), 0x00), false };
    BOOST_REQUIRE(instance.is_oversized());
}

BOOST_AUTO_TEST_CASE(operation__is_underflow__truncated_push__true)
{
    const operation instance{ base16_chunk("4c") };
    BOOST_REQUIRE(instance.is_underflow());
    BOOST_REQUIRE(instance.is_invalid());
}

BOOST_AUTO_TEST_CASE(operation__is_underflow__complete_push__false)
{
    const operation instance{ base16_chunk("0101") };
    BOOST_REQUIRE(!instance.is_underflow());
}

// is_minimal_push/is_nominal_push

BOOST_AUTO_TEST_CASE(operation__is_minimal_push__small_integer_opcode__true)
{
    BOOST_REQUIRE(operation(opcode::push_positive_1).is_minimal_push());
    BOOST_REQUIRE(operation(opcode::push_positive_16).is_minimal_push());
    BOOST_REQUIRE(operation(opcode::push_negative_1).is_minimal_push());
    BOOST_REQUIRE(operation(opcode::push_size_0).is_minimal_push());
}

BOOST_AUTO_TEST_CASE(operation__is_nominal_push__small_integer_opcode__true)
{
    BOOST_REQUIRE(operation(opcode::push_positive_1).is_nominal_push());
    BOOST_REQUIRE(operation(opcode::push_positive_16).is_nominal_push());
    BOOST_REQUIRE(operation(opcode::push_negative_1).is_nominal_push());
    BOOST_REQUIRE(operation(opcode::push_size_0).is_nominal_push());
}

BOOST_AUTO_TEST_CASE(operation__is_nominal_push__non_push_opcode__false)
{
    BOOST_REQUIRE(!operation(opcode::nop).is_nominal_push());
    BOOST_REQUIRE(!operation(opcode::checksig).is_nominal_push());
}

BOOST_AUTO_TEST_CASE(operation__is_minimal_push__non_push_opcode__false)
{
    BOOST_REQUIRE(!operation(opcode::nop).is_minimal_push());
    BOOST_REQUIRE(!operation(opcode::checksig).is_minimal_push());
}

BOOST_AUTO_TEST_CASE(operation__is_minimal_push__sized_small_integer__false)
{
    const operation instance{ data_chunk{ 0x01 }, false };
    BOOST_REQUIRE(instance.code() == opcode::push_size_1);
    BOOST_REQUIRE(instance.is_nominal_push());
    BOOST_REQUIRE(!instance.is_minimal_push());
}

BOOST_AUTO_TEST_CASE(operation__is_minimal_push__sized_non_numeric__true)
{
    const operation instance{ data_chunk{ 0x11 }, false };
    BOOST_REQUIRE(instance.code() == opcode::push_size_1);
    BOOST_REQUIRE(instance.is_nominal_push());
    BOOST_REQUIRE(instance.is_minimal_push());
}

BOOST_AUTO_TEST_CASE(operation__is_minimal_push__multiple_bytes__true)
{
    const operation instance{ data_chunk{ 0x01, 0x02 }, false };
    BOOST_REQUIRE(instance.is_nominal_push());
    BOOST_REQUIRE(instance.is_minimal_push());
}

BOOST_AUTO_TEST_CASE(operation__is_unsigned32__always__expected)
{
    BOOST_CHECK(operation(opcode::push_size_0).is_unsigned32());
    BOOST_CHECK(operation(opcode::push_positive_1).is_unsigned32());
    BOOST_CHECK(operation(opcode::push_positive_16).is_unsigned32());

    BOOST_CHECK(operation(data_chunk{ 0x00 }, true).is_unsigned32());
    BOOST_CHECK(operation({ 0xff, 0xff, 0xff, 0x7f }, true).is_unsigned32());

    BOOST_CHECK(!operation(data_chunk{ 0x81 }, true).is_unsigned32());
    BOOST_CHECK(!operation(data_chunk{ 0x82 }, true).is_unsigned32());
    BOOST_CHECK(!operation({ 0x80, 0x00, 0x00, 0x80 }, true).is_unsigned32());

    BOOST_CHECK(!operation({ 0x00, 0x00, 0x00, 0x00, 0x00 }, true).is_unsigned32());
}

BOOST_AUTO_TEST_CASE(operation__is_unsigned40__always__expected)
{
    BOOST_CHECK(operation(opcode::push_size_0).is_unsigned40());
    BOOST_CHECK(operation(opcode::push_positive_1).is_unsigned40());
    BOOST_CHECK(operation(opcode::push_positive_16).is_unsigned40());

    BOOST_CHECK(operation({ 0xff, 0xff, 0xff, 0xff, 0x7f }, true).is_unsigned40());

    BOOST_CHECK(!operation(data_chunk{ 0x81 }, true).is_unsigned40());
    BOOST_CHECK(!operation({ 0x42, 0x00, 0x00, 0x00, 0x80 }, true).is_unsigned40());

    BOOST_CHECK(!operation({ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, true).is_unsigned40());
}

BOOST_AUTO_TEST_CASE(operation__as_unsigned32__always__expected)
{
    uint32_t value{};
    BOOST_CHECK(operation(opcode::push_size_0).as_unsigned32(value));
    BOOST_CHECK_EQUAL(value, 0u);

    BOOST_CHECK(operation(opcode::push_positive_1).as_unsigned32(value));
    BOOST_CHECK_EQUAL(value, 1u);

    BOOST_CHECK(operation({ 0xff, 0xff, 0xff, 0x7f }, true).as_unsigned32(value));
    BOOST_CHECK_EQUAL(value, 0x7fffffff_u32);

    BOOST_CHECK(!operation(data_chunk{ 0x81 }, true).as_unsigned32(value));
    BOOST_CHECK(!operation({ 0x80, 0x00, 0x00, 0x80 }, true).as_unsigned32(value));

    BOOST_CHECK(!operation({ 0x00, 0x00, 0x00, 0x00, 0x00 }, true).as_unsigned32(value));
}

BOOST_AUTO_TEST_CASE(operation__as_unsigned40__always__expected)
{
    uint64_t value{};
    BOOST_CHECK(operation(opcode::push_size_0).as_unsigned40(value));
    BOOST_CHECK_EQUAL(value, 0u);

    BOOST_CHECK(operation(opcode::push_positive_1).as_unsigned40(value));
    BOOST_CHECK_EQUAL(value, 1u);

    BOOST_CHECK(operation({ 0xff, 0xff, 0xff, 0xff, 0x7f }, true).as_unsigned40(value));
    BOOST_CHECK_EQUAL(value, 0x0000007fffffffff_u64);

    BOOST_CHECK(!operation(data_chunk{ 0x81 }, true).as_unsigned40(value));
    BOOST_CHECK(!operation({ 0x42, 0x00, 0x00, 0x00, 0x80 }, true).as_unsigned40(value));

    BOOST_CHECK(!operation({ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, true).as_unsigned40(value));
}

// opcode_to_maximum_size
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(operation__opcode_to_maximum_size__size_codes__own_value)
{
    BOOST_CHECK_EQUAL(operation::opcode_to_maximum_size(opcode::push_size_0), 0u);
    BOOST_CHECK_EQUAL(operation::opcode_to_maximum_size(opcode::push_size_1), 1u);
    BOOST_CHECK_EQUAL(operation::opcode_to_maximum_size(opcode::push_size_75), 75u);
}

BOOST_AUTO_TEST_CASE(operation__opcode_to_maximum_size__prefixed_codes__operand_domain)
{
    BOOST_CHECK_EQUAL(operation::opcode_to_maximum_size(opcode::push_one_size), max_uint8);
    BOOST_CHECK_EQUAL(operation::opcode_to_maximum_size(opcode::push_two_size), max_uint16);
    BOOST_CHECK_EQUAL(operation::opcode_to_maximum_size(opcode::push_four_size), max_uint32);
}

// Codes above push_size_75 that are not prefixed pushes carry no payload.
BOOST_AUTO_TEST_CASE(operation__opcode_to_maximum_size__non_payload_codes__zero)
{
    BOOST_CHECK_EQUAL(operation::opcode_to_maximum_size(opcode::push_negative_1), 0u);
    BOOST_CHECK_EQUAL(operation::opcode_to_maximum_size(opcode::push_positive_1), 0u);
    BOOST_CHECK_EQUAL(operation::opcode_to_maximum_size(opcode::checksig), 0u);
}

// A non-push operation carries no data, so the shared default is returned.
BOOST_AUTO_TEST_CASE(operation__data_ptr__non_push__shared_empty)
{
    const operation instance{ opcode::nop };
    const operation other{ opcode::dup };
    BOOST_REQUIRE(instance.data_ptr());
    BOOST_REQUIRE(instance.data_ptr()->empty());
    BOOST_REQUIRE_EQUAL(instance.data_ptr(), other.data_ptr());
}

// The default operation is the invalid sentinel.
BOOST_AUTO_TEST_CASE(operation__to_string__default__question)
{
    const operation instance{};
    BOOST_REQUIRE(!instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.to_string(0), "?");
}

BOOST_AUTO_TEST_CASE(operation__to_string__bitcoind_reserved_138__op_reserved2)
{
    const operation instance{ opcode::reserved_138 };
    BOOST_REQUIRE_EQUAL(instance.to_string(0, true), "OP_RESERVED2");
}

// The token is numeric but exceeds the int64 domain.
BOOST_AUTO_TEST_CASE(operation__construct__overflow_decimal__invalid)
{
    const operation instance{ std::string_view("9223372036854775808") };
    BOOST_REQUIRE(!instance.is_valid());
}

// Deserialization does not resume from a previously-invalidated stream.
BOOST_AUTO_TEST_CASE(operation__construct__invalid_source__default)
{
    const data_chunk empty{};
    read::bytes::copy source(empty);
    source.invalidate();
    BOOST_REQUIRE(!source);

    const operation instance{ source };
    BOOST_REQUIRE(!instance.is_valid());
}

// Access the protected code/data constructor.
class accessor
  : public operation
{
public:
    accessor(opcode code, const chunk_cptr& data, bool underflow) NOEXCEPT
      : operation(code, data, underflow)
    {
    }
};

// A push code that is not minimal for its data size carries a size prefix.
BOOST_AUTO_TEST_CASE(operation__to_string__non_minimal_push_size__prefixed)
{
    const auto data = to_shared<const data_chunk>(data_chunk{ 0x07 });
    const accessor instance{ opcode::push_size_2, data, false };
    BOOST_REQUIRE_EQUAL(instance.to_string(0), "[0.07]");
}

BOOST_AUTO_TEST_SUITE_END()
