/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#include <cstdint>
#include <iostream>
#include <limits>
////#include <sstream>
#include "number.hpp"

BOOST_AUTO_TEST_SUITE(number_tests)

using namespace bc::system::machine;

// Helpers
// ----------------------------------------------------------------------------

////#define BC_SCRIPT_NUMBER_CHECK_EQ(buffer_num, script_num, value, offset, test) \
////    BOOST_CHECK_MESSAGE( \
////        encode_base16((buffer_num).bytes) == encode_base16((script_num).data()), \
////        "\n\tvalue index : " << value << \
////        "\n\tvalue       : " << number_values[value] << \
////        "\n\toffset index: " << offset << \
////        "\n\toffset      : " << number_offsets[offset] << \
////        "\n\ttest        : " << test << \
////        "\n\tFAILURE     : [" << encode_base16((buffer_num).bytes) << " != " << \
////        encode_base16((script_num).data()) << "]"); \
////    BOOST_CHECK_MESSAGE((buffer_num).number == (script_num).int32(), \
////        "\n\tvalue index : " << value << \
////        "\n\tvalue       : " << number_values[value] << \
////        "\n\toffset index: " << offset << \
////        "\n\toffset      : " << number_offsets[offset] << \
////        "\n\ttest        : " << test << \
////        "\n\tFAILURE     : [" << (buffer_num).number << " != " << \
////        (script_num).int32() << "]")

////static bool is(uint8_t byte)
////{
////    return byte != 0;
////}
////
////// check left - right
////constexpr bool subtract_overflow64(const int64_t left, const int64_t right)
////{
////    return
////        ((right > 0 && left < std::numeric_limits<int64_t>::min() + right) ||
////        (right < 0 && left > std::numeric_limits<int64_t>::max() + right));
////}
////
////constexpr bool add_overflow64(const int64_t left, const int64_t right)
////{
////    return
////        ((right > 0 && left > (std::numeric_limits<int64_t>::max() - right)) ||
////        (right < 0 && left < (std::numeric_limits<int64_t>::min() - right)));
////}
////
////constexpr bool negate_overflow64(const int64_t number)
////{
////    return number == std::numeric_limits<int64_t>::min();
////}

class number_accessor
  : public number
{
public:
    using number::number;

    int32_t int32() const noexcept
    {
        return number::int32();
    }

    int64_t int64() const noexcept
    {
        return number::int64();
    }
};

////// Operators
////// ----------------------------------------------------------------------------
////
////static void CheckAdd(const int64_t num1, const int64_t num2, size_t value,
////    size_t offset, size_t test)
////{
////    const number_buffer& add = number_adds[value][offset][test];
////    const number_accessor scriptnum1(num1);
////    const number_accessor scriptnum2(num2);
////
////    if (!add_overflow64(num1, num2))
////    {
////        BC_SCRIPT_NUMBER_CHECK_EQ(add, scriptnum1 + scriptnum2, value, offset, test);
////        BC_SCRIPT_NUMBER_CHECK_EQ(add, scriptnum1 + num2, value, offset, test);
////        BC_SCRIPT_NUMBER_CHECK_EQ(add, scriptnum2 + num1, value, offset, test);
////    }
////}
////
////static void CheckNegate(const int64_t num, size_t value,
////    size_t offset, size_t test)
////{
////    const number_buffer& negated = number_negates[value][offset][test];
////    const number_accessor scriptnum(num);
////
////    if (!negate_overflow64(num))
////    {
////        BC_SCRIPT_NUMBER_CHECK_EQ(negated, -scriptnum, value, offset, test);
////    }
////}
////
////static void CheckSubtract(const int64_t num1, const int64_t num2, size_t value,
////    size_t offset, size_t test)
////{
////    const number_subtract& subtract = number_subtracts[value][offset][test];
////    const number_accessor scriptnum1(num1);
////    const number_accessor scriptnum2(num2);
////
////    if (!subtract_overflow64(num1, num2))
////    {
////        BC_SCRIPT_NUMBER_CHECK_EQ(subtract.forward, scriptnum1 - scriptnum2, value, offset, test);
////        BC_SCRIPT_NUMBER_CHECK_EQ(subtract.forward, scriptnum1 - num2, value, offset, test);
////    }
////
////    if (!subtract_overflow64(num2, num1))
////    {
////        BC_SCRIPT_NUMBER_CHECK_EQ(subtract.reverse, scriptnum2 - scriptnum1, value, offset, test);
////        BC_SCRIPT_NUMBER_CHECK_EQ(subtract.reverse, scriptnum2 - num1, value, offset, test);
////    }
////}

