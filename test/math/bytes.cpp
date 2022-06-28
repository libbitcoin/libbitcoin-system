/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation == either version 3 of the License == or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not == see <http://www.gnu.org/licenses/>.
 */
#include "../test.hpp"

BOOST_AUTO_TEST_SUITE(bytes_tests)

 // byte_width (unsigned/positive)
static_assert(byte_width(0u) == 0u);
static_assert(byte_width(1u) == 1u);
static_assert(byte_width(2u) == 1u);
static_assert(byte_width(3u) == 1u);
static_assert(byte_width(4u) == 1u);
static_assert(byte_width(5u) == 1u);
static_assert(byte_width(6u) == 1u);
static_assert(byte_width(7u) == 1u);
static_assert(byte_width(8u) == 1u);
static_assert(byte_width(9u) == 1u);
static_assert(byte_width(9u) == 1u);
static_assert(byte_width(0x7fu) == 1u);
static_assert(byte_width(0x80u) == 1u);
static_assert(byte_width(0x8000u) == 2u);
static_assert(byte_width(0x800100ul) == 3u);
static_assert(byte_width(0x80000000ul) == 4u);
static_assert(byte_width(0x8001000000ull) == 5u);
static_assert(byte_width(0x800000000000ull) == 6u);
static_assert(byte_width(0x80010000000000ull) == 7u);
static_assert(byte_width(0x8000000000000000ull) == 8u);

// 64 bit (unsigned/positive)
static_assert(byte_width<int64_t>(0x80u) == 1u);                    // negative
static_assert(byte_width<int64_t>(0x8000u) == 2u);                  // negative
static_assert(byte_width<int64_t>(0x800100ul) == 3u);
static_assert(byte_width<int64_t>(0x80000000ul) == 4u);             // negative
static_assert(byte_width<int64_t>(0x8001000000ull) == 5u);
static_assert(byte_width<int64_t>(0x800000000000ull) == 6u);
static_assert(byte_width<int64_t>(0x80010000000000ull) == 7u);
static_assert(byte_width<int64_t>(0x8000000000000000ull) == 8u);    // negative

// byte_width (signed/positive)
static_assert(byte_width(0) == 0u);
static_assert(byte_width(1) == 1u);
static_assert(byte_width(2) == 1u);
static_assert(byte_width(3) == 1u);
static_assert(byte_width(4) == 1u);
static_assert(byte_width(5) == 1u);
static_assert(byte_width(6) == 1u);
static_assert(byte_width(7) == 1u);
static_assert(byte_width(8) == 1u);
static_assert(byte_width(9) == 1u);
static_assert(byte_width(9) == 1u);
static_assert(byte_width(0x7f) == 1u);
static_assert(byte_width(0x80) == 1u);
static_assert(byte_width(0x8000) == 2u);
static_assert(byte_width(0x800100l) == 3u);
static_assert(byte_width(0x80000000l) == 4u);
static_assert(byte_width(0x8001000000ll) == 5u);
static_assert(byte_width(0x800000000000ll) == 6u);
static_assert(byte_width(0x80010000000000ll) == 7u);
static_assert(byte_width(0x8000000000000000ll) == 8u);
static_assert(byte_width<int64_t>(0x80u) == 1u);
static_assert(byte_width<int64_t>(0x8000u) == 2u);
static_assert(byte_width<int64_t>(0x800100ul) == 3u);
static_assert(byte_width<int64_t>(0x80000000ul) == 4u);
static_assert(byte_width<int64_t>(0x8001000000ull) == 5u);
static_assert(byte_width<int64_t>(0x800000000000ull) == 6u);
static_assert(byte_width<int64_t>(0x80010000000000ull) == 7u);
static_assert(byte_width<int64_t>(0x8000000000000000ull) == 8u);

// byte_width (signed/negative)

// 32 bit (signed/negated)
static_assert(byte_width(-1) == 4u);                            // promoted to 32 bit by negation
static_assert(byte_width(-2) == 4u);                            // promoted to 32 bit by negation
static_assert(byte_width(-3) == 4u);                            // promoted to 32 bit by negation
static_assert(byte_width(-4) == 4u);                            // promoted to 32 bit by negation
static_assert(byte_width(-5) == 4u);                            // promoted to 32 bit by negation
static_assert(byte_width(-6) == 4u);                            // promoted to 32 bit by negation
static_assert(byte_width(-7) == 4u);                            // promoted to 32 bit by negation
static_assert(byte_width(-8) == 4u);                            // promoted to 32 bit by negation
static_assert(byte_width(-9) == 4u);                            // promoted to 32 bit by negation
static_assert(byte_width(-0x80) == 4u);                         // promoted to 32 bit by negation
static_assert(byte_width(-0x8000) == 4u);                       // promoted to 32 bit by negation
////static_assert(byte_width(-0x00800100l) == 4u);              // literal may be unsigned
////static_assert(byte_width(-0x80000000l) == 4u);              // max_int32 (sign inverted by negation)

