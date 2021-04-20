/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system.hpp>

using namespace bc::system;

BOOST_AUTO_TEST_SUITE(data_tests)

// to_array(byte)

BOOST_AUTO_TEST_CASE(data__to_array1__value__expected_size_and_value)
{
    const uint8_t expected = 42;
    const auto result = to_array(expected);
    BOOST_REQUIRE_EQUAL(result.size(), 1u);
    BOOST_REQUIRE_EQUAL(result[0], expected);
}

// to_array<>

BOOST_AUTO_TEST_CASE(data__to_array2__double_long_hash__expected)
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

// to_chunk()

BOOST_AUTO_TEST_CASE(data__to_chunk2__long_hash__expected)
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

// build_array

BOOST_AUTO_TEST_CASE(data__build_array__capacity_empty__zeroed)
{
    const auto result = build_array<3>({});
    BOOST_REQUIRE_EQUAL(result.size(), 3u);
    BOOST_REQUIRE_EQUAL(result[0], 0u);
    BOOST_REQUIRE_EQUAL(result[1], 0u);
    BOOST_REQUIRE_EQUAL(result[2], 0u);
}

BOOST_AUTO_TEST_CASE(data__build_array__capacity__expected_values)
{
    const auto result = build_array<2 + 1 + 3>(
    {
        byte_array<2>{ { 42, 14 } },
        byte_array<1>{ { 12 } },
        byte_array<3>{ { 24, 0, 15 } }
    });
    BOOST_REQUIRE_EQUAL(result.size(), 6u);
    BOOST_REQUIRE_EQUAL(result[0], 42u);
    BOOST_REQUIRE_EQUAL(result[1], 14u);
    BOOST_REQUIRE_EQUAL(result[2], 12u);
    BOOST_REQUIRE_EQUAL(result[3], 24u);
    BOOST_REQUIRE_EQUAL(result[4], 0u);
    BOOST_REQUIRE_EQUAL(result[5], 15u);
}

BOOST_AUTO_TEST_CASE(data__build_array__under_capacity__expected_zeroed)
{
    const auto result = build_array<3>(
    {
        byte_array<2>{ { 24, 42 } }
    });
    BOOST_REQUIRE_EQUAL(result.size(), 3u);
    BOOST_REQUIRE_EQUAL(result[0], 24);
    BOOST_REQUIRE_EQUAL(result[1], 42);
    BOOST_REQUIRE_EQUAL(result[2], 0);
}

BOOST_AUTO_TEST_CASE(data__build_array__overflow__truncated_values)
{
    const auto result = build_array<3>(
    {
        byte_array<2>{ { 1, 2 } },
        byte_array<2>{ { 3, 4 } },
        byte_array<3>{ { 5, 6, 7 } }
    });
    BOOST_REQUIRE_EQUAL(result.size(), 3u);
    BOOST_REQUIRE_EQUAL(result[0], 1u);
    BOOST_REQUIRE_EQUAL(result[1], 2u);
    BOOST_REQUIRE_EQUAL(result[2], 3u);
}

// build_chunk()

BOOST_AUTO_TEST_CASE(data__build_chunk__empty__empty)
{
    const auto result = build_chunk({});
    BOOST_REQUIRE(result.empty());
}

BOOST_AUTO_TEST_CASE(data__build_chunk__three_vectors__expected_size_and_value)
{
    const auto result = build_chunk(
    {
        data_chunk{ 24 },
        data_chunk{ 42 },
        data_chunk{ 48 }
    });
    BOOST_REQUIRE_EQUAL(result.size(), 3u);
    BOOST_REQUIRE_EQUAL(result[0], 24u);
    BOOST_REQUIRE_EQUAL(result[1], 42u);
    BOOST_REQUIRE_EQUAL(result[2], 48u);
}

