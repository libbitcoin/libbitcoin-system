/*
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <cstdint>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <boost/format.hpp>
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>
#include "script_number.hpp"
#ifdef ENABLE_DATAGEN
#include "big_number.hpp"
#endif

using namespace bc;

BOOST_AUTO_TEST_SUITE(scriptnum_tests)

// Helpers
// ----------------------------------------------------------------------------

#define BC_SCRIPT_NUMBER_CHECK_EQ(buffer_num, script_num) \
    BOOST_CHECK_EQUAL(bc::encode_hex((buffer_num).bytes), \
        bc::encode_hex((script_num).data())); \
    BOOST_CHECK_EQUAL((buffer_num).number, (script_num).int32())

static bool is(const uint8_t byte)
{
    return byte != 0;
}

// check left - right
static bool subtract_overflow64(const int64_t left, const int64_t right)
{
    return
        ((right > 0 && left < std::numeric_limits<int64_t>::min() + right) ||
        (right < 0 && left > std::numeric_limits<int64_t>::max() + right));
}

static bool add_overflow64(const int64_t left, const int64_t right)
{
    return
        ((right > 0 && left > (std::numeric_limits<int64_t>::max() - right)) ||
        (right < 0 && left < (std::numeric_limits<int64_t>::min() - right)));
}

static bool negate_overflow64(const int64_t number)
{
    return number == std::numeric_limits<int64_t>::min();
}

// Operators
// ----------------------------------------------------------------------------

static void CheckAdd(const int64_t num1, const int64_t num2,
    const script_number_buffer& add)
{
    const script_number scriptnum1(num1);
    const script_number scriptnum2(num2);

    if (!add_overflow64(num1, num2))
    {
        BC_SCRIPT_NUMBER_CHECK_EQ(add, scriptnum1 + scriptnum2);
        BC_SCRIPT_NUMBER_CHECK_EQ(add, scriptnum1 + num2);
        BC_SCRIPT_NUMBER_CHECK_EQ(add, scriptnum2 + num1);
    }
}

static void CheckNegate(const int64_t number,
    const script_number_buffer& negated)
{
    const script_number scriptnum(number);

    if (!negate_overflow64(number))
    {
        BC_SCRIPT_NUMBER_CHECK_EQ(negated, -scriptnum);
    }
}

static void CheckSubtract(const int64_t num1, const int64_t num2,
    const script_number_subtract& subtract)
{
    const script_number scriptnum1(num1);
    const script_number scriptnum2(num2);

    if (!subtract_overflow64(num1, num2))
    {
        BC_SCRIPT_NUMBER_CHECK_EQ(subtract.forward, scriptnum1 - scriptnum2);
        BC_SCRIPT_NUMBER_CHECK_EQ(subtract.forward, scriptnum1 - num2);
    }
    
    if (!subtract_overflow64(num2, num1))
    {
        BC_SCRIPT_NUMBER_CHECK_EQ(subtract.reverse, scriptnum2 - scriptnum1);
        BC_SCRIPT_NUMBER_CHECK_EQ(subtract.reverse, scriptnum2 - num1);
    }
}

static void CheckCompare(const int64_t num1, const int64_t num2,
    const script_number_compare& compare)
{
    const script_number scriptnum1(num1);
    const script_number scriptnum2(num2);

    BOOST_CHECK(scriptnum1 == scriptnum1);
    BOOST_CHECK(scriptnum1 >= scriptnum1);
    BOOST_CHECK(scriptnum1 <= scriptnum1);
    BOOST_CHECK(!(scriptnum1 != scriptnum1));
    BOOST_CHECK(!(scriptnum1 < scriptnum1));
    BOOST_CHECK(!(scriptnum1 > scriptnum1));

    BOOST_CHECK(scriptnum1 == num1);
    BOOST_CHECK(scriptnum1 >= num1);
    BOOST_CHECK(scriptnum1 <= num1);
    BOOST_CHECK(!(scriptnum1 != num1));
    BOOST_CHECK(!(scriptnum1 < num1));
    BOOST_CHECK(!(scriptnum1 > num1));

    BOOST_CHECK_EQUAL(is(compare.eq), (scriptnum1 == scriptnum2));
    BOOST_CHECK_EQUAL(is(compare.ne), (scriptnum1 != scriptnum2));
    BOOST_CHECK_EQUAL(is(compare.lt), (scriptnum1 < scriptnum2));
    BOOST_CHECK_EQUAL(is(compare.gt), (scriptnum1 > scriptnum2));
    BOOST_CHECK_EQUAL(is(compare.ge), (scriptnum1 >= scriptnum2));
    BOOST_CHECK_EQUAL(is(compare.le), (scriptnum1 <= scriptnum2));

    BOOST_CHECK_EQUAL(is(compare.eq), (scriptnum1 == num2));
    BOOST_CHECK_EQUAL(is(compare.ne), (scriptnum1 != num2));
    BOOST_CHECK_EQUAL(is(compare.lt), (scriptnum1 < num2));
    BOOST_CHECK_EQUAL(is(compare.gt), (scriptnum1 > num2));
    BOOST_CHECK_EQUAL(is(compare.ge), (scriptnum1 >= num2));
    BOOST_CHECK_EQUAL(is(compare.le), (scriptnum1 <= num2));
}

#ifndef ENABLE_DATAGEN

static void RunOperators(const int64_t num1, const int64_t num2,
    const size_t value, const size_t offset, const size_t test)
{
    std::stringstream message;
    std::cout << boost::format(
        ">>> RunOperators: %1% : %2% : %3% : %4% : %5%\n")
        % num1 % num2 % value % offset % test;
    BOOST_MESSAGE(message.str());

    CheckAdd(num1, num2, script_number_adds[value][offset][test]);
    CheckSubtract(num1, num2, script_number_subtracts[value][offset][test]);
    CheckNegate(num1, script_number_negates[value][offset][test]);
    CheckCompare(num1, num2, script_number_compares[value][offset][test]);
}

BOOST_AUTO_TEST_CASE(check_operators)
{
    for (size_t i = 0; i < script_number_values_count; ++i)
    {
        for (size_t j = 0; j < script_number_offsets_count; ++j)
        {
            auto a = script_number_values[i];
            auto b = script_number_offsets[j];

            RunOperators(a, +a,         i, j, 0);
            RunOperators(a, -a,         i, j, 1);
            RunOperators(a, +b,         i, j, 2);
            RunOperators(a, -b,         i, j, 3);
            RunOperators(a + b, +b,     i, j, 4);
            RunOperators(a + b, -b,     i, j, 5);
            RunOperators(a - b, +b,     i, j, 6);
            RunOperators(a - b, -b,     i, j, 7);
            RunOperators(a + b, +a + b, i, j, 8);
            RunOperators(a + b, +a - b, i, j, 9);
            RunOperators(a - b, +a + b, i, j, 10);
            RunOperators(a - b, +a - b, i, j, 11);
        }
    }
}

#else

std::stringstream expectations;

static std::string format_bytes(bc::data_chunk chunk)
{
    std::stringstream buffer;
    buffer << "{";

    for (const auto& byte : chunk)
        buffer << (boost::format(" 0x%02x, ") % static_cast<uint16_t>(byte));

    buffer << "}";
    return buffer.str();
}

static script_number_buffer MakeAdd(const int64_t num1, const int64_t num2)
{
    if (add_overflow64(num1, num2))
        return script_number_buffer();

    big_number bignum1;
    bignum1.set_int64(num1);

    big_number bignum2;
    bignum2.set_int64(num2);

    auto sum = bignum1 + bignum2;
    const script_number_buffer add
    {
        sum.int32(),
        sum.data()
    };

    // Side effect is used to generate the test expectation vector.
    expectations << boost::format("{ %1%, %2% }, ") %
        add.number % format_bytes(add.bytes);

    // Return value is used for verifying the result.
    return add;
}

static script_number_buffer MakeNegate(const int64_t number)
{
    if (negate_overflow64(number))
        return script_number_buffer();

    big_number bignum;
    bignum.set_int64(number);

    auto negative = -bignum;
    const script_number_buffer negated
    {
        negative.int32(),
        negative.data()
    };

    // Side effect is used to generate the test expectation vector.
    expectations << boost::format("{ %1%, %2% }, ") %
        negated.number % format_bytes(negated.bytes);

    // Return value is used for verifying the result.
    return negated;
}

static script_number_subtract MakeSubtract(const int64_t num1,
    const int64_t num2)
{
    big_number bignum1;
    bignum1.set_int64(num1);
    big_number bignum2;
    bignum2.set_int64(num2);

    big_number forward;
    if (!subtract_overflow64(num1, num2))
        forward = bignum1 - bignum2;

    big_number reverse;
    if (!subtract_overflow64(num2, num1))
        reverse = bignum2 - bignum1;

    const script_number_subtract subtract
    {
        { forward.int32(), forward.data() },
        { reverse.int32(), reverse.data() }
    };

    // Side effect is used to generate the test expectation vector.
    expectations << boost::format("{ { %1%, %2% }, { %3%, %4% } }, ") %
        subtract.forward.number % format_bytes(subtract.forward.bytes) %
        subtract.reverse.number % format_bytes(subtract.reverse.bytes);

    // Return value is used for verifying the result.
    return subtract;
}

static script_number_compare MakeCompare(const int64_t num1,
    const int64_t num2)
{
    big_number bignum1;
    bignum1.set_int64(num1);
    big_number bignum2;
    bignum2.set_int64(num2);

    script_number_compare compare
    {
        bignum1 == bignum2,
        bignum1 != bignum2,
        bignum1 < bignum2,
        bignum1 > bignum2,
        bignum1 <= bignum2,
        bignum1 >= bignum2
    };

    // Side effect is used to generate the test expectation vector.
    expectations <<
        boost::format("{ %1%, %2%, %3%, %4%, %5%, %6% },") % compare.eq %
        compare.ne % compare.lt % compare.gt % compare.le % compare.ge;

    // Return value is used for verifying the result.
    return compare;
}

static void MakeOperators(const int64_t num1, const int64_t num2)
{
    // Enable individually to build expectation vector.
    CheckAdd(num1, num2, MakeAdd(num1, num2));
    //CheckSubtract(num1, num2, MakeSubtract(num1, num2));
    //CheckNegate(num1, MakeNegate(num1));
    //CheckCompare(num1, num2, MakeCompare(num1, num2));
}

BOOST_AUTO_TEST_CASE(make_operator_expectations)
{
    expectations << boost::format("[%1%][%2%][%3%]=\n{\n") %
        script_number_values_count % script_number_offsets_count % 12;

    for (size_t i = 7; i < script_number_values_count; ++i)
    {
        expectations << "    {\n";
        for (size_t j = 0; j < script_number_offsets_count; ++j)
        {
            expectations << "        {";

            auto a = script_number_values[i];
            auto b = script_number_offsets[j];

            expectations << "\n              ";
            MakeOperators(a, +a);
            expectations << "\n              ";
            MakeOperators(a, -a);
            expectations << "\n              ";
            MakeOperators(a, +b);
            expectations << "\n              ";
            MakeOperators(a, -b);
            expectations << "\n              ";
            MakeOperators(a + b, +b);
            expectations << "\n              ";
            MakeOperators(a + b, -b);
            expectations << "\n              ";
            MakeOperators(a - b, +b);
            expectations << "\n              ";
            MakeOperators(a - b, -b);
            expectations << "\n              ";
            MakeOperators(a + b, +a + b);
            expectations << "\n              ";
            MakeOperators(a + b, +a - b);
            expectations << "\n              ";
            MakeOperators(a - b, +a + b);
            expectations << "\n              ";
            MakeOperators(a - b, +a - b);
            expectations << "\n        },\n";
        }

        expectations << "    },\n";
    }

    expectations << "};\n";

    // Copy from debug buffer to source code expectation vector.
    const auto& dump = expectations.str();
}
#endif

BOOST_AUTO_TEST_SUITE_END()