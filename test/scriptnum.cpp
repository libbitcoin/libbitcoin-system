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
#include <bitcoin/utility/assert.hpp>
#include <bitcoin/utility/big_number.hpp>
#include <bitcoin/format.hpp>

using namespace libbitcoin;

BOOST_AUTO_TEST_SUITE(scriptnum_tests)

static const int64_t values[] = \
{ 0, 1, CHAR_MIN, CHAR_MAX, UCHAR_MAX, SHRT_MIN, USHRT_MAX, INT_MIN, INT_MAX, UINT_MAX, LONG_MIN, LONG_MAX };
static const int64_t offsets[] = { 1, 0x79, 0x80, 0x81, 0xFF, 0x7FFF, 0x8000, 0xFFFF, 0x10000};

constexpr size_t max_script_number_size = 4;

/**
 * Numeric opcodes (OP_1ADD, etc) are restricted to operating on
 * 4-byte integers. The semantics are subtle, though: operands must be
 * in the range [-2^31 +1...2^31 -1], but results may overflow (and are
 * valid as long as they are not used in a subsequent numeric operation).
 *
 * script_number enforces those semantics by storing results as
 * an int64 and allowing out-of-range values to be returned as a vector of
 * bytes but throwing an exception if arithmetic is done or the result is
 * interpreted as an integer.
 */
class script_number
{
public:
    explicit script_number(const int64_t value);
    // Undefined state. set_data() must be called after.
    script_number();

    bool set_data(const data_chunk& data);
    data_chunk data() const;

    int32_t int32() const;

    // Arithmetic with a number.
    script_number operator+(const int64_t value) const;
    script_number operator-(const int64_t value) const;
    // Arithmetic with another script_number.
    script_number operator+(const script_number& other) const;
    script_number operator-(const script_number& other) const;
    // -script_number
    script_number operator-() const;

    // Comparison operators with a number.
    bool operator==(const int64_t value) const;
    bool operator!=(const int64_t value) const;
    bool operator<=(const int64_t value) const;
    bool operator< (const int64_t value) const;
    bool operator>=(const int64_t value) const;
    bool operator> (const int64_t value) const;
    // Comparison operators with another script_number.
    bool operator==(const script_number& other) const;
    bool operator!=(const script_number& other) const;
    bool operator<=(const script_number& other) const;
    bool operator<(const script_number& other) const;
    bool operator>=(const script_number& other) const;
    bool operator>(const script_number& other) const;

private:
    int64_t value_;
};

data_chunk script_number_serialize(const int64_t value)
{
    if(value == 0)
        return data_chunk();

    data_chunk result;
    const bool is_negative = value < 0;
    uint64_t abs_value = is_negative ? -value : value;

    while(abs_value)
    {
        result.push_back(abs_value & 0xff);
        abs_value >>= 8;
    }


    // - If the most significant byte is >= 0x80 and the value is positive,
    // push a new zero-byte to make the significant byte < 0x80 again.

    // - If the most significant byte is >= 0x80 and the value is negative,
    // push a new 0x80 byte that will be popped off when converting to
    // an integral.

    // - If the most significant byte is < 0x80 and the value is negative,
    // add 0x80 to it, since it will be subtracted and interpreted as
    // a negative when converting to an integral.

    if (result.back() & 0x80)
        result.push_back(is_negative ? 0x80 : 0);
    else if (is_negative)
        result.back() |= 0x80;

    return result;
}

int64_t script_number_deserialize(const data_chunk& data)
{
    BITCOIN_ASSERT(data.size() <= max_script_number_size);
    if (data.empty())
        return 0;

    int64_t result = 0;
    for (size_t i = 0; i != data.size(); ++i)
    {
        result |= static_cast<int64_t>(data[i]) << 8 * i;
    }

    // If the input vector's most significant byte is 0x80, remove it from
    // the result's msb and return a negative.
    if (data.back() & 0x80)
        return -(result & ~(0x80 << (8 * (data.size() - 1))));

    return result;
}

script_number::script_number(const int64_t value)
  : value_(value)
{
}
script_number::script_number()
{
}

bool script_number::set_data(const data_chunk& data)
{
    if (data.size() > max_script_number_size)
        return false;
    value_ = script_number_deserialize(data);
    return true;
}
data_chunk script_number::data() const
{
    return script_number_serialize(value_);
}

int32_t script_number::int32() const
{
    constexpr int32_t max_int32 = std::numeric_limits<int32_t>::max();
    constexpr int32_t min_int32 = std::numeric_limits<int32_t>::min();
    if (value_ > max_int32)
        return max_int32;
    else if (value_ < min_int32)
        return min_int32;
    return value_;
}

bool script_number::operator==(const int64_t value) const
{
    return value_ == value;
}
bool script_number::operator!=(const int64_t value) const
{
    return value_ != value;
}
bool script_number::operator<=(const int64_t value) const
{
    return value_ <= value;
}
bool script_number::operator< (const int64_t value) const
{
    return value_ <  value;
}
bool script_number::operator>=(const int64_t value) const
{
    return value_ >= value;
}
bool script_number::operator> (const int64_t value) const
{
    return value_ >  value;
}

bool script_number::operator==(const script_number& other) const
{
    return operator==(other.value_);
}
bool script_number::operator!=(const script_number& other) const
{
    return operator!=(other.value_);
}
bool script_number::operator<=(const script_number& other) const
{
    return operator<=(other.value_);
}
bool script_number::operator<(const script_number& other) const
{
    return operator<(other.value_);
}
bool script_number::operator>=(const script_number& other) const
{
    return operator>=(other.value_);
}
bool script_number::operator>(const script_number& other) const
{
    return operator>(other.value_);
}

script_number script_number::operator+(const int64_t value) const
{
    return script_number(value_ + value);
}

script_number script_number::operator-(const int64_t value) const
{
    return script_number(value_ - value);
}

script_number script_number::operator+(const script_number& other) const
{
    return operator+(other.value_);
}

script_number script_number::operator-(const script_number& other) const
{
    return operator-(other.value_);
}

#if 0
inline CScriptNum& operator+=( const CScriptNum& rhs)       { return operator+=(rhs.m_value);  }
inline CScriptNum& operator-=( const CScriptNum& rhs)       { return operator-=(rhs.m_value);  }

#endif

script_number script_number::operator-() const
{
    BITCOIN_ASSERT(value_ != std::numeric_limits<int64_t>::min());
    return script_number(-value_);
}

#if 0

inline CScriptNum& operator=( const int64_t& rhs)
{
    m_value = rhs;
    return *this;
}

inline CScriptNum& operator+=( const int64_t& rhs)
{
    assert(rhs == 0 || (rhs > 0 && m_value <= std::numeric_limits<int64_t>::max() - rhs) ||
                       (rhs < 0 && m_value >= std::numeric_limits<int64_t>::min() - rhs));
    m_value += rhs;
    return *this;
}

inline CScriptNum& operator-=( const int64_t& rhs)
{
    assert(rhs == 0 || (rhs > 0 && m_value >= std::numeric_limits<int64_t>::min() + rhs) ||
                       (rhs < 0 && m_value <= std::numeric_limits<int64_t>::max() + rhs));
    m_value -= rhs;
    return *this;
}
#endif

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

