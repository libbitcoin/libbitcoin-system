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

BC_AUTO_TEST_SUITE(uint256_t_tests)

// Vectors from Bitcoin Core src/test/arith_uint256_tests.cpp (MIT license).

constexpr auto random1_hash = base16_hash("7d1de5eaf9b156d53208f033b5aa8122d2d2355d5e12292b121156cfdb4a529c");
constexpr auto random2_hash = base16_hash("d781cab4f072134971da2d19a3473013bfb69ca6c30a7e26406ba5477c1d3270");
constexpr auto one_hash = base16_hash("0000000000000000000000000000000000000000000000000000000000000001");
constexpr auto max_hash = base16_hash("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");

constexpr auto random1 = to_uintx(random1_hash);
constexpr auto random2 = to_uintx(random2_hash);
constexpr auto zero256 = to_uintx(null_hash);
constexpr auto one256 = to_uintx(one_hash);
constexpr auto max256 = to_uintx(max_hash);
constexpr auto half256 = one256 << 255u;
constexpr auto random1_low64 = 0x121156cfdb4a529c_u64;
constexpr auto random1_double = 0.4887374590559308955;

static hash_digest shift_left(const hash_digest& from, size_t bits) NOEXCEPT
{
    hash_digest to{};
    const auto bytes = bits / byte_bits;
    const auto offset = bits % byte_bits;
    for (auto index = bytes; index < to.size(); ++index)
    {
        auto byte = depromote<uint8_t>(from[index - bytes] << offset);
        if (index > bytes && is_nonzero(offset))
            byte |= depromote<uint8_t>(from[index - bytes - 1u] >> (byte_bits - offset));

        to[index] = byte;
    }

    return to;
}

static hash_digest shift_right(const hash_digest& from, size_t bits) NOEXCEPT
{
    hash_digest to{};
    const auto bytes = bits / byte_bits;
    const auto offset = bits % byte_bits;
    for (size_t index = 0; index + bytes < to.size(); ++index)
    {
        auto byte = depromote<uint8_t>(from[index + bytes] >> offset);
        if (index + bytes + 1u < to.size() && is_nonzero(offset))
            byte |= depromote<uint8_t>(from[index + bytes + 1u] << (byte_bits - offset));

        to[index] = byte;
    }

    return to;
}

template <typename Operation>
static hash_digest bytewise(const hash_digest& left, const hash_digest& right, Operation operation) NOEXCEPT
{
    hash_digest out{};
    std::transform(left.begin(), left.end(), right.begin(), out.begin(), [&](uint8_t first, uint8_t second) NOEXCEPT { return depromote<uint8_t>(operation(first, second)); });
    return out;
}

static bool almost_equal(double left, double right) NOEXCEPT
{
    return std::fabs(left - right) <= 4.0 * std::fabs(left) * std::numeric_limits<double>::epsilon();
}

BC_AUTO_TEST_CASE(uint256_t__basics__expected)
{
    BC_REQUIRE_NE(random1, random2);
    BC_REQUIRE_NE(zero256, one256);
    BC_REQUIRE_NE(max256, zero256);
    BC_REQUIRE_EQUAL(~max256, zero256);
    BC_REQUIRE_EQUAL((random1 ^ random2) ^ random1, random2);
    BC_REQUIRE_EQUAL(one256 << 256u, zero256);
    BC_REQUIRE_EQUAL(random1 & uint256_t{ max_uint64 }, uint256_t{ random1_low64 });
    BC_REQUIRE_EQUAL(uint256_t{ 0 }, zero256);
    BC_REQUIRE_EQUAL(uint256_t{ 1 }, one256);

    constexpr auto value = 0xc4dab720d9c7acaa_u64;
    for (size_t bit = 0; bit < 256u; ++bit)
    {
        BC_REQUIRE_NE(zero256, one256 << bit);
        BC_REQUIRE_NE(random1, random1 ^ (one256 << bit));
        BC_REQUIRE_NE(uint256_t{ value } ^ (one256 << bit), value);
    }
}