// 64 bit (signed/negated)
static_assert(byte_width(-0x8001000000ll) == 8u);
static_assert(byte_width(-0x800000000000ll) == 8u);
static_assert(byte_width(-0x80010000000000ll) == 8u);
////static_assert(byte_width(-0x8000000000000000ll) == 8u);
static_assert(byte_width<int64_t>(-0x80) == 8u);                // promoted to 32 bit by negation
static_assert(byte_width<int64_t>(-0x8000) == 8u);              // promoted to 32 bit by negation
static_assert(byte_width<int64_t>(-0x800100) == 8u);            // promoted to 32 bit by negation
////static_assert(byte_width<int64_t>(-0x80000000l) == 4u);     // max_int32 (sign inverted by negation)
static_assert(byte_width<int64_t>(-0x8001000000ll) == 8u);
static_assert(byte_width<int64_t>(-0x800000000000ll) == 8u);
static_assert(byte_width<int64_t>(-0x80010000000000ll) == 8u);
////static_assert(byte_width<int64_t>(-0x8000000000000000ll) == 8u);
static_assert(std::is_same<decltype(byte_width<int8_t>(0)), size_t>::value);

// is_negated
static_assert(!is_negated(0x00));
static_assert(!is_negated(0x01));
static_assert(!is_negated(0x0f));
static_assert(!is_negated(0x7f));
static_assert(!is_negated(0x7f));
static_assert( is_negated(0x80));
static_assert( is_negated(0xff));
static_assert(!is_negated(0x0042));
static_assert(!is_negated(0x0142));
static_assert(!is_negated(0x0f42));
static_assert(!is_negated(0x7f42));
static_assert(!is_negated(0x7f42));
static_assert( is_negated(0x8042));
static_assert( is_negated(0xff42));
static_assert(!is_negated(0x000042l));
static_assert(!is_negated(0x010042l));
static_assert(!is_negated(0x0f0042l));
static_assert(!is_negated(0x7f0042l));
static_assert(!is_negated(0x7f0042l));
static_assert( is_negated(0x800042l));
static_assert( is_negated(0xff0042l));
static_assert(!is_negated(0x00000042l));
static_assert(!is_negated(0x01000042l));
static_assert(!is_negated(0x0f000042l));
static_assert(!is_negated(0x7f000042l));
static_assert(!is_negated(0x7f000042l));
static_assert( is_negated(0x80000042l));
static_assert( is_negated(0xff000042l));
static_assert(!is_negated(0x0000000042ll));
static_assert(!is_negated(0x0100000042ll));
static_assert(!is_negated(0x0f00000042ll));
static_assert(!is_negated(0x7f00000042ll));
static_assert(!is_negated(0x7f00000042ll));
static_assert( is_negated(0x8000000042ll));
static_assert( is_negated(0xff00000042ll));
static_assert(!is_negated(0x000000000042ll));
static_assert(!is_negated(0x010000000042ll));
static_assert(!is_negated(0x0f0000000042ll));
static_assert(!is_negated(0x7f0000000042ll));
static_assert(!is_negated(0x7f0000000042ll));
static_assert( is_negated(0x800000000042ll));
static_assert( is_negated(0xff0000000042ll));
static_assert(!is_negated(0x00000000000042ll));
static_assert(!is_negated(0x01000000000042ll));
static_assert(!is_negated(0x0f000000000042ll));
static_assert(!is_negated(0x7f000000000042ll));
static_assert(!is_negated(0x7f000000000042ll));
static_assert( is_negated(0x80000000000042ll));
static_assert( is_negated(0xff000000000042ll));
static_assert(std::is_same<decltype(is_negated<int32_t>(0)), bool>::value);

// to_negated (integral)
static_assert(to_negated<int8_t>(0x00) == -128);
static_assert(to_negated<uint8_t>(0x01) == 0x81u);
static_assert(to_negated<uint8_t>(0x7f) == 0xffu);
static_assert(to_negated<int16_t>(0x7f00) == -256);
static_assert(to_negated<uint16_t>(0x7f00) == 0xff00u);
////static_assert(to_negated(0x7f000000l) == 0xff000000l);                          // literal is unsigned
static_assert(to_negated<uint32_t>(0x7f000000l) == 0xff000000l);
static_assert(to_negated(0x7f00000000000000ll) == 0xff00000000000000ll);            // literal is unsigned
static_assert(std::is_same<decltype(to_negated<int32_t>(0)), int32_t>::value);

