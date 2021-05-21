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
#include "electrum.hpp"

BOOST_AUTO_TEST_SUITE(dictionaries_electrum_tests)

using namespace test::dictionaries_electrum;
using namespace bc::system::wallet;

const auto dictionary_count = 10u;

// The french combining diacritics are in nfkd (normal) form.
const auto accents_es = 334;
const auto accents_fr = 366;
const auto accents_ja = 644;

BOOST_AUTO_TEST_CASE(dictionaries_electrum__count__all__expected)
{
    // Any new dictionary must be added below to guarantee lack of normalization.
    // Failure to do so may lead to invalid seed generation, which is very bad.
    BOOST_REQUIRE_MESSAGE(electrum::dictionaries::count() == dictionary_count, "new dictionary");
}

// nfkd testing is performed on the mnemonic dictionaries.
// This test ensures electrum dictionaries are a subset of mnemonic dictionaries.
BOOST_AUTO_TEST_CASE(dictionaries_electrum__mnemonic__subset__true)
{
    BOOST_REQUIRE_EQUAL(electrum::en, mnemonic::en);
    BOOST_REQUIRE_EQUAL(electrum::es, mnemonic::es);
    BOOST_REQUIRE_EQUAL(electrum::it, mnemonic::it);
    BOOST_REQUIRE_EQUAL(electrum::fr, mnemonic::fr);
    BOOST_REQUIRE_EQUAL(electrum::cs, mnemonic::cs);
    BOOST_REQUIRE_EQUAL(electrum::pt, mnemonic::pt);
    BOOST_REQUIRE_EQUAL(electrum::ja, mnemonic::ja);
    BOOST_REQUIRE_EQUAL(electrum::ko, mnemonic::ko);
    BOOST_REQUIRE_EQUAL(electrum::zh_Hans, mnemonic::zh_Hans);
    BOOST_REQUIRE_EQUAL(electrum::zh_Hant, mnemonic::zh_Hant);
}

#ifdef WITH_ICU

// All of the abnormals are diacritics.
const auto abnormals_es = 334;
const auto abnormals_ja = 644;

// abnormal (requires ICU)

// These dictionaries from the electrum repo are not in nfkd form.
// But the others are identical to BIP39 dictionaries.
// We do not use these with in electrum sources, we use BIP39 words.
// This test verifies that the differences are only in nfkd normalization.
BOOST_AUTO_TEST_CASE(dictionaries_electrum__abnormal__unused_words__false)
{
    BOOST_REQUIRE_EQUAL(abnormals(electrum_es), abnormals_es);
    BOOST_REQUIRE_EQUAL(abnormals(electrum_ja), abnormals_ja);
}

// This verifies that divergence is limited to the expected difference in form.
BOOST_AUTO_TEST_CASE(dictionaries_electrum__diverges__unused_words__false)
{
    BOOST_REQUIRE(!diverged(electrum::es, electrum_es));
    BOOST_REQUIRE(!diverged(electrum::ja, electrum_ja));
}

#endif

// combined

// The spanish, french and japanese dictionaries contain combining diacritics.
// This requires combinings removal in these (only) for wordlist-based seedings.
BOOST_AUTO_TEST_CASE(dictionaries_electrum__accents__accented_words__true)
{
    BOOST_REQUIRE_EQUAL(combinings(electrum::es), accents_es);
    BOOST_REQUIRE_EQUAL(combinings(electrum::fr), accents_fr);
    BOOST_REQUIRE_EQUAL(combinings(electrum::ja), accents_ja);
}

// No words in these dictionaries contain combining diacritics.
// So there is no need to normalize combinings these for wordlist-based seedings.
BOOST_AUTO_TEST_CASE(dictionaries_electrum__accented__not_accented_words__false)
{
    BOOST_REQUIRE(!combined(electrum::en));
    BOOST_REQUIRE(!combined(electrum::it));
    BOOST_REQUIRE(!combined(electrum::cs));
    BOOST_REQUIRE(!combined(electrum::pt));
    BOOST_REQUIRE(!combined(electrum::ko));
    BOOST_REQUIRE(!combined(electrum::zh_Hans));
    BOOST_REQUIRE(!combined(electrum::zh_Hant));
}

// compressed_cjk

// No words in any supported dictionaries are compressed.
// So there is no need to normalize compression for wordlist-based seeding.
BOOST_AUTO_TEST_CASE(dictionaries_electrum__compressed__not_compressed_words__false)
{
    BOOST_REQUIRE(!compressed(electrum::en));
    BOOST_REQUIRE(!compressed(electrum::es));
    BOOST_REQUIRE(!compressed(electrum::it));
    BOOST_REQUIRE(!compressed(electrum::fr));
    BOOST_REQUIRE(!compressed(electrum::cs));
    BOOST_REQUIRE(!compressed(electrum::pt));
    BOOST_REQUIRE(!compressed(electrum::ja));
    BOOST_REQUIRE(!compressed(electrum::ko));
    BOOST_REQUIRE(!compressed(electrum::zh_Hans));
    BOOST_REQUIRE(!compressed(electrum::zh_Hant));
}

BOOST_AUTO_TEST_SUITE_END()