BC_AUTO_TEST_CASE(uint256_t__shifts__bytewise_shifts)
{
    for (size_t bit = 0; bit < 256u; ++bit)
    {
        BC_REQUIRE_EQUAL(to_uintx(shift_left(one_hash, bit)), one256 << bit);
        BC_REQUIRE_EQUAL(half256 >> (255u - bit), one256 << bit);
        BC_REQUIRE_EQUAL(to_uintx(shift_left(random1_hash, bit)), random1 << bit);
        BC_REQUIRE_EQUAL(to_uintx(shift_right(random1_hash, bit)), random1 >> bit);
        BC_REQUIRE_EQUAL(to_uintx(shift_left(max_hash, bit)), max256 << bit);
        BC_REQUIRE_EQUAL(to_uintx(shift_right(max_hash, bit)), max256 >> bit);

        auto left = random1;
        left <<= bit;
        BC_REQUIRE_EQUAL(left, random1 << bit);

        auto right = random1;
        right >>= bit;
        BC_REQUIRE_EQUAL(right, random1 >> bit);
    }

    constexpr uint256_t low{ 0x0123456789abcdef_u64 };
    constexpr auto high = low << 128u;
    for (size_t bit = 0; bit < 128u; ++bit)
    {
        BC_REQUIRE_EQUAL(low << bit, high >> (128u - bit));
    }

    for (size_t bit = 128; bit < 256u; ++bit)
    {
        BC_REQUIRE_EQUAL(low << bit, high << (bit - 128u));
    }
}

BC_AUTO_TEST_CASE(uint256_t__unary__expected)
{
    BC_REQUIRE_EQUAL(~zero256, max256);
    BC_REQUIRE_EQUAL(~random1, to_uintx(bytewise(random1_hash, max_hash, std::bit_xor<>{})));
    BC_REQUIRE_EQUAL(-zero256, zero256);
    BC_REQUIRE_EQUAL(-random1, ~random1 + 1u);

    for (size_t bit = 0; bit < 256u; ++bit)
    {
        BC_REQUIRE_EQUAL(-(one256 << bit), max256 << bit);
    }
}

BC_AUTO_TEST_CASE(uint256_t__bitwise__bytewise)
{
    const std::vector<std::pair<hash_digest, hash_digest>> pairs
    {
        { random1_hash, random2_hash },
        { random1_hash, null_hash },
        { random1_hash, max_hash },
        { null_hash, random1_hash },
        { max_hash, random1_hash },
        { from_uintx(~random1), random2_hash },
        { random1_hash, from_uintx(~random2) }
    };

    for (const auto& [left_hash, right_hash]: pairs)
    {
        const auto left = to_uintx(left_hash);
        const auto right = to_uintx(right_hash);
        BC_REQUIRE_EQUAL(to_uintx(bytewise(left_hash, right_hash, std::bit_or<>{})), left | right);
        BC_REQUIRE_EQUAL(to_uintx(bytewise(left_hash, right_hash, std::bit_xor<>{})), left ^ right);
        BC_REQUIRE_EQUAL(to_uintx(bytewise(left_hash, right_hash, std::bit_and<>{})), left & right);

        auto value = left;
        value |= right;
        BC_REQUIRE_EQUAL(value, left | right);

        value = left;
        value ^= right;
        BC_REQUIRE_EQUAL(value, left ^ right);

        value = left;
        value &= right;
        BC_REQUIRE_EQUAL(value, left & right);
    }

    constexpr auto value = 0xe1db685c9a0b47a2_u64;
    auto out = random1;
    out |= value;
    BC_REQUIRE_EQUAL(out, random1 | uint256_t{ value });

    out = random1;
    out |= 0;
    BC_REQUIRE_EQUAL(out, random1);

    out ^= 0;
    BC_REQUIRE_EQUAL(out, random1);

    out ^= value;
    BC_REQUIRE_EQUAL(out, random1 ^ uint256_t{ value });
}

