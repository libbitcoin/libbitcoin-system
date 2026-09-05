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

BOOST_AUTO_TEST_SUITE(maybe_random_tests)

BOOST_AUTO_TEST_CASE(maybe_random__fill__empty__unchanged)
{
    data_chunk out{};
    maybe_random::fill(out);
    BOOST_REQUIRE(out.empty());
}

BOOST_AUTO_TEST_CASE(maybe_random__fill__partial_and_whole_blocks__filled)
{
    data_array<1> one{};
    data_array<33> partial{};
    data_array<64> whole{};
    data_chunk chunk(100, 0x00);
    maybe_random::fill(one);
    maybe_random::fill(partial);
    maybe_random::fill(whole);
    maybe_random::fill(chunk);

    BOOST_REQUIRE_NE(to_chunk(partial), data_chunk(33, 0x00));
    BOOST_REQUIRE_NE(to_chunk(whole), data_chunk(64, 0x00));
    BOOST_REQUIRE_NE(chunk, data_chunk(100, 0x00));

    // The blocks of a fill are independently drawn.
    const auto first = slice<0, 32>(whole);
    const auto second = slice<32, 64>(whole);
    BOOST_REQUIRE_NE(first, second);
}

BOOST_AUTO_TEST_CASE(maybe_random__fill__twice__distinct)
{
    data_array<32> first{};
    data_array<32> second{};
    maybe_random::fill(first);
    maybe_random::fill(second);
    BOOST_REQUIRE_NE(first, second);
}

BOOST_AUTO_TEST_CASE(maybe_random__next__unsigned_range__within)
{
    for (size_t count{}; count < 100; ++count)
    {
        const auto value = maybe_random::next<size_t>(10, 20);
        BOOST_REQUIRE_GE(value, 10u);
        BOOST_REQUIRE_LE(value, 20u);
    }
}

BOOST_AUTO_TEST_CASE(maybe_random__next__signed_range__within)
{
    for (size_t count{}; count < 100; ++count)
    {
        const auto value = maybe_random::next<int32_t>(-5, 5);
        BOOST_REQUIRE_GE(value, -5);
        BOOST_REQUIRE_LE(value, 5);
    }
}

BOOST_AUTO_TEST_CASE(maybe_random__next__inverted_range__zero)
{
    BOOST_REQUIRE_EQUAL(maybe_random::next<uint32_t>(5, 4), 0u);
}

BOOST_AUTO_TEST_CASE(maybe_random__next__full_domain_twice__distinct)
{
    const auto first = maybe_random::next<uint64_t>(0, max_uint64);
    const auto second = maybe_random::next<uint64_t>(0, max_uint64);
    BOOST_REQUIRE_NE(first, second);
}

BOOST_AUTO_TEST_CASE(maybe_random__shuffle__vector__same_elements)
{
    const data_chunk values{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    auto shuffled = values;
    maybe_random::shuffle(shuffled);
    BOOST_REQUIRE_EQUAL(shuffled.size(), values.size());

    std::sort(shuffled.begin(), shuffled.end());
    BOOST_REQUIRE_EQUAL(shuffled, values);
}

BOOST_AUTO_TEST_SUITE_END()
