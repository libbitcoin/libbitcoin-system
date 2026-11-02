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

BC_AUTO_TEST_SUITE(config_path_tests)

BC_AUTO_TEST_CASE(path__construct__default__empty)
{
    const path instance{};
    BC_REQUIRE(instance.to_string().empty());
}

BC_AUTO_TEST_CASE(path__construct__string__expected)
{
    const path instance{ std::string{ "foo/bar" } };
    BC_REQUIRE_EQUAL(instance.to_string(), "foo/bar");
}

BC_AUTO_TEST_CASE(path__construct__literal__expected)
{
    const path instance{ "foo/bar" };
    BC_REQUIRE_EQUAL(instance.to_string(), "foo/bar");
}

BC_AUTO_TEST_CASE(path__construct__empty_literal__empty)
{
    const path instance{ "" };
    BC_REQUIRE(instance.to_string().empty());
}

BC_AUTO_TEST_CASE(path__construct__empty_string__empty)
{
    const path instance{ std::string{} };
    BC_REQUIRE(instance.to_string().empty());
}

BC_AUTO_TEST_CASE(path__construct__spaced_string__expected)
{
    const path instance{ std::string{ "foo bar/baz qux" } };
    BC_REQUIRE_EQUAL(instance.to_string(), "foo bar/baz qux");
}

BC_AUTO_TEST_CASE(path__cast__value__expected)
{
    const path instance{ std::string{ "foo/bar" } };
    const std::filesystem::path& value = instance;
    BC_REQUIRE(value == std::filesystem::path{ "foo/bar" });
}

BC_AUTO_TEST_CASE(path__equality__same__true)
{
    BC_REQUIRE(path{ "foo/bar" } == path{ "foo/bar" });
    BC_REQUIRE(!(path{ "foo/bar" } != path{ "foo/bar" }));
}

BC_AUTO_TEST_CASE(path__equality__different__false)
{
    BC_REQUIRE(path{ "foo" } != path{ "bar" });
    BC_REQUIRE(!(path{ "foo" } == path{ "bar" }));
}

BC_AUTO_TEST_CASE(path__equality__literal__true)
{
    BC_REQUIRE(path{ "foo" } == "foo");
}

BC_AUTO_TEST_CASE(path__empty__default__true)
{
    BC_REQUIRE(path{}.empty());
}

BC_AUTO_TEST_CASE(path__string__default__empty)
{
    BC_REQUIRE(path{}.string().empty());
}

BC_AUTO_TEST_CASE(path__string__value__native)
{
    const path instance{ "foo/bar" };
    BC_REQUIRE_EQUAL(instance.string(), std::filesystem::path{ "foo/bar" }.string());
}

BC_AUTO_TEST_CASE(path__empty__value__false)
{
    BC_REQUIRE(!path{ std::string{ "foo" } }.empty());
}

BC_AUTO_TEST_CASE(path__divide__relative__appended)
{
    const path instance{ std::string{ "foo" } };
    BC_REQUIRE((instance / "bar") == std::filesystem::path{ "foo" } / "bar");
}

BC_AUTO_TEST_CASE(path__stream_out__empty__empty)
{
    std::ostringstream output{};
    output << path{};
    BC_REQUIRE(output.str().empty());
}

BC_AUTO_TEST_CASE(path__stream_out__spaced__unquoted)
{
    std::ostringstream output{};
    output << path{ std::string{ "foo bar" } };
    BC_REQUIRE_EQUAL(output.str(), "foo bar");
}

BC_AUTO_TEST_CASE(path__stream_in__spaced__full_line)
{
    path instance{};
    std::istringstream{ "foo bar/baz" } >> instance;
    BC_REQUIRE_EQUAL(instance.to_string(), "foo bar/baz");
}

BC_AUTO_TEST_CASE(path__stream_in__empty__empty)
{
    path instance{ std::string{ "foo" } };
    std::istringstream{ "" } >> instance;
    BC_REQUIRE(instance.to_string().empty());
}

BC_AUTO_TEST_CASE(path__option_value__empty_token__empty)
{
    std::any value{};
    const std::unique_ptr<const options::value_semantic> semantic{ options::value<path>() };
    semantic->parse(value, string_list{ "" }, "path");
    BC_REQUIRE(std::any_cast<path>(value).to_string().empty());
}

BC_AUTO_TEST_CASE(path__option_value__spaced_token__expected)
{
    std::any value{};
    const std::unique_ptr<const options::value_semantic> semantic{ options::value<path>() };
    semantic->parse(value, string_list{ "foo bar/baz" }, "path");
    BC_REQUIRE_EQUAL(std::any_cast<path>(value).to_string(), "foo bar/baz");
}

BC_AUTO_TEST_CASE(path__option_value__multiple_tokens__throws)
{
    std::any value{};
    const std::unique_ptr<const options::value_semantic> semantic{ options::value<path>() };
    BC_REQUIRE_THROW(semantic->parse(value, string_list{ "foo", "bar" }, "path"), validation_exception);
}

BC_AUTO_TEST_CASE(path__construct__type__expected)
{
    const config::path::type value{ "foo/bar" };
    const config::path instance(value);
    BC_REQUIRE((const config::path::type&)instance == value);
}

BC_AUTO_TEST_SUITE_END()
