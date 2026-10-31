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
#include "../../test.hpp"
#include "electrum_v1.hpp"

BC_AUTO_TEST_SUITE(catalogs_electrum_v1_tests)

using namespace test::catalogs_electrum_v1;
using namespace bc::system::words;

constexpr auto dictionary_count = 2u;
constexpr auto dictionary_size = 1626;
constexpr auto intersection_en_pt = 5;
constexpr auto intersection_en_electrum_en = 861;
constexpr auto intersection_en_electrum_es = 7;
constexpr auto intersection_en_electrum_fr = 50;
constexpr auto intersection_en_electrum_cs = 2;
constexpr auto intersection_en_electrum_pt = 1;
constexpr auto intersection_pt_electrum_en = 14;
constexpr auto intersection_pt_electrum_es = 69;
constexpr auto intersection_pt_electrum_it = 42;
constexpr auto intersection_pt_electrum_fr = 7;
constexpr auto intersection_pt_electrum_cs = 2;
constexpr auto intersection_pt_electrum_pt = 185;

// These are not "reversed" as with typical bitcoin hash presentations.
constexpr auto identity_en = base16_array("430ce8fc15a42a6097782a10144d1277e2596b7591be338254a257dfcf1f57d6");
constexpr auto identity_pt = base16_array("43da9ad5a079483e224bcdd61cd1728a68c2b2647f0f1b87832dbaab5e159a12");

// count

BC_AUTO_TEST_CASE(catalogs_electrum_v1__count__all__expected)
{
    // Any new dictionary must be added below to guarantee lack of normalization.
    // Failure to do so may lead to invalid seed generation, which is very bad.
    BC_CHECK_MESSAGE(electrum_v1::catalogs::count() == dictionary_count, "new dictionary");
}

// identity

BC_AUTO_TEST_CASE(catalogs_electrum_v1__all__identity__expected)
{
    // These sha256 checks fail under vectorization.
    BC_CHECK_EQUAL(identity(electrum_v1::en), identity_en);
    BC_CHECK_EQUAL(identity(electrum_v1::pt), identity_pt);
}

// distinct

BC_AUTO_TEST_CASE(catalogs_electrum_v1__all__distinct__true)
{
    BC_CHECK(distinct(electrum_v1::en));
    BC_CHECK(distinct(electrum_v1::pt));
}

// sorted

BC_AUTO_TEST_CASE(catalogs_electrum_v1__sorted__unsorted__false)
{
    // Compare is not locale-aware, but these are consistent across encodings.
    BC_CHECK(!sorted8(electrum_v1::pt) && !electrum_v1::pt.sorted);
    BC_CHECK(!sorted8(electrum_v1::en) && !electrum_v1::en.sorted);
    BC_CHECK(!sorted16(electrum_v1::pt) && !electrum_v1::pt.sorted);
    BC_CHECK(!sorted16(electrum_v1::en) && !electrum_v1::en.sorted);
    BC_CHECK(!sorted32(electrum_v1::pt) && !electrum_v1::pt.sorted);
    BC_CHECK(!sorted32(electrum_v1::en) && !electrum_v1::en.sorted);
}

// abnormal

// These dictionaries are in normal form.
// So there is no need to nfkd normalize these for wordlist-based seedings.
BC_AUTO_TEST_CASE(catalogs_electrum_v1__normal__normal_words__true)
{
    BC_CHECK(!abnormal(electrum_v1::en));
    BC_CHECK(!abnormal(electrum_v1::pt));
}

// intersection

// INTERSECTION/DEVIATION IS ALWAYS ZERO IN MSVC DEBUG BUILD TEST RUNS. ???

BC_AUTO_TEST_CASE(catalogs_electrum_v1__intersections__en__expected)
{
    BC_CHECK_EQUAL(intersection(electrum_v1::en, electrum_v1::en), dictionary_size);
    BC_CHECK_EQUAL(intersection(electrum_v1::en, electrum_v1::pt), intersection_en_pt);
}

BC_AUTO_TEST_CASE(catalogs_electrum_v1__intersections__pt__expected)
{
    ////BC_CHECK_EQUAL(intersection(electrum_v1::pt, electrum_v1::en), intersection_en_pt);
    BC_CHECK_EQUAL(intersection(electrum_v1::pt, electrum_v1::pt), dictionary_size);
}

BC_AUTO_TEST_CASE(catalogs_electrum_v1__intersections__en_electrum__expected)
{
    BC_CHECK_EQUAL(intersection(electrum_v1::en, electrum::en), intersection_en_electrum_en);
    BC_CHECK_EQUAL(intersection(electrum_v1::en, electrum::es), intersection_en_electrum_es);
    BC_CHECK(!intersects(electrum_v1::en, electrum::it));
    BC_CHECK_EQUAL(intersection(electrum_v1::en, electrum::fr), intersection_en_electrum_fr);
    BC_CHECK_EQUAL(intersection(electrum_v1::en, electrum::cs), intersection_en_electrum_cs);
    BC_CHECK_EQUAL(intersection(electrum_v1::en, electrum::pt), intersection_en_electrum_pt);
    BC_CHECK(!intersects(electrum_v1::en, electrum::ja));
    BC_CHECK(!intersects(electrum_v1::en, electrum::ko));
    BC_CHECK(!intersects(electrum_v1::en, electrum::zh_Hans));
    BC_CHECK(!intersects(electrum_v1::en, electrum::zh_Hant));
}

BC_AUTO_TEST_CASE(catalogs_electrum_v1__intersections__pt_electrum__expected)
{
    BC_CHECK_EQUAL(intersection(electrum_v1::pt, electrum::en), intersection_pt_electrum_en);
    BC_CHECK_EQUAL(intersection(electrum_v1::pt, electrum::es), intersection_pt_electrum_es);
    BC_CHECK_EQUAL(intersection(electrum_v1::pt, electrum::it), intersection_pt_electrum_it);
    BC_CHECK_EQUAL(intersection(electrum_v1::pt, electrum::fr), intersection_pt_electrum_fr);
    BC_CHECK_EQUAL(intersection(electrum_v1::pt, electrum::cs), intersection_pt_electrum_cs);
    BC_CHECK_EQUAL(intersection(electrum_v1::pt, electrum::pt), intersection_pt_electrum_pt);
    BC_CHECK(!intersects(electrum_v1::pt, electrum::ja));
    BC_CHECK(!intersects(electrum_v1::pt, electrum::ko));
    BC_CHECK(!intersects(electrum_v1::pt, electrum::zh_Hans));
    BC_CHECK(!intersects(electrum_v1::pt, electrum::zh_Hant));
}

BC_AUTO_TEST_SUITE_END()
