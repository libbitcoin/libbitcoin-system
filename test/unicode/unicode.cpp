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

#include <cstring>
#include <stdexcept>
#include <vector>
#include <boost/test/unit_test.hpp>
#include <bitcoin/system.hpp>

using namespace bc::system;

BOOST_AUTO_TEST_SUITE(unicode_tests)

#ifdef WITH_ICU

// to_lower

BOOST_AUTO_TEST_CASE(unicode__to_lower__empty__empty)
{
    BOOST_REQUIRE(to_lower("").empty());
}

BOOST_AUTO_TEST_CASE(unicode__to_lower__lower_ascii__unchanged)
{
    const std::string lower = "abcdefghijklmnopqrstuvwxyz";
    const std::string expected = "abcdefghijklmnopqrstuvwxyz";
    BOOST_REQUIRE_EQUAL(to_lower(lower), expected);
}

BOOST_AUTO_TEST_CASE(unicode__to_lower__upper_ascii__lowered)
{
    const std::string upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const std::string expected = "abcdefghijklmnopqrstuvwxyz";
    BOOST_REQUIRE_EQUAL(to_lower(upper), expected);
}

BOOST_AUTO_TEST_CASE(unicode__to_lower__mixed_ascii__lowered)
{
    const std::string upper = "AbCdEfGhIjKlMnOpQrStUvWxYz";
    const std::string expected = "abcdefghijklmnopqrstuvwxyz";
    BOOST_REQUIRE_EQUAL(to_lower(upper), expected);
}

// This fails on Windows, may be an invalid case, kinda complex/obscure.
////BOOST_AUTO_TEST_CASE(unicode__to_lower__denormalized_upper__lowered)
////{
////    // unicode.org/reports/tr21/tr21-5.html
////    const std::string upper = "J◌̌◌";
////    const std::string expected = "ǰ◌̱";
////    BOOST_REQUIRE_EQUAL(to_lower(upper), expected);
////}

// to_normal_nfc_form

BOOST_AUTO_TEST_CASE(unicode__to_normal_nfc_form__empty__empty)
{
    BOOST_REQUIRE(to_normal_nfc_form("").empty());
}

// github.com/bitcoin/bips/blob/master/bip-0038.mediawiki
BOOST_AUTO_TEST_CASE(unicode__to_normal_nfc_form__bip38__expected)
{
    data_chunk original;
    BOOST_REQUIRE(decode_base16(original, "cf92cc8100f0909080f09f92a9"));
    std::string original_string(original.begin(), original.end());

    data_chunk normal;
    BOOST_REQUIRE(decode_base16(normal, "cf9300f0909080f09f92a9"));
    std::string expected_normal_string(normal.begin(), normal.end());

    const auto derived_normal_string = to_normal_nfc_form(original_string);
    BOOST_REQUIRE_EQUAL(expected_normal_string, derived_normal_string);
}

// to_normal_nfkd_form

BOOST_AUTO_TEST_CASE(unicode__to_normal_nfkd_form__ideographic_space_sandwich__expected)
{
    const auto ascii_space_sandwich = "space-> <-space";
    const auto ideographic_space_sandwich = "space->　<-space";
    const auto normalized = to_normal_nfkd_form(ideographic_space_sandwich);
    BOOST_REQUIRE_EQUAL(normalized.c_str(), ascii_space_sandwich);
}

BOOST_AUTO_TEST_CASE(unicode__to_normal_nfkd_form__61cc81__c3a1)
{
    const std::string literal_c3a1 = "á";

    // This is pasted from the the BIP39 encoding:
    //// github.com/bitcoin/bips/blob/master/bip-0039/spanish.txt#L93
    const std::string literal_61cc81 = "á";

    const std::string string_c3a1 = "\xc3\xa1";
    const std::string string_61cc81 = "\x61\xcc\x81";
    BOOST_REQUIRE_EQUAL(string_c3a1, literal_c3a1);
    BOOST_REQUIRE_EQUAL(string_61cc81, literal_61cc81);

    const auto normalized_c3a1 = to_normal_nfkd_form(string_c3a1);
    const auto normalized_61cc81 = to_normal_nfkd_form(string_61cc81);
    BOOST_REQUIRE_EQUAL(normalized_61cc81.c_str(), string_61cc81);
    BOOST_REQUIRE_EQUAL(normalized_c3a1.c_str(), string_61cc81);
}

// to_unaccented_form

