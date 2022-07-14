/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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

template <size_t Size, if_integral_size<Size> = true>
inline unsigned_type<Size> byte_swap_slow(const data_array<Size>& data) NOEXCEPT
{
    uint32_t value{};
    auto& cast = byte_cast<uint8_t>(value);
    std::copy_n(data.data(), Size, cast.data());

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

template <size_t Size, if_integral_size<Size> = true>
inline unsigned_type<Size> byte_swap_medium(const data_array<Size>& data) NOEXCEPT
{
    uint32_t value{};
    auto& cast = byte_cast<uint8_t>(value);
    std::copy_n(data.data(), Size, cast.data());

    // Compare medium (bitwise).
    return byte_swap32_native(to_unsigned(value));
}

template <size_t Size, if_integral_size<Size> = true>
inline unsigned_type<Size> byte_swap_fast(const data_array<Size>& data) NOEXCEPT
{
    uint32_t value{};
    auto& cast = byte_cast<uint8_t>(value);
    std::copy_n(data.data(), Size, cast.data());

    // Compare fast (intrinsic).
    return byte_swap32(to_unsigned(value));
}

BOOST_AUTO_TEST_CASE(byte_cast__byte_swap__disassembly__comparison)
{
    constexpr auto expected = 0x01020304_u32;
    constexpr data_array<sizeof(uint32_t)> bytes{ 0x01_u8, 0x02_u8, 0x03_u8, 0x04_u8 };

    const auto fast = byte_swap_fast(bytes);
    const auto medi = byte_swap_medium(bytes);
    const auto slow = byte_swap_slow(bytes);

    BOOST_REQUIRE_EQUAL(fast, expected);
    BOOST_REQUIRE_EQUAL(medi, expected);
    BOOST_REQUIRE_EQUAL(slow, expected);
}

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

BOOST_AUTO_TEST_CASE(byte_cast__get_byte__always__expected)
{
    constexpr auto value = native_to_little_end(0x01020304_u32);
    const auto byte0 = get_byte<0, uint8_t>(value);
    const auto byte1 = get_byte<1,  int8_t>(value);
    const auto byte2 = get_byte<2, uint8_t>(value);
    const auto byte3 = get_byte<3,  int8_t>(value);
    BOOST_REQUIRE_EQUAL(byte0, 0x04_u8);
    BOOST_REQUIRE_EQUAL(byte1, 0x03_u8);
    BOOST_REQUIRE_EQUAL(byte2, 0x02_u8);
    BOOST_REQUIRE_EQUAL(byte3, 0x01_u8);
}

BOOST_AUTO_TEST_CASE(byte_cast__set_byte__always__expected)
{
    constexpr auto expected = native_to_little_end(0x01020304_u32);
    auto value = native_to_little_end(0xaabbccdd_u32);
    set_byte<0,  int8_t>(value, 0x04);
    set_byte<1, uint8_t>(value, 0x03);
    set_byte<2,  int8_t>(value, 0x02);
    set_byte<3, uint8_t>(value, 0x01);
    BOOST_REQUIRE_EQUAL(value, expected);
}

BOOST_AUTO_TEST_SUITE_END()
