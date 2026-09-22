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

BOOST_AUTO_TEST_SUITE(endian_integrals_tests)

template <size_t Offset, typename Integral>
constexpr data_array<sizeof(Integral) + Offset> to_big_test(Integral value) NOEXCEPT
{
    data_array<sizeof(Integral) + Offset> out{};
    to_big<Offset>(out, value);
    return out;
}

template <size_t Offset, typename Integral>
constexpr data_array<sizeof(Integral) + Offset> to_little_test(Integral value) NOEXCEPT
{
    data_array<sizeof(Integral) + Offset> out{};
    to_little<Offset>(out, value);
    return out;
}

// Test both return (no offset option) and out parameter (with no out parameter offset).
static_assert(to_big   (0x01_u8) == data_array<1>{ 1 });
static_assert(to_little(0x01_u8) == data_array<1>{ 1 });
static_assert(to_big   (0x0102_u16) == data_array<2>{ 1, 2 });
static_assert(to_little(0x0102_u16) == data_array<2>{ 2, 1 });
static_assert(to_big   (0x01020304_u32) == data_array<4>{ 1, 2, 3, 4 });
static_assert(to_little(0x01020304_u32) == data_array<4>{ 4, 3, 2, 1 });
static_assert(to_big   (0x0102030405060708_u64) == data_array<8>{ 1, 2, 3, 4, 5, 6, 7, 8 });
static_assert(to_little(0x0102030405060708_u64) == data_array<8>{ 8, 7, 6, 5, 4, 3, 2, 1 });

// Test offset into data parameter.
// Offsetting into data array requires test helper as only constexpr parameters are allowed.
static_assert(to_big_test<1>   (0x01_u8) == data_array<2>{ 0, 1 });
static_assert(to_little_test<1>(0x01_u8) == data_array<2>{ 0, 1 });
static_assert(to_big_test<1>   (0x0102_u16) == data_array<3>{ 0, 1, 2 });
static_assert(to_little_test<1>(0x0102_u16) == data_array<3>{ 0, 2, 1 });
static_assert(to_big_test<1>   (0x01020304_u32) == data_array<5>{ 0, 1, 2, 3, 4  });
static_assert(to_little_test<1>(0x01020304_u32) == data_array<5>{ 0, 4, 3, 2, 1 });
static_assert(to_big_test<1>   (0x0102030405060708_u64) == data_array<9>{ 0, 1, 2, 3, 4, 5, 6, 7, 8 });
static_assert(to_little_test<1>(0x0102030405060708_u64) == data_array<9>{ 0, 8, 7, 6, 5, 4, 3, 2, 1 });

// Test both return and out parameter with no offset, using inferred integral type.
static_assert(from_big   (data_array<1>{ 1 }) == 0x01_u8);
static_assert(from_little(data_array<1>{ 1 }) == 0x01_u8);
static_assert(from_big   (data_array<2>{ 1, 2 }) == 0x0102_u16);
static_assert(from_little(data_array<2>{ 2, 1 }) == 0x0102_u16);
static_assert(from_big   (data_array<4>{ 1, 2, 3, 4 }) == 0x01020304_u32);
static_assert(from_little(data_array<4>{ 4, 3, 2, 1 }) == 0x01020304_u32);
static_assert(from_big   (data_array<8>{ 1, 2, 3, 4, 5, 6, 7, 8 }) == 0x0102030405060708_u64);
static_assert(from_little(data_array<8>{ 8, 7, 6, 5, 4, 3, 2, 1 }) == 0x0102030405060708_u64);

// Test both return and out parameter with no offset.
static_assert(from_big   <uint8_t >(data_array<1>{ 1 }) == 0x01_u8);
static_assert(from_little<uint8_t >(data_array<1>{ 1 }) == 0x01_u8);
static_assert(from_big   <uint16_t>(data_array<2>{ 1, 2 }) == 0x0102_u16);
static_assert(from_little<uint16_t>(data_array<2>{ 2, 1 }) == 0x0102_u16);
static_assert(from_big   <uint32_t>(data_array<4>{ 1, 2, 3, 4 }) == 0x01020304_u32);
static_assert(from_little<uint32_t>(data_array<4>{ 4, 3, 2, 1 }) == 0x01020304_u32);
static_assert(from_big   <uint64_t>(data_array<8>{ 1, 2, 3, 4, 5, 6, 7, 8 }) == 0x0102030405060708_u64);
static_assert(from_little<uint64_t>(data_array<8>{ 8, 7, 6, 5, 4, 3, 2, 1 }) == 0x0102030405060708_u64);

