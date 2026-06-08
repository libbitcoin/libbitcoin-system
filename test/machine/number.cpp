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

BOOST_AUTO_TEST_SUITE(number_tests)

using namespace system::machine;

template <size_t Size>
class number_test_accessor
  : public number::integer<Size>
{
public:
    using base = number::integer<Size>;
    using base::strict_zero;
    using base::is_overflow;
};

// from_chunk - basic parsing
// -----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(number__8_from_chunk__canonical_zero__expected)
{
    int64_t value64{};
    BOOST_CHECK(number::integer<8>::from_chunk(value64, {}));
    BOOST_CHECK_EQUAL(value64, 0);
}

BOOST_AUTO_TEST_CASE(number__from_chunk__positive_zero__expected)
{
    int8_t value8{};
    BOOST_CHECK(number::integer<1>::from_chunk(value8, { 0x00 }));
    BOOST_CHECK_EQUAL(value8, 0);
    BOOST_CHECK(!is_negative(value8));
}

BOOST_AUTO_TEST_CASE(number__8_from_chunk__positive_1__expected)
{
    int64_t value64{};
    BOOST_CHECK(number::integer<8>::from_chunk(value64, { 0x01 }));
    BOOST_CHECK_EQUAL(value64, 1);
}

BOOST_AUTO_TEST_CASE(number__8_from_chunk__positive_limit__expected)
{
    int64_t value64{};
    BOOST_CHECK(number::integer<8>::from_chunk(value64, { 0xff, 0xff, 0xff, 0x7f }));
    BOOST_CHECK_EQUAL(value64, 0x7fffffff);
}

BOOST_AUTO_TEST_CASE(number__from_chunk__negative_zero__expected)
{
    int8_t value8{};
    BOOST_CHECK(number::integer<1>::from_chunk(value8, { 0x80 }));
    BOOST_CHECK_EQUAL(value8, 0);
    BOOST_CHECK(!is_negative(value8));
}

BOOST_AUTO_TEST_CASE(number__8_from_chunk__negative_1__expected)
{
    // 1-byte encodings (work with any Size >= 1)
    int64_t value64{};
    BOOST_CHECK(number::integer<8>::from_chunk(value64, { 0x81 }));
    BOOST_CHECK(is_negative(value64));
    BOOST_CHECK_EQUAL(value64, -1);
}

BOOST_AUTO_TEST_CASE(number__8_from_chunk__negative_2__expected)
{
    int64_t value64{};
    BOOST_CHECK(number::integer<8>::from_chunk(value64, { 0x82 }));
    BOOST_CHECK(is_negative(value64));
    BOOST_CHECK_EQUAL(value64, -2);
}

BOOST_AUTO_TEST_CASE(number__4_from_chunk__most_negative__expected)
{
    // 4-byte encoding of the most negative value representable with Size == 4
    // (sign bit occupies the high bit of the final byte; magnitude uses the
    // remaining 31 bits). This is the lower bound for integer<4>.
    int32_t value32{};
    BOOST_CHECK(number::integer<4>::from_chunk(value32, { 0xff, 0xff, 0xff, 0xff }));
    BOOST_CHECK(is_negative(value32));
    BOOST_CHECK_EQUAL(value32, -(0x7fffffff_i32));
}

BOOST_AUTO_TEST_CASE(number__5_from_chunk__min_int32__expected)
{
    // 5-byte encoding of min_int32 (requires the extra sign byte because
    // magnitude bit 31 would otherwise share the sign-bit position).
    // This is representable with Size == 5 but not with Size == 4.
    int64_t value64{};
    BOOST_CHECK(number::integer<5>::from_chunk(value64, { 0x00, 0x00, 0x00, 0x80, 0x80 }));
    BOOST_CHECK(is_negative(value64));
    BOOST_CHECK_EQUAL(value64, 0xffffffff80000000_i64);
}

BOOST_AUTO_TEST_CASE(number__8_from_chunk__most_negative__expected)
{
    // 8-byte encoding of the most negative value representable with Size == 8
    int64_t value64{};
    BOOST_CHECK(number::integer<8>::from_chunk(value64, { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }));
    BOOST_CHECK(is_negative(value64));
    BOOST_CHECK_EQUAL(value64, -(0x7fffffffffffffff_i64));
}

// from_chunk - overflow and size limits
// -----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(number__from_chunk__overflow__false)
{
    int32_t value32{};
    BOOST_CHECK(!number::integer<4>::from_chunk(value32, { 0x00, 0x00, 0x00, 0x00, 0x01 }));

    int64_t value64{};
    BOOST_CHECK(!number::integer<8>::from_chunk(value64, data_chunk(9, 0x00)));
}

