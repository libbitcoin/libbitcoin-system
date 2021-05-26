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
#include "../../../test.hpp"
#include "mnemonic.hpp"

BOOST_AUTO_TEST_SUITE(dictionaries_mnemonic_tests)

using namespace test::dictionaries_mnemonic;
using namespace bc::system::wallet;

const auto dictionary_count = 10u;
const auto dictionary_size = 2048;
const auto deviations_en_fr = 100;
const auto intersecton_en_fr = 100;
const auto deviations_zh = 0;
const auto intersection_zh = 1275;

BOOST_AUTO_TEST_CASE(dictionaries_mnemonic__count__all__expected)
{
    // Any new dictionary must be added below to guarantee lack of normalization.
    // Failure to do so may lead to invalid seed generation, which is very bad.
    BOOST_REQUIRE_MESSAGE(mnemonic::dictionaries::count() == dictionary_count, "new dictionary");
}

// abnormal (requires WITH_ICU)

// These dictionaries are in normal form.
// So there is no need to nfkd normalize these for wordlist-based seedings.
// This also removes the ICU dependency for these language.
BOOST_AUTO_TEST_CASE(dictionaries_mnemonic__normal__normal_words__true)
{
    // The result is definitive only when WITH_ICU is defined.
    BOOST_REQUIRE(!abnormal(mnemonic::en));
    BOOST_REQUIRE(!abnormal(mnemonic::es));
    BOOST_REQUIRE(!abnormal(mnemonic::it));
    BOOST_REQUIRE(!abnormal(mnemonic::fr));
    BOOST_REQUIRE(!abnormal(mnemonic::cs));
    BOOST_REQUIRE(!abnormal(mnemonic::pt));
    BOOST_REQUIRE(!abnormal(mnemonic::ja));
    BOOST_REQUIRE(!abnormal(mnemonic::ko));
    BOOST_REQUIRE(!abnormal(mnemonic::zh_Hans));
    BOOST_REQUIRE(!abnormal(mnemonic::zh_Hant));
}

// All zh_Hans/zh_Hant collisions are interchangeable.
// None of the en/fr collisions are interchangeable.
// It is unsafe to deduce en/fr dictionary, but only if all words collide.
// Always check both dictionaries in both zh_Hans/zh_Hant and en/fr.
// If zh_Hans/zh_Hant collide, may pick either (but should be consistent).
// If en/fr collide, must fail as decode requires explicit language identifier.
BOOST_AUTO_TEST_CASE(dictionaries_mnemonic__deviation__deviants__expected)
{
    BOOST_REQUIRE_EQUAL(deviation(mnemonic::en, mnemonic::fr), deviations_en_fr);
    BOOST_REQUIRE_EQUAL(deviation(mnemonic::zh_Hans, mnemonic::zh_Hant), deviations_zh);
}

BOOST_AUTO_TEST_CASE(dictionaries_mnemonic__intersections__en__expected)
{
    BOOST_REQUIRE_EQUAL(intersection(mnemonic::en, mnemonic::en), dictionary_size);
    BOOST_REQUIRE(!intersects(mnemonic::en, mnemonic::es));
    BOOST_REQUIRE(!intersects(mnemonic::en, mnemonic::it));
    BOOST_REQUIRE_EQUAL(intersection(mnemonic::en, mnemonic::fr), intersecton_en_fr);
    BOOST_REQUIRE(!intersects(mnemonic::en, mnemonic::cs));
    BOOST_REQUIRE(!intersects(mnemonic::en, mnemonic::pt));
    BOOST_REQUIRE(!intersects(mnemonic::en, mnemonic::ja));
    BOOST_REQUIRE(!intersects(mnemonic::en, mnemonic::ko));
    BOOST_REQUIRE(!intersects(mnemonic::en, mnemonic::zh_Hans));
    BOOST_REQUIRE(!intersects(mnemonic::en, mnemonic::zh_Hant));
}

BOOST_AUTO_TEST_CASE(dictionaries_mnemonic__intersections__es__expected)
{
    ////BOOST_REQUIRE(!intersects(mnemonic::es, mnemonic::en));
    BOOST_REQUIRE_EQUAL(intersection(mnemonic::es, mnemonic::es), dictionary_size);
    BOOST_REQUIRE(!intersects(mnemonic::es, mnemonic::it));
    BOOST_REQUIRE(!intersects(mnemonic::es, mnemonic::fr));
    BOOST_REQUIRE(!intersects(mnemonic::es, mnemonic::cs));
    BOOST_REQUIRE(!intersects(mnemonic::es, mnemonic::pt));
    BOOST_REQUIRE(!intersects(mnemonic::es, mnemonic::ja));
    BOOST_REQUIRE(!intersects(mnemonic::es, mnemonic::ko));
    BOOST_REQUIRE(!intersects(mnemonic::es, mnemonic::zh_Hans));
    BOOST_REQUIRE(!intersects(mnemonic::es, mnemonic::zh_Hant));
}