BOOST_AUTO_TEST_CASE(data__build_chunk__three_arrays__expected_size_and_value)
{
    const auto result = build_chunk(
    {
        byte_array<2>{ { 0, 2 } },
        byte_array<1>{ { 4 } },
        byte_array<3>{ { 6, 8, 10 } }
    });
    BOOST_REQUIRE_EQUAL(result.size(), 6u);
    BOOST_REQUIRE_EQUAL(result[0], 0u);
    BOOST_REQUIRE_EQUAL(result[1], 2u);
    BOOST_REQUIRE_EQUAL(result[2], 4u);
    BOOST_REQUIRE_EQUAL(result[3], 6u);
    BOOST_REQUIRE_EQUAL(result[4], 8u);
    BOOST_REQUIRE_EQUAL(result[5], 10u);
}

// build_chunk(,)

BOOST_AUTO_TEST_CASE(data__build_chunk__extra_reserve__expected_size_and_reserve)
{
    const auto result = build_chunk(
    {
        byte_array<2>{ { 1, 1 } },
        byte_array<1>{ { 42 } },
        byte_array<3>{ { 1, 1, 1 } }
    }, 2);
    BOOST_REQUIRE_EQUAL(result[0], 1u);
    BOOST_REQUIRE_EQUAL(result[1], 1u);
    BOOST_REQUIRE_EQUAL(result[2], 42u);
    BOOST_REQUIRE_EQUAL(result[3], 1u);
    BOOST_REQUIRE_EQUAL(result[4], 1u);
    BOOST_REQUIRE_EQUAL(result[5], 1u);
    BOOST_REQUIRE_EQUAL(result.capacity(), 2u + 1u + 3u + 2);
    BOOST_REQUIRE_EQUAL(result.size(), 2u + 1u + 3u);
}

// extend_data

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

// slice

BOOST_AUTO_TEST_CASE(data__slice__empty_selection__empty)
{
    const byte_array<3> source
    {
        { 0, 0, 0 }
    };
    const auto result = slice<2, 2>(source);
    BOOST_REQUIRE(result.empty());
}

BOOST_AUTO_TEST_CASE(data__slice__three_bytes_front__expected)
{
    const byte_array<3> source
    {
        { 24, 42, 55 }
    };
    const auto result = slice<0, 3>(source);
    BOOST_REQUIRE_EQUAL(result.size(), 3u);
    BOOST_REQUIRE_EQUAL(result[0], 24u);
    BOOST_REQUIRE_EQUAL(result[1], 42u);
    BOOST_REQUIRE_EQUAL(result[2], 55u);
}

BOOST_AUTO_TEST_CASE(data__slice__three_bytes_middle__expected)
{
    const byte_array<3> source
    {
        { 7, 42, 5 }
    };
    const auto result = slice<1, 2>(source);
    BOOST_REQUIRE_EQUAL(result.size(), 1u);
    BOOST_REQUIRE_EQUAL(result[0], 42u);
}

BOOST_AUTO_TEST_CASE(data__slice__three_bytes_end__expected)
{
    const byte_array<3> source
    {
        { 2, 7, 11 }
    };
    const auto result = slice<1, 3>(source);
    BOOST_REQUIRE_EQUAL(result.size(), 2u);
    BOOST_REQUIRE_EQUAL(result[0], 7u);
    BOOST_REQUIRE_EQUAL(result[1], 11u);
}

// splice(,)

BOOST_AUTO_TEST_CASE(data__splice2__two_bytes_each__expected)
{
    const byte_array<2> left
    {
        { 42, 0 }
    };
    const byte_array<2> right
    {
        { 0, 24 }
    };
    const auto result = splice(left, right);
    BOOST_REQUIRE_EQUAL(result.size(), 4u);
    BOOST_REQUIRE_EQUAL(result[0], 42u);
    BOOST_REQUIRE_EQUAL(result[1], 0u);
    BOOST_REQUIRE_EQUAL(result[2], 0u);
    BOOST_REQUIRE_EQUAL(result[3], 24u);
}

// splice(,,)

