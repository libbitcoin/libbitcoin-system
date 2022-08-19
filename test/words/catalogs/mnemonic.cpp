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
#include "../../test.hpp"
#include "mnemonic.hpp"

BOOST_AUTO_TEST_SUITE(catalogs_mnemonic_tests)

using namespace test::catalogs_mnemonic;
using namespace bc::system::words;

constexpr auto dictionary_count = 10u;
constexpr auto dictionary_size = 2048;
constexpr auto deviations_en_fr = 100;
constexpr auto intersecton_en_fr = 100;
constexpr auto deviations_zh = 0;
constexpr auto intersection_zh = 1275;

// These are not "reversed" as with typical bitcoin hash presentations.
constexpr auto identity_en = base16_array("f18b9a84c83e38e98eceb0102b275e26438af83ab08f080cdb780a2caa9f3a6d");
constexpr auto identity_es = base16_array("27e99ad4328299108663c19eb611310bd3b77260af852169108713019831d07d");
constexpr auto identity_it = base16_array("d9664953fe4c49e0b40eb6f6378c421f5f01dc9360aaac6d2a3c294f046ef520");
constexpr auto identity_fr = base16_array("fd4cd57c8e86a99ac53cd0f21a6b89af366769b0143ac0d268ac7d7a39200145");
constexpr auto identity_cs = base16_array("92e035ca0e3680fab85fa18b9e5c7e7582b3bd9caeb6d4bc8a2ecc8e492100c9");
constexpr auto identity_pt = base16_array("85fea658f90c42c182823807a1d30db587abb72c14a3098e1a65f8b78d1ffcf4");
constexpr auto identity_ja = base16_array("e641781d89213031233ebc5130020c3de297cad96065ecc688d1c00e010ea71c");
constexpr auto identity_ko = base16_array("0c26059ed7ede977d7fa1c40443e71793e7850aa6a3d8aabf0cbcec91c1f95ec");
constexpr auto identity_zh_Hans = base16_array("a86e2bd870e228d19a887c9faae8c1374b4e624e85d2b245b59e36583cba4afc");
constexpr auto identity_zh_Hant = base16_array("11ef479f2f44b6d4f7fb6239dff06e3cd7a1473b83df6cf91adcbbbee598acf6");

// count

BOOST_AUTO_TEST_CASE(catalogs_mnemonic__count__all__expected)
{
    // Any new dictionary must be added below to guarantee lack of normalization.
    // Failure to do so may lead to invalid seed generation, which is very bad.
    BOOST_CHECK_MESSAGE(mnemonic::catalogs::count() == dictionary_count, "new dictionary");
}

// identity

BOOST_AUTO_TEST_CASE(catalogs_mnemonic__all__identity__expected)
{
    BOOST_CHECK_EQUAL(identity(mnemonic::en), identity_en);
    BOOST_CHECK_EQUAL(identity(mnemonic::es), identity_es);
    BOOST_CHECK_EQUAL(identity(mnemonic::it), identity_it);
    BOOST_CHECK_EQUAL(identity(mnemonic::fr), identity_fr);
    BOOST_CHECK_EQUAL(identity(mnemonic::cs), identity_cs);
    BOOST_CHECK_EQUAL(identity(mnemonic::pt), identity_pt);
    BOOST_CHECK_EQUAL(identity(mnemonic::ja), identity_ja);
    BOOST_CHECK_EQUAL(identity(mnemonic::ko), identity_ko);
    BOOST_CHECK_EQUAL(identity(mnemonic::zh_Hans), identity_zh_Hans);
    BOOST_CHECK_EQUAL(identity(mnemonic::zh_Hant), identity_zh_Hant);
}

// distinct

BOOST_AUTO_TEST_CASE(catalogs_mnemonic__all__distinct__true)
{
    BOOST_CHECK(distinct(mnemonic::en));
    BOOST_CHECK(distinct(mnemonic::es));
    BOOST_CHECK(distinct(mnemonic::it));
    BOOST_CHECK(distinct(mnemonic::fr));
    BOOST_CHECK(distinct(mnemonic::cs));
    BOOST_CHECK(distinct(mnemonic::pt));
    BOOST_CHECK(distinct(mnemonic::ja));
    BOOST_CHECK(distinct(mnemonic::ko));
    BOOST_CHECK(distinct(mnemonic::zh_Hans));
    BOOST_CHECK(distinct(mnemonic::zh_Hant));
}

