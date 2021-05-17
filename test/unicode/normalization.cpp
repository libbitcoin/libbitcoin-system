/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(unicode_normalization_tests)

#ifdef WITH_ICU

// to_lower

BOOST_AUTO_TEST_CASE(normalization__to_lower__empty__empty)
{
    BOOST_REQUIRE(to_lower("").empty());
}

BOOST_AUTO_TEST_CASE(normalization__to_lower__lower_ascii__unchanged)
{
    const std::string lower = "abcdefghijklmnopqrstuvwxyz";
    BOOST_REQUIRE_EQUAL(to_lower(lower), lower);
}

BOOST_AUTO_TEST_CASE(normalization__to_lower__upper_ascii__lowered)
{
    const auto upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const auto expected = "abcdefghijklmnopqrstuvwxyz";
    BOOST_REQUIRE_EQUAL(to_lower(upper), expected);
}

BOOST_AUTO_TEST_CASE(normalization__to_lower__mixed_ascii__lowered)
{
    const auto mixed = "AbCdEfGhIjKlMnOpQrStUvWxYz";
    const auto expected = "abcdefghijklmnopqrstuvwxyz";
    BOOST_REQUIRE_EQUAL(to_lower(mixed), expected);
}

BOOST_AUTO_TEST_CASE(normalization__to_lower__ideographic_space__unchanged)
{
    BOOST_REQUIRE_EQUAL(to_lower(ideographic_space), ideographic_space);
}

// to_upper

BOOST_AUTO_TEST_CASE(normalization__to_upper__empty__empty)
{
    BOOST_REQUIRE(to_upper("").empty());
}

BOOST_AUTO_TEST_CASE(normalization__to_upper__upper_ascii__unchanged)
{
    const auto upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    BOOST_REQUIRE_EQUAL(to_upper(upper), upper);
}

BOOST_AUTO_TEST_CASE(normalization__to_upper__lower_ascii__uppered)
{
    const auto lower = "abcdefghijklmnopqrstuvwxyz";
    const auto expected = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    BOOST_REQUIRE_EQUAL(to_upper(lower), expected);
}

BOOST_AUTO_TEST_CASE(normalization__to_upper__mixed_ascii__uppered)
{
    const auto mixed = "AbCdEfGhIjKlMnOpQrStUvWxYz";
    const auto expected = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    BOOST_REQUIRE_EQUAL(to_upper(mixed), expected);
}

BOOST_AUTO_TEST_CASE(normalization__to_upper__ideographic_space__unchanged)
{
    BOOST_REQUIRE_EQUAL(to_upper(ideographic_space), ideographic_space);
}

// to_canonical_composition (nfc)

BOOST_AUTO_TEST_CASE(normalization__to_canonical_composition__empty__empty)
{
    BOOST_REQUIRE(to_canonical_composition("").empty());
}

// github.com/bitcoin/bips/blob/master/bip-0038.mediawiki
BOOST_AUTO_TEST_CASE(normalization__to_canonical_composition__bip38__expected)
{
    const auto original = to_string(base16_literal("cf92cc8100f0909080f09f92a9"));
    const auto expected = to_string(base16_literal("cf9300f0909080f09f92a9"));
    BOOST_REQUIRE_EQUAL(to_canonical_composition(original), expected);
}

// to_canonical_decomposition (nfkc)

BOOST_AUTO_TEST_CASE(normalization__to_canonical_decomposition__empty__empty)
{
    BOOST_REQUIRE(to_canonical_decomposition("").empty());
}

BOOST_AUTO_TEST_CASE(normalization__to_canonical_decomposition__todo__expected)
{
    ////// TODO: create test vector (this is copied from nfc).
    ////const auto original = to_string(base16_literal("cf92cc8100f0909080f09f92a9"));
    ////const auto expected = to_string(base16_literal("cf9300f0909080f09f92a9"));
    ////BOOST_REQUIRE_EQUAL(to_canonical_decomposition(original), expected);
}

// to_compatibility_composition (nfd)

BOOST_AUTO_TEST_CASE(normalization__to_compatibility_composition__empty__empty)
{
    BOOST_REQUIRE(to_compatibility_composition("").empty());
}

BOOST_AUTO_TEST_CASE(normalization__to_compatibility_composition__todo__expected)
{
    ////// TODO: create test vector (this is copied from nfc).
    ////const auto original = to_string(base16_literal("cf92cc8100f0909080f09f92a9"));
    ////const auto expected = to_string(base16_literal("cf9300f0909080f09f92a9"));
    ////BOOST_REQUIRE_EQUAL(to_compatibility_composition(original), expected);
}

// to_compatibility_demposition (nfkd)

