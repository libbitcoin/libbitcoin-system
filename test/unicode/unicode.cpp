/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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

// github.com/bitcoin/bips/blob/master/bip-0038.mediawiki
BOOST_AUTO_TEST_CASE(unicode__to_normal_nfc_form__validate__test)
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

BOOST_AUTO_TEST_CASE(unicode__to_normal_nfkd_form__validate__test)
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

#ifdef _MSC_VER
    BOOST_REQUIRE_EQUAL(normalized_61cc81.c_str(), string_c3a1);
    BOOST_REQUIRE_EQUAL(normalized_c3a1.c_str(), string_c3a1);
#else
    BOOST_REQUIRE_EQUAL(normalized_61cc81.c_str(), string_61cc81);
    BOOST_REQUIRE_EQUAL(normalized_c3a1.c_str(), string_61cc81);
#endif
}

#endif

// Use of L is not recommended as it will only work for ascii.
BOOST_AUTO_TEST_CASE(unicode__to_utf8_string__ascii__test)
{
    const auto utf8_ascii = "ascii";
    const auto utf16_ascii = L"ascii";
    const auto converted = to_utf8(utf16_ascii);
    BOOST_REQUIRE_EQUAL(converted, utf8_ascii);
}

// Use of L is not recommended as it will only work for ascii.
BOOST_AUTO_TEST_CASE(unicode__to_utf16_string__ascii__test)
{
    const auto utf8_ascii = "ascii";
    const auto utf16_ascii = L"ascii";
    const auto converted = to_utf16(utf8_ascii);
    BOOST_REQUIRE_EQUAL(converted.c_str(), utf16_ascii);
}

BOOST_AUTO_TEST_CASE(unicode__string_round_trip__ascii__test)
{
    const auto utf8_ascii = "ascii";
    const auto narrowed = to_utf8(to_utf16(utf8_ascii));
    BOOST_REQUIRE_EQUAL(narrowed, utf8_ascii);
}

BOOST_AUTO_TEST_CASE(unicode__string_round_trip__utf8__test)
{
    const auto utf8 = "テスト";
    const auto narrowed = to_utf8(to_utf16(utf8));
    BOOST_REQUIRE_EQUAL(narrowed, utf8);
}

BOOST_AUTO_TEST_CASE(unicode__string_round_trip__wide_literal__test)
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

BOOST_AUTO_TEST_CASE(unicode__to_utf16_array__ascii__test)
{
    wchar_t utf16[20];

    // Text buffer provides null termination for test comparison.
    wmemset(utf16, 0, sizeof(utf16) / sizeof(wchar_t));

    // Use of L is not recommended as it will only work for ascii.
    const std::wstring expected_utf16(L"ascii");
    const std::string utf8("ascii");

    uint8_t truncated;
    const auto size = to_utf16(utf16, sizeof(utf16), utf8.c_str(), (int)utf8.size(), truncated);

    BOOST_REQUIRE_EQUAL(utf16, expected_utf16.c_str());
    BOOST_REQUIRE_EQUAL(size, expected_utf16.size());
    BOOST_REQUIRE_EQUAL(truncated, 0);
}

BOOST_AUTO_TEST_CASE(unicode__to_utf16_array__non_ascii__test)
{
    wchar_t utf16[36];

    // Text buffer provides null termination for test comparison.
    wmemset(utf16, 0, sizeof(utf16) / sizeof(wchar_t));

    const std::string utf8("テスト");
    const auto expected_utf16 = to_utf16(utf8);

    uint8_t truncated;
    const auto size = to_utf16(utf16, sizeof(utf16), utf8.c_str(), (int)utf8.size(), truncated);

    BOOST_REQUIRE_EQUAL(utf16, expected_utf16.c_str());
    BOOST_REQUIRE_EQUAL(size, expected_utf16.size());
    BOOST_REQUIRE_EQUAL(truncated, 0);
}