// sorted

BOOST_AUTO_TEST_CASE(catalogs_mnemonic__sorted8__sorted__true)
{
    // Compare is not locale-aware, but these are consistent across encodings.
    BOOST_CHECK(sorted8(mnemonic::en) && mnemonic::en.sorted);
    BOOST_CHECK(sorted8(mnemonic::it) && mnemonic::it.sorted);
    BOOST_CHECK(sorted8(mnemonic::pt) && mnemonic::pt.sorted);
    BOOST_CHECK(sorted8(mnemonic::ko) && mnemonic::ko.sorted);
    BOOST_CHECK(sorted16(mnemonic::en) && mnemonic::en.sorted);
    BOOST_CHECK(sorted16(mnemonic::it) && mnemonic::it.sorted);
    BOOST_CHECK(sorted16(mnemonic::pt) && mnemonic::pt.sorted);
    BOOST_CHECK(sorted16(mnemonic::ko) && mnemonic::ko.sorted);
    BOOST_CHECK(sorted32(mnemonic::en) && mnemonic::en.sorted);
    BOOST_CHECK(sorted32(mnemonic::it) && mnemonic::it.sorted);
    BOOST_CHECK(sorted32(mnemonic::pt) && mnemonic::pt.sorted);
    BOOST_CHECK(sorted32(mnemonic::ko) && mnemonic::ko.sorted);
}

BOOST_AUTO_TEST_CASE(catalogs_mnemonic__sorted8__unsorted__false)
{
    // Compare is not locale-aware, but these are consistent across encodings.
    BOOST_CHECK(!sorted8(mnemonic::es) && !mnemonic::es.sorted);
    BOOST_CHECK(!sorted8(mnemonic::fr) && !mnemonic::fr.sorted);
    BOOST_CHECK(!sorted8(mnemonic::cs) && !mnemonic::cs.sorted);
    BOOST_CHECK(!sorted8(mnemonic::ja) && !mnemonic::ja.sorted);
    BOOST_CHECK(!sorted8(mnemonic::zh_Hans) && !mnemonic::zh_Hans.sorted);
    BOOST_CHECK(!sorted8(mnemonic::zh_Hant) && !mnemonic::zh_Hant.sorted);
    BOOST_CHECK(!sorted16(mnemonic::es) && !mnemonic::es.sorted);
    BOOST_CHECK(!sorted16(mnemonic::fr) && !mnemonic::fr.sorted);
    BOOST_CHECK(!sorted16(mnemonic::cs) && !mnemonic::cs.sorted);
    BOOST_CHECK(!sorted16(mnemonic::ja) && !mnemonic::ja.sorted);
    BOOST_CHECK(!sorted16(mnemonic::zh_Hans) && !mnemonic::zh_Hans.sorted);
    BOOST_CHECK(!sorted16(mnemonic::zh_Hant) && !mnemonic::zh_Hant.sorted);
    BOOST_CHECK(!sorted32(mnemonic::es) && !mnemonic::es.sorted);
    BOOST_CHECK(!sorted32(mnemonic::fr) && !mnemonic::fr.sorted);
    BOOST_CHECK(!sorted32(mnemonic::cs) && !mnemonic::cs.sorted);
    BOOST_CHECK(!sorted32(mnemonic::ja) && !mnemonic::ja.sorted);
    BOOST_CHECK(!sorted32(mnemonic::zh_Hans) && !mnemonic::zh_Hans.sorted);
    BOOST_CHECK(!sorted32(mnemonic::zh_Hant) && !mnemonic::zh_Hant.sorted);
}

// abnormal

