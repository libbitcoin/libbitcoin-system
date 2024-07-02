/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(byte_cast_tests)

constexpr auto size4 = sizeof(uint32_t);
constexpr auto value4 = native_to_big_end(0x01020304_u32);
constexpr data_array<size4> bytes4{ 0x01_u8, 0x02_u8, 0x03_u8, 0x04_u8 };

// from integral to bytes
// ----------------------------------------------------------------------------

template <size_t Index, typename Byte, typename Integral,
    if_one_byte<Byte> = true,
    if_integral_integer<Integral> = true,
    if_lesser<Index, sizeof(Integral)> = true>
inline Byte get_byte(const Integral& value) NOEXCEPT
{
    return byte_cast<Byte>(value)[Index];
}

template <size_t Index, typename Byte, typename Integral,
    if_one_byte<Byte> = true,
    if_integral_integer<Integral> = true,
    if_lesser<Index, sizeof(Integral)> = true>
inline void set_byte(Integral& value, Byte set) NOEXCEPT
{
    byte_cast<Byte>(value)[Index] = set;
}

template <size_t Index, typename Byte, typename Integral,
    if_one_byte<Byte> = true,
    if_integral_integer<Integral> = true,
    if_lesser<Index, sizeof(Integral)> = true>
inline Byte move_byte(Integral&& value) NOEXCEPT
{
    return byte_cast<Byte>(value)[Index];
}

BOOST_AUTO_TEST_CASE(byte_cast__byte_cast_from_const_ref_integral__always__expected)
{
    const auto byte0 = get_byte<0, uint8_t>(value4);
    const auto byte1 = get_byte<1, int8_t>(value4);
    const auto byte2 = get_byte<2, uint8_t>(value4);
    const auto byte3 = get_byte<3, int8_t>(value4);
    BOOST_REQUIRE_EQUAL(byte0, 0x01_u8);
    BOOST_REQUIRE_EQUAL(byte1, 0x02_u8);
    BOOST_REQUIRE_EQUAL(byte2, 0x03_u8);
    BOOST_REQUIRE_EQUAL(byte3, 0x04_u8);
}

BOOST_AUTO_TEST_CASE(byte_cast__byte_cast_from_non_const_ref_integral__always__expected)
{
    // demonstrate assignment through the cast
    auto value = 0xaabbccdd_u32;
    set_byte<0, int8_t>(value, 0x01);
    set_byte<1, uint8_t>(value, 0x02);
    set_byte<2, int8_t>(value, 0x03);
    set_byte<3, uint8_t>(value, 0x04);
    BOOST_REQUIRE_EQUAL(value, value4);
}

BOOST_AUTO_TEST_CASE(byte_cast__byte_cast_from_rvalue_integral__always__expected)
{
    const auto byte0 = move_byte<0, uint8_t>(native_to_big_end(0x01020304_u32));
    const auto byte1 = move_byte<1, int8_t>(native_to_big_end(0x01020304_u32));
    const auto byte2 = move_byte<2, uint8_t>(native_to_big_end(0x01020304_u32));
    const auto byte3 = move_byte<3, int8_t>(native_to_big_end(0x01020304_u32));
    BOOST_REQUIRE_EQUAL(byte0, 0x01_u8);
    BOOST_REQUIRE_EQUAL(byte1, 0x02_u8);
    BOOST_REQUIRE_EQUAL(byte2, 0x03_u8);
    BOOST_REQUIRE_EQUAL(byte3, 0x04_u8);
}

// from byte array to integral
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(byte_cast__byte_cast_from_const_ref_bytes__always__expected)
{
    auto& value = byte_cast(bytes4);
    BOOST_REQUIRE_EQUAL(value, value4);

    // byte_cast(non-const) produces non-const inferred unsigned integral type.
    static_assert(is_same_type<decltype(value), const unsigned_type<size4>&>);
}

BOOST_AUTO_TEST_CASE(byte_cast__byte_cast_from_non_const_ref_bytes__always__expected)
{
    std_array<int8_t, size4> bytes{ 0x01_i8, 0x02_i8, 0x03_i8, 0x04_i8 };
    auto& value = byte_cast(bytes);
    BOOST_REQUIRE_EQUAL(value, value4);

    // byte_cast(non-const) produces non-const inferred unsigned integral type.
    static_assert(is_same_type<decltype(value), unsigned_type<size4>&>);

    // demonstrate assignment through the cast
    value = native_to_big_end(0xaabbccdd_u32);
    BOOST_REQUIRE_EQUAL(bytes[0], 0xaa_i8);
    BOOST_REQUIRE_EQUAL(bytes[1], 0xbb_i8);
    BOOST_REQUIRE_EQUAL(bytes[2], 0xcc_i8);
    BOOST_REQUIRE_EQUAL(bytes[3], 0xdd_i8);
}

BOOST_AUTO_TEST_CASE(byte_cast__byte_cast_from_rvalue_bytes__always__expected)
{
    BOOST_REQUIRE_EQUAL(byte_cast(data_array<size4>{ 0x01_u8, 0x02_u8, 0x03_u8, 0x04_u8 }), value4);
}

// from byte* to integral
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(byte_cast__byte_cast_from_const_byte_ptr__always__expected)
{
    const auto bytes4_ptr = bytes4.data();
    auto& value = unsafe_byte_cast<uint32_t>(bytes4_ptr);

    // byte_cast(const) produces const specified integral type.
    static_assert(is_same_type<decltype(value), const uint32_t&>);

    BOOST_REQUIRE_EQUAL(value, value4);
}

BOOST_AUTO_TEST_CASE(byte_cast__byte_cast_from_non_const_byte_ptr__always__expected)
{
    std_array<int8_t, size4> bytes{ 0x01_i8, 0x02_i8, 0x03_i8, 0x04_i8 };
    auto bytes_ptr = bytes.data();
    auto& value = unsafe_byte_cast<uint32_t, int8_t>(bytes_ptr);
    BOOST_REQUIRE_EQUAL(value, value4);

    // byte_cast(non-const) produces non-const specified integral type.
    static_assert(is_same_type<decltype(value), uint32_t&>);

    // demonstrate assignment through the cast
    value = native_to_big_end(0xaabbccdd_u32);
    BOOST_REQUIRE_EQUAL(bytes[0], 0xaa_i8);
    BOOST_REQUIRE_EQUAL(bytes[1], 0xbb_i8);
    BOOST_REQUIRE_EQUAL(bytes[2], 0xcc_i8);
    BOOST_REQUIRE_EQUAL(bytes[3], 0xdd_i8);
}

BOOST_AUTO_TEST_SUITE_END()