BOOST_AUTO_TEST_CASE(unicode__to_utf16_array__non_ascii_truncation1__test)
{
    wchar_t utf16[36];

    // Text buffer provides null termination for test comparison.
    wmemset(utf16, 0, sizeof(utf16) / sizeof(wchar_t));

    std::string utf8("テスト");
    auto expected_utf16 = to_utf16(utf8);

    // Lop off last byte, which will split the last character.
    const auto drop_bytes = 1;
    utf8.resize(utf8.size() - drop_bytes);

    // Expect the loss of the last wide character.
    expected_utf16.resize(expected_utf16.size() - 1);

    // Expect the truncation of the remaining bytes of the last character.
    const auto expected_truncated = strlen("ト") - 1;

    uint8_t truncated;
    const auto size = to_utf16(utf16, sizeof(utf16), utf8.c_str(), (int)utf8.size(), truncated);

    BOOST_REQUIRE_EQUAL(truncated, expected_truncated);
    BOOST_REQUIRE_EQUAL(utf16, expected_utf16.c_str());
    BOOST_REQUIRE_EQUAL(size, expected_utf16.size());
}

BOOST_AUTO_TEST_CASE(unicode__to_utf16_array__non_ascii_truncation2__test)
{
    wchar_t utf16[36];

    // Text buffer provides null termination for test comparison.
    wmemset(utf16, 0, sizeof(utf16) / sizeof(wchar_t));

    std::string utf8("テスト");
    auto expected_utf16 = to_utf16(utf8);

    // Lop off last two bytes, which will split the last character.
    const auto drop_bytes = 2;
    utf8.resize(utf8.size() - drop_bytes);

    // Expect the loss of the last wide character.
    expected_utf16.resize(expected_utf16.size() - 1);

    // Expect the truncation of the remaining bytes of the last character.
    const auto expected_truncated = strlen("ト") - drop_bytes;

    uint8_t truncated;
    const auto size = to_utf16(utf16, sizeof(utf16), utf8.c_str(), (int)utf8.size(), truncated);

    BOOST_REQUIRE_EQUAL(truncated, expected_truncated);
    BOOST_REQUIRE_EQUAL(utf16, expected_utf16.c_str());
    BOOST_REQUIRE_EQUAL(size, expected_utf16.size());
}

BOOST_AUTO_TEST_CASE(unicode__to_utf8_environment__ascii__test)
{
    std::vector<const wchar_t*> wide_environment = { L"ascii", nullptr };
    auto variables = const_cast<wchar_t**>(&wide_environment[0]);
    auto narrow_environment = allocate_environment(variables);
    BOOST_REQUIRE_EQUAL(narrow_environment[0], "ascii");
    free_environment(narrow_environment);
}

BOOST_AUTO_TEST_CASE(unicode__to_utf8_environment__utf16__test)
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

BOOST_AUTO_TEST_CASE(unicode__to_utf8_environment__null_termination__test)
{
    std::vector<const wchar_t*> wide_environment = { L"ascii", nullptr };
    auto variables = const_cast<wchar_t**>(&wide_environment[0]);
    auto expected_count = static_cast<int>(wide_environment.size()) - 1;
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

BOOST_AUTO_TEST_CASE(unicode__to_utf8_main__ascii__test)
{
    std::vector<const wchar_t*> wide_args = { L"ascii", nullptr };
    auto argv = const_cast<wchar_t**>(&wide_args[0]);
    auto argc = static_cast<int>(wide_args.size()) - 1;
    auto narrow_args = allocate_environment(argc, argv);
    BOOST_REQUIRE_EQUAL(narrow_args[0], "ascii");
    free_environment(narrow_args);
}

BOOST_AUTO_TEST_CASE(unicode__to_utf8_main__utf16__test)
{
    // We cannot use L for literal encoding of non-ascii text on Windows.
    auto utf16 = to_utf16("テスト");
    auto non_literal_utf16 = utf16.c_str();
    std::vector<const wchar_t*> wide_args = { L"ascii", non_literal_utf16, nullptr };
    auto argv = const_cast<wchar_t**>(&wide_args[0]);
    auto argc = static_cast<int>(wide_args.size()) - 1;
    auto narrow_args = allocate_environment(argc, argv);
    BOOST_REQUIRE_EQUAL(narrow_args[0], "ascii");
    BOOST_REQUIRE_EQUAL(narrow_args[1], "テスト");
    free_environment(narrow_args);
}

BOOST_AUTO_TEST_CASE(unicode__to_utf8_main__null_termination__test)
{
    std::vector<const wchar_t*> wide_args = { L"ascii", nullptr };
    auto argv = const_cast<wchar_t**>(&wide_args[0]);
    auto argc = static_cast<int>(wide_args.size()) - 1;
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
