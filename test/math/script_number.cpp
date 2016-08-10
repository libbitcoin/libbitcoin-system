/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(script_number_tests)

// Helpers
// ----------------------------------------------------------------------------

#define BC_SCRIPT_NUMBER_CHECK_EQ(buffer_num, script_num) \
    BOOST_CHECK_EQUAL(encode_base16((buffer_num).bytes), \
        encode_base16((script_num).data())); \
    BOOST_CHECK_EQUAL((buffer_num).number, (script_num).int32())

static bool is(uint8_t byte)
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
    BOOST_CHECK_EQUAL(is(compare.ge), (scriptnum1 >= scriptnum2));
    BOOST_CHECK_EQUAL(is(compare.le), (scriptnum1 <= scriptnum2));
    BOOST_CHECK_EQUAL(is(compare.ne), (scriptnum1 != scriptnum2));
    BOOST_CHECK_EQUAL(is(compare.lt), (scriptnum1 < scriptnum2));
    BOOST_CHECK_EQUAL(is(compare.gt), (scriptnum1 > scriptnum2));

    BOOST_CHECK_EQUAL(is(compare.eq), (scriptnum1 == num2));
    BOOST_CHECK_EQUAL(is(compare.ge), (scriptnum1 >= num2));
    BOOST_CHECK_EQUAL(is(compare.le), (scriptnum1 <= num2));
    BOOST_CHECK_EQUAL(is(compare.ne), (scriptnum1 != num2));
    BOOST_CHECK_EQUAL(is(compare.lt), (scriptnum1 < num2));
    BOOST_CHECK_EQUAL(is(compare.gt), (scriptnum1 > num2));
}

#ifndef ENABLE_DATAGEN

// Test
// ----------------------------------------------------------------------------

static void RunOperators(const int64_t num1, const int64_t num2,
    const size_t value, const size_t offset, const size_t test)
{
    //// Diagnostics
    //std::stringstream message;
    //std::cout << boost::format(
    //    ">>> RunOperators: %1% : %2% : %3% : %4% : %5%\n")
    //    % num1 % num2 % value % offset % test;
    //BOOST_MESSAGE(message.str());

    CheckAdd(num1, num2, script_number_adds[value][offset][test]);
    CheckNegate(num1, script_number_negates[value][offset][test]);
    CheckSubtract(num1, num2, script_number_subtracts[value][offset][test]);
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

// big_number value generators
// ----------------------------------------------------------------------------

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

    return compare;
}

// Formatter Helpers
// ----------------------------------------------------------------------------

static void write_bytes(bc::data_chunk chunk, std::ostream& out)
{
    for (const auto& byte : chunk)
        out << (boost::format(" 0x%02x, ") % static_cast<uint16_t>(byte));
}

static void write_buffer(script_number_buffer buffer, std::ostream& out)
{
    out << boost::format("{ %1%, {") % buffer.number;
    write_bytes(buffer.bytes, out);
    out << "} }, ";
}

static void write_compare(script_number_compare compare, std::ostream& out)
{
    out << boost::format("{ %1%, %2%, %3%, %4%, %5%, %6% }, ") % compare.eq %
        compare.ne % compare.lt % compare.gt % compare.le % compare.ge;
}

static void write_subtract(script_number_subtract subtract, std::ostream& out)
{
    out << "{ ";
    write_buffer(subtract.forward, out);
    write_buffer(subtract.reverse, out);
    out << "}, ";
}

static void write_names(const std::string& name, size_t count,
    std::ostream& out)
{
    out << boost::format("const %1%[%2%][%3%][%4%]=\n{\n") % name %
        script_number_values_count % script_number_offsets_count % count;
}

static void write(const std::string& text, std::ostream& add_out,
    std::ostream& neg_out, std::ostream& sub_out, std::ostream& cmp_out)
{
    add_out << text;
    neg_out << text;
    sub_out << text;
    cmp_out << text;
}

static void replace(std::string& buffer, const std::string& find,
    const std::string& replacement)
{
    size_t pos = 0;
    while ((pos = buffer.find(find, pos)) != std::string::npos)
    {
        buffer.replace(pos, find.length(), replacement);
        pos += replacement.length();
    }
}

// Maker
// ----------------------------------------------------------------------------

static void MakeOperators(const int64_t num1, const int64_t num2,
    std::ostream& add_out, std::ostream& neg_out, std::ostream& sub_out,
    std::ostream& cmp_out)
{
    write("\n              ", add_out, neg_out, sub_out, cmp_out);

    auto add = MakeAdd(num1, num2);
    CheckAdd(num1, num2, add);
    write_buffer(add, add_out);

    auto negate = MakeNegate(num1);
    CheckNegate(num1, negate);
    write_buffer(negate, neg_out);

    auto subtract = MakeSubtract(num1, num2);
    CheckSubtract(num1, num2, subtract);
    write_subtract(subtract, sub_out);

    auto compare = MakeCompare(num1, num2);
    CheckCompare(num1, num2, compare);
    write_compare(compare, cmp_out);
}

BOOST_AUTO_TEST_CASE(make_operator_expectations)
{
    std::stringstream add_out;
    std::stringstream neg_out;
    std::stringstream sub_out;
    std::stringstream cmp_out;

    write_names("script_number_buffer script_number_adds", 12, add_out);
    write_names("script_number_buffer script_number_negates", 12, neg_out);
    write_names("script_number_subtract script_number_subtracts", 12, sub_out);
    write_names("script_number_compare script_number_compares", 12, cmp_out);

    for (size_t i = 0; i < script_number_values_count; ++i)
    {
        write("    {\n", add_out, neg_out, sub_out, cmp_out);

        for (size_t j = 0; j < script_number_offsets_count; ++j)
        {
            write("        {", add_out, neg_out, sub_out, cmp_out);

            auto a = script_number_values[i];
            auto b = script_number_offsets[j];

            MakeOperators(a, +a, add_out, neg_out, sub_out, cmp_out);
            MakeOperators(a, -a, add_out, neg_out, sub_out, cmp_out);
            MakeOperators(a, +b, add_out, neg_out, sub_out, cmp_out);
            MakeOperators(a, -b, add_out, neg_out, sub_out, cmp_out);
            MakeOperators(a + b, +b, add_out, neg_out, sub_out, cmp_out);
            MakeOperators(a + b, -b, add_out, neg_out, sub_out, cmp_out);
            MakeOperators(a - b, +b, add_out, neg_out, sub_out, cmp_out);
            MakeOperators(a - b, -b, add_out, neg_out, sub_out, cmp_out);
            MakeOperators(a + b, +a + b, add_out, neg_out, sub_out, cmp_out);
            MakeOperators(a + b, +a - b, add_out, neg_out, sub_out, cmp_out);
            MakeOperators(a - b, +a + b, add_out, neg_out, sub_out, cmp_out);
            MakeOperators(a - b, +a - b, add_out, neg_out, sub_out, cmp_out);

            write("\n        },\n", add_out, neg_out, sub_out, cmp_out);
        }

        write("    },\n", add_out, neg_out, sub_out, cmp_out);
    }

    write("};\n\n", add_out, neg_out, sub_out, cmp_out);

    std::stringstream dump;
    dump << add_out.str();
    dump << neg_out.str();
    dump << sub_out.str();
    dump << cmp_out.str();

    auto source = dump.str();
    replace(source, "-2147483648", "(-2147483647 - 1)");
    replace(source, "-9223372036854775808", "(-9223372036854775807 - 1)");
}
#endif

BOOST_AUTO_TEST_SUITE_END()