// to_unnegated (signed integral)
static_assert(to_unnegated(0x00) == 0x00);
static_assert(to_unnegated(0x80) == 0x00);
static_assert(to_unnegated(0xff) == -0x7f);
static_assert(to_unnegated(0xff00) == -0x7f00);
static_assert(to_unnegated(0xff0000l) == -0x7f0000l);
static_assert(to_unnegated<int32_t>(0xff000000l) == -0x7f000000l);                  // literal is unsigned
static_assert(to_unnegated(0xff00000000l) == -0x7f00000000l);
static_assert(to_unnegated(0xff0000000000ll) == -0x7f0000000000ll);
static_assert(to_unnegated(0xff000000000000ll) == -0x7f000000000000ll);
static_assert(to_unnegated<int64_t>(0xff00000000000000ll) == -0x7f00000000000000ll); // literal is unsigned
static_assert(std::is_same<decltype(to_unnegated<int32_t>(0)), int32_t>::value);

// endian

static_assert(to_big_end(narrow_sign_cast<uint8_t>(0x01)) == narrow_sign_cast<uint8_t>(0x01));
static_assert(to_big_end(narrow_sign_cast<uint16_t>(0x0102)) == narrow_sign_cast<uint16_t>(0x0201));
static_assert(to_big_end(0x01020304ul) == 0x04030201ul);
static_assert(to_big_end(0x0102030405060708ull) == 0x0807060504030201ull);

BOOST_AUTO_TEST_CASE(to_big_end__not_constant_evaluated__always__expected)
{
    BOOST_REQUIRE_EQUAL(to_big_end(narrow_sign_cast<uint8_t>(0x01)), narrow_sign_cast<uint8_t>(0x01));
    BOOST_REQUIRE_EQUAL(to_big_end(narrow_sign_cast<uint16_t>(0x0102)), narrow_sign_cast<uint16_t>(0x0201));
    BOOST_REQUIRE_EQUAL(to_big_end(0x01020304ul), 0x04030201ul);
    BOOST_REQUIRE_EQUAL(to_big_end(0x0102030405060708ull), 0x0807060504030201ull);
}

static_assert(to_little_end(narrow_sign_cast<uint8_t>(0x01)) == narrow_sign_cast<uint8_t>(0x01));
static_assert(to_little_end(narrow_sign_cast<uint16_t>(0x0102)) == narrow_sign_cast<uint16_t>(0x0102));
static_assert(to_little_end(0x01020304ul) == 0x01020304ul);
static_assert(to_little_end(0x0102030405060708ull) == 0x0102030405060708ull);

BOOST_AUTO_TEST_CASE(to_little_end__not_constant_evaluated__always__expected)
{
    BOOST_REQUIRE_EQUAL(to_little_end(narrow_sign_cast<uint8_t>(0x01)), narrow_sign_cast<uint8_t>(0x01));
    BOOST_REQUIRE_EQUAL(to_little_end(narrow_sign_cast<uint16_t>(0x0102)), narrow_sign_cast<uint16_t>(0x0102));
    BOOST_REQUIRE_EQUAL(to_little_end(0x01020304ul), 0x01020304ul);
    BOOST_REQUIRE_EQUAL(to_little_end(0x0102030405060708ull), 0x0102030405060708ull);
}

// byteswap

 // Beware of type promotion (use explicit argument).
static_assert(byteswap(0x01u) == 0x01000000ul);
static_assert(byteswap(0x0102u) == 0x02010000ul);
static_assert(byteswap(0x01020304ul) == 0x04030201ul);
static_assert(byteswap(0x0102030405060708ull) == 0x0807060504030201ull);

static_assert(byteswap<uint8_t>(0x01u) == 0x01u);
static_assert(byteswap<uint16_t>(0x0102u) == 0x0201u);
static_assert(byteswap<uint32_t>(0x01020304ul) == 0x04030201ul);
static_assert(byteswap<uint64_t>(0x0102030405060708ull) == 0x0807060504030201ull);

static_assert(byteswap(0x01) == 0x01000000l);
static_assert(byteswap(0x0102) == 0x02010000l);
static_assert(byteswap(0x01020304l) == 0x04030201l);
static_assert(byteswap(0x0102030405060708ll) == 0x0807060504030201ll);

static_assert(byteswap<int8_t>(0x01) == 0x01);
static_assert(byteswap<int16_t>(0x0102) == 0x0201);
static_assert(byteswap<int32_t>(0x01020304l) == 0x04030201l);
static_assert(byteswap<int64_t>(0x0102030405060708ll) == 0x0807060504030201ll);

