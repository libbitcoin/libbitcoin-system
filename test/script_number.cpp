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

// If defined this creates a dependency on OpenSSL and requires big_number.
#undef MAKE_EXPECTATIONS

using namespace bc;

BOOST_AUTO_TEST_SUITE(scriptnum_tests)

// Helpers
// ----------------------------------------------------------------------------

static bool is(const uint8_t byte)
{
    return byte != 0;
}

static bool verify(const script_number_buffer& buffer,
    const script_number& script_num)
{
    return buffer.bytes == script_num.data() &&
        buffer.number == script_num.int32();
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

static script_number_buffer GetAdd(const size_t value,
    const size_t offset, const size_t scenario)
{
    return script_number_adds[value][offset][scenario];
}

static void CheckAdd(const int64_t num1, const int64_t num2,
    const script_number_buffer& add)
{
    const script_number scriptnum1(num1);
    const script_number scriptnum2(num2);

    if (!add_overflow64(num1, num2))
    {
        BOOST_CHECK(verify(add, scriptnum1 + scriptnum2));
        BOOST_CHECK(verify(add, scriptnum1 + num2));
        BOOST_CHECK(verify(add, scriptnum2 + num1));
    }
}

static script_number_buffer GetNegate(const size_t value,
    const size_t offset, const size_t scenario)
{
    return script_number_negates[value][offset][scenario];
}

static void CheckNegate(const int64_t number,
    const script_number_buffer& negated)
{
    const script_number scriptnum(number);

    if (!negate_overflow64(number))
    {
        BOOST_CHECK(verify(negated, -scriptnum));
    }
}

static script_number_subtract GetSubtract(const size_t value,
    const size_t offset, const size_t scenario)
{
    return script_number_subtracts[value][offset][scenario];
}

static void CheckSubtract(const int64_t num1, const int64_t num2,
    const script_number_subtract& subtract)
{
    const script_number scriptnum1(num1);
    const script_number scriptnum2(num2);

    if (!subtract_overflow64(num1, num2))
    {
        BOOST_CHECK(verify(subtract.forward, scriptnum1 - scriptnum2));
        BOOST_CHECK(verify(subtract.forward, scriptnum1 - num2));
    }
    
    if (!subtract_overflow64(num2, num1))
    {
        BOOST_CHECK(verify(subtract.reverse, scriptnum2 - scriptnum1));
        BOOST_CHECK(verify(subtract.reverse, scriptnum2 - num1));
    }
}

static script_number_compare GetCompare(const size_t value,
    const size_t offset, const size_t scenario)
{
    return script_number_compares[value][offset][scenario];
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

    BOOST_CHECK(is(compare.eq) == (scriptnum1 == scriptnum2));
    BOOST_CHECK(is(compare.ne) == (scriptnum1 != scriptnum2));
    BOOST_CHECK(is(compare.lt) == (scriptnum1 < scriptnum2));
    BOOST_CHECK(is(compare.gt) == (scriptnum1 > scriptnum2));
    BOOST_CHECK(is(compare.ge) == (scriptnum1 >= scriptnum2));
    BOOST_CHECK(is(compare.le) == (scriptnum1 <= scriptnum2));

    BOOST_CHECK(is(compare.eq) == (scriptnum1 == num2));
    BOOST_CHECK(is(compare.ne) == (scriptnum1 != num2));
    BOOST_CHECK(is(compare.lt) == (scriptnum1 < num2));
    BOOST_CHECK(is(compare.gt) == (scriptnum1 > num2));
    BOOST_CHECK(is(compare.ge) == (scriptnum1 >= num2));
    BOOST_CHECK(is(compare.le) == (scriptnum1 <= num2));
}

static void RunOperators(const int64_t num1, const int64_t num2,
    const size_t value, const size_t offset, const size_t scenario)
{
    CheckAdd(num1, num2, GetAdd(value, offset, scenario));
    CheckSubtract(num1, num2, GetSubtract(value, offset, scenario));
    CheckNegate(num1, GetNegate(value, offset, scenario));
    CheckCompare(num1, num2, GetCompare(value, offset, scenario));
}

BOOST_AUTO_TEST_CASE(check_operators)
{
    for (size_t i = 0; i < script_number_values_count; ++i)
    {
        for (size_t j = 0; j < script_number_offsets_count; ++j)
        {

            auto a = script_number_values[i];
            auto b = script_number_offsets[j];

            // scenarios_count [0 .. 11]
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

#ifdef MAKE_EXPECTATIONS
#include "big_number.hpp"
std::stringstream expectations;

static std::string format_bytes(bc::data_chunk chunk)
{
    std::stringstream buffer;
    buffer << "{";

    for (const auto& byte : chunk)
    {
        buffer << (boost::format(" 0x%02x, ") % static_cast<uint16_t>(byte));
    }

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
    //CheckAdd(num1, num2, MakeAdd(num1, num2));
    //CheckSubtract(num1, num2, MakeSubtract(num1, num2));
    //CheckNegate(num1, MakeNegate(num1));
    //CheckCompare(num1, num2, MakeCompare(num1, num2));
}

BOOST_AUTO_TEST_CASE(make_operator_expectations)
{
    expectations << boost::format("const script_number_xxxxxx "\
        "script_number_xxxxxxs[%1%][%2%][%3%]=\n{\n") %
        script_number_values_count % script_number_offsets_count % 12;

    for (size_t i = 0; i < script_number_values_count; ++i)
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

// Creation
// ----------------------------------------------------------------------------
//
//static void CheckCreateVch(const int64_t num)
//{
//    big_number bignum;
//    bignum.set_int64(num);
//    script_number scriptnum(num);
//    BOOST_CHECK(verify(bignum, scriptnum));
//
//    big_number bignum2;
//    bignum2.set_data(bignum.data());
//    script_number scriptnum2;
//    BOOST_CHECK(scriptnum2.set_data(scriptnum.data()));
//    BOOST_CHECK(verify(bignum2, scriptnum2));
//
//    big_number bignum3;
//    bignum3.set_data(scriptnum2.data());
//    script_number scriptnum3;
//    BOOST_CHECK(scriptnum3.set_data(bignum2.data()));
//    BOOST_CHECK(verify(bignum3, scriptnum3));
//}
//
//static void CheckCreateVch_bad_set_data(const int64_t num)
//{
//    big_number bignum;
//    bignum.set_int64(num);
//    script_number scriptnum(num);
//    BOOST_CHECK(verify(bignum, scriptnum));
//
//    big_number bignum2;
//    bignum2.set_data(bignum.data());
//    script_number scriptnum2;
//    BOOST_CHECK(!scriptnum2.set_data(scriptnum.data()));
//
//    big_number bignum3;
//    bignum3.set_data(scriptnum2.data());
//    script_number scriptnum3;
//    BOOST_CHECK(!scriptnum3.set_data(bignum2.data()));
//}
//
//static void CheckCreateInt(const int64_t num)
//{
//    big_number bignum;
//    bignum.set_int64(num);
//    script_number scriptnum(num);
//    BOOST_CHECK(verify(bignum, scriptnum));
//
//    big_number bignum1;
//    bignum1.set_int64(scriptnum.int32());
//
//    BOOST_CHECK(verify(bignum1, script_number(scriptnum.int32())));
//    BOOST_CHECK(bignum.int32() == scriptnum.int32());
//    BOOST_CHECK(verify(bignum1, script_number(bignum.int32())));
//    BOOST_CHECK(verify(bignum1, script_number(script_number(bignum.int32()).int32())));
//}
//
//static void RunCreate(const int64_t num)
//{
//    CheckCreateInt(num);
//    script_number scriptnum(num);
//    if (scriptnum.data().size() <= max_script_number_size)
//        CheckCreateVch(num);
//    else
//        CheckCreateVch_bad_set_data(num);
//}
//
//BOOST_AUTO_TEST_CASE(creation)
//{
//    size_t count_values = sizeof(values) / sizeof(values[0]);
//    size_t count_offsets = sizeof(offsets) / sizeof(offsets[0]);
//
//    for (size_t i = 0; i < count_values; ++i)
//    {
//        for (size_t j = 0; j < count_offsets; ++j)
//        {
//            RunCreate(values[i]);
//            RunCreate(values[i] + offsets[j]);
//            RunCreate(values[i] - offsets[j]);
//        }
//    }
//}

BOOST_AUTO_TEST_SUITE_END()