BOOST_AUTO_TEST_CASE(normalization__to_compatibility_demposition__ideographic_space_sandwich__expected)
{
    const auto ascii_space_sandwich = "space-> <-space";
    const auto ideographic_space_sandwich = "space->　<-space";
    const auto normalized = to_compatibility_demposition(ideographic_space_sandwich);
    BOOST_REQUIRE_EQUAL(normalized.c_str(), ascii_space_sandwich);
}

BOOST_AUTO_TEST_CASE(normalization__to_compatibility_demposition__61cc81__c3a1)
{
    const std::string literal_c3a1 = "á";

    // This is copied from the the BIP39 encoding:
    // github.com/bitcoin/bips/blob/master/bip-0039/spanish.txt#L93
    const std::string literal_61cc81 = "á";

    const std::string string_c3a1 = "\xc3\xa1";
    const std::string string_61cc81 = "\x61\xcc\x81";
    BOOST_REQUIRE_EQUAL(string_c3a1, literal_c3a1);
    BOOST_REQUIRE_EQUAL(string_61cc81, literal_61cc81);

    const auto normalized_c3a1 = to_compatibility_demposition(string_c3a1);
    const auto normalized_61cc81 = to_compatibility_demposition(string_61cc81);
    BOOST_REQUIRE_EQUAL(normalized_61cc81.c_str(), string_61cc81);
    BOOST_REQUIRE_EQUAL(normalized_c3a1.c_str(), string_61cc81);
}

#endif // WITH_ICU

// to_unaccented_form (Electrum)

BOOST_AUTO_TEST_CASE(normalization__to_unaccented_form__empty__empty)
{
    BOOST_REQUIRE(to_unaccented_form("").empty());
}

BOOST_AUTO_TEST_CASE(normalization__to_unaccented_form__ascii_only__unchanged)
{
    const std::u32string ascii{ 0x42, 0x43 };
    const std::string expected{ 0x42, 0x43 };
    BOOST_REQUIRE_EQUAL(to_unaccented_form(to_utf8(ascii)), expected);
}

BOOST_AUTO_TEST_CASE(normalization__to_unaccented_form__circumflex_accent_only__empty)
{
    const std::u32string circumflex_accent{ 0x0000005e };
    BOOST_REQUIRE(to_unaccented_form(to_utf8(circumflex_accent)).empty());
}

BOOST_AUTO_TEST_CASE(normalization__to_unaccented_form__bassa_vah_tones_only__empty)
{
    const std::u32string bassa_vah{ 0x00016af1, 0x00016af2, 0x00016af3 };
    BOOST_REQUIRE(to_unaccented_form(to_utf8(bassa_vah)).empty());
}

BOOST_AUTO_TEST_CASE(normalization__to_unaccented_form__bassa_vah_tones_ascii_sandwich__stripped)
{
    const std::string expected{ 0x42, 0x43 };
    const std::u32string bassa_vah{ 0x42, 0x00016af1, 0x00016af2, 0x00016af3, 0x43 };
    BOOST_REQUIRE_EQUAL(to_unaccented_form(to_utf8(bassa_vah)), expected);
}

BOOST_AUTO_TEST_CASE(normalization__to_unaccented_form__circumflex_accent_and_bassa_vah_tones_and_ascii_mix__stripped)
{
    const std::string expected{ 0x42, 0x43, 0x44 };
    const std::u32string bassa_vah{ 0x00016af1, 0x42, 0x43, 0x0000005e, 0x00016af2, 0x44, 0x00016af3 };
    BOOST_REQUIRE_EQUAL(to_unaccented_form(to_utf8(bassa_vah)), expected);
}

// to_compressed_cjk_form (Electrum)

BOOST_AUTO_TEST_CASE(normalization__to_compressed_cjk_form__empty__empty)
{
    BOOST_REQUIRE(to_compressed_cjk_form("").empty());
}

BOOST_AUTO_TEST_CASE(normalization__to_compressed_cjk_form__ascii_with_spaces__unchanged)
{
    const auto value = " foo  bar  baz ";
    const auto result = to_compressed_cjk_form(value);
    BOOST_REQUIRE_EQUAL(result.c_str(), value);
}

BOOST_AUTO_TEST_CASE(normalization__to_compressed_cjk__cjk_unified_ideograph_only__unchanged)
{
    const std::u32string cjk_unified_ideograph{ 0x00004e00 };
    const auto utf8 = to_utf8(cjk_unified_ideograph);
    BOOST_REQUIRE_EQUAL(to_compressed_cjk_form(utf8), utf8);
}

BOOST_AUTO_TEST_CASE(normalization__to_compressed_cjk__cjk_unified_ideograph_sandwich__stripped)
{
    const std::u32string expected{ 0x00004e00, 0x00004e00 };
    const std::u32string cjk_unified_ideograph_sandwich{ 0x00004e00, 0x20, 0x00004e00 };
    const auto utf8 = to_utf8(cjk_unified_ideograph_sandwich);
    BOOST_REQUIRE_EQUAL(to_compressed_cjk_form(utf8), to_utf8(expected));
}