BOOST_AUTO_TEST_CASE(data__splice3__one_two_three_bytes__expected)
{
    const byte_array<1> left
    {
        { 42 }
    };
    const byte_array<2> middle
    {
        { 7, 11 }
    };
    const byte_array<3> right
    {
        { 5, 0, 24 }
    };
    const auto result = splice(left, middle, right);
    BOOST_REQUIRE_EQUAL(result.size(), 6u);
    BOOST_REQUIRE_EQUAL(result[0], 42u);
    BOOST_REQUIRE_EQUAL(result[1], 7u);
    BOOST_REQUIRE_EQUAL(result[2], 11u);
    BOOST_REQUIRE_EQUAL(result[3], 5u);
    BOOST_REQUIRE_EQUAL(result[4], 0u);
    BOOST_REQUIRE_EQUAL(result[5], 24u);
}

// split

BOOST_AUTO_TEST_CASE(data__split__two_bytes__expected)
{
    const byte_array<2> source
    {
        { 42, 24 }
    };

    const auto result = split(source);
    BOOST_REQUIRE_EQUAL(result.first.size(), 1u);
    BOOST_REQUIRE_EQUAL(result.first[0], 42u);
    BOOST_REQUIRE_EQUAL(result.second.size(), 1u);
    BOOST_REQUIRE_EQUAL(result.second[0], 24u);
}

BOOST_AUTO_TEST_CASE(data__split__long_hash__expected)
{
    const long_hash source
    {
        {
            5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        }
    };
    const auto result = split(source);
    BOOST_REQUIRE_EQUAL(result.first.size(), 32u);
    BOOST_REQUIRE_EQUAL(result.first[0], 5u);
    BOOST_REQUIRE_EQUAL(result.second.size(), 32u);
    BOOST_REQUIRE_EQUAL(result.second[0], 9u);
}

// starts_with

BOOST_AUTO_TEST_CASE(data__starts_with__empty_empty__true)
{
    static const data_chunk buffer;
    static const data_chunk sequence;
    BOOST_REQUIRE(starts_with(buffer.begin(), buffer.end(), sequence));
}

BOOST_AUTO_TEST_CASE(data__starts_with__not_empty_empty__false)
{
    static const data_chunk buffer;
    static const data_chunk sequence{ 42 };
    BOOST_REQUIRE(!starts_with(buffer.begin(), buffer.end(), sequence));
}

BOOST_AUTO_TEST_CASE(data__starts_with__same_vectors__true)
{
    static const data_chunk buffer{ 42 };
    static const data_chunk sequence{ 42 };
    BOOST_REQUIRE(starts_with(buffer.begin(), buffer.end(), sequence));
}

BOOST_AUTO_TEST_CASE(data__starts_with__matched_vectors__true)
{
    static const data_chunk buffer{ 42, 24, 15 };
    static const data_chunk sequence{ 42 };
    BOOST_REQUIRE(starts_with(buffer.begin(), buffer.end(), sequence));
}

BOOST_AUTO_TEST_CASE(data__starts_with__mismatched_arrays__false)
{
    // Must be the same length when using arrays.
    static const byte_array<2> buffer{ { 24, 0 } };
    static const byte_array<2> sequence{ { 24, 42 } };
    const auto result = starts_with(buffer.begin(), buffer.end(), sequence);
    BOOST_REQUIRE(!result);
}

BOOST_AUTO_TEST_CASE(data__starts_with__matched_arrays__true)
{
    // Must be the same length when using arrays.
    static const byte_array<2> buffer{ { 24, 42 } };
    static const byte_array<2> sequence{ { 24, 42 } };
    const auto result = starts_with(buffer.begin(), buffer.end(), sequence);
    BOOST_REQUIRE(result);
}

// xor_data

BOOST_AUTO_TEST_CASE(data__xor_data1__empty__empty)
{
    static const byte_array<1> source{ { 0x0 } };
    const auto result = xor_data<0>(source, source);
    BOOST_REQUIRE_EQUAL(result.size(), 0u);
}