// Test both return and out parameter with offset into data parameter.
static_assert(from_big   <uint8_t,  1>(data_array<2>{ 1, 2 }) == 0x02_u8);
static_assert(from_little<uint8_t,  1>(data_array<2>{ 2, 1 }) == 0x01_u8);
static_assert(from_big   <uint16_t, 2>(data_array<4>{ 1, 2, 3, 4 }) == 0x0304_u16);
static_assert(from_little<uint16_t, 2>(data_array<4>{ 4, 3, 2, 1 }) == 0x0102_u16);
static_assert(from_big   <uint32_t, 4>(data_array<8>{ 1, 2, 3, 4, 5, 6, 7, 8 }) == 0x05060708_u32);
static_assert(from_little<uint32_t, 4>(data_array<8>{ 8, 7, 6, 5, 4, 3, 2, 1 }) == 0x01020304_u32);
static_assert(from_big   <uint64_t, 8>(data_array<16>{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 }) == 0x090a0b0c0d0e0f10_u64);
static_assert(from_little<uint64_t, 8>(data_array<16>{ 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 }) == 0x0102030405060708_u64);


// Runtime instances, as the assertions above are evaluated by the compiler.

BOOST_AUTO_TEST_CASE(endian_integrals__to_from__runtime_widths__round_trip)
{
    const auto value8 = 0x01_u8;
    const auto value16 = 0x0102_u16;
    const auto value32 = 0x01020304_u32;
    const auto value64 = 0x0102030405060708_u64;

    BOOST_REQUIRE_EQUAL(to_big(value8), (data_array<1>{ 1 }));
    BOOST_REQUIRE_EQUAL(to_little(value8), (data_array<1>{ 1 }));
    BOOST_REQUIRE_EQUAL(to_big(value16), (data_array<2>{ 1, 2 }));
    BOOST_REQUIRE_EQUAL(to_little(value16), (data_array<2>{ 2, 1 }));
    BOOST_REQUIRE_EQUAL(to_big(value32), (data_array<4>{ 1, 2, 3, 4 }));
    BOOST_REQUIRE_EQUAL(to_little(value32), (data_array<4>{ 4, 3, 2, 1 }));
    BOOST_REQUIRE_EQUAL(to_big(value64), (data_array<8>{ 1, 2, 3, 4, 5, 6, 7, 8 }));
    BOOST_REQUIRE_EQUAL(to_little(value64), (data_array<8>{ 8, 7, 6, 5, 4, 3, 2, 1 }));

    BOOST_REQUIRE_EQUAL(from_big<uint8_t>(to_big(value8)), value8);
    BOOST_REQUIRE_EQUAL(from_little<uint8_t>(to_little(value8)), value8);
    BOOST_REQUIRE_EQUAL(from_big<uint16_t>(to_big(value16)), value16);
    BOOST_REQUIRE_EQUAL(from_little<uint16_t>(to_little(value16)), value16);
    BOOST_REQUIRE_EQUAL(from_big<uint32_t>(to_big(value32)), value32);
    BOOST_REQUIRE_EQUAL(from_little<uint32_t>(to_little(value32)), value32);
    BOOST_REQUIRE_EQUAL(from_big<uint64_t>(to_big(value64)), value64);
    BOOST_REQUIRE_EQUAL(from_little<uint64_t>(to_little(value64)), value64);
}

BOOST_AUTO_TEST_CASE(endian_integrals__to__runtime_offset__round_trip)
{
    const auto value32 = 0x01020304_u32;
    BOOST_REQUIRE_EQUAL(to_big_test<2>(value32), (data_array<6>{ 0, 0, 1, 2, 3, 4 }));
    BOOST_REQUIRE_EQUAL(to_little_test<2>(value32), (data_array<6>{ 0, 0, 4, 3, 2, 1 }));
}

BOOST_AUTO_TEST_SUITE_END()