// These dictionaries are in normal form.
// So there is no need to nfkd normalize these for wordlist-based seedings.
// This also removes the ICU dependency for these languages.
BOOST_AUTO_TEST_CASE(catalogs_mnemonic__normal__normal_words__true)
{
    // The result is definitive only when HAVE_ICU is defined.
    BOOST_CHECK(!abnormal(mnemonic::en));
    BOOST_CHECK(!abnormal(mnemonic::es));
    BOOST_CHECK(!abnormal(mnemonic::it));
    BOOST_CHECK(!abnormal(mnemonic::fr));
    BOOST_CHECK(!abnormal(mnemonic::cs));
    BOOST_CHECK(!abnormal(mnemonic::pt));
    BOOST_CHECK(!abnormal(mnemonic::ja));
    BOOST_CHECK(!abnormal(mnemonic::ko));
    BOOST_CHECK(!abnormal(mnemonic::zh_Hans));
    BOOST_CHECK(!abnormal(mnemonic::zh_Hant));
}

// deviation

// INTERSECTION/DEVIATION IS ALWAYS ZERO IN MSVC DEBUG BUILD TEST RUNS. ???

// All zh_Hans/zh_Hant collisions are interchangeable.
// None of the en/fr collisions are interchangeable.
// It is unsafe to deduce en/fr dictionary, but only if all words collide.
// Always check both dictionaries in both zh_Hans/zh_Hant and en/fr.
// If zh_Hans/zh_Hant collide, may pick either (but should be consistent).
// If en/fr collide, must fail as decode requires explicit language identifier.
BOOST_AUTO_TEST_CASE(catalogs_mnemonic__deviation__deviants__expected)
{
    BOOST_CHECK_EQUAL(deviation(mnemonic::en, mnemonic::fr), deviations_en_fr);
    BOOST_CHECK_EQUAL(deviation(mnemonic::zh_Hans, mnemonic::zh_Hant), deviations_zh);
}

// intersection

BOOST_AUTO_TEST_CASE(catalogs_mnemonic__intersections__en__expected)
{
    BOOST_CHECK_EQUAL(intersection(mnemonic::en, mnemonic::en), dictionary_size);
    BOOST_CHECK(!intersects(mnemonic::en, mnemonic::es));
    BOOST_CHECK(!intersects(mnemonic::en, mnemonic::it));
    BOOST_CHECK_EQUAL(intersection(mnemonic::en, mnemonic::fr), intersecton_en_fr);
    BOOST_CHECK(!intersects(mnemonic::en, mnemonic::cs));
    BOOST_CHECK(!intersects(mnemonic::en, mnemonic::pt));
    BOOST_CHECK(!intersects(mnemonic::en, mnemonic::ja));
    BOOST_CHECK(!intersects(mnemonic::en, mnemonic::ko));
    BOOST_CHECK(!intersects(mnemonic::en, mnemonic::zh_Hans));
    BOOST_CHECK(!intersects(mnemonic::en, mnemonic::zh_Hant));
}

BOOST_AUTO_TEST_CASE(catalogs_mnemonic__intersections__es__expected)
{
    ////BOOST_CHECK(!intersects(mnemonic::es, mnemonic::en));
    BOOST_CHECK_EQUAL(intersection(mnemonic::es, mnemonic::es), dictionary_size);
    BOOST_CHECK(!intersects(mnemonic::es, mnemonic::it));
    BOOST_CHECK(!intersects(mnemonic::es, mnemonic::fr));
    BOOST_CHECK(!intersects(mnemonic::es, mnemonic::cs));
    BOOST_CHECK(!intersects(mnemonic::es, mnemonic::pt));
    BOOST_CHECK(!intersects(mnemonic::es, mnemonic::ja));
    BOOST_CHECK(!intersects(mnemonic::es, mnemonic::ko));
    BOOST_CHECK(!intersects(mnemonic::es, mnemonic::zh_Hans));
    BOOST_CHECK(!intersects(mnemonic::es, mnemonic::zh_Hant));
}

