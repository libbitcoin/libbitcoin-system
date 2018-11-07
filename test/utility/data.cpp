/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <map>
#include <vector>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(data_tests)

BOOST_AUTO_TEST_CASE(data__to_byte__value__expected_size_and_value)
{
    const uint8_t expected = 42;
    const auto result = to_array(expected);
    BOOST_REQUIRE_EQUAL(result.size(), 1u);
    BOOST_REQUIRE_EQUAL(result[0], expected);
}

BOOST_AUTO_TEST_CASE(data__build_chunk__empty__empty)
{
    const auto result = build_chunk({});
    BOOST_REQUIRE(result.empty());
}

BOOST_AUTO_TEST_CASE(data__build_chunk__one_slice__expected_size_and_value)
{
    const uint8_t expected = 42;
    const auto chunk1 = std::vector<uint8_t>{ 24 };
    const auto chunk2 = std::vector<uint8_t>{ expected };
    const auto chunk3 = std::vector<uint8_t>{ 48 };
    const auto result = build_chunk(
    {
        // Inline initialization doesn't work with vector (?).
        chunk1, chunk2, chunk3
    });
    BOOST_REQUIRE_EQUAL(result.size(), 3);
    BOOST_REQUIRE_EQUAL(result[1], expected);
}

BOOST_AUTO_TEST_CASE(data__build_chunk__three_slices__expected_size_and_value)
{
    const size_t size1 = 2;
    const size_t size2 = 1;
    const size_t size3 = 3;
    const uint8_t expected = 42;
    const auto result = build_chunk(
    {
        std::array<uint8_t, size1>{ { 0, 0} },
        std::array<uint8_t, size2>{ { expected } },
        std::array<uint8_t, size3>{ { 0, 0, 0 } }
    });
    BOOST_REQUIRE_EQUAL(result.size(), size1 + size2 + size3);
    BOOST_REQUIRE_EQUAL(result[size1], expected);
}

BOOST_AUTO_TEST_CASE(data__build_chunk__extra_reserve__expected_size_and_capacity)
{
    const uint8_t size1 = 2;
    const uint8_t size2 = 1;
    const uint8_t size3 = 3;
    const auto reserve = 42;
    const auto result = build_chunk(
    {
        std::array<uint8_t, size1>{ { 0, 0 } },
        std::array<uint8_t, size2>{ { 0 } },
        std::array<uint8_t, size3>{ { 0, 0, 0 } }
    }, reserve);
    BOOST_REQUIRE_EQUAL(result.size(), size1 + size2 + size3);
    BOOST_REQUIRE_EQUAL(result.capacity(), size1 + size2 + size3 + reserve);
}

BOOST_AUTO_TEST_CASE(data__build_array__empty__true_unchanged)
{
    const uint8_t expected = 42;
    std::array<uint8_t, 3> value{ { 0, expected, 0 } };
    const auto result = build_array(value, {});
    BOOST_REQUIRE(result);
    BOOST_REQUIRE_EQUAL(value[1], expected);
}

BOOST_AUTO_TEST_CASE(data__build_array__under_capacity__true_excess_unchanged)
{
    const uint8_t expected1 = 24;
    const uint8_t expected2 = 42;
    const uint8_t expected3 = 48;
    std::array<uint8_t, 3> value{ { 0, 0, expected3 } };
    const auto result = build_array(value,
    {
        std::array<uint8_t, 2>{ { expected1, expected2 } }
    });
    BOOST_REQUIRE(result);
    BOOST_REQUIRE_EQUAL(value[0], expected1);
    BOOST_REQUIRE_EQUAL(value[1], expected2);
    BOOST_REQUIRE_EQUAL(value[2], expected3);
}

BOOST_AUTO_TEST_CASE(data__build_array__exact_fill_multiple_slices__true_expected_values)
{
    const size_t size1 = 2;
    const size_t size2 = 1;
    const size_t size3 = 3;
    const uint8_t expected = 42;
    std::array<uint8_t, size1 + size2 + size3> value;
    const auto two_byte_vector = std::vector<uint8_t>{ { expected, expected } };
    const auto result = build_array(value,
    {
        two_byte_vector,
        std::array<uint8_t, size2>{ { expected } },
        std::array<uint8_t, size3>{ { expected, 0, 0 } }
    });
    BOOST_REQUIRE(result);
    BOOST_REQUIRE_EQUAL(value[size1], expected);
    BOOST_REQUIRE_EQUAL(value[size1 + size2], expected);
}

