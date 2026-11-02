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
#include <sstream>

BC_AUTO_TEST_SUITE(uintx_t_tests)

constexpr auto max256 = std::numeric_limits<uint256_t>::max();

// constexpr
static_assert(uint256_t{ 1 } + uint256_t{ 2 } == 3);
static_assert(uint256_t{ 0 } - 1u == max256);
static_assert(((uint256_t{ 1 } << 255u) >> 255u) == 1);
static_assert(uint256_t{ 6 } * 7u == 42);
static_assert(uint256_t{ 42 } / 5u == 8);
static_assert(uint256_t{ 42 } % 5u == 2);
static_assert(uint5_t{ 31 } + 1u == 0);
static_assert(uint5_t{ 33 } == 1);
static_assert(uint5_t{ -1 } == 31);
static_assert(uint128_t{ 1 } < uint256_t{ 2 });
static_assert(uint256_t{ 2 } > uint128_t{ 1 });
static_assert(uint256_t{ 7 } == uint5_t{ 7 });
static_assert(std::numeric_limits<uint5_t>::digits == 5);
static_assert(std::numeric_limits<uint256_t>::is_integer);
static_assert(!std::numeric_limits<uint256_t>::is_signed);
static_assert(std::numeric_limits<uint5_t>::max() == 31);
static_assert(std::numeric_limits<uint256_t>::min() == 0);

// construct

BC_AUTO_TEST_CASE(uintx_t__construct__default__zero)
{
    const uint256_t value{};
    BC_REQUIRE(!value);
    BC_REQUIRE_EQUAL(value.str(), "0");
}

BC_AUTO_TEST_CASE(uintx_t__construct__negative__twos_complement)
{
    BC_REQUIRE_EQUAL(uint256_t{ -1 }, max256);
    BC_REQUIRE_EQUAL(uint128_t{ int8_t{ -2 } }, std::numeric_limits<uint128_t>::max() - 1u);
    BC_REQUIRE_EQUAL(uint5_t{ -3 }, uint5_t{ 29 });
}

BC_AUTO_TEST_CASE(uintx_t__construct__narrowing__truncated)
{
    const uint512_t wide = (uint512_t{ 1 } << 300u) + 42u;
    const uint256_t narrow{ wide };
    BC_REQUIRE_EQUAL(narrow, uint256_t{ 42 });
}

BC_AUTO_TEST_CASE(uintx_t__construct__widening__preserved)
{
    const uint512_t wide{ max256 };
    BC_REQUIRE_EQUAL(wide + 1u, uint512_t{ 1 } << 256u);
}

BC_AUTO_TEST_CASE(uintx_t__construct__hex_text__expected)
{
    const uint256_t value{ "0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff" };
    BC_REQUIRE_EQUAL(value, max256);
    BC_REQUIRE_EQUAL(uint256_t{ "0X2A" }, 42u);
    BC_REQUIRE_EQUAL(uint256_t{ "0xAbCdEf" }, 0xabcdefu);
}

BC_AUTO_TEST_CASE(uintx_t__construct__decimal_text__expected)
{
    const uint256_t value{ "115792089237316195423570985008687907853269984665640564039457584007913129639935" };
    BC_REQUIRE_EQUAL(value, max256);
    BC_REQUIRE_EQUAL(uint256_t{ "42" }, 42u);
}

BC_AUTO_TEST_CASE(uintx_t__construct__invalid_text__zero)
{
    BC_REQUIRE_EQUAL(uint256_t{ "" }, 0u);
    BC_REQUIRE_EQUAL(uint256_t{ "0x" }, 0u);
    BC_REQUIRE_EQUAL(uint256_t{ "12a" }, 0u);
    BC_REQUIRE_EQUAL(uint256_t{ "0xfg" }, 0u);
    BC_REQUIRE_EQUAL(uint256_t{ "--1" }, 0u);
}

BC_AUTO_TEST_CASE(uintx_t__construct__negative_text__twos_complement)
{
    BC_REQUIRE_EQUAL(uint256_t{ "-1" }, max256);
    BC_REQUIRE_EQUAL(uint256_t{ "-0x2" }, max256 - 1u);
}

// conversions

BC_AUTO_TEST_CASE(uintx_t__conversion__integral__low_bits)
{
    const auto value = (uint256_t{ 1 } << 200u) + 0x1234u;
    BC_REQUIRE_EQUAL(static_cast<uint8_t>(value), 0x34u);
    BC_REQUIRE_EQUAL(static_cast<uint64_t>(value), 0x1234u);
    BC_REQUIRE_EQUAL(value.convert_to<uint16_t>(), 0x1234u);
    BC_REQUIRE_EQUAL(static_cast<int64_t>(max256), -1);
}

BC_AUTO_TEST_CASE(uintx_t__conversion__bool__nonzero)
{
    BC_REQUIRE(static_cast<bool>(uint256_t{ 1 } << 255u));
    BC_REQUIRE(!static_cast<bool>(uint256_t{}));
}

