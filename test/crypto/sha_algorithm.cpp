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

BOOST_AUTO_TEST_SUITE(sha_algorithm_tests)

constexpr auto half = base16_array("0000000000000000000000000000000000000000000000000000000000000000");
constexpr auto full = base16_array("00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
constexpr auto twin = base16_array("0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");

BOOST_AUTO_TEST_CASE(sha_algorithm__constexpr_half_block__null_hash__expected)
{
    constexpr auto expected = base16_array("66687aadf862bd776c8fc18b8e9f8e20089714856ee233b3902a591d0d5f2925");
    static_assert(sha::hash(half) == expected);
    BOOST_REQUIRE_EQUAL(sha::hash(half), expected);
    BOOST_REQUIRE_EQUAL(sha256_hash(half), expected);
}

BOOST_AUTO_TEST_CASE(sha_algorithm__constexpr_full_block__null_hash__expected)
{
    constexpr auto expected = base16_array("f5a5fd42d16a20302798ef6ed309979b43003d2320d9f0e8ea9831a92759fb4b");
    static_assert(sha::hash(full) == expected);
    BOOST_REQUIRE_EQUAL(sha::hash(full), expected);
    BOOST_REQUIRE_EQUAL(sha256_hash(full), expected);
}

BOOST_AUTO_TEST_CASE(sha_algorithm__const_twin_block__null_hash__unexpected)
{
    constexpr auto expected = base16_array("38723a2e5e8a17aa7950dc008209944e898f69a7bd10a23c839d341e935fd5ca");
    const std_vector<std::reference_wrapper<const sha::block>> twin_refs{ std::cref(full), std::cref(full) };

    // sha::hash(twin_refs): f6b8e1b204d06f15cfda035de7e224d9bdfda16cbd17966c92c22b8268e66862
    BOOST_CHECK_NE(sha::hash(twin_refs), expected);
    BOOST_REQUIRE_EQUAL(sha256_hash(twin), expected);
}

// merkle(full)
// merkle(twin)

BOOST_AUTO_TEST_SUITE_END()