BOOST_AUTO_TEST_CASE(normalization__to_compressed_cjk__cjk_unified_ideograph_right__unchanged)
{
    const std::u32string expected{ 0x20, 0x00004e00, 0x00004e00 };
    const std::u32string cjk_unified_ideograph_right{ 0x20, 0x00004e00, 0x00004e00 };
    const auto utf8 = to_utf8(cjk_unified_ideograph_right);
    BOOST_REQUIRE_EQUAL(to_compressed_cjk_form(utf8), to_utf8(expected));
}

BOOST_AUTO_TEST_CASE(normalization__to_compressed_cjk__cjk_unified_ideograph_left__unchanged)
{
    const std::u32string expected{ 0x00004e00, 0x00004e00, 0x20 };
    const std::u32string cjk_unified_ideograph_left{ 0x00004e00, 0x00004e00, 0x20 };
    const auto utf8 = to_utf8(cjk_unified_ideograph_left);
    BOOST_REQUIRE_EQUAL(to_compressed_cjk_form(utf8), to_utf8(expected));
}

BOOST_AUTO_TEST_CASE(normalization__to_compressed_cjk__cjk_unified_ideograph_both__unchanged)
{
    const std::u32string expected{ 0x20, 0x00004e00, 0x00004e00, 0x20 };
    const std::u32string cjk_unified_ideograph_both{ 0x20, 0x00004e00, 0x00004e00, 0x20 };
    const auto utf8 = to_utf8(cjk_unified_ideograph_both);
    BOOST_REQUIRE_EQUAL(to_compressed_cjk_form(utf8), to_utf8(expected));
}

BOOST_AUTO_TEST_CASE(normalization__to_compressed_cjk__cjk_unified_ideograph_all__middle_stripped)
{
    const std::u32string expected{ 0x20, 0x00004e00, 0x00004e00, 0x20 };
    const std::u32string cjk_unified_ideograph_all{ 0x20, 0x00004e00, 0x0c, 0x00004e00, 0x20 };
    const auto utf8 = to_utf8(cjk_unified_ideograph_all);
    BOOST_REQUIRE_EQUAL(to_compressed_cjk_form(utf8), to_utf8(expected));
}

BOOST_AUTO_TEST_CASE(normalization__to_compressed_cjk__single_and_double_contained_whitespace__single_stripped)
{
    const std::u32string expected
    {
        0x20, 0x00004e00, 0x00004e00, 0x20,
        0x20, 0x00004e00, 0x00004e00, 0x20,
        0x20, 0x00004e00, 0x00004e00, 0x20,
        0x20, 0x00004e00, 0x00004e00, 0x20,
        0x20, 0x00004e00, 0x00004e00, 0x20,
        0x20, 0x00004e00, 0x00004e00, 0x20
    };
    const std::u32string single_and_double_contained
    {
        0x20, 0x00004e00, 0x20, 0x00004e00, 0x20,
        0x20, 0x00004e00, 0x0c, 0x00004e00, 0x20,
        0x20, 0x00004e00, 0x0a, 0x00004e00, 0x20,
        0x20, 0x00004e00, 0x0d, 0x00004e00, 0x20,
        0x20, 0x00004e00, 0x09, 0x00004e00, 0x20,
        0x20, 0x00004e00, 0x0b, 0x00004e00, 0x20
    };
    const auto utf8 = to_utf8(single_and_double_contained);
    BOOST_REQUIRE_EQUAL(to_compressed_cjk_form(utf8), to_utf8(expected));
}

BOOST_AUTO_TEST_CASE(normalization__to_compressed_cjk__alternating_single_contained_whitespace__contained_stripped)
{
    const std::u32string expected
    {
        0x20, 
        0x00004e00, 0x00004e00,
        0x00004e00, 0x00004e00,
        0x00004e00, 0x00004e00,
        0x00004e00, 0x00004e00,
        0x00004e00, 0x00004e00,
        0x00004e00, 0x00004e00,
        0x20
    };
    const std::u32string alternating_single_contained
    {
        0x20, 0x00004e00, 0x20, 0x00004e00,
        0x20, 0x00004e00, 0x20, 0x00004e00,
        0x20, 0x00004e00, 0x20, 0x00004e00,
        0x20, 0x00004e00, 0x20, 0x00004e00,
        0x20, 0x00004e00, 0x20, 0x00004e00,
        0x20, 0x00004e00, 0x20, 0x00004e00,
        0x20
    };
    const auto utf8 = to_utf8(alternating_single_contained);
    BOOST_REQUIRE_EQUAL(to_compressed_cjk_form(utf8), to_utf8(expected));
}

BOOST_AUTO_TEST_SUITE_END()
