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

BC_AUTO_TEST_SUITE(number_tests)

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

BC_AUTO_TEST_CASE(number__8_from_chunk__canonical_zero__expected)
{
    int64_t value64{};
    BC_CHECK(number::integer<8>::from_chunk(value64, {}));
    BC_CHECK_EQUAL(value64, 0);
}

BC_AUTO_TEST_CASE(number__from_chunk__positive_zero__expected)
{
    int8_t value8{};
    BC_CHECK(number::integer<1>::from_chunk(value8, { 0x00 }));
    BC_CHECK_EQUAL(value8, 0);
    BC_CHECK(!is_negative(value8));
}

BC_AUTO_TEST_CASE(number__8_from_chunk__positive_1__expected)
{
    int64_t value64{};
    BC_CHECK(number::integer<8>::from_chunk(value64, { 0x01 }));
    BC_CHECK_EQUAL(value64, 1);
}

BC_AUTO_TEST_CASE(number__8_from_chunk__positive_limit__expected)
{
    int64_t value64{};
    BC_CHECK(number::integer<8>::from_chunk(value64, { 0xff, 0xff, 0xff, 0x7f }));
    BC_CHECK_EQUAL(value64, 0x7fffffff);
}

BC_AUTO_TEST_CASE(number__from_chunk__negative_zero__expected)
{
    int8_t value8{};
    BC_CHECK(number::integer<1>::from_chunk(value8, { 0x80 }));
    BC_CHECK_EQUAL(value8, 0);
    BC_CHECK(!is_negative(value8));
}

BC_AUTO_TEST_CASE(number__8_from_chunk__negative_1__expected)
{
    // 1-byte encodings (work with any Size >= 1)
    int64_t value64{};
    BC_CHECK(number::integer<8>::from_chunk(value64, { 0x81 }));
    BC_CHECK(is_negative(value64));
    BC_CHECK_EQUAL(value64, -1);
}

BC_AUTO_TEST_CASE(number__8_from_chunk__negative_2__expected)
{
    int64_t value64{};
    BC_CHECK(number::integer<8>::from_chunk(value64, { 0x82 }));
    BC_CHECK(is_negative(value64));
    BC_CHECK_EQUAL(value64, -2);
}

BC_AUTO_TEST_CASE(number__4_from_chunk__most_negative__expected)
{
    // 4-byte encoding of the most negative value representable with Size == 4
    // (sign bit occupies the high bit of the final byte; magnitude uses the
    // remaining 31 bits). This is the lower bound for integer<4>.
    int32_t value32{};
    BC_CHECK(number::integer<4>::from_chunk(value32, { 0xff, 0xff, 0xff, 0xff }));
    BC_CHECK(is_negative(value32));
    BC_CHECK_EQUAL(value32, -(0x7fffffff_i32));
}

BC_AUTO_TEST_CASE(number__5_from_chunk__min_int32__expected)
{
    // 5-byte encoding of min_int32 (requires the extra sign byte because
    // magnitude bit 31 would otherwise share the sign-bit position).
    // This is representable with Size == 5 but not with Size == 4.
    int64_t value64{};
    BC_CHECK(number::integer<5>::from_chunk(value64, { 0x00, 0x00, 0x00, 0x80, 0x80 }));
    BC_CHECK(is_negative(value64));
    BC_CHECK_EQUAL(value64, 0xffffffff80000000_i64);
}

BC_AUTO_TEST_CASE(number__8_from_chunk__most_negative__expected)
{
    // 8-byte encoding of the most negative value representable with Size == 8
    int64_t value64{};
    BC_CHECK(number::integer<8>::from_chunk(value64, { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }));
    BC_CHECK(is_negative(value64));
    BC_CHECK_EQUAL(value64, -(0x7fffffffffffffff_i64));
}

// from_chunk - overflow and size limits
// -----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(number__from_chunk__overflow__false)
{
    int32_t value32{};
    BC_CHECK(!number::integer<4>::from_chunk(value32, { 0x00, 0x00, 0x00, 0x00, 0x01 }));

    int64_t value64{};
    BC_CHECK(!number::integer<8>::from_chunk(value64, data_chunk(9, 0x00)));
}

