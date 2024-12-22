/**
 * Copyright (c) 2011-2024 libbitcoin developers (see AUTHORS)
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
#include "utreexo.hpp"

using namespace utreexo;

BOOST_AUTO_TEST_SUITE(utreexo_tests)

// inferred from rustreexo implementation (no test provided)

BOOST_AUTO_TEST_CASE(utreexo__parent__various__expected)
{
    static_assert(parent(0, 0) == 1);
    static_assert(parent(0, 1) == 2);
    static_assert(parent(0, 2) == 4);
    static_assert(parent(0, 3) == 8);
    static_assert(parent(0, 4) == 16);
    static_assert(parent(0, 5) == 32);
    static_assert(parent(0, 6) == 64);

    static_assert(parent(1, 0) == 1);
    static_assert(parent(1, 1) == 2);
    static_assert(parent(1, 2) == 4);
    static_assert(parent(1, 3) == 8);
    static_assert(parent(1, 4) == 16);
    static_assert(parent(1, 5) == 32);
    static_assert(parent(1, 6) == 64);

    static_assert(parent(128, 0) == 64 + 1);
    static_assert(parent(128, 1) == 64 + 2);
    static_assert(parent(128, 2) == 64 + 4);
    static_assert(parent(128, 3) == 64 + 8);
    static_assert(parent(128, 4) == 64 + 16);
    static_assert(parent(128, 5) == 64 + 32);
    static_assert(parent(128, 6) == 64);

    BOOST_REQUIRE_EQUAL(parent(128, 0), 65_u64);
    BOOST_REQUIRE_EQUAL(parent(128, 1), 66_u64);
    BOOST_REQUIRE_EQUAL(parent(128, 2), 68_u64);
    BOOST_REQUIRE_EQUAL(parent(128, 3), 72_u64);
    BOOST_REQUIRE_EQUAL(parent(128, 4), 80_u64);
    BOOST_REQUIRE_EQUAL(parent(128, 5), 96_u64);
    BOOST_REQUIRE_EQUAL(parent(128, 6), 64_u64);
}

BOOST_AUTO_TEST_CASE(utreexo__left_child__various__expected)
{
    // same as children()
    static_assert(left_child(4, 2) == 0);
    static_assert(left_child(49, 5) == 34);
    static_assert(left_child(50, 5) == 36);
    static_assert(left_child(44, 5) == 24);
    BOOST_REQUIRE_EQUAL(left_child(4, 2), 0_u64);
    BOOST_REQUIRE_EQUAL(left_child(49, 5), 34_u64);
    BOOST_REQUIRE_EQUAL(left_child(50, 5), 36_u64);
    BOOST_REQUIRE_EQUAL(left_child(44, 5), 24_u64);
}

BOOST_AUTO_TEST_CASE(utreexo__right_child__various__expected)
{
    // same as add1(children())
    static_assert(right_child(4, 2) == 0 + 1);
    static_assert(right_child(49, 5) == 34 + 1);
    static_assert(right_child(50, 5) == 36 + 1);
    static_assert(right_child(44, 5) == 24 + 1);
    BOOST_REQUIRE_EQUAL(right_child(4, 2), 1_u64);
    BOOST_REQUIRE_EQUAL(right_child(49, 5), 35_u64);
    BOOST_REQUIRE_EQUAL(right_child(50, 5), 37_u64);
    BOOST_REQUIRE_EQUAL(right_child(44, 5), 25_u64);
}

BOOST_AUTO_TEST_CASE(utreexo__is_root_populated__various__expected)
{
    static_assert(!is_root_populated(0b00000000, 0));
    static_assert( is_root_populated(0b00000001, 0));
    static_assert(!is_root_populated(0b00000000, 1));
    static_assert( is_root_populated(0b00000010, 1));
    static_assert(!is_root_populated(0b00000000, 2));
    static_assert( is_root_populated(0b00000100, 2));
    BOOST_REQUIRE(!is_root_populated(0, 0));
    BOOST_REQUIRE( is_root_populated(1, 0));
    BOOST_REQUIRE(!is_root_populated(0, 1));
    BOOST_REQUIRE( is_root_populated(2, 1));
    BOOST_REQUIRE(!is_root_populated(0, 2));
    BOOST_REQUIRE( is_root_populated(4, 2));
}

BOOST_AUTO_TEST_CASE(utreexo__is_left_niece__various__expected)
{
    static_assert( is_left_niece(0b00000000));
    static_assert(!is_left_niece(0b00000001));
    static_assert( is_left_niece(0b00000010));
    BOOST_REQUIRE( is_left_niece(0));
    BOOST_REQUIRE(!is_left_niece(1));
    BOOST_REQUIRE( is_left_niece(2));
}

BOOST_AUTO_TEST_CASE(utreexo__left_sibling__various__expected)
{
    static_assert(left_sibling(0b00000000) == 0b00000000);
    static_assert(left_sibling(0b00000001) == 0b00000000);
    static_assert(left_sibling(0b10101010) == 0b10101010);
    static_assert(left_sibling(0b10101011) == 0b10101010);
    BOOST_REQUIRE_EQUAL(left_sibling(0b00000000), 0b00000000);
    BOOST_REQUIRE_EQUAL(left_sibling(0b00000001), 0b00000000);
    BOOST_REQUIRE_EQUAL(left_sibling(0b10101010), 0b10101010);
    BOOST_REQUIRE_EQUAL(left_sibling(0b10101011), 0b10101010);
}

BOOST_AUTO_TEST_CASE(utreexo__start_position_at_row__various__expected)
{
    // forest_rows must be >= row.
    static_assert(start_position_at_row(0, 0) == 0);
    static_assert(start_position_at_row(0, 1) == 0);
    static_assert(start_position_at_row(1, 1) == 2);
    static_assert(start_position_at_row(1, 2) == 4);
    static_assert(start_position_at_row(1, 3) == 8);
    static_assert(start_position_at_row(2, 2) == 6);
    static_assert(start_position_at_row(2, 3) == 12);
    static_assert(start_position_at_row(3, 3) == 14);
    BOOST_REQUIRE_EQUAL(start_position_at_row(0, 0), 0_u64);
    BOOST_REQUIRE_EQUAL(start_position_at_row(0, 1), 0_u64);
    BOOST_REQUIRE_EQUAL(start_position_at_row(1, 1), 2_u64);
    BOOST_REQUIRE_EQUAL(start_position_at_row(1, 2), 4_u64);
    BOOST_REQUIRE_EQUAL(start_position_at_row(1, 3), 8_u64);
    BOOST_REQUIRE_EQUAL(start_position_at_row(2, 2), 6_u64);
    BOOST_REQUIRE_EQUAL(start_position_at_row(2, 3), 12_u64);
    BOOST_REQUIRE_EQUAL(start_position_at_row(3, 3), 14_u64);
}

BOOST_AUTO_TEST_CASE(utreexo__number_of_roots__various__expected)
{
    static_assert(number_of_roots(0) == 0);
    static_assert(number_of_roots(1) == 1);
    static_assert(number_of_roots(2) == 1);
    static_assert(number_of_roots(3) == 2);
    static_assert(number_of_roots(0xfefefefefefefefe) == (64 - 8));
    BOOST_REQUIRE_EQUAL(number_of_roots(0), 0_size);
    BOOST_REQUIRE_EQUAL(number_of_roots(1), 1_size);
    BOOST_REQUIRE_EQUAL(number_of_roots(2), 1_size);
    BOOST_REQUIRE_EQUAL(number_of_roots(3), 2_size);
    BOOST_REQUIRE_EQUAL(number_of_roots(0xfefefefefefefefe), 56_size);
}

// based on rustreexo tests

// github.com/mit-dci/rustreexo/blob/main/src/accumulator/node_hash.rs#L327
BOOST_AUTO_TEST_CASE(utreexo__parent_hash__zero_one__expected)
{
    constexpr auto expected = base16_array("02242b37d8e851f1e86f46790298c7097df06893d6226b7c1453c213e91717de");
    static_assert(parent_hash(hash_from_u8(0), hash_from_u8(1)) == expected);
    BOOST_REQUIRE_EQUAL(parent_hash(hash_from_u8(0), hash_from_u8(1)), expected);
}

// github.com/mit-dci/rustreexo/blob/main/src/accumulator/util.rs#L368
BOOST_AUTO_TEST_CASE(utreexo__is_right_sibling__zero_one__true)
{
    static_assert(is_right_sibling(0, 1));
    BOOST_REQUIRE(is_right_sibling(0, 1));
}

// github.com/mit-dci/rustreexo/blob/main/src/accumulator/util.rs#L351
BOOST_AUTO_TEST_CASE(utreexo__is_sibling__various__expected)
{
    static_assert( is_sibling(0, 1));
    static_assert( is_sibling(1, 0));
    static_assert(!is_sibling(1, 2));
    static_assert(!is_sibling(2, 1));
    BOOST_REQUIRE( is_sibling(0, 1));
    BOOST_REQUIRE( is_sibling(1, 0));
    BOOST_REQUIRE(!is_sibling(1, 2));
    BOOST_REQUIRE(!is_sibling(2, 1));
}

// github.com/mit-dci/rustreexo/blob/main/src/accumulator/util.rs#L474
BOOST_AUTO_TEST_CASE(utreexo__children__various__expected)
{
    static_assert(children(4, 2) == 0);
    static_assert(children(49, 5) == 34);
    static_assert(children(50, 5) == 36);
    static_assert(children(44, 5) == 24);
    BOOST_REQUIRE_EQUAL(children(4, 2), 0_u64);
    BOOST_REQUIRE_EQUAL(children(49, 5), 34_u64);
    BOOST_REQUIRE_EQUAL(children(50, 5), 36_u64);
    BOOST_REQUIRE_EQUAL(children(44, 5), 24_u64);
}

// github.com/mit-dci/rustreexo/blob/main/src/accumulator/util.rs#L468
BOOST_AUTO_TEST_CASE(utreexo__is_root_position__various__expected)
{
    static_assert(is_root_position(14, 8, 3));
    BOOST_REQUIRE(is_root_position(14, 8, 3));
}

// github.com/mit-dci/rustreexo/blob/main/src/accumulator/util.rs#L424
BOOST_AUTO_TEST_CASE(utreexo__tree_rows__various__expected)
{
    static_assert(tree_rows(8) == 3);
    static_assert(tree_rows(9) == 4);
    static_assert(tree_rows(12) == 4);
    static_assert(tree_rows(255) == 8);
    BOOST_REQUIRE_EQUAL(tree_rows(8), 3_u8);
    BOOST_REQUIRE_EQUAL(tree_rows(9), 4_u8);
    BOOST_REQUIRE_EQUAL(tree_rows(12), 4_u8);
    BOOST_REQUIRE_EQUAL(tree_rows(255), 8_u8);
}

// github.com/mit-dci/rustreexo/blob/main/src/accumulator/util.rs#L439
BOOST_AUTO_TEST_CASE(utreexo__detect_row__scenario__expected)
{
    constexpr auto test_detect_row_ = []() NOEXCEPT
    {
        auto success{ true };

        // This is start_position_at_row.
        constexpr auto row_offset = [](auto row, auto forest_rows) NOEXCEPT
        {
            return subtract(
                bit_right<uint64_t>(add1<size_t>(forest_rows)),
                bit_right<uint64_t>(add1<size_t>(subtract(forest_rows, row))));
        };

        // NONSTANDARD TEST (non-declarative).
        for (uint8_t forest_rows = 1; forest_rows < 64; ++forest_rows)
        {
            const auto top = sub1(sub1(shift_left<uint64_t>(2, forest_rows)));
            success &= (detect_row(top, forest_rows) == forest_rows);

            for (uint8_t row = 0; row < forest_rows; ++row)
            {
                const auto offset = row_offset(row, forest_rows);
                success &= (detect_row(offset, forest_rows) == row);
            }
        }

        return success;
    };


    // constexpr evaluation hit maximum step limit.
    ////static_assert(test_detect_row_());
    BOOST_REQUIRE(test_detect_row_());
}

// github.com/mit-dci/rustreexo/blob/main/src/accumulator/util.rs#L359
BOOST_AUTO_TEST_CASE(utreexo__root_position__various__expected)
{
    static_assert(root_position(5, 2, 3) == 12);
    static_assert(root_position(5, 0, 3) == 4);
    BOOST_REQUIRE_EQUAL(root_position(5, 2, 3), 12_u64);
    BOOST_REQUIRE_EQUAL(root_position(5, 0, 3), 4_u64);
}

// github.com/mit-dci/rustreexo/blob/main/src/accumulator/util.rs#L391
BOOST_AUTO_TEST_CASE(utreexo__remove_bit__various__expected)
{
    static_assert(remove_bit(15, 2) == 7);
    static_assert(remove_bit(14, 1) == 6);
    static_assert(remove_bit(10, 0) == 5);
    BOOST_REQUIRE_EQUAL(remove_bit(15, 2), 7_u64);
    BOOST_REQUIRE_EQUAL(remove_bit(14, 1), 6_u64);
    BOOST_REQUIRE_EQUAL(remove_bit(10, 0), 5_u64);
}

// github.com/mit-dci/rustreexo/blob/main/src/accumulator/util.rs#L482
BOOST_AUTO_TEST_CASE(utreexo__calculate_next__various__expected)
{
    constexpr auto calculate_next_ =
        [](uint64_t a, uint64_t b, uint8_t c, uint64_t expected) NOEXCEPT
        {
            uint64_t next{};
            return calculate_next(next, a, b, c) && (next == expected);
        };

    static_assert(calculate_next_(0, 1, 3, 8));
    static_assert(calculate_next_(1, 9, 3, 9));

    uint64_t next{};
    BOOST_REQUIRE(calculate_next(next, 0, 1, 3) && next == 8);
    BOOST_REQUIRE(calculate_next(next, 1, 9, 3) && next == 9);
}

// github.com/mit-dci/rustreexo/blob/main/src/accumulator/util.rs#L406
BOOST_AUTO_TEST_CASE(utreexo__detwin__various__expected)
{
    // 14
    // |---------------\
    // 12              13
    // |-------\       |-------\
    // 8       9       10      11
    // |---\   |---\   |---\   |---\
    // 0   1   2   3   4   5   6   7

    const positions expected1{ 7, 8, 10 };
    positions targets1{ 0, 1, 4, 5, 7 };
    detwin(targets1, 3);
    BOOST_REQUIRE_EQUAL(targets1, expected1);

    const positions expected2{ 4, 6, 12 };
    positions targets2{ 4, 6, 8, 9 };
    detwin(targets2, 3);
    BOOST_REQUIRE_EQUAL(targets2, expected2);
}

// rustreexo examples
// -------------------------------------------------------------------------------------------

// github.com/mit-dci/rustreexo/blob/main/src/accumulator/node_hash.rs#L260
BOOST_AUTO_TEST_CASE(utreexo__node_hash__rustreexo_example__expected)
{
    constexpr auto left = base16_array("0000000000000000000000000000000000000000000000000000000000000000");
    constexpr auto right = base16_array("0101010101010101010101010101010101010101010101010101010101010101");
    constexpr auto expected = base16_array("34e33ca0c40b7bd33d28932ca9e35170def7309a3bf91ecda5e1ceb067548a12");
    static_assert(parent_hash(left, right) == expected);
    BOOST_REQUIRE_EQUAL(parent_hash(left, right), expected);
}

// github.com/mit-dci/rustreexo/blob/main/src/accumulator/node_hash.rs#L346
BOOST_AUTO_TEST_CASE(utreexo__empty_hash__rustreexo_example__expected)
{
    constexpr auto expected = base16_array("0000000000000000000000000000000000000000000000000000000000000000");
    static_assert(empty_hash == expected);
    BOOST_REQUIRE_EQUAL(empty_hash, expected);
}

// github.com/mit-dci/rustreexo/blob/main/src/accumulator/node_hash.rs#L338
BOOST_AUTO_TEST_CASE(utreexo__hash_from_u8__rustreexo_example__expected)
{
    constexpr auto expected = base16_array("6e340b9cffb37a989ca544e6bb780a2c78901d3fb33738768511a30617afa01d");
    static_assert(hash_from_u8(0) == expected);
    BOOST_REQUIRE_EQUAL(hash_from_u8(0), expected);
}

BOOST_AUTO_TEST_SUITE_END()