////static void CheckCompare(const int64_t num1, const int64_t num2,
////    size_t value, size_t offset, size_t test)
////{
////    const number_compare& compare = number_compares[value][offset][test];
////    const number_accessor scriptnum1(num1);
////    const number_accessor scriptnum2(num2);
////
////    BOOST_CHECK(scriptnum1 == scriptnum1);
////    BOOST_CHECK(scriptnum1 >= scriptnum1);
////    BOOST_CHECK(scriptnum1 <= scriptnum1);
////    BOOST_CHECK(!(scriptnum1 != scriptnum1));
////    BOOST_CHECK(!(scriptnum1 < scriptnum1));
////    BOOST_CHECK(!(scriptnum1 > scriptnum1));
////
////    BOOST_CHECK(scriptnum1 == num1);
////    BOOST_CHECK(scriptnum1 >= num1);
////    BOOST_CHECK(scriptnum1 <= num1);
////    BOOST_CHECK(!(scriptnum1 != num1));
////    BOOST_CHECK(!(scriptnum1 < num1));
////    BOOST_CHECK(!(scriptnum1 > num1));
////
////    BOOST_CHECK_EQUAL(is(compare.eq), (scriptnum1 == scriptnum2));
////    BOOST_CHECK_EQUAL(is(compare.ge), (scriptnum1 >= scriptnum2));
////    BOOST_CHECK_EQUAL(is(compare.le), (scriptnum1 <= scriptnum2));
////    BOOST_CHECK_EQUAL(is(compare.ne), (scriptnum1 != scriptnum2));
////    BOOST_CHECK_EQUAL(is(compare.lt), (scriptnum1 < scriptnum2));
////    BOOST_CHECK_EQUAL(is(compare.gt), (scriptnum1 > scriptnum2));
////
////    BOOST_CHECK_EQUAL(is(compare.eq), (scriptnum1 == num2));
////    BOOST_CHECK_EQUAL(is(compare.ge), (scriptnum1 >= num2));
////    BOOST_CHECK_EQUAL(is(compare.le), (scriptnum1 <= num2));
////    BOOST_CHECK_EQUAL(is(compare.ne), (scriptnum1 != num2));
////    BOOST_CHECK_EQUAL(is(compare.lt), (scriptnum1 < num2));
////    BOOST_CHECK_EQUAL(is(compare.gt), (scriptnum1 > num2));
////}
////
////static void RunOperators(const int64_t num1, int64_t num2, size_t value,
////    size_t offset, size_t test)
////{
////    //// Diagnostics
////    //std::stringstream message;
////    //std::cout << boost::format(
////    //    ">>> RunOperators: %1% : %2% : %3% : %4% : %5%\n")
////    //    % num1 % num2 % value % offset % test;
////    //BOOST_MESSAGE(message.str());
////
////    CheckAdd(num1, num2, value, offset, test);
////    CheckNegate(num1, value, offset, test);
////    CheckSubtract(num1, num2, value, offset, test);
////    CheckCompare(num1, num2, value, offset, test);
////}

// Tests
// ----------------------------------------------------------------------------