// from_integer / round-trip
// -----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(number__from_integer__round_trip__zero)
{
    int64_t restored{};
    const auto bytes = number::chunk::from_integer(0);
    BC_CHECK(number::integer<8>::from_chunk(restored, bytes));
    BC_CHECK_EQUAL(restored, 0);
}

BC_AUTO_TEST_CASE(number__from_integer__round_trip__positive_one)
{
    int64_t restored{};
    const auto bytes = number::chunk::from_integer(1);
    BC_CHECK(number::integer<8>::from_chunk(restored, bytes));
    BC_CHECK_EQUAL(restored, 1);
}

BC_AUTO_TEST_CASE(number__from_integer__round_trip__negative_one)
{
    int64_t restored{};
    const auto bytes = number::chunk::from_integer(-1);
    BC_CHECK(number::integer<8>::from_chunk(restored, bytes));
    BC_CHECK_EQUAL(restored, -1);
}

BC_AUTO_TEST_CASE(number__from_integer__round_trip__positive_two)
{
    int64_t restored{};
    const auto bytes = number::chunk::from_integer(2);
    BC_CHECK(number::integer<8>::from_chunk(restored, bytes));
    BC_CHECK_EQUAL(restored, 2);
}

BC_AUTO_TEST_CASE(number__from_integer__round_trip__negative_two)
{
    int64_t restored{};
    const auto bytes = number::chunk::from_integer(-2);
    BC_CHECK(number::integer<8>::from_chunk(restored, bytes));
    BC_CHECK_EQUAL(restored, -2);
}

BC_AUTO_TEST_CASE(number__from_integer__round_trip__int32_max)
{
    int64_t restored{};
    const auto bytes = number::chunk::from_integer(0x7fffffff);
    BC_CHECK(number::integer<8>::from_chunk(restored, bytes));
    BC_CHECK_EQUAL(restored, 0x7fffffff);
}

BC_AUTO_TEST_CASE(number__from_integer__round_trip__int32_min)
{
    int64_t restored{};
    const auto bytes = number::chunk::from_integer(-2147483648_i64);
    BC_CHECK(number::integer<8>::from_chunk(restored, bytes));
    BC_CHECK_EQUAL(restored, -2147483648_i64);
}

BC_AUTO_TEST_CASE(number__from_integer__round_trip__int64_max)
{
    int64_t restored{};
    const auto bytes = number::chunk::from_integer(0x7fffffffffffffff_i64);
    BC_CHECK(number::integer<8>::from_chunk(restored, bytes));
    BC_CHECK_EQUAL(restored, 0x7fffffffffffffff_i64);
}

// Causes an implementation assertion.
// min_int64 is out of range because of the 1 bit domain reduction (+0/-0).
////BC_AUTO_TEST_CASE(number__from_integer__round_trip__int64_min_fails)
////{
////    int64_t restored{};
////    const auto bytes = number::chunk::from_integer(min_int64);
////
////    
////    BC_CHECK(!number::integer<8>::from_chunk(restored, bytes));
////}

// is_overflow (protected)
// -----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(number__is_overflow__chunk__expected)
{
    BC_CHECK(!number_test_accessor<8>::is_overflow(data_chunk{}));
    BC_CHECK(!number_test_accessor<8>::is_overflow(data_chunk(8, 0x00)));
    BC_CHECK( number_test_accessor<8>::is_overflow(data_chunk(9, 0x00)));
}

BC_AUTO_TEST_CASE(number__is_overflow__int64__expected)
{
    BC_CHECK(!number_test_accessor<8>::is_overflow(0_i64));
    BC_CHECK(!number_test_accessor<8>::is_overflow(1_i64));
    BC_CHECK(!number_test_accessor<8>::is_overflow(-1_i64));
    BC_CHECK(!number_test_accessor<8>::is_overflow(max_int64));

    // Causes an implementation assertion.
    // min_int64 is out of range because of the 1 bit domain reduction (+0/-0).
    ////BC_CHECK( number_test_accessor<8>::is_overflow(min_int64));
}

// Zero handling (strict zero vs semantic zero)
// -----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(number__from_chunk__strict_zero__true)
{
    int64_t value{};
    BC_CHECK(number::integer<8>::from_chunk(value, {}));
    BC_CHECK_EQUAL(value, 0);
}

