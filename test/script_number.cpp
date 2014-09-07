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
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>
#include "big_number.hpp"

// These tests fail on Windows x64.
// https://github.com/libbitcoin/libbitcoin/issues/26

using namespace bc;

BOOST_AUTO_TEST_SUITE(scriptnum_tests)

static const int64_t values[] = \
{ 0, 1, CHAR_MIN, CHAR_MAX, UCHAR_MAX, SHRT_MIN, USHRT_MAX, INT_MIN, INT_MAX,
  UINT_MAX, LONG_MIN, LONG_MAX };
static const int64_t offsets[] = \
{ 1, 0x79, 0x80, 0x81, 0xFF, 0x7FFF, 0x8000, 0xFFFF, 0x10000};

static bool verify(const big_number& bignum, const script_number& scriptnum)
{
    return bignum.data() == scriptnum.data() &&
        bignum.int32() == scriptnum.int32();
}

static void CheckCreateVch(const int64_t& num)
{
    big_number bignum;
    bignum.set_int64(num);
    script_number scriptnum(num);
    BOOST_CHECK(verify(bignum, scriptnum));

    big_number bignum2;
    bignum2.set_data(bignum.data());
    script_number scriptnum2;
    BOOST_CHECK(scriptnum2.set_data(scriptnum.data()));
    BOOST_CHECK(verify(bignum2, scriptnum2));

    big_number bignum3;
    bignum3.set_data(scriptnum2.data());
    script_number scriptnum3;
    BOOST_CHECK(scriptnum3.set_data(bignum2.data()));
    BOOST_CHECK(verify(bignum3, scriptnum3));
}

static void CheckCreateVch_bad_set_data(const int64_t& num)
{
    big_number bignum;
    bignum.set_int64(num);
    script_number scriptnum(num);
    BOOST_CHECK(verify(bignum, scriptnum));

    big_number bignum2;
    bignum2.set_data(bignum.data());
    script_number scriptnum2;
    BOOST_CHECK(scriptnum2.set_data(scriptnum.data()) == false);

    big_number bignum3;
    bignum3.set_data(scriptnum2.data());
    script_number scriptnum3;
    BOOST_CHECK(scriptnum3.set_data(bignum2.data()) == false);
}

static void CheckCreateInt(const int64_t& num)
{
    big_number bignum;
    bignum.set_int64(num);
    script_number scriptnum(num);
    BOOST_CHECK(verify(bignum, scriptnum));

    big_number bignum1;
    bignum1.set_int64(scriptnum.int32());
    BOOST_CHECK(verify(bignum1, script_number(scriptnum.int32())));
    BOOST_CHECK(bignum.int32() == scriptnum.int32());
    BOOST_CHECK(verify(bignum1, script_number(bignum.int32())));
    BOOST_CHECK(verify(
        bignum1,
        script_number(script_number(bignum.int32()).int32())));
}

static void CheckAdd(const int64_t& num1, const int64_t& num2)
{
    big_number bignum1;
    bignum1.set_int64(num1);

    big_number bignum2;
    bignum2.set_int64(num2);

    const script_number scriptnum1(num1);
    const script_number scriptnum2(num2);

    big_number bignum3;
    bignum3.set_int64(num1);
    big_number bignum4;
    bignum4.set_int64(num1);

    script_number scriptnum3(num1);
    script_number scriptnum4(num1);

    BOOST_CHECK(bignum1.int32() == scriptnum3.int32());
    BOOST_CHECK(bignum2.int32() == scriptnum2.int32());

    // int64_t overflow is undefined.
    bool invalid = (((num2 > 0) && (num1 > (std::numeric_limits<int64_t>::max() - num2))) ||
                    ((num2 < 0) && (num1 < (std::numeric_limits<int64_t>::min() - num2))));
    if (!invalid)
    {
        BOOST_CHECK(verify(bignum1 + bignum2, scriptnum1 + scriptnum2));
        BOOST_CHECK(verify(bignum1 + bignum2, scriptnum1 + num2));
        BOOST_CHECK(verify(bignum1 + bignum2, scriptnum2 + num1));
    }
}

static void CheckNegate(const int64_t& num)
{
    big_number bignum;
    bignum.set_int64(num);
    const script_number scriptnum(num);

    // -INT64_MIN is undefined
    if (num != std::numeric_limits<int64_t>::min())
    {
        bignum = -bignum;
        BOOST_CHECK(verify(bignum, -scriptnum));
    }
}

static void CheckSubtract(const int64_t& num1, const int64_t& num2)
{
    big_number bignum1;
    bignum1.set_int64(num1);
    big_number bignum2;
    bignum2.set_int64(num2);
    const script_number scriptnum1(num1);
    const script_number scriptnum2(num2);
    bool invalid = false;

    // int64_t overflow is undefined.
    invalid = ((num2 > 0 && num1 < std::numeric_limits<int64_t>::min() + num2) ||
               (num2 < 0 && num1 > std::numeric_limits<int64_t>::max() + num2));
    if (!invalid)
    {
        BOOST_CHECK(verify(bignum1 - bignum2, scriptnum1 - scriptnum2));
        BOOST_CHECK(verify(bignum1 - bignum2, scriptnum1 - num2));
    }

    invalid = ((num1 > 0 && num2 < std::numeric_limits<int64_t>::min() + num1) ||
               (num1 < 0 && num2 > std::numeric_limits<int64_t>::max() + num1));
    if (!invalid)
    {
        BOOST_CHECK(verify(bignum2 - bignum1, scriptnum2 - scriptnum1));
        BOOST_CHECK(verify(bignum2 - bignum1, scriptnum2 - num1));
    }
}

