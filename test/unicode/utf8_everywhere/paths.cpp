/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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

// Use of L and U is not recommended as it will only work for ASCII when
// the source file does not have a BOM (which we avoid for other reasons).
// We use it below to simplify creation of ASCII test vectors.

BOOST_AUTO_TEST_SUITE(paths_tests)

BOOST_AUTO_TEST_CASE(paths__cast_to_string__always__expected)
{
    // ASCII
    const std::u8string ascii_input{ u8"test.txt" };
    auto result = cast_to_string(ascii_input);
    BOOST_REQUIRE_EQUAL(result, "test.txt");

    // Unicode
    const std::u8string unicode_input{ u8"Unicode 文件名.txt" };
    result = cast_to_string(unicode_input);
    BOOST_REQUIRE_EQUAL(result, std::string(reinterpret_cast<const char*>(unicode_input.c_str())));

    // Empty string
    const std::u8string empty_input{ u8"" };
    result = cast_to_string(empty_input);
    BOOST_REQUIRE_EQUAL(result, "");

    // Long path
    const std::u8string long_path{ u8"\\\\?\\C:\\very\\long\\path\\exceeding\\MAX_PATH\\Unicode 文件名.txt" };
    result = cast_to_string(long_path);
    BOOST_REQUIRE_EQUAL(result, std::string(reinterpret_cast<const char*>(long_path.c_str())));
}

BOOST_AUTO_TEST_CASE(paths__cast_to_u8string__always__expected)
{
    // ASCII
    const std::string ascii_input{ "test.txt" };
    std::u8string result = cast_to_u8string(ascii_input);
    BOOST_REQUIRE(result == u8"test.txt");

    // Unicode
    const std::string unicode_input{ reinterpret_cast<const char*>(u8"Unicode 文件名.txt") };
    result = cast_to_u8string(unicode_input);
    BOOST_REQUIRE(result == u8"Unicode 文件名.txt");

    // Empty string
    const std::string empty_input{ "" };
    result = cast_to_u8string(empty_input);
    BOOST_REQUIRE(result == u8"");

    // Long path
    const std::string long_path{ reinterpret_cast<const char*>(u8"\\\\?\\C:\\very\\long\\path\\exceeding\\MAX_PATH\\Unicode 文件名.txt") };
    result = cast_to_u8string(long_path);
    BOOST_REQUIRE(result == u8"\\\\?\\C:\\very\\long\\path\\exceeding\\MAX_PATH\\Unicode 文件名.txt");
}

BOOST_AUTO_TEST_CASE(paths__from_path__always__expected)
{
    // ASCII
    const std::filesystem::path ascii_path{ u8"test.txt" };
    std::string result = from_path(ascii_path);
    BOOST_REQUIRE_EQUAL(result, "test.txt");

    // Unicode
    const std::filesystem::path unicode_path{ u8"Unicode 文件名.txt" };
    result = from_path(unicode_path);
    BOOST_REQUIRE_EQUAL(result, std::string(reinterpret_cast<const char*>(u8"Unicode 文件名.txt")));

    // Empty path
    const std::filesystem::path empty_path{};
    result = from_path(empty_path);
    BOOST_REQUIRE_EQUAL(result, "");

    // Long path
    const std::filesystem::path long_path{ u8"\\\\?\\C:\\very\\long\\path\\exceeding\\MAX_PATH\\Unicode 文件名.txt" };
    result = from_path(long_path);
    BOOST_REQUIRE_EQUAL(result, std::string(reinterpret_cast<const char*>(u8"\\\\?\\C:\\very\\long\\path\\exceeding\\MAX_PATH\\Unicode 文件名.txt")));
}

BOOST_AUTO_TEST_CASE(paths__to_path__always__expected)
{
    // ASCII
    const std::string ascii_input{ "test.txt" };
    std::filesystem::path result = to_path(ascii_input);
    BOOST_REQUIRE(result.u8string() == u8"test.txt");

    // Unicode
    const std::string unicode_input{ reinterpret_cast<const char*>(u8"Unicode 文件名.txt") };
    result = to_path(unicode_input);
    BOOST_REQUIRE(result.u8string() == u8"Unicode 文件名.txt");

    // Empty string
    const std::string empty_input{ "" };
    result = to_path(empty_input);
    BOOST_REQUIRE(result.u8string() == u8"");

    // Long path
    const std::string long_input{ reinterpret_cast<const char*>(u8"\\\\?\\C:\\very\\long\\path\\exceeding\\MAX_PATH\\Unicode 文件名.txt") };
    result = to_path(long_input);
    BOOST_REQUIRE(result.u8string() == u8"\\\\?\\C:\\very\\long\\path\\exceeding\\MAX_PATH\\Unicode 文件名.txt");

    // Verify UTF-16 internal representation
    const std::wstring wide_expected{ L"\\\\?\\C:\\very\\long\\path\\exceeding\\MAX_PATH\\Unicode \xe6\x96\x87\xe4\xbb\xb6\xe5\x90\x8d.txt" };
    BOOST_REQUIRE(result.wstring() == wide_expected);
}

BOOST_AUTO_TEST_SUITE_END()