BC_AUTO_TEST_CASE(number__from_chunk__positive_zero__true)
{
    int64_t value{};
    BC_CHECK(number::integer<8>::from_chunk(value, { 0x00 }));
    BC_CHECK_EQUAL(value, 0);
}

BC_AUTO_TEST_CASE(number__from_chunk__negative_zero__true)
{
    int64_t value{};
    BC_CHECK(number::integer<8>::from_chunk(value, { 0x80 }));
    BC_CHECK_EQUAL(value, 0);
}

// boolean::from_chunk - non-minimal (any chunk is a valid boolean)
// -----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(number__boolean_from_chunk__empty__false)
{
    BC_CHECK(!number::boolean::from_chunk({}));
}

BC_AUTO_TEST_CASE(number__boolean_from_chunk__zeros__false)
{
    BC_CHECK(!number::boolean::from_chunk({ 0x00 }));
    BC_CHECK(!number::boolean::from_chunk({ 0x80 }));
    BC_CHECK(!number::boolean::from_chunk({ 0x00, 0x00 }));
    BC_CHECK(!number::boolean::from_chunk({ 0x00, 0x80 }));
}

BC_AUTO_TEST_CASE(number__boolean_from_chunk__nonzeros__true)
{
    BC_CHECK(number::boolean::from_chunk({ 0x01 }));
    BC_CHECK(number::boolean::from_chunk({ 0x02 }));
    BC_CHECK(number::boolean::from_chunk({ 0x00, 0x01 }));
    BC_CHECK(number::boolean::from_chunk({ 0x01, 0x00 }));
    BC_CHECK(number::boolean::from_chunk({ 0x01, 0x80 }));
}

// boolean::from_chunk - minimal [bip342], chunk must be [] or [0x01]
// -----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(number__boolean_minimal_from_chunk__empty__true_false)
{
    bool value{ true };
    BC_CHECK(number::boolean::from_chunk(value, {}));
    BC_CHECK(!value);
}

BC_AUTO_TEST_CASE(number__boolean_minimal_from_chunk__one__true_true)
{
    bool value{};
    BC_CHECK(number::boolean::from_chunk(value, { 0x01 }));
    BC_CHECK(value);
}

BC_AUTO_TEST_CASE(number__boolean_minimal_from_chunk__positive_zero__false)
{
    // [0x00] is a non-minimal false, must be [].
    bool value{ true };
    BC_CHECK(!number::boolean::from_chunk(value, { 0x00 }));
    BC_CHECK(!value);
}

BC_AUTO_TEST_CASE(number__boolean_minimal_from_chunk__negative_zero__false)
{
    // [0x80] is a non-minimal false, must be [].
    bool value{ true };
    BC_CHECK(!number::boolean::from_chunk(value, { 0x80 }));
    BC_CHECK(!value);
}

BC_AUTO_TEST_CASE(number__boolean_minimal_from_chunk__negative_one__false)
{
    // [0x81] is a non-minimal true, must be [0x01].
    bool value{};
    BC_CHECK(!number::boolean::from_chunk(value, { 0x81 }));
    BC_CHECK(value);
}

BC_AUTO_TEST_CASE(number__boolean_minimal_from_chunk__two__false)
{
    // [0x02] is a non-minimal true, must be [0x01].
    bool value{};
    BC_CHECK(!number::boolean::from_chunk(value, { 0x02 }));
    BC_CHECK(value);
}

BC_AUTO_TEST_CASE(number__boolean_minimal_from_chunk__one_with_trailing_bytes__false)
{
    // All bytes are material, the leading 0x01 is insufficient.
    bool value{};
    BC_CHECK(!number::boolean::from_chunk(value, { 0x01, 0x00 }));
    BC_CHECK(value);

    value = false;
    BC_CHECK(!number::boolean::from_chunk(value, { 0x01, 0x80 }));
    BC_CHECK(value);

    value = false;
    BC_CHECK(!number::boolean::from_chunk(value, { 0x01, 0x00, 0x00, 0x00 }));
    BC_CHECK(value);
}