BOOST_AUTO_TEST_CASE(unicode__to_unaccented_form__empty__empty)
{
    BOOST_REQUIRE(to_unaccented_form("").empty());
}

BOOST_AUTO_TEST_CASE(unicode__to_unaccented_form__ascii_only__unchanged)
{
    const std::u32string ascii{ 0x42, 0x43 };
    const std::string expected{ 0x42, 0x43 };
    BOOST_REQUIRE_EQUAL(to_unaccented_form(to_utf8(ascii)), expected);
}

BOOST_AUTO_TEST_CASE(unicode__to_unaccented_form__circumflex_accent_only__empty)
{
    const std::u32string circumflex_accent{ 0x0000005e };
    BOOST_REQUIRE(to_unaccented_form(to_utf8(circumflex_accent)).empty());
}

BOOST_AUTO_TEST_CASE(unicode__to_unaccented_form__bassa_vah_tones_only__empty)
{
    const std::u32string bassa_vah{ 0x00016af1, 0x00016af2, 0x00016af3 };
    BOOST_REQUIRE(to_unaccented_form(to_utf8(bassa_vah)).empty());
}

BOOST_AUTO_TEST_CASE(unicode__to_unaccented_form__bassa_vah_tones_ascii_sandwich__stripped)
{
    const std::string expected{ 0x42, 0x43 };
    const std::u32string bassa_vah{ 0x42, 0x00016af1, 0x00016af2, 0x00016af3, 0x43 };
    BOOST_REQUIRE_EQUAL(to_unaccented_form(to_utf8(bassa_vah)), expected);
}

BOOST_AUTO_TEST_CASE(unicode__to_unaccented_form__circumflex_accent_and_bassa_vah_tones_and_ascii_mix__stripped)
{
    const std::string expected{ 0x42, 0x43, 0x44 };
    const std::u32string bassa_vah{ 0x00016af1, 0x42, 0x43, 0x0000005e, 0x00016af2, 0x44, 0x00016af3 };
    BOOST_REQUIRE_EQUAL(to_unaccented_form(to_utf8(bassa_vah)), expected);
}

// to_compressed_cjk_form

BOOST_AUTO_TEST_CASE(unicode__to_compressed_cjk_form__empty__empty)
{
    BOOST_REQUIRE(to_compressed_cjk_form("").empty());
}

BOOST_AUTO_TEST_CASE(unicode__to_compressed_cjk_form__ascii_with_spaces__unchanged)
{
    const auto value = " foo  bar  baz ";
    const auto result = to_compressed_cjk_form(value);
    BOOST_REQUIRE_EQUAL(result.c_str(), value);
}

BOOST_AUTO_TEST_CASE(unicode__to_compressed_cjk__cjk_unified_ideograph_only__unchanged)
{
    const std::u32string cjk_unified_ideograph{ 0x00004e00 };
    const auto utf8 = to_utf8(cjk_unified_ideograph);
    BOOST_REQUIRE_EQUAL(to_compressed_cjk_form(utf8), utf8);
}

BOOST_AUTO_TEST_CASE(unicode__to_compressed_cjk__cjk_unified_ideograph_sandwich__stripped)
{
    const std::u32string expected{ 0x00004e00, 0x00004e00 };
    const std::u32string cjk_unified_ideograph_sandwich{ 0x00004e00, 0x20, 0x00004e00 };
    const auto utf8 = to_utf8(cjk_unified_ideograph_sandwich);
    BOOST_REQUIRE_EQUAL(to_compressed_cjk_form(utf8), to_utf8(expected));
}

BOOST_AUTO_TEST_CASE(unicode__to_compressed_cjk__cjk_unified_ideograph_right__unchanged)
{
    const std::u32string expected{ 0x20, 0x00004e00, 0x00004e00 };
    const std::u32string cjk_unified_ideograph_right{ 0x20, 0x00004e00, 0x00004e00 };
    const auto utf8 = to_utf8(cjk_unified_ideograph_right);
    BOOST_REQUIRE_EQUAL(to_compressed_cjk_form(utf8), to_utf8(expected));
}

BOOST_AUTO_TEST_CASE(unicode__to_compressed_cjk__cjk_unified_ideograph_left__unchanged)
{
    const std::u32string expected{ 0x00004e00, 0x00004e00, 0x20 };
    const std::u32string cjk_unified_ideograph_left{ 0x00004e00, 0x00004e00, 0x20 };
    const auto utf8 = to_utf8(cjk_unified_ideograph_left);
    BOOST_REQUIRE_EQUAL(to_compressed_cjk_form(utf8), to_utf8(expected));
}

