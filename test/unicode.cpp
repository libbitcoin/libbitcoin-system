/*
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <string>
#include <vector>
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(unicode_tests)

// Use of L is not recommented as it will only work for ascii.
BOOST_AUTO_TEST_CASE(unicode__to_utf8__ascii__test)
{
    const auto utf8_ascii = "ascii";
    const auto utf16_ascii = L"ascii";
    const auto converted = to_utf8(utf16_ascii);
    BOOST_REQUIRE_EQUAL(converted, utf8_ascii);
}

// Use of L is not recommented as it will only work for ascii.
BOOST_AUTO_TEST_CASE(unicode__to_utf16__ascii__test)
{
    const auto utf8_ascii = "ascii";
    const auto utf16_ascii = L"ascii";
    const auto converted = to_utf16(utf8_ascii);
    BOOST_REQUIRE_EQUAL(converted.c_str(), utf16_ascii);
}

BOOST_AUTO_TEST_CASE(unicode__round_trip__ascii__test)
{
    const auto utf8_ascii = "ascii";
    const auto narrowed = to_utf8(to_utf16(utf8_ascii));
    BOOST_REQUIRE_EQUAL(narrowed, utf8_ascii);
}

BOOST_AUTO_TEST_CASE(unicode__round_trip__utf8__test)
{
    const auto utf8 = "テスト";
    const auto narrowed = to_utf8(to_utf16(utf8));
    BOOST_REQUIRE_EQUAL(narrowed, utf8);
}

BOOST_AUTO_TEST_CASE(unicode__utf16__literal__test)
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

BOOST_AUTO_TEST_CASE(unicode__to_utf8_main__ascii__test)
{
    std::vector<const wchar_t*> wide_args = { L"ascii", nullptr };

    auto argv = const_cast<wchar_t**>(&wide_args[0]);
    auto argc = static_cast<int>(wide_args.size()) - 1;

    auto buffer = to_utf8(argc, argv);
    auto narrow_args = reinterpret_cast<char**>(&buffer[0]);

    BOOST_REQUIRE_EQUAL(narrow_args[0], "ascii");
}

BOOST_AUTO_TEST_CASE(unicode__to_utf8_main__utf16__test)
{
    // We cannot use L for literal encoding of non-ascii text on Windows.
    auto utf16 = to_utf16("テスト");
    auto non_literal_utf16 = utf16.c_str();
    std::vector<const wchar_t*> wide_args = { L"ascii", non_literal_utf16, nullptr };

    auto argv = const_cast<wchar_t**>(&wide_args[0]);
    auto argc = static_cast<int>(wide_args.size()) - 1;

    auto buffer = to_utf8(argc, argv);
    auto narrow_args = reinterpret_cast<char**>(&buffer[0]);

    BOOST_REQUIRE_EQUAL(narrow_args[0], "ascii");
    BOOST_REQUIRE_EQUAL(narrow_args[1], "テスト");
}

BOOST_AUTO_TEST_CASE(unicode__to_utf8_main__null_termination__test)
{
    std::vector<const wchar_t*> wide_args = { L"ascii", nullptr };

    auto argv = const_cast<wchar_t**>(&wide_args[0]);
    auto argc = static_cast<int>(wide_args.size()) - 1;

    auto buffer = to_utf8(argc, argv);
    auto narrow_args = reinterpret_cast<char**>(&buffer[0]);

    // Each argument is a null terminated string.
    const auto length = strlen(narrow_args[0]);
    auto arg_terminator = narrow_args[0][length];
    BOOST_REQUIRE_EQUAL(arg_terminator, '\0');

    // The argument vector is a null terminated array.
    auto argv_terminator = narrow_args[argc];
    BOOST_REQUIRE_EQUAL(argv_terminator, static_cast<char*>(nullptr));
}

BOOST_AUTO_TEST_SUITE_END()