BC_AUTO_TEST_CASE(number__boolean_minimal_from_chunk__zero_with_trailing_bytes__false)
{
    bool value{ true };
    BC_CHECK(!number::boolean::from_chunk(value, { 0x00, 0x00 }));
    BC_CHECK(!value);

    value = false;
    BC_CHECK(!number::boolean::from_chunk(value, { 0x00, 0x01 }));
    BC_CHECK(value);
}

// boolean::from_integer - minimal [bip342], integer must be 0 or 1
// -----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(number__boolean_minimal_from_integer__zero_one__true)
{
    bool value{ true };
    BC_CHECK(number::boolean::from_integer(value, 0));
    BC_CHECK(!value);

    value = false;
    BC_CHECK(number::boolean::from_integer(value, 1));
    BC_CHECK(value);
}

BC_AUTO_TEST_CASE(number__boolean_minimal_from_integer__not_zero_one__false)
{
    bool value{};
    BC_CHECK(!number::boolean::from_integer(value, 2));
    BC_CHECK(value);

    value = false;
    BC_CHECK(!number::boolean::from_integer(value, -1));
    BC_CHECK(value);

    value = true;
    BC_CHECK(!number::boolean::from_integer(value, max_int64));
    BC_CHECK(value);
}

// boolean::from_chunk_strict - strict [bip147], false must be []
// -----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(number__boolean_from_chunk_strict__empty__false)
{
    BC_CHECK(!number::boolean::from_chunk_strict({}));
}

BC_AUTO_TEST_CASE(number__boolean_from_chunk_strict__not_empty__true)
{
    BC_CHECK(number::boolean::from_chunk_strict({ 0x00 }));
    BC_CHECK(number::boolean::from_chunk_strict({ 0x80 }));
    BC_CHECK(number::boolean::from_chunk_strict({ 0x01 }));
}

// integer from_integer - the domain check applied on every arithmetic pop
// -----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(number__4_from_integer__max_int32__true)
{
    int32_t value32{};
    BC_CHECK(number::integer<4>::from_integer(value32, max_int32));
    BC_CHECK_EQUAL(value32, max_int32);
}

BC_AUTO_TEST_CASE(number__4_from_integer__negated_max_int32__true)
{
    int32_t value32{};
    BC_CHECK(number::integer<4>::from_integer(value32, -max_int32));
    BC_CHECK_EQUAL(value32, -max_int32);
}

BC_AUTO_TEST_CASE(number__4_from_integer__five_byte_value__false)
{
    constexpr auto five_byte = add1<int64_t>(max_int32);
    int32_t value32{};
    BC_CHECK(!number::integer<4>::from_integer(value32, five_byte));
}

BC_AUTO_TEST_CASE(number__4_from_integer__min_int32__false)
{
    int32_t value32{};
    BC_CHECK(!number::integer<4>::from_integer(value32, min_int32));
}

BC_AUTO_TEST_CASE(number__5_from_integer__five_byte_maximum__true)
{
    constexpr auto maximum40 = sub1(power2<int64_t>(39u));
    int64_t value64{};
    BC_CHECK(number::integer<5>::from_integer(value64, maximum40));
    BC_CHECK_EQUAL(value64, maximum40);
}

BC_AUTO_TEST_CASE(number__5_from_integer__four_byte_overflow__true)
{
    constexpr auto five_byte = add1<int64_t>(max_int32);
    int64_t value64{};
    BC_CHECK(number::integer<5>::from_integer(value64, five_byte));
    BC_CHECK_EQUAL(value64, five_byte);
}

BC_AUTO_TEST_CASE(number__5_from_integer__six_byte_value__false)
{
    constexpr auto six_byte = power2<int64_t>(39u);
    int64_t value64{};
    BC_CHECK(!number::integer<5>::from_integer(value64, six_byte));
}

// chunk from_integer - encoded bytes (not round-tripped)
// -----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(number__chunk_from_integer__zero__empty)
{
    BC_CHECK_EQUAL(number::chunk::from_integer(0), data_chunk{});
}

BC_AUTO_TEST_CASE(number__chunk_from_integer__one__expected)
{
    BC_CHECK_EQUAL(number::chunk::from_integer(1), base16_chunk("01"));
}

