/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <map>
#include <vector>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(utility)

BOOST_AUTO_TEST_CASE(data__to_byte__value__expected_size_and_value)
{
    const uint8_t expected = 42;
    const auto result = to_byte(expected);
    BOOST_REQUIRE_EQUAL(result.size(), 1u);
    BOOST_REQUIRE_EQUAL(result[0], expected);
}

BOOST_AUTO_TEST_CASE(data__build_data__empty__empty)
{
    const auto result = build_data({});
    BOOST_REQUIRE(result.empty());
}

BOOST_AUTO_TEST_CASE(data__build_data__one_slice__expected_size_and_value)
{
    const uint8_t expected = 42;
    const auto chunk1 = std::vector<uint8_t>{ { 24 } };
    const auto chunk2 = std::vector<uint8_t>{ { expected } };
    const auto chunk3 = std::vector<uint8_t>{ { 48 } };
    const auto result = build_data(
    {
        // Inline initialization doesn't work with vector (?).
        chunk1, chunk2, chunk3
    });
    BOOST_REQUIRE_EQUAL(result.size(), 3);
    BOOST_REQUIRE_EQUAL(result[1], expected);
}

BOOST_AUTO_TEST_CASE(data__build_data__three_slices__expected_size_and_value)
{
    const size_t size1 = 2;
    const size_t size2 = 1;
    const size_t size3 = 3;
    const uint8_t expected = 42;
    const auto result = build_data(
    {
        std::array<uint8_t, size1>{ { 0, 0} },
        std::array<uint8_t, size2>{ { expected } },
        std::array<uint8_t, size3>{ { 0, 0, 0 } }
    });
    BOOST_REQUIRE_EQUAL(result.size(), size1 + size2 + size3);
    BOOST_REQUIRE_EQUAL(result[size1], expected);
}

BOOST_AUTO_TEST_CASE(data__build_data__extra_reserve__expected_size_and_capacity)
{
    const uint8_t size1 = 2;
    const uint8_t size2 = 1;
    const uint8_t size3 = 3;
    const auto reserve = 42;
    const auto result = build_data(
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
    data_chunk buffer1{ { 0 } };
    extend_data(buffer1, null_hash);
    data_chunk buffer2{ { expected } };
    extend_data(buffer1, buffer2);
    extend_data(buffer1, null_hash);
    BOOST_REQUIRE_EQUAL(buffer1.size(), 2u * hash_size + 2u);
    BOOST_REQUIRE_EQUAL(buffer1[hash_size + 1], expected);
}

BOOST_AUTO_TEST_CASE(data__range_constrain__over__max)
{
    const size_t expected = 10;
    const auto result = range_constrain(size_t(42), size_t(1), expected);
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(data__range_constrain__under__min)
{
    const size_t expected = 50;
    const auto result = range_constrain(size_t(42), expected, size_t(100));
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(data__range_constrain__internal__unchanged)
{
    const size_t expected = 42;
    const auto result = range_constrain(expected, size_t(10), size_t(100));
    BOOST_REQUIRE_EQUAL(result, expected);
}

BOOST_AUTO_TEST_CASE(data__slice1__three_bytes_front__expected)
{
    const uint8_t expected = 24;
    const data_chunk source
    {
        expected, 42, 42
    };

    BOOST_REQUIRE_EQUAL(slice(source, 0, 3)[0], expected);
}

BOOST_AUTO_TEST_CASE(data__slice1__three_bytes_middle__expected)
{
    const uint8_t expected = 24;
    const data_chunk source
    {
        42, expected, 42
    };

    BOOST_REQUIRE_EQUAL(slice(source, 1, 2)[0], expected);
}

BOOST_AUTO_TEST_CASE(data__slice1__three_bytes_end__expected)
{
    const uint8_t expected = 24;
    const data_chunk source
    {
        42, 42, expected
    };

    BOOST_REQUIRE_EQUAL(slice(source, 2, 3)[0], expected);
}

BOOST_AUTO_TEST_CASE(data__slice1__three_bytes_middle_empty__expected)
{
    const uint8_t expected = 24;
    const data_chunk source
    {
        42, 42, 42
    };

    BOOST_REQUIRE(slice(source, 2, 2).empty());
}

BOOST_AUTO_TEST_CASE(data__slice1__three_bytes_end_empty__expected)
{
    const uint8_t expected = 24;
    const data_chunk source
    {
        42, 42, 42
    };

    BOOST_REQUIRE(slice(source, 3, 3).empty());
}

BOOST_AUTO_TEST_CASE(data__slice2__three_bytes_end__expected)
{
    const uint8_t expected = 24;
    const data_chunk source
    {
        42, 42, expected
    };

    BOOST_REQUIRE_EQUAL(slice(source, bounds{ 2, 3 })[0], expected);
}

BOOST_AUTO_TEST_CASE(data__split__data_chunk__expected)
{
    const uint8_t expected_lower = 42;
    const uint8_t expected_upper = 24;
    data_chunk source
    {
        expected_lower, expected_upper
    };

    data_chunk lower_half;
    data_chunk upper_half;
    split(source, lower_half, upper_half, source.size());
    BOOST_REQUIRE_EQUAL(lower_half[0], expected_lower);
    BOOST_REQUIRE_EQUAL(upper_half[0], expected_upper);
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
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        } 
    };

    data_chunk lower_half;
    data_chunk upper_half;
    split(source, lower_half, upper_half, source.size());
    BOOST_REQUIRE_EQUAL(lower_half[0], l);
    BOOST_REQUIRE_EQUAL(upper_half[0], u);
}

BOOST_AUTO_TEST_CASE(data__to_data_chunk__long_hash__expected)
{
    const uint8_t l = 42;
    const uint8_t u = 24;
    long_hash source
    { 
        {
            l, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            u, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        } 
    };

    const auto result = to_data_chunk(source);
    BOOST_REQUIRE_EQUAL(result[0], l);
    BOOST_REQUIRE_EQUAL(result[32], u);
}

BOOST_AUTO_TEST_SUITE_END()
