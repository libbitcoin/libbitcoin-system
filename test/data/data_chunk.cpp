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

BOOST_AUTO_TEST_SUITE(data_chunk_tests)

BOOST_AUTO_TEST_CASE(data_chunk__types__all__expected)
{
    constexpr uint8_t byte{ 0u };

    // chunk_[c]ptr

    // mutable (private)
    chunk_ptr p1 = std::make_shared<data_chunk>(data_chunk{});
    p1->push_back(byte);        // modify container
    p1->back() = byte;          // modify element

    // immutable (public)
    chunk_cptr p2 = std::make_shared<const data_chunk>(data_chunk{});
    ////p2->push_back(byte);    // const container
    ////p2->back() = byte;      // const element

    // const_ptr is NOT move/copy assignable to ptr.

    // use const& to avoid copy.
    // ptr is move/copy assignable to const_ptr.
    const chunk_cptr copy{ p1 };

    // chunk_[c]ptrs

    // mutable (private)
    chunk_ptrs p3 = std::vector<chunk_ptr>({ p1 });
    p3.front()->push_back({ byte });        // modify container
    p3.front()->front() = byte;             // modify element

    // immutable (public)
    chunk_cptrs p4 = std::vector<chunk_cptr>({ p1, p2 });
    ////p4.front()->push_back({ byte });    // const container
    ////p4.front()->front() = byte;         // const element

    // A vector of non-const is NOT assignable to a vector of const.

    // chunk_[c]ptrs_[c]ptr

    // fully mutable
    chunk_ptrs_ptr p5 = std::make_shared<chunk_ptrs>(p3);
    p5->front()->push_back({ byte });       // modify container
    p5->front()->front() = byte;            // modify element
    p5->front().reset();                    // modify pointer to container

    // mutable container/elements
    chunk_ptrs_cptr p6 = std::make_shared<const chunk_ptrs>(p3);
    p6->front()->push_back({ byte });       // modify container
    p6->front()->front() = byte;            // modify element
    ////p6->front().reset();                // const pointer to container

    // mutable container pointer
    chunk_cptrs_ptr p7 = std::make_shared<chunk_cptrs>(p4);
    ////p7->front()->push_back({ byte });   // const container
    ////p7->front()->front() = byte;        // const element
    p7->front().reset();                    // modify pointer to container

    // fully immutable (public)
    chunk_cptrs_cptr p8 = std::make_shared<const chunk_cptrs>(p4);
    ////p8->front()->push_back({ byte });   // const container
    ////p8->front()->front() = byte;        // const element
    ////p8->front().reset();                // const pointer to container

    // use const& to avoid copy.
    // ptr is move/copy assignable only to const_ptr.
    const chunk_ptrs_cptr copy1 = p5;
    const chunk_cptrs_cptr copy2 = p7;

    // The fully immutable chunk_cptrs_cptr can ony be publicly exposed
    // by pointer copy (or const&) from chunk_cptrs_ptr.
    const chunk_cptrs_cptr& reference3 = p7;

    // Use reference to suppress warning.
    BOOST_REQUIRE(reference3);

    // There is no way to expose a container of pointers to mutable data as
    // pointers to immutable data without at least a vector copy. Cast copies
    // contents, though a move overload could be added as necessary. So usage:
    // [chunk_cptrs_cptr] non-mutable internal storage.
    // [chunk_ptrs_ptr] internal mutable container not exposed publicly.
}

// to_chunk (byte)

BOOST_AUTO_TEST_CASE(data_chunk__to_chunk1__value__expected_size_and_value)
{
    constexpr uint8_t expected = 42;
    const auto result = to_chunk(expected);
    BOOST_REQUIRE_EQUAL(result.size(), 1u);
    BOOST_REQUIRE_EQUAL(result[0], expected);
}

// to_chunk (data_slice)

BOOST_AUTO_TEST_CASE(data_chunk__to_chunk2__to_string__inverse)
{
    data_chunk result{ 24, 0, 15 };
    BOOST_REQUIRE_EQUAL(to_chunk(to_string(result)), result);
}

BOOST_AUTO_TEST_CASE(data_chunk__to_chunk2__array__expected)
{
    constexpr uint8_t l = 42;
    constexpr uint8_t u = 24;
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

BOOST_AUTO_TEST_CASE(data_chunk__to_chunk2__vector__expected)
{
    constexpr uint8_t l = 42;
    constexpr uint8_t u = 24;
    const data_chunk source
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

BOOST_AUTO_TEST_CASE(data_chunk__to_chunk2__string__expected)
{
    const std::string value{ "abc" };
    const data_chunk expected{ 'a', 'b', 'c' };
    BOOST_REQUIRE_EQUAL(to_chunk(value), expected);
}

BOOST_AUTO_TEST_CASE(data_chunk__to_chunk2__literal__expected)
{
    const data_chunk expected{ 'a', 'b', 'c' };
    BOOST_REQUIRE_EQUAL(to_chunk("abc"), expected);
}

// build_chunk()

BOOST_AUTO_TEST_CASE(data_chunk__build_chunk__empty__empty)
{
    const auto result = build_chunk({});
    BOOST_REQUIRE(result.empty());
}

BOOST_AUTO_TEST_CASE(data_chunk__build_chunk__three_vectors__expected_size_and_value)
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

BOOST_AUTO_TEST_CASE(data_chunk__build_chunk__three_arrays__expected_size_and_value)
{
    const auto result = build_chunk(
    {
        data_array<2>{ { 0, 2 } },
        data_array<1>{ { 4 } },
        data_array<3>{ { 6, 8, 10 } }
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

BOOST_AUTO_TEST_CASE(data_chunk__build_chunk__extra_reserve__expected_size_and_capacity)
{
    const auto result = build_chunk(
    {
        data_array<2>{ { 1, 1 } },
        data_array<1>{ { 42 } },
        data_array<3>{ { 1, 1, 1 } }
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

// splice

BOOST_AUTO_TEST_CASE(data_chunk__splice__empty__empty)
{
    const auto result = splice(data_chunk{}, {});
    BOOST_REQUIRE(result.empty());
}

BOOST_AUTO_TEST_CASE(data_chunk__splice__not_empty__expected_size_and_capacity)
{
    const auto result = splice(data_chunk{ 1, 2 }, { 3, 4, 5 });
    BOOST_REQUIRE_EQUAL(result[0], 1u);
    BOOST_REQUIRE_EQUAL(result[1], 2u);
    BOOST_REQUIRE_EQUAL(result[2], 3u);
    BOOST_REQUIRE_EQUAL(result[3], 4u);
    BOOST_REQUIRE_EQUAL(result[4], 5u);
    BOOST_REQUIRE_EQUAL(result.capacity(), 2u + 3u);
    BOOST_REQUIRE_EQUAL(result.size(), 2u + 3u);
}

BOOST_AUTO_TEST_SUITE_END()
