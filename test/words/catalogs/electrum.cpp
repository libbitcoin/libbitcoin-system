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
#include "electrum.hpp"

BC_AUTO_TEST_SUITE(catalogs_electrum_tests)

using namespace test::catalogs_electrum;
using namespace bc::system::words;

const auto dictionary_count = 10u;

// The french combining characters are diacritics in nfkd (normal) form.
const auto combinings_es = 334;
const auto combinings_fr = 366;
const auto combinings_ja = 644;

const auto abnormals_es = 334;
const auto abnormals_ja = 644;
const auto divergences_es = 334;
const auto divergences_ja = 644;

// count

BC_AUTO_TEST_CASE(catalogs_electrum__count__all__expected)
{
    // Any new dictionary must be added below to guarantee lack of normalization.
    // Failure to do so may lead to invalid seed generation, which is very bad.
    BC_CHECK_MESSAGE(electrum::catalogs::count() == dictionary_count, "new dictionary");
}

// subset

// Identities are the same as the mnemonic dictionaries.
// nfkd testing is performed on the mnemonic dictionaries.
// This test ensures electrum dictionaries are a subset of mnemonic dictionaries.
BC_AUTO_TEST_CASE(catalogs_electrum__mnemonic__subset__true)
{
    BC_CHECK_EQUAL(electrum::en.word, mnemonic::en.word);
    BC_CHECK_EQUAL(electrum::es.word, mnemonic::es.word);
    BC_CHECK_EQUAL(electrum::it.word, mnemonic::it.word);
    BC_CHECK_EQUAL(electrum::fr.word, mnemonic::fr.word);
    BC_CHECK_EQUAL(electrum::cs.word, mnemonic::cs.word);
    BC_CHECK_EQUAL(electrum::pt.word, mnemonic::pt.word);
    BC_CHECK_EQUAL(electrum::ja.word, mnemonic::ja.word);
    BC_CHECK_EQUAL(electrum::ko.word, mnemonic::ko.word);
    BC_CHECK_EQUAL(electrum::zh_Hans.word, mnemonic::zh_Hans.word);
    BC_CHECK_EQUAL(electrum::zh_Hant.word, mnemonic::zh_Hant.word);
}
// combined

// The spanish, french and japanese dictionaries contain combining characters.
// This requires combinings removal in these (only) for wordlist-based seedings.
BC_AUTO_TEST_CASE(catalogs_electrum__combinings__combininged_words__true)
{
    BC_CHECK_EQUAL(combinings(electrum::es), combinings_es);
    BC_CHECK_EQUAL(combinings(electrum::fr), combinings_fr);
    BC_CHECK_EQUAL(combinings(electrum::ja), combinings_ja);
}

// No words in these dictionaries contain combining characters.
// So there is no need to normalize combinings these for wordlist-based seedings.
BC_AUTO_TEST_CASE(catalogs_electrum__combined__not_combininged_words__false)
{
    BC_CHECK(!combined(electrum::en));
    BC_CHECK(!combined(electrum::it));
    BC_CHECK(!combined(electrum::cs));
    BC_CHECK(!combined(electrum::pt));
    BC_CHECK(!combined(electrum::ko));
    BC_CHECK(!combined(electrum::zh_Hans));
    BC_CHECK(!combined(electrum::zh_Hant));
}

// compressed_cjk

// No words in any supported dictionaries are uncompressed.
// So there is no need to normalize compression for wordlist-based seeding.
BC_AUTO_TEST_CASE(catalogs_electrum__compressed__not_compressed_words__false)
{
    BC_CHECK(!compressed(electrum::en));
    BC_CHECK(!compressed(electrum::es));
    BC_CHECK(!compressed(electrum::it));
    BC_CHECK(!compressed(electrum::fr));
    BC_CHECK(!compressed(electrum::cs));
    BC_CHECK(!compressed(electrum::pt));
    BC_CHECK(!compressed(electrum::ja));
    BC_CHECK(!compressed(electrum::ko));
    BC_CHECK(!compressed(electrum::zh_Hans));
    BC_CHECK(!compressed(electrum::zh_Hant));
}

// abnormal
// ----------------------------------------------------------------------------
// These dictionaries from the electrum repo are not in nfkd form, but are
// otherwise identical to BIP39 dictionaries. We do not use these words within
// electrum sources, we use BIP39 words. These tests verify that the BIP39
// dictionaries are functionally identical.

// This verifies the number of abnormals in the two divergent dictionaries.
BC_AUTO_TEST_CASE(catalogs_electrum__abnormal__unused_words__false)
{
    BC_CHECK_EQUAL(abnormals(electrum_es), abnormals_es);
    BC_CHECK_EQUAL(abnormals(electrum_ja), abnormals_ja);
}

// divergence

// This verifies that the differences are only in nfkd normalization.
// Otherwise there could be differences in word value and/or position.
BC_AUTO_TEST_CASE(catalogs_electrum__divergences__unused_words__false)
{
    BC_CHECK_EQUAL(divergences(electrum::es, electrum_es), divergences_es);
    BC_CHECK_EQUAL(divergences(electrum::ja, electrum_ja), divergences_ja);
}

BC_AUTO_TEST_SUITE_END()