BOOST_AUTO_TEST_CASE(dictionaries_mnemonic__intersections__it__expected)
{
    ////BOOST_REQUIRE(!intersects(mnemonic::it, mnemonic::en));
    ////BOOST_REQUIRE(!intersects(mnemonic::it, mnemonic::es));
    BOOST_REQUIRE_EQUAL(intersection(mnemonic::it, mnemonic::it), dictionary_size);
    BOOST_REQUIRE(!intersects(mnemonic::it, mnemonic::fr));
    BOOST_REQUIRE(!intersects(mnemonic::it, mnemonic::cs));
    BOOST_REQUIRE(!intersects(mnemonic::it, mnemonic::pt));
    BOOST_REQUIRE(!intersects(mnemonic::it, mnemonic::ja));
    BOOST_REQUIRE(!intersects(mnemonic::it, mnemonic::ko));
    BOOST_REQUIRE(!intersects(mnemonic::it, mnemonic::zh_Hans));
    BOOST_REQUIRE(!intersects(mnemonic::it, mnemonic::zh_Hant));
}

BOOST_AUTO_TEST_CASE(dictionaries_mnemonic__intersections__fr__expected)
{
    ////BOOST_REQUIRE_EQUAL(intersection(mnemonic::fr, mnemonic::en), intersecton_en_fr);
    ////BOOST_REQUIRE(!intersects(mnemonic::fr, mnemonic::es));
    ////BOOST_REQUIRE(!intersects(mnemonic::fr, mnemonic::it));
    BOOST_REQUIRE_EQUAL(intersection(mnemonic::fr, mnemonic::fr), dictionary_size);
    BOOST_REQUIRE(!intersects(mnemonic::fr, mnemonic::cs));
    BOOST_REQUIRE(!intersects(mnemonic::fr, mnemonic::pt));
    BOOST_REQUIRE(!intersects(mnemonic::fr, mnemonic::ja));
    BOOST_REQUIRE(!intersects(mnemonic::fr, mnemonic::ko));
    BOOST_REQUIRE(!intersects(mnemonic::fr, mnemonic::zh_Hans));
    BOOST_REQUIRE(!intersects(mnemonic::fr, mnemonic::zh_Hant));
}

BOOST_AUTO_TEST_CASE(dictionaries_mnemonic__intersections__cs__expected)
{
    ////BOOST_REQUIRE(!intersects(mnemonic::cs, mnemonic::en));
    ////BOOST_REQUIRE(!intersects(mnemonic::cs, mnemonic::es));
    ////BOOST_REQUIRE(!intersects(mnemonic::cs, mnemonic::it));
    ////BOOST_REQUIRE(!intersects(mnemonic::cs, mnemonic::fr));
    BOOST_REQUIRE_EQUAL(intersection(mnemonic::cs, mnemonic::cs), dictionary_size);
    BOOST_REQUIRE(!intersects(mnemonic::cs, mnemonic::pt));
    BOOST_REQUIRE(!intersects(mnemonic::cs, mnemonic::ja));
    BOOST_REQUIRE(!intersects(mnemonic::cs, mnemonic::ko));
    BOOST_REQUIRE(!intersects(mnemonic::cs, mnemonic::zh_Hans));
    BOOST_REQUIRE(!intersects(mnemonic::cs, mnemonic::zh_Hant));
}

BOOST_AUTO_TEST_CASE(dictionaries_mnemonic__intersections__pt__expected)
{
    ////BOOST_REQUIRE(!intersects(mnemonic::pt, mnemonic::en));
    ////BOOST_REQUIRE(!intersects(mnemonic::pt, mnemonic::es));
    ////BOOST_REQUIRE(!intersects(mnemonic::pt, mnemonic::it));
    ////BOOST_REQUIRE(!intersects(mnemonic::pt, mnemonic::fr));
    ////BOOST_REQUIRE(!intersects(mnemonic::pt, mnemonic::cs));
    BOOST_REQUIRE_EQUAL(intersection(mnemonic::pt, mnemonic::pt), dictionary_size);
    BOOST_REQUIRE(!intersects(mnemonic::pt, mnemonic::ja));
    BOOST_REQUIRE(!intersects(mnemonic::pt, mnemonic::ko));
    BOOST_REQUIRE(!intersects(mnemonic::pt, mnemonic::zh_Hans));
    BOOST_REQUIRE(!intersects(mnemonic::pt, mnemonic::zh_Hant));
}

