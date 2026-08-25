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

BOOST_AUTO_TEST_SUITE(muhash_tests)

constexpr auto muhash_element_0 = base16_array(
    "0000000000000000000000000000000000000000000000000000000000000000");
constexpr auto muhash_element_1 = base16_array(
    "0100000000000000000000000000000000000000000000000000000000000000");
constexpr auto muhash_element_2 = base16_array(
    "0200000000000000000000000000000000000000000000000000000000000000");

// bitcoind crypto_tests muhash_tests vector.
BOOST_AUTO_TEST_CASE(muhash__flush__bitcoind_vector__expected)
{
    const auto expected = base16_hash(
        "10d312b100cbd32ada024a6646e40d3482fcff103668d2625f10002a607d5863");

    muhash3072 set{};
    set.insert(muhash_element_0);
    set.insert(muhash_element_1);
    set.remove(muhash_element_2);
    BOOST_REQUIRE_EQUAL(set.flush(), expected);
}

BOOST_AUTO_TEST_CASE(muhash__flush__insert_remove__empty)
{
    muhash3072 empty{};
    muhash3072 set{};
    set.insert(muhash_element_1);
    set.remove(muhash_element_1);
    BOOST_REQUIRE_EQUAL(set.flush(), empty.flush());
}

BOOST_AUTO_TEST_CASE(muhash__flush__insertion_order__invariant)
{
    muhash3072 forward{};
    forward.insert(muhash_element_1);
    forward.insert(muhash_element_2);

    muhash3072 reverse{};
    reverse.insert(muhash_element_2);
    reverse.insert(muhash_element_1);
    BOOST_REQUIRE_EQUAL(forward.flush(), reverse.flush());
}

BOOST_AUTO_TEST_CASE(muhash__construct__element__inserted)
{
    muhash3072 constructed{ muhash_element_1 };

    muhash3072 set{};
    set.insert(muhash_element_1);
    BOOST_REQUIRE_EQUAL(constructed.flush(), set.flush());
}

BOOST_AUTO_TEST_CASE(muhash__flush__continued_mutation__valid)
{
    muhash3072 empty{};
    muhash3072 set{};
    set.insert(muhash_element_1);
    set.flush();
    set.remove(muhash_element_1);
    BOOST_REQUIRE_EQUAL(set.flush(), empty.flush());
}

BOOST_AUTO_TEST_SUITE_END()