// from_integer / round-trip
// -----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(number__from_integer__round_trip__zero)
{
    int64_t restored{};
    const auto bytes = number::chunk::from_integer(0);
    BOOST_CHECK(number::integer<8>::from_chunk(restored, bytes));
    BOOST_CHECK_EQUAL(restored, 0);
}

BOOST_AUTO_TEST_CASE(number__from_integer__round_trip__positive_one)
{
    int64_t restored{};
    const auto bytes = number::chunk::from_integer(1);
    BOOST_CHECK(number::integer<8>::from_chunk(restored, bytes));
    BOOST_CHECK_EQUAL(restored, 1);
}

BOOST_AUTO_TEST_CASE(number__from_integer__round_trip__negative_one)
{
    int64_t restored{};
    const auto bytes = number::chunk::from_integer(-1);
    BOOST_CHECK(number::integer<8>::from_chunk(restored, bytes));
    BOOST_CHECK_EQUAL(restored, -1);
}

BOOST_AUTO_TEST_CASE(number__from_integer__round_trip__positive_two)
{
    int64_t restored{};
    const auto bytes = number::chunk::from_integer(2);
    BOOST_CHECK(number::integer<8>::from_chunk(restored, bytes));
    BOOST_CHECK_EQUAL(restored, 2);
}

BOOST_AUTO_TEST_CASE(number__from_integer__round_trip__negative_two)
{
    int64_t restored{};
    const auto bytes = number::chunk::from_integer(-2);
    BOOST_CHECK(number::integer<8>::from_chunk(restored, bytes));
    BOOST_CHECK_EQUAL(restored, -2);
}

BOOST_AUTO_TEST_CASE(number__from_integer__round_trip__int32_max)
{
    int64_t restored{};
    const auto bytes = number::chunk::from_integer(0x7fffffff);
    BOOST_CHECK(number::integer<8>::from_chunk(restored, bytes));
    BOOST_CHECK_EQUAL(restored, 0x7fffffff);
}

BOOST_AUTO_TEST_CASE(number__from_integer__round_trip__int32_min)
{
    int64_t restored{};
    const auto bytes = number::chunk::from_integer(-2147483648_i64);
    BOOST_CHECK(number::integer<8>::from_chunk(restored, bytes));
    BOOST_CHECK_EQUAL(restored, -2147483648_i64);
}

BOOST_AUTO_TEST_CASE(number__from_integer__round_trip__int64_max)
{
    int64_t restored{};
    const auto bytes = number::chunk::from_integer(0x7fffffffffffffff_i64);
    BOOST_CHECK(number::integer<8>::from_chunk(restored, bytes));
    BOOST_CHECK_EQUAL(restored, 0x7fffffffffffffff_i64);
}

BOOST_AUTO_TEST_CASE(number__from_integer__round_trip__int64_min_fails)
{
    int64_t restored{};
    const auto bytes = number::chunk::from_integer(min_int64);

    // min_int64 is out of range because of the 1 bit domain reduction (+0/-0).
    BOOST_CHECK(!number::integer<8>::from_chunk(restored, bytes));
}

// is_overflow (protected)
// -----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(number__is_overflow__chunk__expected)
{
    BOOST_CHECK(!number_test_accessor<8>::is_overflow(data_chunk{}));
    BOOST_CHECK(!number_test_accessor<8>::is_overflow(data_chunk(8, 0x00)));
    BOOST_CHECK( number_test_accessor<8>::is_overflow(data_chunk(9, 0x00)));
}

BOOST_AUTO_TEST_CASE(number__is_overflow__int64__expected)
{
    BOOST_CHECK(!number_test_accessor<8>::is_overflow(0_i64));
    BOOST_CHECK(!number_test_accessor<8>::is_overflow(1_i64));
    BOOST_CHECK(!number_test_accessor<8>::is_overflow(-1_i64));
    BOOST_CHECK(!number_test_accessor<8>::is_overflow(max_int64));

    // min_int64 is out of range because of the 1 bit domain reduction (+0/-0).
    BOOST_CHECK( number_test_accessor<8>::is_overflow(min_int64));
}

// Zero handling (strict zero vs semantic zero)
// -----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(number__from_chunk__strict_zero__true)
{
    int64_t value{};
    BOOST_CHECK(number::integer<8>::from_chunk(value, {}));
    BOOST_CHECK_EQUAL(value, 0);
}

BOOST_AUTO_TEST_CASE(number__from_chunk__positive_zero__true)
{
    int64_t value{};
    BOOST_CHECK(number::integer<8>::from_chunk(value, { 0x00 }));
    BOOST_CHECK_EQUAL(value, 0);
}

BOOST_AUTO_TEST_CASE(number__from_chunk__negative_zero__true)
{
    int64_t value{};
    BOOST_CHECK(number::integer<8>::from_chunk(value, { 0x80 }));
    BOOST_CHECK_EQUAL(value, 0);
}

BOOST_AUTO_TEST_SUITE_END()
