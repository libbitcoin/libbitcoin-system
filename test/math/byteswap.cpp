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

BOOST_AUTO_TEST_SUITE(byteswap_tests)

// Beware of type promotion (use explicit types).

// byteswap (constexpr)
static_assert(byteswap(0x01_i8) == 0x01);
static_assert(byteswap(0x0102_i16) == 0x0201);
static_assert(byteswap(0x01020304_i32) == 0x04030201);
static_assert(byteswap(0x0102030405060708_i64) == 0x0807060504030201);
static_assert(byteswap(0x01_u8) == 0x01);
static_assert(byteswap(0x0102_u16) == 0x0201);
static_assert(byteswap(0x01020304_u32) == 0x04030201);
static_assert(byteswap(0x0102030405060708_u64) == 0x0807060504030201);

BOOST_AUTO_TEST_CASE(byteswap__signed__not_constant_evaluated_signed__swapped)
{
    BOOST_REQUIRE_EQUAL(byteswap(0x01_i8), 0x01_i8);
    BOOST_REQUIRE_EQUAL(byteswap(0x0102_i16), 0x0201_i16);
    BOOST_REQUIRE_EQUAL(byteswap(0x01020304_i32), 0x04030201_i32);
    BOOST_REQUIRE_EQUAL(byteswap(0x0102030405060708_i64), 0x0807060504030201_i64);
}

BOOST_AUTO_TEST_CASE(byteswap__unsigned___not_constant_evaluated_unsigned__swapped)
{
    BOOST_REQUIRE_EQUAL(byteswap(0x01_u8), 0x01_u8);
    BOOST_REQUIRE_EQUAL(byteswap(0x0102_u16), 0x0201_u16);
    BOOST_REQUIRE_EQUAL(byteswap(0x01020304_u32), 0x04030201_u32);
    BOOST_REQUIRE_EQUAL(byteswap(0x0102030405060708_u64), 0x0807060504030201_u64);
}

// from byte array to integral (performance comparison)
// ----------------------------------------------------------------------------

// inline for std::swap intrinsic compile
// This approach is the slowest of the three.
template <size_t Size, if_integral_size<Size> = true>
inline unsigned_type<Size> byte_swap_slow(const data_array<Size>& data) NOEXCEPT
{
    uint32_t value{};
    auto& cast = byte_cast<uint8_t>(value);
    BC_PUSH_WARNING(NO_UNSAFE_COPY_N)
    std::copy_n(data.data(), Size, cast.data());
    BC_POP_WARNING()

    if constexpr (Size == sizeof(uint16_t))
    {
        std::swap(cast[0], cast[1]);
    }

    if constexpr (Size == sizeof(uint32_t))
    {
        // Compare slow (bytewise).
        std::swap(cast[0], cast[3]);
        std::swap(cast[1], cast[2]);
    }

    if constexpr (Size == sizeof(uint64_t))
    {
        std::swap(cast[0], cast[7]);
        std::swap(cast[1], cast[6]);
        std::swap(cast[2], cast[5]);
        std::swap(cast[3], cast[4]);
    }

    return value;
}

// The native (shifting) approach is the next best approach.
template <size_t Size, if_integral_size<Size> = true>
inline unsigned_type<Size> byte_swap_medium(const data_array<Size>& data) NOEXCEPT
{
    uint32_t value{};
    auto& cast = byte_cast<uint8_t>(value);
    BC_PUSH_WARNING(NO_UNSAFE_COPY_N)
    std::copy_n(data.data(), Size, cast.data());
    BC_POP_WARNING()

    // Compare medium (bitwise).
    return byte_swap32_native(to_unsigned(value));
}

// The intrinsic approach is the fastest, a single CPU operation.
template <size_t Size, if_integral_size<Size> = true>
inline unsigned_type<Size> byte_swap_fast(const data_array<Size>& data) NOEXCEPT
{
    uint32_t value{};
    auto& cast = byte_cast<uint8_t>(value);
    BC_PUSH_WARNING(NO_UNSAFE_COPY_N)
    std::copy_n(data.data(), Size, cast.data());
    BC_POP_WARNING()

    // Compare fast (intrinsic).
    return byte_swap32(to_unsigned(value));
}

BOOST_AUTO_TEST_CASE(byteswap__three_approaches__disassembly__comparison)
{
    constexpr auto value = native_to_little_end(0x01020304_u32);
    constexpr data_array<sizeof(uint32_t)> bytes{ 0x01_u8, 0x02_u8, 0x03_u8, 0x04_u8 };

    const auto fast = byte_swap_fast(bytes);
    const auto medi = byte_swap_medium(bytes);
    const auto slow = byte_swap_slow(bytes);

    BOOST_REQUIRE_EQUAL(fast, value);
    BOOST_REQUIRE_EQUAL(medi, value);
    BOOST_REQUIRE_EQUAL(slow, value);
}

BOOST_AUTO_TEST_SUITE_END()