BOOST_AUTO_TEST_CASE(catalogs_mnemonic__intersections__it__expected)
{
    ////BOOST_CHECK(!intersects(mnemonic::it, mnemonic::en));
    ////BOOST_CHECK(!intersects(mnemonic::it, mnemonic::es));
    BOOST_CHECK_EQUAL(intersection(mnemonic::it, mnemonic::it), dictionary_size);
    BOOST_CHECK(!intersects(mnemonic::it, mnemonic::fr));
    BOOST_CHECK(!intersects(mnemonic::it, mnemonic::cs));
    BOOST_CHECK(!intersects(mnemonic::it, mnemonic::pt));
    BOOST_CHECK(!intersects(mnemonic::it, mnemonic::ja));
    BOOST_CHECK(!intersects(mnemonic::it, mnemonic::ko));
    BOOST_CHECK(!intersects(mnemonic::it, mnemonic::zh_Hans));
    BOOST_CHECK(!intersects(mnemonic::it, mnemonic::zh_Hant));
}

BOOST_AUTO_TEST_CASE(catalogs_mnemonic__intersections__fr__expected)
{
    ////BOOST_CHECK_EQUAL(intersection(mnemonic::fr, mnemonic::en), intersecton_en_fr);
    ////BOOST_CHECK(!intersects(mnemonic::fr, mnemonic::es));
    ////BOOST_CHECK(!intersects(mnemonic::fr, mnemonic::it));
    BOOST_CHECK_EQUAL(intersection(mnemonic::fr, mnemonic::fr), dictionary_size);
    BOOST_CHECK(!intersects(mnemonic::fr, mnemonic::cs));
    BOOST_CHECK(!intersects(mnemonic::fr, mnemonic::pt));
    BOOST_CHECK(!intersects(mnemonic::fr, mnemonic::ja));
    BOOST_CHECK(!intersects(mnemonic::fr, mnemonic::ko));
    BOOST_CHECK(!intersects(mnemonic::fr, mnemonic::zh_Hans));
    BOOST_CHECK(!intersects(mnemonic::fr, mnemonic::zh_Hant));
}

BOOST_AUTO_TEST_CASE(catalogs_mnemonic__intersections__cs__expected)
{
    ////BOOST_CHECK(!intersects(mnemonic::cs, mnemonic::en));
    ////BOOST_CHECK(!intersects(mnemonic::cs, mnemonic::es));
    ////BOOST_CHECK(!intersects(mnemonic::cs, mnemonic::it));
    ////BOOST_CHECK(!intersects(mnemonic::cs, mnemonic::fr));
    BOOST_CHECK_EQUAL(intersection(mnemonic::cs, mnemonic::cs), dictionary_size);
    BOOST_CHECK(!intersects(mnemonic::cs, mnemonic::pt));
    BOOST_CHECK(!intersects(mnemonic::cs, mnemonic::ja));
    BOOST_CHECK(!intersects(mnemonic::cs, mnemonic::ko));
    BOOST_CHECK(!intersects(mnemonic::cs, mnemonic::zh_Hans));
    BOOST_CHECK(!intersects(mnemonic::cs, mnemonic::zh_Hant));
}

BOOST_AUTO_TEST_CASE(catalogs_mnemonic__intersections__pt__expected)
{
    ////BOOST_CHECK(!intersects(mnemonic::pt, mnemonic::en));
    ////BOOST_CHECK(!intersects(mnemonic::pt, mnemonic::es));
    ////BOOST_CHECK(!intersects(mnemonic::pt, mnemonic::it));
    ////BOOST_CHECK(!intersects(mnemonic::pt, mnemonic::fr));
    ////BOOST_CHECK(!intersects(mnemonic::pt, mnemonic::cs));
    BOOST_CHECK_EQUAL(intersection(mnemonic::pt, mnemonic::pt), dictionary_size);
    BOOST_CHECK(!intersects(mnemonic::pt, mnemonic::ja));
    BOOST_CHECK(!intersects(mnemonic::pt, mnemonic::ko));
    BOOST_CHECK(!intersects(mnemonic::pt, mnemonic::zh_Hans));
    BOOST_CHECK(!intersects(mnemonic::pt, mnemonic::zh_Hant));
}