BOOST_AUTO_TEST_CASE(unicode__to_compressed_cjk__cjk_unified_ideograph_both__unchanged)
{
    const std::u32string expected{ 0x20, 0x00004e00, 0x00004e00, 0x20 };
    const std::u32string cjk_unified_ideograph_both{ 0x20, 0x00004e00, 0x00004e00, 0x20 };
    const auto utf8 = to_utf8(cjk_unified_ideograph_both);
    BOOST_REQUIRE_EQUAL(to_compressed_cjk_form(utf8), to_utf8(expected));
}

BOOST_AUTO_TEST_CASE(unicode__to_compressed_cjk__cjk_unified_ideograph_all__middle_stripped)
{
    const std::u32string expected{ 0x20, 0x00004e00, 0x00004e00, 0x20 };
    const std::u32string cjk_unified_ideograph_all{ 0x20, 0x00004e00, 0x0c, 0x00004e00, 0x20 };
    const auto utf8 = to_utf8(cjk_unified_ideograph_all);
    BOOST_REQUIRE_EQUAL(to_compressed_cjk_form(utf8), to_utf8(expected));
}

BOOST_AUTO_TEST_CASE(unicode__to_compressed_cjk__single_and_double_contained_whitespace__single_stripped)
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

BOOST_AUTO_TEST_CASE(unicode__to_compressed_cjk__alternating_single_contained_whitespace__contained_stripped)
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

#endif

// to_utf8 (u32string)

// Use of U is not recommended as it will only work for ascii when
// the source file does not have a BOM (which we avoid for other reasons).
BOOST_AUTO_TEST_CASE(unicode__to_utf8_32__empty__empty)
{
    BOOST_REQUIRE(to_utf8(U"").empty());
}

// Use of U is not recommended as it will only work for ascii when
// the source file does not have a BOM (which we avoid for other reasons).
BOOST_AUTO_TEST_CASE(unicode__to_utf8_32__ascii__expected)
{
    const auto utf8_ascii = "ascii";
    const auto utf32_ascii = U"ascii";
    const auto converted = to_utf8(utf32_ascii);
    BOOST_REQUIRE_EQUAL(converted, utf8_ascii);
}

// to_utf8 (wstring)

// Use of L is not recommended as it will only work for ascii when
// the source file does not have a BOM (which we avoid for other reasons).
BOOST_AUTO_TEST_CASE(unicode__to_utf8_16__empty__empty)
{
    BOOST_REQUIRE(to_utf8(L"").empty());
}

// Use of L is not recommended as it will only work for ascii when
// the source file does not have a BOM (which we avoid for other reasons).
BOOST_AUTO_TEST_CASE(unicode__to_utf8_16__ascii__expected)
{
    const auto utf8_ascii = "ascii";
    const auto utf16_ascii = L"ascii";
    const auto converted = to_utf8(utf16_ascii);
    BOOST_REQUIRE_EQUAL(converted, utf8_ascii);
}

// to_utf16 (string)

BOOST_AUTO_TEST_CASE(unicode__to_utf16__empty__empty)
{
    BOOST_REQUIRE(to_utf16("").empty());
}

// Use of L is not recommended as it will only work for ascii when
// the source file does not have a BOM (which we avoid for other reasons).
BOOST_AUTO_TEST_CASE(unicode__to_utf16__ascii__expected)
{
    const auto utf8_ascii = "ascii";
    const auto utf16_ascii = L"ascii";
    const auto converted = to_utf16(utf8_ascii);
    BOOST_REQUIRE_EQUAL(converted.c_str(), utf16_ascii);
}

// to_utf32 (string)

BOOST_AUTO_TEST_CASE(unicode__to_utf32__empty__empty)
{
    BOOST_REQUIRE(to_utf32("").empty());
}

// Use of U is not recommended as it will only work for ascii when
// the source file does not have a BOM (which we avoid for other reasons).
BOOST_AUTO_TEST_CASE(unicode__to_utf32__ascii__expected)
{
    const auto utf8_ascii = "ascii";
    const auto utf32_ascii = U"ascii";
    const auto converted = to_utf8(utf32_ascii);
    BOOST_REQUIRE_EQUAL(converted, utf8_ascii);
}

// to_utf16 (string) / to_utf8 (wstring)