BOOST_AUTO_TEST_CASE(data__build_array__overflow__returns_false)
{
    std::array<uint8_t, 2> value;
    const auto result = build_array(value,
    {
        std::array<uint8_t, 2>{ { 1, 2 } },
        std::array<uint8_t, 2>{ { 3, 4 } }
    });
    BOOST_REQUIRE(!result);
}

BOOST_AUTO_TEST_CASE(data__extend_data__twice__expected)
{
    const uint8_t expected = 24;
    data_chunk buffer1{ 0 };
    extend_data(buffer1, null_hash);
    data_chunk buffer2{ expected };
    extend_data(buffer1, buffer2);
    extend_data(buffer1, null_hash);
    BOOST_REQUIRE_EQUAL(buffer1.size(), 2u * hash_size + 2u);
    BOOST_REQUIRE_EQUAL(buffer1[hash_size + 1], expected);
}

BOOST_AUTO_TEST_CASE(data__slice__empty_selection__compiles)
{
    const byte_array<3> source
    {
        { 0, 0, 0 }
    };

    slice<2, 2>(source);
    BOOST_REQUIRE(true);
}

BOOST_AUTO_TEST_CASE(data__slice__three_bytes_front__expected)
{
    const uint8_t expected = 24;
    const byte_array<3> source
    {
        { expected, 42, 42 }
    };

    const auto result = slice<0, 3>(source)[0];
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(data__slice__three_bytes_middle__expected)
{
    const uint8_t expected = 24;
    const byte_array<3> source
    {
        { 42, expected, 42 }
    };

    const auto result = slice<1, 2>(source)[0];
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(data__slice__three_bytes_end__expected)
{
    const uint8_t expected = 24;
    const byte_array<3> source
    {
        { 42, 42, expected }
    };

    const auto result = slice<2, 3>(source)[0];
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(data__split__two_bytes__expected)
{
    const uint8_t expected_left = 42;
    const uint8_t expected_right = 24;
    const byte_array<2> source
    {
        { expected_left, expected_right }
    };

    const auto parts = split(source);
    BOOST_REQUIRE_EQUAL(parts.left[0], expected_left);
    BOOST_REQUIRE_EQUAL(parts.right[0], expected_right);
}

BOOST_AUTO_TEST_CASE(data__split__long_hash__expected)
{
    const uint8_t l = 42;
    const uint8_t u = 24;
    long_hash source
    {
        {
            l, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            u, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        }
    };

    const auto parts = split(source);
    BOOST_REQUIRE_EQUAL(parts.left[0], l);
    BOOST_REQUIRE_EQUAL(parts.right[0], u);
}

BOOST_AUTO_TEST_CASE(data__splice_two__two_bytes_each__expected)
{
    const uint8_t expected_left = 42;
    const uint8_t expected_right = 24;

    const byte_array<2> left
    {
        { expected_left, 0 }
    };

    const byte_array<2> right
    {
        { 0, expected_right }
    };

    const auto combined = splice(left, right);
    BOOST_REQUIRE_EQUAL(combined[0], expected_left);
    BOOST_REQUIRE_EQUAL(combined[3], expected_right);
}

BOOST_AUTO_TEST_CASE(data__splice_three__one_two_three_bytes__expected)
{
    const uint8_t expected_left = 42;
    const uint8_t expected_right = 24;

    const byte_array<1> left
    {
        { expected_left }
    };

    const byte_array<2> middle
    {
        { 0, 0 }
    };

    const byte_array<3> right
    {
        { 0, 0, expected_right }
    };

    const auto combined = splice(left, middle, right);
    BOOST_REQUIRE_EQUAL(combined[0], expected_left);
    BOOST_REQUIRE_EQUAL(combined[5], expected_right);
}

BOOST_AUTO_TEST_CASE(data__to_array_slice__double_long_hash__expected)
{
    const uint8_t l = 42;
    const uint8_t u = 24;
    const data_chunk source
    {
        l, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        u, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    const auto result = to_array<long_hash_size>(source);
    BOOST_REQUIRE_EQUAL(result[0], l);
    BOOST_REQUIRE_EQUAL(result[long_hash_size / 2], u);
}

BOOST_AUTO_TEST_CASE(data__to_chunk__long_hash__expected)
{
    const uint8_t l = 42;
    const uint8_t u = 24;
    const long_hash source
    {
        {
            l, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            u, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        }
    };

    const auto result = to_chunk(source);
    BOOST_REQUIRE_EQUAL(result[0], l);
    BOOST_REQUIRE_EQUAL(result[32], u);
}

BOOST_AUTO_TEST_CASE(data__starts_with__empty_empty__true)
{
    static const data_chunk buffer{};
    static const data_chunk sequence{};
    BOOST_REQUIRE(starts_with(buffer.begin(), buffer.end(), sequence));
}

BOOST_AUTO_TEST_CASE(data__starts_with__not_empty_empty__false)
{
    static const data_chunk buffer{};
    static const data_chunk sequence{ 42 };
    BOOST_REQUIRE(!starts_with(buffer.begin(), buffer.end(), sequence));
}

BOOST_AUTO_TEST_CASE(data__starts_with__same_same__true)
{
    static const data_chunk buffer{ 42 };
    static const data_chunk sequence{ 42 };
    BOOST_REQUIRE(starts_with(buffer.begin(), buffer.end(), sequence));
}

BOOST_AUTO_TEST_CASE(data__starts_with__too_short__false)
{
    static const data_chunk buffer{ 42 };
    static const data_chunk sequence{ 42, 24 };
    BOOST_REQUIRE(!starts_with(buffer.begin(), buffer.end(), sequence));
}

BOOST_AUTO_TEST_CASE(data__xor_data0__same__zeros)
{
    static const data_chunk source{ 0, 1 };
    const auto result = xor_data<2>(source, source);
    BOOST_REQUIRE_EQUAL(result.size(), 2u);
    BOOST_REQUIRE_EQUAL(result[0], 0);
    BOOST_REQUIRE_EQUAL(result[1], 0);
}

BOOST_AUTO_TEST_CASE(data__xor_data1__empty__empty)
{
    static const data_chunk source{};
    const auto result = xor_data<0>(source, source, 0);
    BOOST_REQUIRE_EQUAL(result.size(), 0);
}

BOOST_AUTO_TEST_CASE(data__xor_data1__same__zeros)
{
    static const data_chunk source{ 0, 1 };
    const auto result = xor_data<2>(source, source, 0);
    BOOST_REQUIRE_EQUAL(result.size(), 2u);
    BOOST_REQUIRE_EQUAL(result[0], 0);
    BOOST_REQUIRE_EQUAL(result[1], 0);
}

BOOST_AUTO_TEST_CASE(data__xor_data1__same_offset__zeros)
{
    static const data_chunk source{ 0, 1, 0 };
    const auto result = xor_data<2>(source, source, 1);
    BOOST_REQUIRE_EQUAL(result.size(), 2u);
    BOOST_REQUIRE_EQUAL(result[0], 0);
    BOOST_REQUIRE_EQUAL(result[1], 0);
}

BOOST_AUTO_TEST_CASE(data__xor_data1__distinct__ones)
{
    static const data_chunk source{ 0, 1, 0 };
    static const data_chunk opposite{ 1, 0, 1 };
    const auto result = xor_data<3>(source, opposite, 0);
    BOOST_REQUIRE_EQUAL(result.size(), 3u);
    BOOST_REQUIRE_EQUAL(result[0], 1);
    BOOST_REQUIRE_EQUAL(result[1], 1);
    BOOST_REQUIRE_EQUAL(result[2], 1);
}

BOOST_AUTO_TEST_CASE(data__xor_data1__distinct_bites__bits)
{
    static const data_chunk source1{ 42, 13 };
    static const data_chunk source2{ 24, 13 };
    const auto result = xor_data<2>(source1, source2, 0);
    BOOST_REQUIRE_EQUAL(result.size(), 2u);
    BOOST_REQUIRE_EQUAL(result[0], 50u);
    BOOST_REQUIRE_EQUAL(result[1], 0);
}

BOOST_AUTO_TEST_CASE(data__xor_data2__distinct_bites__bits)
{
    static const data_chunk source1{ 0, 42, 13 };
    static const data_chunk source2{ 0, 0, 24, 13 };
    const auto result = xor_data<2>(source1, source2, 1, 2);
    BOOST_REQUIRE_EQUAL(result.size(), 2u);
    BOOST_REQUIRE_EQUAL(result[0], 50u);
    BOOST_REQUIRE_EQUAL(result[1], 0);
}

BOOST_AUTO_TEST_SUITE_END()