static_assert(byteswap(0xfe) == 0xfe000000);
static_assert(byteswap(0xfffe) == 0xfeff0000l);
static_assert(byteswap(0xffffff00l) == 0x00ffffffl);
static_assert(byteswap(0xffffffffffffff00ll) == 0x00ffffffffffffffll);

static_assert(byteswap<int8_t>(narrow_cast<int8_t>(0xfe)) == narrow_cast<int8_t>(0xfe));
static_assert(byteswap<int16_t>(narrow_cast<int8_t>(0xfffe)) == narrow_cast<int16_t>(0xfeff));
static_assert(byteswap<int32_t>(0xffffff00l) == 0x00ffffffl);
static_assert(byteswap<int64_t>(0xffffffffffffff00ll) == 0x00ffffffffffffffll);

BOOST_AUTO_TEST_CASE(byteswap__not_constant_evaluated__always__swapped)
{
    BOOST_REQUIRE_EQUAL(byteswap(0x01u), 0x01000000ul);
    BOOST_REQUIRE_EQUAL(byteswap(0x0102u), 0x02010000ul);
    BOOST_REQUIRE_EQUAL(byteswap(0x01020304ul), 0x04030201ul);
    BOOST_REQUIRE_EQUAL(byteswap(0x0102030405060708ull), 0x0807060504030201ull);

    BOOST_REQUIRE_EQUAL(byteswap<uint8_t>(0x01u), 0x01u);
    BOOST_REQUIRE_EQUAL(byteswap<uint16_t>(0x0102u), 0x0201u);
    BOOST_REQUIRE_EQUAL(byteswap<uint32_t>(0x01020304ul), 0x04030201ul);
    BOOST_REQUIRE_EQUAL(byteswap<uint64_t>(0x0102030405060708ull), 0x0807060504030201ull);

    BOOST_REQUIRE_EQUAL(byteswap<int8_t>(0x01), 0x01);
    BOOST_REQUIRE_EQUAL(byteswap<int16_t>(0x0102), 0x0201);
    BOOST_REQUIRE_EQUAL(byteswap<int32_t>(0x01020304l), 0x04030201l);
    BOOST_REQUIRE_EQUAL(byteswap<int64_t>(0x0102030405060708ll), 0x0807060504030201ll);

    BOOST_REQUIRE_EQUAL(byteswap(0xfe), to_signed(0xfe000000l));
    BOOST_REQUIRE_EQUAL(byteswap(0xff00), 0x00ff0000l);
    BOOST_REQUIRE_EQUAL(byteswap(0xffffff00l), to_unsigned(0x00ffffffl));
    BOOST_REQUIRE_EQUAL(byteswap(0xffffffffffffff00ll), to_unsigned(0x00ffffffffffffffll));

    BOOST_REQUIRE_EQUAL(byteswap<int8_t>(narrow_cast<int8_t>(0xfe)), narrow_cast<int8_t>(0xfe));
    BOOST_REQUIRE_EQUAL(byteswap<int16_t>(narrow_cast<int8_t>(0xfffe)), narrow_cast<int16_t>(0xfeff));
    BOOST_REQUIRE_EQUAL(byteswap<int32_t>(0xffffff00l), 0x00ffffffl);
    BOOST_REQUIRE_EQUAL(byteswap<int64_t>(0xffffffffffffff00ll), 0x00ffffffffffffffll);
}

////static_assert(to_bytes<0u>() == 0u);
static_assert(to_bytes<8u>() == 1u);
static_assert(to_bytes<16u>() == 2u);
static_assert(to_bytes<24u>() == 3u);
static_assert(to_bytes<32u>() == 4u);
static_assert(to_bytes<40u>() == 5u);
static_assert(to_bytes<48u>() == 6u);
static_assert(to_bytes<56u>() == 7u);
static_assert(to_bytes<64u>() == 8u);
static_assert(to_bytes<128u>() == 16u);
static_assert(to_bytes<256u>() == 32u);
static_assert(std::is_same<decltype(to_bytes<8u>()), size_t>::value);

static_assert(to_ceilinged_bytes(0u) == 0);
static_assert(to_ceilinged_bytes(1u) == 1);
static_assert(to_ceilinged_bytes(42u) == (42 + 7)/ 8);
static_assert(to_ceilinged_bytes(0xffu) == (0xff + 7) / 8);
static_assert(std::is_same<decltype(to_ceilinged_bytes<uint16_t>(0)), uint16_t>::value);

static_assert(to_floored_bytes(0u) == 0);
static_assert(to_floored_bytes(1u) == 0);
static_assert(to_floored_bytes(42u) == 42 / 8);
static_assert(to_floored_bytes(0xffu) == 0xff / 8);
static_assert(std::is_same<decltype(to_floored_bytes<uint16_t>(0)), uint16_t>::value);

BOOST_AUTO_TEST_SUITE_END()