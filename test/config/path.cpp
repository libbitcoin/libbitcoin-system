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

using namespace bc::system::config;

BOOST_AUTO_TEST_SUITE(config_path_tests)

BOOST_AUTO_TEST_CASE(path__construct__default__empty)
{
    const path instance{};
    BOOST_REQUIRE(instance.to_string().empty());
}

BOOST_AUTO_TEST_CASE(path__construct__string__expected)
{
    const path instance{ std::string{ "foo/bar" } };
    BOOST_REQUIRE_EQUAL(instance.to_string(), "foo/bar");
}

BOOST_AUTO_TEST_CASE(path__construct__literal__expected)
{
    const path instance{ "foo/bar" };
    BOOST_REQUIRE_EQUAL(instance.to_string(), "foo/bar");
}

BOOST_AUTO_TEST_CASE(path__construct__empty_literal__empty)
{
    const path instance{ "" };
    BOOST_REQUIRE(instance.to_string().empty());
}

BOOST_AUTO_TEST_CASE(path__construct__empty_string__empty)
{
    const path instance{ std::string{} };
    BOOST_REQUIRE(instance.to_string().empty());
}

BOOST_AUTO_TEST_CASE(path__construct__spaced_string__expected)
{
    const path instance{ std::string{ "foo bar/baz qux" } };
    BOOST_REQUIRE_EQUAL(instance.to_string(), "foo bar/baz qux");
}

BOOST_AUTO_TEST_CASE(path__cast__value__expected)
{
    const path instance{ std::string{ "foo/bar" } };
    const std::filesystem::path& value = instance;
    BOOST_REQUIRE(value == std::filesystem::path{ "foo/bar" });
}

BOOST_AUTO_TEST_CASE(path__equality__same__true)
{
    BOOST_REQUIRE(path{ "foo/bar" } == path{ "foo/bar" });
    BOOST_REQUIRE(!(path{ "foo/bar" } != path{ "foo/bar" }));
}

BOOST_AUTO_TEST_CASE(path__equality__different__false)
{
    BOOST_REQUIRE(path{ "foo" } != path{ "bar" });
    BOOST_REQUIRE(!(path{ "foo" } == path{ "bar" }));
}

BOOST_AUTO_TEST_CASE(path__equality__literal__true)
{
    BOOST_REQUIRE(path{ "foo" } == "foo");
}

BOOST_AUTO_TEST_CASE(path__empty__default__true)
{
    BOOST_REQUIRE(path{}.empty());
}

BOOST_AUTO_TEST_CASE(path__empty__value__false)
{
    BOOST_REQUIRE(!path{ std::string{ "foo" } }.empty());
}

BOOST_AUTO_TEST_CASE(path__divide__relative__appended)
{
    const path instance{ std::string{ "foo" } };
    BOOST_REQUIRE((instance / "bar") == std::filesystem::path{ "foo" } / "bar");
}

BOOST_AUTO_TEST_CASE(path__stream_out__empty__empty)
{
    std::ostringstream output{};
    output << path{};
    BOOST_REQUIRE(output.str().empty());
}

BOOST_AUTO_TEST_CASE(path__stream_out__spaced__unquoted)
{
    std::ostringstream output{};
    output << path{ std::string{ "foo bar" } };
    BOOST_REQUIRE_EQUAL(output.str(), "foo bar");
}

BOOST_AUTO_TEST_CASE(path__stream_in__spaced__full_line)
{
    path instance{};
    std::istringstream{ "foo bar/baz" } >> instance;
    BOOST_REQUIRE_EQUAL(instance.to_string(), "foo bar/baz");
}

BOOST_AUTO_TEST_CASE(path__stream_in__empty__empty)
{
    path instance{ std::string{ "foo" } };
    std::istringstream{ "" } >> instance;
    BOOST_REQUIRE(instance.to_string().empty());
}

BOOST_AUTO_TEST_CASE(path__validate__empty_token__empty)
{
    boost::any value{};
    validate(value, string_list{ "" }, static_cast<path*>(nullptr), 0);
    BOOST_REQUIRE(boost::any_cast<path>(value).to_string().empty());
}

BOOST_AUTO_TEST_CASE(path__validate__no_tokens__empty)
{
    boost::any value{};
    validate(value, string_list{}, static_cast<path*>(nullptr), 0);
    BOOST_REQUIRE(boost::any_cast<path>(value).to_string().empty());
}

BOOST_AUTO_TEST_CASE(path__validate__spaced_token__expected)
{
    boost::any value{};
    validate(value, string_list{ "foo bar/baz" }, static_cast<path*>(nullptr), 0);
    BOOST_REQUIRE_EQUAL(boost::any_cast<path>(value).to_string(), "foo bar/baz");
}

BOOST_AUTO_TEST_CASE(path__validate__multiple_tokens__throws)
{
    boost::any value{};
    BOOST_REQUIRE_THROW(validate(value, string_list{ "foo", "bar" }, static_cast<path*>(nullptr), 0), boost::program_options::validation_error);
}

BOOST_AUTO_TEST_SUITE_END()