////// operators
////
////BOOST_AUTO_TEST_CASE(number__operators__data_driven__expected)
////{
////    for (size_t i = 0; i < number_values_count; ++i)
////    {
////        for (size_t j = 0; j < number_offsets_count; ++j)
////        {
////            const auto a = number_values[i];
////            const auto b = number_offsets[j];
////
////            RunOperators(a, +a,         i, j, 0);
////            RunOperators(a, -a,         i, j, 1);
////            RunOperators(a, +b,         i, j, 2);
////            RunOperators(a, -b,         i, j, 3);
////            RunOperators(a + b, +b,     i, j, 4);
////            RunOperators(a + b, -b,     i, j, 5);
////            RunOperators(a - b, +b,     i, j, 6);
////            RunOperators(a - b, -b,     i, j, 7);
////            RunOperators(a + b, +a + b, i, j, 8);
////            RunOperators(a + b, +a - b, i, j, 9);
////            RunOperators(a - b, +a + b, i, j, 10);
////            RunOperators(a - b, +a - b, i, j, 11);
////        }
////    }
////}

// construct (data/int32/int64)

BOOST_AUTO_TEST_CASE(number__construct__default__zero)
{
    const number_accessor instance{};
    BOOST_REQUIRE(instance.data().empty());
    BOOST_REQUIRE_EQUAL(instance.int32(), 0);
    BOOST_REQUIRE_EQUAL(instance.int64(), 0);
}

BOOST_AUTO_TEST_CASE(number__construct__zero__zero)
{
    const number_accessor instance{ 0 };
    BOOST_REQUIRE(instance.data().empty());
    BOOST_REQUIRE_EQUAL(instance.int32(), 0);
    BOOST_REQUIRE_EQUAL(instance.int64(), 0);
}

constexpr auto min64 = std::numeric_limits<int64_t>::min();
constexpr auto max64 = std::numeric_limits<int64_t>::max();
constexpr auto min32 = std::numeric_limits<int32_t>::min();
constexpr auto max32 = std::numeric_limits<int32_t>::max();

// little-endian (with 0x80 negative sentinel)
static const data_chunk min64_data{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80 };
static const data_chunk max64_data{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f };

BOOST_AUTO_TEST_CASE(number__construct__minimum__minimums)
{
    const number_accessor instance{ min64 };
    BOOST_REQUIRE_EQUAL(instance.data(), min64_data );
    BOOST_REQUIRE_EQUAL(instance.int32(), min32);
    BOOST_REQUIRE_EQUAL(instance.int64(), min64);
}

BOOST_AUTO_TEST_CASE(number__construct__maximum__maximums)
{
    const number_accessor instance{ max64 };
    BOOST_REQUIRE_EQUAL(instance.data(), max64_data);
    BOOST_REQUIRE_EQUAL(instance.int32(), max32);
    BOOST_REQUIRE_EQUAL(instance.int64(), max64);
}

// set_data (data/int32/int64)

BOOST_AUTO_TEST_CASE(number__set_data__empty_four__zero)
{
    number_accessor instance{};
    BOOST_REQUIRE(instance.set_data({}, 4));
    BOOST_REQUIRE(instance.data().empty());
    BOOST_REQUIRE_EQUAL(instance.int32(), 0);
    BOOST_REQUIRE_EQUAL(instance.int64(), 0);
}

BOOST_AUTO_TEST_CASE(number__set_data__one_four__expected)
{
    constexpr uint8_t value = 0x42;
    const data_chunk data{ value };
    number_accessor instance{};
    BOOST_REQUIRE(instance.set_data(data, 4));
    BOOST_REQUIRE_EQUAL(instance.data(), data);
    BOOST_REQUIRE_EQUAL(instance.int32(), value);
    BOOST_REQUIRE_EQUAL(instance.int64(), value);
}

BOOST_AUTO_TEST_CASE(number__set_data__one_five__expected)
{
    constexpr uint8_t value = 0x42;
    const data_chunk data{ value };
    number_accessor instance{};
    BOOST_REQUIRE(instance.set_data(data, 5));
    BOOST_REQUIRE_EQUAL(instance.data(), data);
    BOOST_REQUIRE_EQUAL(instance.int32(), value);
    BOOST_REQUIRE_EQUAL(instance.int64(), value);
}