static void CheckCompare(const int64_t& num1, const int64_t& num2)
{
    big_number bignum1;
    bignum1.set_int64(num1);
    big_number bignum2;
    bignum2.set_int64(num2);
    const script_number scriptnum1(num1);
    const script_number scriptnum2(num2);

    BOOST_CHECK((bignum1 == bignum1) == (scriptnum1 == scriptnum1));
    BOOST_CHECK((bignum1 != bignum1) ==  (scriptnum1 != scriptnum1));
    BOOST_CHECK((bignum1 < bignum1) ==  (scriptnum1 < scriptnum1));
    BOOST_CHECK((bignum1 > bignum1) ==  (scriptnum1 > scriptnum1));
    BOOST_CHECK((bignum1 >= bignum1) ==  (scriptnum1 >= scriptnum1));
    BOOST_CHECK((bignum1 <= bignum1) ==  (scriptnum1 <= scriptnum1));

    BOOST_CHECK((bignum1 == bignum1) == (scriptnum1 == num1));
    BOOST_CHECK((bignum1 != bignum1) ==  (scriptnum1 != num1));
    BOOST_CHECK((bignum1 < bignum1) ==  (scriptnum1 < num1));
    BOOST_CHECK((bignum1 > bignum1) ==  (scriptnum1 > num1));
    BOOST_CHECK((bignum1 >= bignum1) ==  (scriptnum1 >= num1));
    BOOST_CHECK((bignum1 <= bignum1) ==  (scriptnum1 <= num1));

    BOOST_CHECK((bignum1 == bignum2) ==  (scriptnum1 == scriptnum2));
    BOOST_CHECK((bignum1 != bignum2) ==  (scriptnum1 != scriptnum2));
    BOOST_CHECK((bignum1 < bignum2) ==  (scriptnum1 < scriptnum2));
    BOOST_CHECK((bignum1 > bignum2) ==  (scriptnum1 > scriptnum2));
    BOOST_CHECK((bignum1 >= bignum2) ==  (scriptnum1 >= scriptnum2));
    BOOST_CHECK((bignum1 <= bignum2) ==  (scriptnum1 <= scriptnum2));

    BOOST_CHECK((bignum1 == bignum2) ==  (scriptnum1 == num2));
    BOOST_CHECK((bignum1 != bignum2) ==  (scriptnum1 != num2));
    BOOST_CHECK((bignum1 < bignum2) ==  (scriptnum1 < num2));
    BOOST_CHECK((bignum1 > bignum2) ==  (scriptnum1 > num2));
    BOOST_CHECK((bignum1 >= bignum2) ==  (scriptnum1 >= num2));
    BOOST_CHECK((bignum1 <= bignum2) ==  (scriptnum1 <= num2));
}

static void RunCreate(const int64_t& num)
{
    CheckCreateInt(num);
    script_number scriptnum(num);
    if (scriptnum.data().size() <= max_script_number_size)
        CheckCreateVch(num);
    else
    {
        // Is this desirable?
        CheckCreateVch_bad_set_data(num);
    }
}

static void RunOperators(const int64_t& num1, const int64_t& num2)
{
    CheckAdd(num1, num2);
    CheckSubtract(num1, num2);
    CheckNegate(num1);
    CheckCompare(num1, num2);
}

BOOST_AUTO_TEST_CASE(creation)
{
    for(size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i)
    {
        for(size_t j = 0; j < sizeof(offsets) / sizeof(offsets[0]); ++j)
        {
            RunCreate(values[i]);
            RunCreate(values[i] + offsets[j]);
            RunCreate(values[i] - offsets[j]);
        }
    }
}

BOOST_AUTO_TEST_CASE(operators)
{
    for(size_t i = 0; i < sizeof(values) / sizeof(values[0]); ++i)
    {
        for(size_t j = 0; j < sizeof(offsets) / sizeof(offsets[0]); ++j)
        {
            RunOperators(values[i], values[i]);
            RunOperators(values[i], -values[i]);
            RunOperators(values[i], values[j]);
            RunOperators(values[i], -values[j]);
            RunOperators(values[i] + values[j], values[j]);
            RunOperators(values[i] + values[j], -values[j]);
            RunOperators(values[i] - values[j], values[j]);
            RunOperators(values[i] - values[j], -values[j]);
            RunOperators(values[i] + values[j], values[i] + values[j]);
            RunOperators(values[i] + values[j], values[i] - values[j]);
            RunOperators(values[i] - values[j], values[i] + values[j]);
            RunOperators(values[i] - values[j], values[i] - values[j]);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