BOOST_AUTO_TEST_CASE(data__xor_data1__same__zeros)
{
    static const byte_array<2> source{ { 7, 11 } };
    const auto result = xor_data<2>(source, source);
    BOOST_REQUIRE_EQUAL(result.size(), 2u);
    BOOST_REQUIRE_EQUAL(result[0], 0u);
    BOOST_REQUIRE_EQUAL(result[1], 0u);
}

BOOST_AUTO_TEST_CASE(data__xor_data1__distinct__ones)
{
    static const byte_array<2> source1{ { 0x00, 0xff } };
    static const byte_array<2> source2{ { 0xff, 0x00 } };
    const auto result = xor_data<2>(source1, source2);
    BOOST_REQUIRE_EQUAL(result.size(), 2u);
    BOOST_REQUIRE_EQUAL(result[0], 0xff);
    BOOST_REQUIRE_EQUAL(result[1], 0xff);
}

// xor_offset

BOOST_AUTO_TEST_CASE(data__xor_offset__empty__empty)
{
    static const byte_array<1> source{ { 0x0 } };
    const auto result = xor_offset<0, 0, 0>(source, source);
    BOOST_REQUIRE_EQUAL(result.size(), 0u);
}

BOOST_AUTO_TEST_CASE(data__xor_offset__empty_out__empty)
{
    static const byte_array<2> source1{ { 0x00, 0xff } };
    static const byte_array<3> source2{ { 0xff, 0x00, 0xff } };
    const auto result = xor_offset<0, 0, 0>(source1, source2);
    BOOST_REQUIRE_EQUAL(result.size(), 0u);
}

BOOST_AUTO_TEST_CASE(data__xor_offset__same__zeros)
{
    static const byte_array<2> source1{ { 0x00, 0xff } };
    const auto result = xor_offset<2, 0, 0>(source1, source1);
    BOOST_REQUIRE_EQUAL(result.size(), 2u);
    BOOST_REQUIRE_EQUAL(result[0], 0);
    BOOST_REQUIRE_EQUAL(result[1], 0);
}
BOOST_AUTO_TEST_CASE(data__xor_offset__distinct_trimmed__ones)
{
    static const byte_array<3> source1{ { 0x00, 0xff, 0x00 } };
    static const byte_array<4> source2{ { 0xff, 0x00, 0xff, 0x00 } };
    const auto result = xor_offset<3, 0, 0>(source1, source2);
    BOOST_REQUIRE_EQUAL(result.size(), 3u);
    BOOST_REQUIRE_EQUAL(result[0], 0xff);
    BOOST_REQUIRE_EQUAL(result[1], 0xff);
    BOOST_REQUIRE_EQUAL(result[2], 0xff);
}

BOOST_AUTO_TEST_CASE(data__xor_offset__distinct_same_offsets_expected)
{
    const uint8_t expected0 = 42u ^ 24u;
    const uint8_t expected1 = 13u ^ 13u;
    static const byte_array<4> source1{ { 7, 42, 13, 0x00 } };
    static const byte_array<5> source2{ { 7, 24, 13, 0xff, 0x00 } };
    const auto result = xor_offset<2, 1, 1>(source1, source2);
    BOOST_REQUIRE_EQUAL(result.size(), 2u);
    BOOST_REQUIRE_EQUAL(result[0], expected0);
    BOOST_REQUIRE_EQUAL(result[1], expected1);
}

BOOST_AUTO_TEST_CASE(data__xor_offset__distinct_different_offsets_expected)
{
    const uint8_t expected0 = 42u ^ 24u;
    const uint8_t expected1 = 13u ^ 13u;
    static const byte_array<4> source1{ { 7, 42, 13, 0x00 } };
    static const byte_array<5> source2{ { 0, 7, 24, 13, 0xff } };
    const auto result = xor_offset<2, 1, 2>(source1, source2);
    BOOST_REQUIRE_EQUAL(result.size(), 2u);
    BOOST_REQUIRE_EQUAL(result[0], expected0);
    BOOST_REQUIRE_EQUAL(result[1], expected1);
}

BOOST_AUTO_TEST_SUITE_END()
