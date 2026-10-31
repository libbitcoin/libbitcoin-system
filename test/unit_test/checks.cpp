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
#include "../test.hpp"

using namespace bc::system::unit_test;

BC_AUTO_TEST_SUITE(unit_test_checks_tests)

struct printable_scope
{
    int value;
};

static std::ostream& operator<<(std::ostream& stream,
    const printable_scope& value)
{
    stream << "printable " << value.value;
    return stream;
}

// printed

BC_AUTO_TEST_CASE(unit_test_checks__printed__fundamentals__expected)
{
    BC_REQUIRE_EQUAL(printed(true), "true");
    BC_REQUIRE_EQUAL(printed(false), "false");
    BC_REQUIRE_EQUAL(printed(nullptr), "nullptr");
    BC_REQUIRE_EQUAL(printed(42), "42");
    BC_REQUIRE_EQUAL(printed(-42), "-42");
    BC_REQUIRE_EQUAL(printed(42u), "42");
}

BC_AUTO_TEST_CASE(unit_test_checks__printed__characters__quoted_or_numeric)
{
    BC_REQUIRE_EQUAL(printed('a'), "'a'");
    BC_REQUIRE_EQUAL(printed('\n'), "10");
    BC_REQUIRE_EQUAL(printed(uint8_t{ 0x41 }), "'A'");
    BC_REQUIRE_EQUAL(printed(uint8_t{ 0xff }), "255");
}

BC_AUTO_TEST_CASE(unit_test_checks__printed__floating_point__round_trip_precision)
{
    BC_REQUIRE_EQUAL(printed(0.5), "0.5");
    BC_REQUIRE_EQUAL(std::stod(printed(0.1)), 0.1);
    BC_REQUIRE_EQUAL(std::stof(printed(0.1f)), 0.1f);
}

BC_AUTO_TEST_CASE(unit_test_checks__printed__strings__expected)
{
    const char* null{};
    const char* text = "text";
    BC_REQUIRE_EQUAL(printed(null), "null string");
    BC_REQUIRE_EQUAL(printed(text), "text");
    BC_REQUIRE_EQUAL(printed(std::string{ "text" }), "text");
}

BC_AUTO_TEST_CASE(unit_test_checks__printed__user_type__argument_dependent_operator)
{
    BC_REQUIRE_EQUAL(printed(printable_scope{ 7 }), "printable 7");
}

BC_AUTO_TEST_CASE(unit_test_checks__format__writer__written_text)
{
    const auto value = 42;
    BC_REQUIRE_EQUAL(format([&](std::ostream& stream) { stream << "value " << value; }), "value 42");
}

// describe

BC_AUTO_TEST_CASE(unit_test_checks__describe__levels__expected)
{
    BC_REQUIRE_EQUAL(describe(level::warn, "x"), "condition x is not satisfied");
    BC_REQUIRE_EQUAL(describe(level::check, "x"), "check x has failed");
    BC_REQUIRE_EQUAL(describe(level::require, "x"), "critical check x has failed");
}

// comparisons

BC_AUTO_TEST_CASE(unit_test_checks__equal__integrals__usual_conversions)
{
    BC_REQUIRE(equal::test(1, 1));
    BC_REQUIRE(!equal::test(1, 2));
    BC_REQUIRE(equal::test(size_t{ 3 }, 3));
    BC_REQUIRE(equal::test(-1, max_uint32));
}

BC_AUTO_TEST_CASE(unit_test_checks__equal__cstrings__by_content)
{
    const std::string left{ "text" };
    const std::string right{ "text" };
    const char* null{};
    BC_REQUIRE(equal::test(left.c_str(), right.c_str()));
    BC_REQUIRE(!equal::test(left.c_str(), "other"));
    BC_REQUIRE(!equal::test(left.c_str(), null));
    BC_REQUIRE(equal::test(null, null));
}

BC_AUTO_TEST_CASE(unit_test_checks__equal__wide_cstrings__by_content)
{
    const std::wstring left{ L"text" };
    const std::wstring right{ L"text" };
    BC_REQUIRE(equal::test(left.c_str(), right.c_str()));
    BC_REQUIRE(!equal::test(left.c_str(), L"other"));
}

BC_AUTO_TEST_CASE(unit_test_checks__equal__string_and_literal__by_content)
{
    BC_REQUIRE(equal::test(std::string{ "text" }, "text"));
    BC_REQUIRE(!equal::test(std::string{ "text" }, "other"));
}

BC_AUTO_TEST_CASE(unit_test_checks__orderings__expected)
{
    BC_REQUIRE(not_equal::test(1, 2));
    BC_REQUIRE(!not_equal::test(2, 2));
    BC_REQUIRE(less::test(1, 2));
    BC_REQUIRE(!less::test(2, 2));
    BC_REQUIRE(less_equal::test(2, 2));
    BC_REQUIRE(!less_equal::test(3, 2));
    BC_REQUIRE(greater::test(3, 2));
    BC_REQUIRE(!greater::test(2, 2));
    BC_REQUIRE(greater_equal::test(2, 2));
    BC_REQUIRE(!greater_equal::test(1, 2));
}

// assertions

BC_AUTO_TEST_CASE(unit_test_checks__assertions__holding__pass)
{
    const std::vector<int> left{ 1, 2, 3 };
    const std::vector<int> right{ 1, 2, 3 };
    BC_WARN(true);
    BC_CHECK(true);
    BC_CHECK_EQUAL(1, 1);
    BC_CHECK_NE(1, 2);
    BC_CHECK_LT(1, 2);
    BC_CHECK_MESSAGE(true, "unused " << 42);
    BC_WARN_EQUAL(1, 1);
    BC_WARN_MESSAGE(true, "unused");
    BC_REQUIRE_LE(1, 1);
    BC_REQUIRE_GE(1, 1);
    BC_REQUIRE_GT(2, 1);
    BC_REQUIRE_LT(1, 2);
    BC_REQUIRE_NE(1, 2);
    BC_REQUIRE_CLOSE(100.0, 100.0001, 0.001);
    BC_REQUIRE_CLOSE(0.0, 0.0, 0.001);
    BC_REQUIRE_EQUAL_COLLECTIONS(left.begin(), left.end(), right.begin(), right.end());
    BC_REQUIRE_MESSAGE(true, "unused");
}

BC_AUTO_TEST_CASE(unit_test_checks__exception_assertions__holding__pass)
{
    BC_REQUIRE_THROW(throw std::runtime_error("x"), std::runtime_error);
    BC_REQUIRE_THROW(throw std::runtime_error("x"), std::exception);
    BC_REQUIRE_NO_THROW(std::ignore = printed(42));
    BC_REQUIRE_EXCEPTION(throw std::runtime_error("x"), std::exception, [](const std::exception& ex) { return std::string{ ex.what() } == "x"; });
}

BC_AUTO_TEST_CASE(unit_test_checks__test_context__statement__executed_once)
{
    size_t count{};
    BC_TEST_CONTEXT("context " << 42)
    {
        ++count;
    }

    BC_REQUIRE_EQUAL(count, 1u);
}

BC_AUTO_TEST_SUITE_END()