BC_AUTO_TEST_CASE(uint256_t__comparison__expected)
{
    for (size_t bit = 0; bit < 256u; ++bit)
    {
        auto value = one256 << bit;
        BC_REQUIRE(value >= zero256);
        BC_REQUIRE(value > zero256);
        BC_REQUIRE(zero256 < value);
        BC_REQUIRE(zero256 <= value);
        BC_REQUIRE(value >= 0);
        BC_REQUIRE(value > 0);
        BC_REQUIRE(0 < value);
        BC_REQUIRE(0 <= value);

        value |= random1;
        BC_REQUIRE(value >= random1);
        BC_REQUIRE((value == random1) != (value > random1));
        BC_REQUIRE((value == random1) || !(value <= random1));
        BC_REQUIRE(random1 <= value);
        BC_REQUIRE((random1 == value) != (random1 < value));
        BC_REQUIRE((value == random1) || !(random1 >= value));
        BC_REQUIRE(!(value < random1));
        BC_REQUIRE(!(random1 > value));
    }

    BC_REQUIRE_LT(zero256, one256);
}

BC_AUTO_TEST_CASE(uint256_t__plus_minus__expected)
{
    BC_REQUIRE_EQUAL(random1 + random2, to_uintx(base16_hash("549fb09fea236a1ea3e31d4d58f1b1369288d204211ca751527cfc175767850c")));
    BC_REQUIRE_EQUAL(one256 + max256, zero256);
    BC_REQUIRE_EQUAL(max256 + one256, zero256);

    auto sum = zero256;
    sum += random1;
    BC_REQUIRE_EQUAL(sum, random1);

    sum += random2;
    BC_REQUIRE_EQUAL(sum, random1 + random2);

    for (size_t bit = 1; bit < 256u; ++bit)
    {
        const auto ones = max256 >> bit;
        const auto power = half256 >> (bit - 1u);
        BC_REQUIRE_EQUAL(ones + one256, power);
        BC_REQUIRE_EQUAL(one256 + ones, power);
        BC_REQUIRE_EQUAL(ones - (-one256), power);
        BC_REQUIRE_EQUAL(power - one256, ones);

        auto value = ones;
        value += one256;
        BC_REQUIRE_EQUAL(value, power);

        value = ones;
        value += 1;
        BC_REQUIRE_EQUAL(value, power);

        value = ones;
        BC_REQUIRE_EQUAL(value++, ones);
        BC_REQUIRE_EQUAL(value, power);

        value = power;
        BC_REQUIRE_EQUAL(value--, power);
        BC_REQUIRE_EQUAL(value, ones);

        value = power;
        BC_REQUIRE_EQUAL(--value, ones);
    }

    constexpr auto left = 0xbedc77e27940a7_u64;
    constexpr auto right = 0xee8d836fce66fb_u64;
    BC_REQUIRE_EQUAL(uint256_t{ left } + right, uint256_t{ left + right });

    auto value = uint256_t{ left };
    value += right;
    BC_REQUIRE_EQUAL(value, uint256_t{ left + right });

    value -= right;
    BC_REQUIRE_EQUAL(value, left);

    value = random1;
    BC_REQUIRE_EQUAL(++value, random1 + 1u);

    value = random1;
    BC_REQUIRE_EQUAL(--value, random1 - 1u);

    BC_REQUIRE_EQUAL(random1 - (-random2), random1 + random2);
    BC_REQUIRE_EQUAL(random1 - (-one256), random1 + one256);
    BC_REQUIRE_EQUAL(random1 - one256, random1 + (-one256));
}