BOOST_AUTO_TEST_CASE(unicode__to_utf16__to_utf8_ascii_round_trip__unchanged)
{
    const auto utf8_ascii = "ascii";
    const auto converted = to_utf8(to_utf16(utf8_ascii));
    BOOST_REQUIRE_EQUAL(converted, utf8_ascii);
}

BOOST_AUTO_TEST_CASE(unicode__to_utf16__to_utf8_japanese_round_trip__unchanged)
{
    const auto utf8 = "テスト";
    const auto converted = to_utf8(to_utf16(utf8));
    BOOST_REQUIRE_EQUAL(converted, utf8);
}

// Use of L is not recommended as it will only work for ascii when
// the source file does not have a BOM (which we avoid for other reasons).
BOOST_AUTO_TEST_CASE(unicode__to_utf16__round_trip__narraow_and_wide_japanese_literals__expected)
{
    const auto utf8 = "テスト";
    const auto utf16 = L"テスト";

    const auto widened = to_utf16(utf8);
    const auto narrowed = to_utf8(utf16);

#ifdef _MSC_VER
    // This confirms that the L prefix does not work with non-ascii text when
    // the source file does not have a BOM (which we avoid for other reasons).
    BOOST_REQUIRE_NE(widened.c_str(), utf16);
    BOOST_REQUIRE_NE(narrowed, utf8);
#else
    BOOST_REQUIRE_EQUAL(widened.c_str(), utf16);
    BOOST_REQUIRE_EQUAL(narrowed, utf8);
#endif
}

// to_utf8 (wchar_t[])

BOOST_AUTO_TEST_CASE(unicode__to_utf8_array__null_out__zero)
{
    const wchar_t* in = L"";
    BOOST_REQUIRE_EQUAL(to_utf8(nullptr, 42, in, 42), 0u);
}

BOOST_AUTO_TEST_CASE(unicode__to_utf8_array__null_in__zero)
{
    char* out = "";
    BOOST_REQUIRE_EQUAL(to_utf8(out, 42, nullptr, 42), 0u);
}

BOOST_AUTO_TEST_CASE(unicode__to_utf8_array__zero_in__zero)
{
    char* out = "";
    const wchar_t* in = L"";
    BOOST_REQUIRE_EQUAL(to_utf8(out, 42, in, 0), 0u);
}

BOOST_AUTO_TEST_CASE(unicode__to_utf8_array__zero_out__zero)
{
    char* out = "";
    const wchar_t* in = L"";
    BOOST_REQUIRE_EQUAL(to_utf8(out, 0, in, 42), 0u);
}

BOOST_AUTO_TEST_CASE(unicode__to_utf8_array__ascii__test)
{
    char utf8[20];

    // Text buffer provides null termination for test comparison.
    memset(utf8, 0, sizeof(utf8) / sizeof(char));

    // Use of L is not recommended as it will only work for ascii.
    const std::wstring utf16(L"ascii");
    const std::string expected_utf8("ascii");

    const auto size = to_utf8(utf8, sizeof(utf8), utf16.c_str(), (int)utf16.size());
    BOOST_REQUIRE_EQUAL(utf8, expected_utf8);
    BOOST_REQUIRE_EQUAL(size, expected_utf8.size());
}

BOOST_AUTO_TEST_CASE(unicode__to_utf8_array__non_ascii__test)
{
    char utf8[36];

    // Text buffer provides null termination for test comparison.
    memset(utf8, 0, sizeof(utf8) / sizeof(char));

    const std::string expected_utf8("テスト");
    const auto utf16 = to_utf16(expected_utf8);

    const auto size = to_utf8(utf8, sizeof(utf8), utf16.c_str(), (int)utf16.size());

    BOOST_REQUIRE_EQUAL(utf8, expected_utf8);
    BOOST_REQUIRE_EQUAL(size, expected_utf8.size());
}

// to_utf16 (char[])

BOOST_AUTO_TEST_CASE(unicode__to_utf16_array__null_truncated__zero)
{
    uint8_t truncated;
    const char* in = "";
    BOOST_REQUIRE_EQUAL(to_utf16(truncated, nullptr, 42, in, 42), 0u);
}

BOOST_AUTO_TEST_CASE(unicode__to_utf16_array__null_out__zero)
{
    uint8_t truncated;
    const char* in = "";
    BOOST_REQUIRE_EQUAL(to_utf16(truncated, nullptr, 42, in, 42), 0u);
}