BC_AUTO_TEST_CASE(uintx_t__conversion__floating__nearest)
{
    BC_REQUIRE_EQUAL(static_cast<double>(uint256_t{}), 0.0);
    BC_REQUIRE_EQUAL(static_cast<double>(uint256_t{ 42 }), 42.0);
    BC_REQUIRE_EQUAL(static_cast<double>(uint256_t{ 1 } << 200u), std::ldexp(1.0, 200));
    BC_REQUIRE_EQUAL(max256.convert_to<double>(), std::ldexp(1.0, 256));
    BC_REQUIRE_EQUAL(static_cast<float>(uint128_t{ 3 } << 100u), std::ldexp(3.0f, 100));
}

BC_AUTO_TEST_CASE(uintx_t__conversion__floating_tie__even)
{
    // 2^53 + 1 is a tie between 2^53 and 2^53 + 2, rounded to the even 2^53.
    const auto tie = (uint256_t{ 1 } << 53u) + 1u;
    BC_REQUIRE_EQUAL(static_cast<double>(tie), std::ldexp(1.0, 53));

    // A sticky bit below the tie rounds up.
    const auto above = (uint256_t{ 1 } << 80u) + (uint256_t{ 1 } << 27u) + 1u;
    BC_REQUIRE_EQUAL(static_cast<double>(above), std::ldexp(1.0, 80) + std::ldexp(1.0, 28));
}

BC_AUTO_TEST_CASE(uintx_t__convert_to__uintx__truncated)
{
    const auto value = (uint256_t{ 1 } << 130u) + 7u;
    BC_REQUIRE_EQUAL(value.convert_to<uint128_t>(), uint128_t{ 7 });
}

// properties

BC_AUTO_TEST_CASE(uintx_t__bit__set_bits__expected)
{
    auto value = uint256_t{};
    value.set(0).set(64).set(255).set(256);
    BC_REQUIRE(value.bit(0));
    BC_REQUIRE(value.bit(64));
    BC_REQUIRE(value.bit(255));
    BC_REQUIRE(!value.bit(1));
    BC_REQUIRE(!value.bit(256));
    BC_REQUIRE_EQUAL(value, (uint256_t{ 1 } << 255u) + (uint256_t{ 1 } << 64u) + 1u);
}

BC_AUTO_TEST_CASE(uintx_t__msb__values__expected)
{
    BC_REQUIRE_EQUAL(uint256_t{ 1 }.msb(), 0u);
    BC_REQUIRE_EQUAL(uint256_t{ 0x80 }.msb(), 7u);
    BC_REQUIRE_EQUAL((uint256_t{ 1 } << 64u).msb(), 64u);
    BC_REQUIRE_EQUAL(max256.msb(), 255u);
    BC_REQUIRE_EQUAL(uint256_t{}.msb(), 0u);
}

BC_AUTO_TEST_CASE(uintx_t__data__high_bits__masked)
{
    const uint5_t value{ -1 };
    BC_REQUIRE_EQUAL(value.data().front(), 31u);
}

// arithmetic

BC_AUTO_TEST_CASE(uintx_t__add__carry__propagated)
{
    const auto value = uint256_t{ max_uint64 } + 1u;
    BC_REQUIRE_EQUAL(value, uint256_t{ 1 } << 64u);
    BC_REQUIRE_EQUAL(max256 + 1u, 0u);
}

BC_AUTO_TEST_CASE(uintx_t__subtract__borrow__propagated)
{
    const auto value = (uint256_t{ 1 } << 64u) - 1u;
    BC_REQUIRE_EQUAL(value, uint256_t{ max_uint64 });
    BC_REQUIRE_EQUAL(uint256_t{} - 1u, max256);
}

BC_AUTO_TEST_CASE(uintx_t__multiply__cross_limb__expected)
{
    const uint256_t value{ max_uint64 };
    const auto expected = (uint256_t{ 1 } << 128u) - (uint256_t{ 1 } << 65u) + 1u;
    BC_REQUIRE_EQUAL(value * value, expected);
    BC_REQUIRE_EQUAL(max256 * max256, 1u);
}

BC_AUTO_TEST_CASE(uintx_t__divide__multiple_digits__expected)
{
    const uint256_t dividend{ "0x1234567890abcdef1234567890abcdef1234567890abcdef" };
    const uint256_t divisor{ "0xfedcba0987654321fedcba09" };
    const auto quotient = dividend / divisor;
    const auto remainder = dividend % divisor;
    BC_REQUIRE(remainder < divisor);
    BC_REQUIRE_EQUAL(quotient * divisor + remainder, dividend);
    BC_REQUIRE_EQUAL(quotient, uint256_t{ "0x1249249c805663c40aa756c8" });
    BC_REQUIRE_EQUAL(remainder, uint256_t{ "0x2ba6d315ab72ad2597dc70e7" });
}

BC_AUTO_TEST_CASE(uintx_t__divide__by_zero__zero)
{
    BC_REQUIRE_EQUAL(uint256_t{ 42 } / 0u, 0u);
    BC_REQUIRE_EQUAL(uint256_t{ 42 } % 0u, 0u);
}

