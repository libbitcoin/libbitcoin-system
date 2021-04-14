/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#include <boost/test/unit_test.hpp>
#include <algorithm>
#include <bitcoin/system.hpp>

using namespace bc::system;
using namespace bc::system::wallet;

// TODO: determine if any overlaps have distinct indexes.

// Disabled until rebuilt.
#ifdef DISABLED_TESTS

static ptrdiff_t intersection(const dictionary& left, const dictionary& right)
{
    return std::count_if(left.words.begin(), left.words.end(),
        [&](const char test[])
        {
            return std::find(right.words.begin(), right.words.end(), test) !=
                right.words.end();
        });
}

static bool intersects(const dictionary& left, const dictionary& right)
{
    return std::any_of(left.words.begin(), left.words.end(),
        [&](const char test[])
        {
            return std::find(right.words.begin(), right.words.end(), test) !=
                right.words.end();
        });
}

BOOST_AUTO_TEST_SUITE(dictionary_mnemonic_tests)

BOOST_AUTO_TEST_SUITE(dictionary_mnemonic_tests)

BOOST_AUTO_TEST_CASE(dictionary_electrum_v1__construct__en__expected_size)
{
    BOOST_REQUIRE(false);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(mnemonic__dictionary__en_es__no_intersection)
{
    BOOST_REQUIRE(!intersects(language::en, language::es));
}

BOOST_AUTO_TEST_CASE(mnemonic__dictionary__es_it__no_intersection)
{
    BOOST_REQUIRE(!intersects(language::es, language::it));
}

BOOST_AUTO_TEST_CASE(mnemonic__dictionary__it_fr__no_intersection)
{
    BOOST_REQUIRE(!intersects(language::it, language::fr));
}

BOOST_AUTO_TEST_CASE(mnemonic__dictionary__fr_cs__no_intersection)
{
    BOOST_REQUIRE(!intersects(language::fr, language::cs));
}

BOOST_AUTO_TEST_CASE(mnemonic__dictionary__cs_pt__no_intersection)
{
    BOOST_REQUIRE(!intersects(language::cs, language::pt));
}

BOOST_AUTO_TEST_CASE(mnemonic__dictionary__pt_ko__no_intersection)
{
    BOOST_REQUIRE(!intersects(language::pt, language::ko));
}

BOOST_AUTO_TEST_CASE(mnemonic__dictionary__ko_zh_Hans__no_intersection)
{
    BOOST_REQUIRE(!intersects(language::ko, language::zh_Hans));
}

BOOST_AUTO_TEST_CASE(mnemonic__dictionary__zh_Hans_zh_Hant__1275_intersections)
{
    BOOST_REQUIRE_EQUAL(intersection(language::zh_Hans, language::zh_Hant), 1275);
}

BOOST_AUTO_TEST_CASE(mnemonic__dictionary__zh_Hant_en__no_intersection)
{
    BOOST_REQUIRE(!intersects(language::zh_Hant, language::en));
}

BOOST_AUTO_TEST_SUITE_END()

#endif DISABLED_TESTS