BOOST_AUTO_TEST_CASE(unicode__to_utf16_array__null_in__zero)
{
    uint8_t truncated;
    wchar_t* out = L"";
    BOOST_REQUIRE_EQUAL(to_utf16(truncated, out, 42, nullptr, 42), 0u);
}

BOOST_AUTO_TEST_CASE(unicode__to_utf16_array__zero_in__zero)
{
    uint8_t truncated;
    wchar_t* out = L"";
    const char* in = "";
    BOOST_REQUIRE_EQUAL(to_utf16(truncated, out, 42, in, 0), 0u);
}

BOOST_AUTO_TEST_CASE(unicode__to_utf16_array__zero_out__zero)
{
    uint8_t truncated;
    wchar_t* out = L"";
    const char* in = "";
    BOOST_REQUIRE_EQUAL(to_utf16(truncated, out, 0, in, 42), 0u);
}

BOOST_AUTO_TEST_CASE(unicode__to_utf16_array__ascii__expected)
{
    wchar_t utf16[20];

    // Text buffer provides null termination for test comparison.
    wmemset(utf16, 0, sizeof(utf16) / sizeof(wchar_t));

    // Use of L is not recommended as it will only work for ascii.
    const std::wstring expected_utf16(L"ascii");
    const std::string utf8("ascii");

    uint8_t truncated;
    const auto size = to_utf16(truncated, utf16, sizeof(utf16), utf8.c_str(), (int)utf8.size());

    BOOST_REQUIRE_EQUAL(utf16, expected_utf16.c_str());
    BOOST_REQUIRE_EQUAL(size, expected_utf16.size());
    BOOST_REQUIRE_EQUAL(truncated, 0u);
}

BOOST_AUTO_TEST_CASE(unicode__to_utf16_array__non_ascii__expected)
{
    wchar_t utf16[36];

    // Text buffer provides null termination for test comparison.
    wmemset(utf16, 0, sizeof(utf16) / sizeof(wchar_t));

    const std::string utf8("テスト");
    const auto expected_utf16 = to_utf16(utf8);

    uint8_t truncated;
    const auto size = to_utf16(truncated, utf16, sizeof(utf16), utf8.c_str(), (int)utf8.size());

    BOOST_REQUIRE_EQUAL(utf16, expected_utf16.c_str());
    BOOST_REQUIRE_EQUAL(size, expected_utf16.size());
    BOOST_REQUIRE_EQUAL(truncated, 0u);
}

BOOST_AUTO_TEST_CASE(unicode__to_utf16_array__non_ascii_truncation1__expected)
{
    wchar_t utf16[36];

    // Text buffer provides null termination for test comparison.
    wmemset(utf16, 0, sizeof(utf16) / sizeof(wchar_t));

    std::string utf8("テスト");
    auto expected_utf16 = to_utf16(utf8);

    // Lop off last byte, which will split the last character.
    const auto drop_bytes = 1u;
    utf8.resize(utf8.size() - drop_bytes);

    // Expect the loss of the last wide character.
    expected_utf16.resize(expected_utf16.size() - 1u);

    // Expect the truncation of the remaining bytes of the last character.
    const auto expected_truncated = strlen("ト") - 1u;

    uint8_t truncated;
    const auto size = to_utf16(truncated, utf16, sizeof(utf16), utf8.c_str(), (int)utf8.size());

    BOOST_REQUIRE_EQUAL(truncated, expected_truncated);
    BOOST_REQUIRE_EQUAL(utf16, expected_utf16.c_str());
    BOOST_REQUIRE_EQUAL(size, expected_utf16.size());
}

BOOST_AUTO_TEST_CASE(unicode__to_utf16_array__non_ascii_truncation2__expected)
{
    wchar_t utf16[36];

    // Text buffer provides null termination for test comparison.
    wmemset(utf16, 0, sizeof(utf16) / sizeof(wchar_t));

    std::string utf8("テスト");
    auto expected_utf16 = to_utf16(utf8);

    // Lop off last two bytes, which will split the last character.
    const auto drop_bytes = 2u;
    utf8.resize(utf8.size() - drop_bytes);

    // Expect the loss of the last wide character.
    expected_utf16.resize(expected_utf16.size() - 1u);

    // Expect the truncation of the remaining bytes of the last character.
    const auto expected_truncated = strlen("ト") - drop_bytes;

    uint8_t truncated;
    const auto size = to_utf16(truncated, utf16, sizeof(utf16), utf8.c_str(), (int)utf8.size());

    BOOST_REQUIRE_EQUAL(truncated, expected_truncated);
    BOOST_REQUIRE_EQUAL(utf16, expected_utf16.c_str());
    BOOST_REQUIRE_EQUAL(size, expected_utf16.size());
}