BC_AUTO_TEST_CASE(uintx_t__divide__smaller_dividend__remainder_is_dividend)
{
    BC_REQUIRE_EQUAL(uint256_t{ 3 } / uint256_t{ max256 }, 0u);
    BC_REQUIRE_EQUAL(uint256_t{ 3 } % uint256_t{ max256 }, 3u);
}

BC_AUTO_TEST_CASE(uintx_t__increment_decrement__wrap__expected)
{
    auto value = max256;
    BC_REQUIRE_EQUAL(++value, 0u);
    BC_REQUIRE_EQUAL(value--, 0u);
    BC_REQUIRE_EQUAL(value, max256);
}

BC_AUTO_TEST_CASE(uintx_t__negate__value__twos_complement)
{
    BC_REQUIRE_EQUAL(-uint256_t{ 1 }, max256);
    BC_REQUIRE_EQUAL(-uint256_t{}, 0u);
}

// bitwise

BC_AUTO_TEST_CASE(uintx_t__logical__values__expected)
{
    const uint256_t left{ 0b1100 };
    const uint256_t right{ 0b1010 };
    BC_REQUIRE_EQUAL(left & right, 0b1000u);
    BC_REQUIRE_EQUAL(left | right, 0b1110u);
    BC_REQUIRE_EQUAL(left ^ right, 0b0110u);
    BC_REQUIRE_EQUAL(~uint256_t{}, max256);
    BC_REQUIRE_EQUAL(~uint5_t{}, 31u);
}

BC_AUTO_TEST_CASE(uintx_t__shift__beyond_width__zero)
{
    BC_REQUIRE_EQUAL(max256 << 256u, 0u);
    BC_REQUIRE_EQUAL(max256 >> 256u, 0u);
    BC_REQUIRE_EQUAL(max256 << 255u, uint256_t{ 1 } << 255u);
    BC_REQUIRE_EQUAL(max256 >> 255u, 1u);
}

BC_AUTO_TEST_CASE(uintx_t__shift__across_limbs__expected)
{
    const uint256_t value{ 0x8000000000000001u };
    BC_REQUIRE_EQUAL((value << 1u) >> 64u, 1u);
    BC_REQUIRE_EQUAL(((value << 100u) >> 100u), value);
    BC_REQUIRE_EQUAL(uint5_t{ 1 } << 5u, 0u);
}

// comparison

BC_AUTO_TEST_CASE(uintx_t__compare__limbs__high_first)
{
    const auto high = uint256_t{ 1 } << 192u;
    const uint256_t low{ max_uint64 };
    BC_REQUIRE(high > low);
    BC_REQUIRE(low < high);
    BC_REQUIRE(low != high);
    BC_REQUIRE(high == uint256_t{ 1 } << 192u);
}

BC_AUTO_TEST_CASE(uintx_t__compare__negative_integral__modulo)
{
    BC_REQUIRE(max256 == -1);
    BC_REQUIRE(uint256_t{ 5 } < -1);
    BC_REQUIRE(uint5_t{ 3 } == -29);
    BC_REQUIRE(-1 == uint5_t{ 31 });
}

BC_AUTO_TEST_CASE(uintx_t__compare__nonnegative_integral__by_value)
{
    BC_REQUIRE(uint5_t{ 1 } != 33);
    BC_REQUIRE(uint5_t{ 3 } < 33);
    BC_REQUIRE(uint5_t{ 31 } < 32u);
    BC_REQUIRE(33 > uint5_t{ 31 });
    BC_REQUIRE(uint5_t{ 31 } == 31);
    BC_REQUIRE(0 < uint256_t{ 1 });
}

// text

BC_AUTO_TEST_CASE(uintx_t__str__values__decimal)
{
    BC_REQUIRE_EQUAL(uint256_t{ 1000000000 }.str(), "1000000000");
    BC_REQUIRE_EQUAL(uint256_t{ 999999999 }.str(), "999999999");
    BC_REQUIRE_EQUAL(max256.str(), "115792089237316195423570985008687907853269984665640564039457584007913129639935");
    BC_REQUIRE_EQUAL(uint5_t{ 31 }.str(), "31");
}

BC_AUTO_TEST_CASE(uintx_t__ostream__decimal__expected)
{
    std::ostringstream stream{};
    stream << uint256_t{ 42 };
    BC_REQUIRE_EQUAL(stream.str(), "42");
}

BC_AUTO_TEST_CASE(uintx_t__ostream__hex__expected)
{
    std::ostringstream stream{};
    stream << std::hex << uint256_t{ 0xabcdef };
    BC_REQUIRE_EQUAL(stream.str(), "abcdef");
}

BC_AUTO_TEST_CASE(uintx_t__ostream__hex_showbase_uppercase__expected)
{
    std::ostringstream stream{};
    stream << std::hex << std::showbase << std::uppercase << uint256_t{ 0xabcdef };
    BC_REQUIRE_EQUAL(stream.str(), "0XABCDEF");
}

BC_AUTO_TEST_SUITE_END()