BOOST_AUTO_TEST_CASE(dictionaries_mnemonic__intersections__ja__expected)
{
    ////BOOST_REQUIRE(!intersects(mnemonic::ja, mnemonic::en));
    ////BOOST_REQUIRE(!intersects(mnemonic::ja, mnemonic::es));
    ////BOOST_REQUIRE(!intersects(mnemonic::ja, mnemonic::it));
    ////BOOST_REQUIRE(!intersects(mnemonic::ja, mnemonic::fr));
    ////BOOST_REQUIRE(!intersects(mnemonic::ja, mnemonic::cs));
    ////BOOST_REQUIRE(!intersects(mnemonic::ja, mnemonic::pt));
    BOOST_REQUIRE_EQUAL(intersection(mnemonic::ja, mnemonic::ja), dictionary_size);
    BOOST_REQUIRE(!intersects(mnemonic::ja, mnemonic::ko));
    BOOST_REQUIRE(!intersects(mnemonic::ja, mnemonic::zh_Hans));
    BOOST_REQUIRE(!intersects(mnemonic::ja, mnemonic::zh_Hant));
}

BOOST_AUTO_TEST_CASE(dictionaries_mnemonic__intersections__ko__expected)
{
    ////BOOST_REQUIRE(!intersects(mnemonic::ko, mnemonic::en));
    ////BOOST_REQUIRE(!intersects(mnemonic::ko, mnemonic::es));
    ////BOOST_REQUIRE(!intersects(mnemonic::ko, mnemonic::it));
    ////BOOST_REQUIRE(!intersects(mnemonic::ko, mnemonic::fr));
    ////BOOST_REQUIRE(!intersects(mnemonic::ko, mnemonic::cs));
    ////BOOST_REQUIRE(!intersects(mnemonic::ko, mnemonic::pt));
    ////BOOST_REQUIRE(!intersects(mnemonic::ko, mnemonic::ja));
    BOOST_REQUIRE_EQUAL(intersection(mnemonic::ko, mnemonic::ko), dictionary_size);
    BOOST_REQUIRE(!intersects(mnemonic::ko, mnemonic::zh_Hans));
    BOOST_REQUIRE(!intersects(mnemonic::ko, mnemonic::zh_Hant));
}

BOOST_AUTO_TEST_CASE(dictionaries_mnemonic__intersections__zh_Hans__expected)
{
    ////BOOST_REQUIRE(!intersects(mnemonic::zh_Hans, mnemonic::en));
    ////BOOST_REQUIRE(!intersects(mnemonic::zh_Hans, mnemonic::es));
    ////BOOST_REQUIRE(!intersects(mnemonic::zh_Hans, mnemonic::it));
    ////BOOST_REQUIRE(!intersects(mnemonic::zh_Hans, mnemonic::fr));
    ////BOOST_REQUIRE(!intersects(mnemonic::zh_Hans, mnemonic::cs));
    ////BOOST_REQUIRE(!intersects(mnemonic::zh_Hans, mnemonic::pt));
    ////BOOST_REQUIRE(!intersects(mnemonic::zh_Hans, mnemonic::ja));
    ////BOOST_REQUIRE(!intersects(mnemonic::zh_Hans, mnemonic::ko));
    BOOST_REQUIRE_EQUAL(intersection(mnemonic::zh_Hans, mnemonic::zh_Hans), dictionary_size);
    BOOST_REQUIRE_EQUAL(intersection(mnemonic::zh_Hans, mnemonic::zh_Hant), intersection_zh);
}

BOOST_AUTO_TEST_CASE(dictionaries_mnemonic__intersections__zh_Hant__expected)
{
    ////BOOST_REQUIRE(!intersects(mnemonic::zh_Hant, mnemonic::en));
    ////BOOST_REQUIRE(!intersects(mnemonic::zh_Hant, mnemonic::es));
    ////BOOST_REQUIRE(!intersects(mnemonic::zh_Hant, mnemonic::it));
    ////BOOST_REQUIRE(!intersects(mnemonic::zh_Hant, mnemonic::fr));
    ////BOOST_REQUIRE(!intersects(mnemonic::zh_Hant, mnemonic::cs));
    ////BOOST_REQUIRE(!intersects(mnemonic::zh_Hant, mnemonic::pt));
    ////BOOST_REQUIRE(!intersects(mnemonic::zh_Hant, mnemonic::ja));
    ////BOOST_REQUIRE(!intersects(mnemonic::zh_Hant, mnemonic::ko));
    ////BOOST_REQUIRE(intersection(mnemonic::zh_Hant, mnemonic::zh_Hans), intersection_zh);
    BOOST_REQUIRE_EQUAL(intersection(mnemonic::zh_Hant, mnemonic::zh_Hant), dictionary_size);
}

BOOST_AUTO_TEST_SUITE_END()