BOOST_AUTO_TEST_CASE(number__set_data__four_four__expected)
{
    const data_chunk data{ 0x42, 0x43, 0x44, 0x45 };
    number_accessor instance{};
    BOOST_REQUIRE(instance.set_data(data, 4));
    BOOST_REQUIRE_EQUAL(instance.data(), data);
    BOOST_REQUIRE_EQUAL(instance.int32(), from_little_endian<int32_t>(data));
    BOOST_REQUIRE_EQUAL(instance.int64(), from_little_endian<int64_t>(data));
    BOOST_REQUIRE_EQUAL(instance.to_int32(), from_little_endian<int32_t>(data));
    BOOST_REQUIRE_EQUAL(instance.to_int40(), from_little_endian<int64_t>(data));
}

BOOST_AUTO_TEST_CASE(number__set_data__five_five__expected)
{
    const data_chunk data{ 0x42, 0x43, 0x44, 0x45, 0x46 };
    number_accessor instance{};
    BOOST_REQUIRE(instance.set_data(data, 5));
    BOOST_REQUIRE_EQUAL(instance.data(), data);

    // int32 applies limit, not truncation.
    BOOST_REQUIRE_EQUAL(instance.int32(), max32);

    // int64 is unconstrained except by 8 byte data limit.
    BOOST_REQUIRE_EQUAL(instance.int64(), from_little_endian<int64_t>(data));

    // to_int32 applies truncation.
    BOOST_REQUIRE_EQUAL(instance.to_int32(), narrow_cast<int32_t>(from_little_endian<int64_t>(data)));

    //to_int40 applies mask.
    BOOST_REQUIRE_EQUAL(instance.to_int40(), mask_left(from_little_endian<int64_t>(data), 64u - 40u));
}

BOOST_AUTO_TEST_CASE(number__set_data__five_four__expected)
{
    const data_chunk data{ 0x42, 0x43, 0x44, 0x45, 0x46 };
    number instance{};
    BOOST_REQUIRE(!instance.set_data(data, 4));
}

BOOST_AUTO_TEST_CASE(number__set_data__six_five__expected)
{
    const data_chunk data{ 0x42, 0x43, 0x44, 0x45, 0x46, 0x47 };
    number instance{};
    BOOST_REQUIRE(!instance.set_data(data, 5));
}

// booleans

BOOST_AUTO_TEST_CASE(number__is_true__one__true)
{
    const number instance{ 1 };
    BOOST_REQUIRE(instance.is_true());
}

BOOST_AUTO_TEST_CASE(number__is_true__zero__false)
{
    const number instance{ 0 };
    BOOST_REQUIRE(!instance.is_true());
}

BOOST_AUTO_TEST_CASE(number__is_negative__negative__true)
{
    const number instance{ -1 };
    BOOST_REQUIRE(instance.is_negative());
}

BOOST_AUTO_TEST_CASE(number__is_negative__positive__false)
{
    const number instance{ 1 };
    BOOST_REQUIRE(!instance.is_negative());
}

// data

BOOST_AUTO_TEST_CASE(number__data__x00__empty)
{
    const number instance{ 0x00 };
    const data_chunk expected{};
    const auto result = instance.data();
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(number__data__x01__0x01)
{
    const number instance{ 0x01 };
    const data_chunk expected{ 0x01 };
    const auto result = instance.data();
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(number__data__x7f__0x7f)
{
    const number instance{ 0x7f };
    const data_chunk expected{ 0x7f };
    const auto result = instance.data();
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(number__data__xff__0x00ff)
{
    const number instance{ 0xff };
    const data_chunk expected{ 0xff, 0x00 };
    const auto result = instance.data();
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(number__data__negative_1__0x81)
{
    const number instance{ -1 };
    const data_chunk expected{ 0x81 };
    const auto result = instance.data();
    BOOST_REQUIRE_EQUAL(result, expected);
}

constexpr auto foo = absolute(-1);

BOOST_AUTO_TEST_SUITE_END()