BC_AUTO_TEST_CASE(number__chunk_from_integer__negative_one__expected)
{
    BC_CHECK_EQUAL(number::chunk::from_integer(-1), base16_chunk("81"));
}

BC_AUTO_TEST_CASE(number__chunk_from_integer__high_bit_clear__expected)
{
    BC_CHECK_EQUAL(number::chunk::from_integer(127), base16_chunk("7f"));
}

BC_AUTO_TEST_CASE(number__chunk_from_integer__negated_positive__positive_sign_byte)
{
    BC_CHECK_EQUAL(number::chunk::from_integer(128), base16_chunk("8000"));
}

BC_AUTO_TEST_CASE(number__chunk_from_integer__negated_negative__negative_sign_byte)
{
    BC_CHECK_EQUAL(number::chunk::from_integer(-128), base16_chunk("8080"));
}

BC_AUTO_TEST_CASE(number__chunk_from_integer__negative_sign_folded__expected)
{
    BC_CHECK_EQUAL(number::chunk::from_integer(-127), base16_chunk("ff"));
}

BC_AUTO_TEST_CASE(number__chunk_from_integer__two_bytes__little_endian)
{
    BC_CHECK_EQUAL(number::chunk::from_integer(256), base16_chunk("0001"));
}

// chunk from_bool
// -----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(number__chunk_from_bool__true__expected)
{
    BC_CHECK_EQUAL(number::chunk::from_bool(true), base16_chunk("01"));
}

BC_AUTO_TEST_CASE(number__chunk_from_bool__false__empty)
{
    BC_CHECK_EQUAL(number::chunk::from_bool(false), data_chunk{});
}

// chunk::is_minimal

BC_AUTO_TEST_CASE(number__chunk_is_minimal__empty__true)
{
    BC_REQUIRE(number::chunk::is_minimal({}));
}

BC_AUTO_TEST_CASE(number__chunk_is_minimal__magnitude_byte__true)
{
    BC_REQUIRE(number::chunk::is_minimal(base16_chunk("01")));
    BC_REQUIRE(number::chunk::is_minimal(base16_chunk("7f")));
    BC_REQUIRE(number::chunk::is_minimal(base16_chunk("81")));
    BC_REQUIRE(number::chunk::is_minimal(base16_chunk("ff")));
}

BC_AUTO_TEST_CASE(number__chunk_is_minimal__zero__false)
{
    BC_REQUIRE(!number::chunk::is_minimal(base16_chunk("00")));
    BC_REQUIRE(!number::chunk::is_minimal(base16_chunk("80")));
    BC_REQUIRE(!number::chunk::is_minimal(base16_chunk("0000")));
}

BC_AUTO_TEST_CASE(number__chunk_is_minimal__required_sign_byte__true)
{
    BC_REQUIRE(number::chunk::is_minimal(base16_chunk("8000")));
    BC_REQUIRE(number::chunk::is_minimal(base16_chunk("8080")));
    BC_REQUIRE(number::chunk::is_minimal(base16_chunk("ff7f00ff00")));
}

BC_AUTO_TEST_CASE(number__chunk_is_minimal__padded__false)
{
    BC_REQUIRE(!number::chunk::is_minimal(base16_chunk("6400")));
    BC_REQUIRE(!number::chunk::is_minimal(base16_chunk("6480")));
    BC_REQUIRE(!number::chunk::is_minimal(base16_chunk("010000")));
    BC_REQUIRE(!number::chunk::is_minimal(base16_chunk("800000")));
}

BC_AUTO_TEST_CASE(number__chunk_is_minimal__from_integer__true)
{
    BC_REQUIRE(number::chunk::is_minimal(number::chunk::from_integer(0)));
    BC_REQUIRE(number::chunk::is_minimal(number::chunk::from_integer(128)));
    BC_REQUIRE(number::chunk::is_minimal(number::chunk::from_integer(-128)));
    BC_REQUIRE(number::chunk::is_minimal(number::chunk::from_integer(max_int32)));
    BC_REQUIRE(number::chunk::is_minimal(number::chunk::from_integer(min_int32 + 1)));
}

BC_AUTO_TEST_SUITE_END()