BC_AUTO_TEST_CASE(uint256_t__multiply__expected)
{
    BC_REQUIRE_EQUAL(random1 * random1, to_uintx(base16_hash("62a38c0486f01e45879d7910a7761bf30d5237e9873f9bff3642a732c4d84f10")));
    BC_REQUIRE_EQUAL(random1 * random2, to_uintx(base16_hash("de37805e9986996cfba76ff6ba51c008df851987d9dd323f0e5de07760529c40")));
    BC_REQUIRE_EQUAL(random2 * random2, to_uintx(base16_hash("ac8c010096767d3cae5005dec28bb2b45a1d85ab7996ccd3e102a650f74ff100")));
    BC_REQUIRE_EQUAL(random1 * 3u, to_uintx(base16_hash("7759b1c0ed14047f961ad09b20ff83687876a0181a367b813634046f91def7d4")));
    BC_REQUIRE_EQUAL(random2 * 0x87654321_u32, to_uintx(base16_hash("23f7816e30c4ae2017257b7a0fa64d60402f5234d46e746b61c960d09a26d070")));
    BC_REQUIRE_EQUAL(random1 * zero256, zero256);
    BC_REQUIRE_EQUAL(random1 * one256, random1);
    BC_REQUIRE_EQUAL(random1 * max256, -random1);
    BC_REQUIRE_EQUAL(random2 * random1, random1 * random2);
    BC_REQUIRE_EQUAL(random2 * zero256, zero256);
    BC_REQUIRE_EQUAL(random2 * one256, random2);
    BC_REQUIRE_EQUAL(random2 * max256, -random2);
    BC_REQUIRE_EQUAL(max256 * max256, one256);
    BC_REQUIRE_EQUAL(random1 * 0, 0);
    BC_REQUIRE_EQUAL(random1 * 1, random1);
}

BC_AUTO_TEST_CASE(uint256_t__divide__expected)
{
    constexpr auto divisor1 = to_uintx(base16_hash("00000000000000000000000000000000000000000000000ad7133ac1977fa2b7"));
    constexpr auto divisor2 = to_uintx(base16_hash("0000000000000000000000000000000000000000000000000000000ecd751716"));
    BC_REQUIRE_EQUAL(random1 / divisor1, to_uintx(base16_hash("00000000000000000b8ac01106981635d9ed112290f8895545a7654dde28fb3a")));
    BC_REQUIRE_EQUAL(random1 / divisor2, to_uintx(base16_hash("000000000873ce8efec5b67150bad3aa8c5fcb70e947586153bf2cec7c37c57a")));
    BC_REQUIRE_EQUAL(random2 / divisor1, to_uintx(base16_hash("000000000000000013e1665895a1cc981de6d93670105a6b3ec3b73141b3a3c5")));
    BC_REQUIRE_EQUAL(random2 / divisor2, to_uintx(base16_hash("000000000e8f0abe753bb0afe2e9437ee85d280be60882cf0bd1aaf7fa3cc2c4")));
    BC_REQUIRE_EQUAL(random1 / one256, random1);
    BC_REQUIRE_EQUAL(random1 / max256, zero256);
    BC_REQUIRE_EQUAL(max256 / random1, 2);
    BC_REQUIRE_EQUAL(random2 / one256, random2);
    BC_REQUIRE_EQUAL(random2 / max256, zero256);
    BC_REQUIRE_EQUAL(max256 / random2, 1);
}

BC_AUTO_TEST_CASE(uint256_t__double__expected)
{
    BC_REQUIRE_EQUAL(static_cast<double>(zero256), 0.0);

    for (size_t bit = 0; bit < 255u; ++bit)
    {
        BC_REQUIRE_EQUAL(static_cast<double>(one256 << bit), std::ldexp(1.0, possible_narrow_and_sign_cast<int>(bit)));
    }

    for (size_t bit = 256; bit > 53u; --bit)
    {
        BC_REQUIRE(almost_equal(static_cast<double>(random1 >> (256u - bit)), std::ldexp(random1_double, possible_narrow_and_sign_cast<int>(bit))));
    }

    const auto random1_high64 = static_cast<uint64_t>(random1 >> 192u);
    for (size_t bit = 53; bit > 0u; --bit)
    {
        BC_REQUIRE_EQUAL(static_cast<double>(random1 >> (256u - bit)), static_cast<double>(random1_high64 >> (64u - bit)));
    }
}

BC_AUTO_TEST_CASE(uint256_t__self_operand__expected)
{
    uint256_t value{ 2 };
    value *= value;
    BC_REQUIRE_EQUAL(value, 4u);

    value /= value;
    BC_REQUIRE_EQUAL(value, 1u);

    value += value;
    BC_REQUIRE_EQUAL(value, 2u);

    value -= value;
    BC_REQUIRE_EQUAL(value, 0u);
}

BC_AUTO_TEST_SUITE_END()