// allocate_environment (vars)

BOOST_AUTO_TEST_CASE(unicode__allocate_environment_vars__ascii__expected)
{
    std::vector<const wchar_t*> wide_environment = { L"ascii", nullptr };
    auto variables = const_cast<wchar_t**>(&wide_environment[0]);
    auto narrow_environment = allocate_environment(variables);
    BOOST_REQUIRE_EQUAL(narrow_environment[0], "ascii");
    free_environment(narrow_environment);
}

BOOST_AUTO_TEST_CASE(unicode__allocate_environment_vars__utf16__expected)
{
    // We cannot use L for literal encoding of non-ascii text on Windows.
    auto utf16 = to_utf16("テスト");
    auto non_literal_utf16 = utf16.c_str();
    std::vector<const wchar_t*> wide_environment = { L"ascii", non_literal_utf16, nullptr };
    auto variables = const_cast<wchar_t**>(&wide_environment[0]);
    auto narrow_environment = allocate_environment(variables);
    BOOST_REQUIRE_EQUAL(narrow_environment[0], "ascii");
    BOOST_REQUIRE_EQUAL(narrow_environment[1], "テスト");
    free_environment(narrow_environment);
}

BOOST_AUTO_TEST_CASE(unicode__allocate_environment_vars__null_termination__expected)
{
    std::vector<const wchar_t*> wide_environment = { L"ascii", nullptr };
    auto variables = const_cast<wchar_t**>(&wide_environment[0]);
    auto expected_count = static_cast<int>(wide_environment.size()) - 1u;
    auto narrow_environment = allocate_environment(variables);

    // Each argument is a null terminated string.
    const auto length = strlen(narrow_environment[0]);
    auto variable_terminator = narrow_environment[0][length];
    BOOST_REQUIRE_EQUAL(variable_terminator, '\0');

    // The argument vector is a null terminated array.
    auto environment_terminator = narrow_environment[expected_count];
    BOOST_REQUIRE_EQUAL(environment_terminator, (char*)nullptr);
    free_environment(narrow_environment);
}

// allocate_environment (args)

BOOST_AUTO_TEST_CASE(unicode__allocate_environment_args__ascii__expected)
{
    std::vector<const wchar_t*> wide_args = { L"ascii", nullptr };
    auto argv = const_cast<wchar_t**>(&wide_args[0]);
    auto argc = static_cast<int>(wide_args.size()) - 1u;
    auto narrow_args = allocate_environment(argc, argv);
    BOOST_REQUIRE_EQUAL(narrow_args[0], "ascii");
    free_environment(narrow_args);
}

BOOST_AUTO_TEST_CASE(unicode__allocate_environment_args__utf16__expected)
{
    // We cannot use L for literal encoding of non-ascii text on Windows.
    auto utf16 = to_utf16("テスト");
    auto non_literal_utf16 = utf16.c_str();
    std::vector<const wchar_t*> wide_args = { L"ascii", non_literal_utf16, nullptr };
    auto argv = const_cast<wchar_t**>(&wide_args[0]);
    auto argc = static_cast<int>(wide_args.size()) - 1u;
    auto narrow_args = allocate_environment(argc, argv);
    BOOST_REQUIRE_EQUAL(narrow_args[0], "ascii");
    BOOST_REQUIRE_EQUAL(narrow_args[1], "テスト");
    free_environment(narrow_args);
}

BOOST_AUTO_TEST_CASE(unicode__allocate_environment_args__null_termination__expected)
{
    std::vector<const wchar_t*> wide_args = { L"ascii", nullptr };
    auto argv = const_cast<wchar_t**>(&wide_args[0]);
    auto argc = static_cast<int>(wide_args.size()) - 1u;
    auto narrow_args = allocate_environment(argc, argv);

    // Each argument is a null terminated string.
    const auto length = strlen(narrow_args[0]);
    auto arg_terminator = narrow_args[0][length];
    BOOST_REQUIRE_EQUAL(arg_terminator, '\0');

    // The argument vector is a null terminated array.
    auto argv_terminator = narrow_args[argc];
    BOOST_REQUIRE_EQUAL(argv_terminator, (char*)nullptr);
    free_environment(narrow_args);
}

BOOST_AUTO_TEST_SUITE_END()