BOOST_AUTO_TEST_CASE(catalogs_mnemonic__intersections__ja__expected)
{
    ////BOOST_CHECK(!intersects(mnemonic::ja, mnemonic::en));
    ////BOOST_CHECK(!intersects(mnemonic::ja, mnemonic::es));
    ////BOOST_CHECK(!intersects(mnemonic::ja, mnemonic::it));
    ////BOOST_CHECK(!intersects(mnemonic::ja, mnemonic::fr));
    ////BOOST_CHECK(!intersects(mnemonic::ja, mnemonic::cs));
    ////BOOST_CHECK(!intersects(mnemonic::ja, mnemonic::pt));
    BOOST_CHECK_EQUAL(intersection(mnemonic::ja, mnemonic::ja), dictionary_size);
    BOOST_CHECK(!intersects(mnemonic::ja, mnemonic::ko));
    BOOST_CHECK(!intersects(mnemonic::ja, mnemonic::zh_Hans));
    BOOST_CHECK(!intersects(mnemonic::ja, mnemonic::zh_Hant));
}

BOOST_AUTO_TEST_CASE(catalogs_mnemonic__intersections__ko__expected)
{
    ////BOOST_CHECK(!intersects(mnemonic::ko, mnemonic::en));
    ////BOOST_CHECK(!intersects(mnemonic::ko, mnemonic::es));
    ////BOOST_CHECK(!intersects(mnemonic::ko, mnemonic::it));
    ////BOOST_CHECK(!intersects(mnemonic::ko, mnemonic::fr));
    ////BOOST_CHECK(!intersects(mnemonic::ko, mnemonic::cs));
    ////BOOST_CHECK(!intersects(mnemonic::ko, mnemonic::pt));
    ////BOOST_CHECK(!intersects(mnemonic::ko, mnemonic::ja));
    BOOST_CHECK_EQUAL(intersection(mnemonic::ko, mnemonic::ko), dictionary_size);
    BOOST_CHECK(!intersects(mnemonic::ko, mnemonic::zh_Hans));
    BOOST_CHECK(!intersects(mnemonic::ko, mnemonic::zh_Hant));
}

BOOST_AUTO_TEST_CASE(catalogs_mnemonic__intersections__zh_Hans__expected)
{
    ////BOOST_CHECK(!intersects(mnemonic::zh_Hans, mnemonic::en));
    ////BOOST_CHECK(!intersects(mnemonic::zh_Hans, mnemonic::es));
    ////BOOST_CHECK(!intersects(mnemonic::zh_Hans, mnemonic::it));
    ////BOOST_CHECK(!intersects(mnemonic::zh_Hans, mnemonic::fr));
    ////BOOST_CHECK(!intersects(mnemonic::zh_Hans, mnemonic::cs));
    ////BOOST_CHECK(!intersects(mnemonic::zh_Hans, mnemonic::pt));
    ////BOOST_CHECK(!intersects(mnemonic::zh_Hans, mnemonic::ja));
    ////BOOST_CHECK(!intersects(mnemonic::zh_Hans, mnemonic::ko));
    BOOST_CHECK_EQUAL(intersection(mnemonic::zh_Hans, mnemonic::zh_Hans), dictionary_size);
    BOOST_CHECK_EQUAL(intersection(mnemonic::zh_Hans, mnemonic::zh_Hant), intersection_zh);
}

BOOST_AUTO_TEST_CASE(catalogs_mnemonic__intersections__zh_Hant__expected)
{
    ////BOOST_CHECK(!intersects(mnemonic::zh_Hant, mnemonic::en));
    ////BOOST_CHECK(!intersects(mnemonic::zh_Hant, mnemonic::es));
    ////BOOST_CHECK(!intersects(mnemonic::zh_Hant, mnemonic::it));
    ////BOOST_CHECK(!intersects(mnemonic::zh_Hant, mnemonic::fr));
    ////BOOST_CHECK(!intersects(mnemonic::zh_Hant, mnemonic::cs));
    ////BOOST_CHECK(!intersects(mnemonic::zh_Hant, mnemonic::pt));
    ////BOOST_CHECK(!intersects(mnemonic::zh_Hant, mnemonic::ja));
    ////BOOST_CHECK(!intersects(mnemonic::zh_Hant, mnemonic::ko));
    ////BOOST_CHECK(intersection(mnemonic::zh_Hant, mnemonic::zh_Hans), intersection_zh);
    BOOST_CHECK_EQUAL(intersection(mnemonic::zh_Hant, mnemonic::zh_Hant), dictionary_size);
}

BOOST_AUTO_TEST_SUITE_END()
