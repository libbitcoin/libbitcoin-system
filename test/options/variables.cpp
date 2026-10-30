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

using namespace bc::system::options;

BOOST_AUTO_TEST_SUITE(variables_tests)

static parsed_options make(const options_description& options, std::vector<parsed_option> parsed, bool dashed=true)
{
    return { std::move(parsed), &options, dashed };
}

// variable_value

BOOST_AUTO_TEST_CASE(variable_value__default__empty)
{
    const variable_value instance{};
    BOOST_REQUIRE(instance.empty());
    BOOST_REQUIRE(!instance.defaulted());
    BOOST_REQUIRE(!instance.semantic());
}

BOOST_AUTO_TEST_CASE(variable_value__as__typed__expected)
{
    const variable_value instance{ std::any{ 42 }, true, {} };
    BOOST_REQUIRE(!instance.empty());
    BOOST_REQUIRE(instance.defaulted());
    BOOST_REQUIRE_EQUAL(instance.as<int>(), 42);
    BOOST_REQUIRE_THROW(instance.as<std::string>(), std::bad_any_cast);
}

// variables_map

BOOST_AUTO_TEST_CASE(variables_map__subscript__absent__empty_not_inserted)
{
    const variables_map instance{};
    BOOST_REQUIRE(instance["bogus"].empty());
    BOOST_REQUIRE(instance.empty());
}

// store

BOOST_AUTO_TEST_CASE(store__explicit__value_not_defaulted)
{
    options_description options{};
    options.add_options()("name", value<int32_t>()->default_value(7), "");

    variables_map variables{};
    store(make(options, { { "name", { "42" } } }), variables);
    BOOST_REQUIRE_EQUAL(variables["name"].as<int32_t>(), 42);
    BOOST_REQUIRE(!variables["name"].defaulted());
}

BOOST_AUTO_TEST_CASE(store__unspecified__default_defaulted)
{
    options_description options{};
    options.add_options()
        ("name", value<int32_t>()->default_value(7), "")
        ("other", value<int32_t>(), "");

    variables_map variables{};
    store(make(options, {}), variables);
    BOOST_REQUIRE_EQUAL(variables["name"].as<int32_t>(), 7);
    BOOST_REQUIRE(variables["name"].defaulted());
    BOOST_REQUIRE(!variables.contains("other"));
}

BOOST_AUTO_TEST_CASE(store__final__not_overridden_by_later_source)
{
    options_description options{};
    options.add_options()("name", value<int32_t>(), "");

    variables_map variables{};
    store(make(options, { { "name", { "1" } } }), variables);
    store(make(options, { { "name", { "2" } } }), variables);
    BOOST_REQUIRE_EQUAL(variables["name"].as<int32_t>(), 1);
}

BOOST_AUTO_TEST_CASE(store__defaulted__overridden_by_later_source)
{
    options_description options{};
    options.add_options()("name", value<int32_t>()->default_value(7), "");

    variables_map variables{};
    store(make(options, {}), variables);
    store(make(options, { { "name", { "2" } } }), variables);
    BOOST_REQUIRE_EQUAL(variables["name"].as<int32_t>(), 2);
    BOOST_REQUIRE(!variables["name"].defaulted());
}

BOOST_AUTO_TEST_CASE(store__composing__accumulates_across_sources)
{
    options_description options{};
    options.add_options()("name", value<string_list>()->composing(), "");

    variables_map variables{};
    store(make(options, { { "name", { "a" } } }), variables);
    store(make(options, { { "name", { "b" } } }), variables);
    BOOST_REQUIRE_EQUAL(variables["name"].as<string_list>(), (string_list{ "a", "b" }));
}

BOOST_AUTO_TEST_CASE(store__repeated_vector__accumulates_within_source)
{
    options_description options{};
    options.add_options()("name", value<string_list>(), "");

    variables_map variables{};
    store(make(options, { { "name", { "a" } }, { "name", { "b" } } }), variables);
    BOOST_REQUIRE_EQUAL(variables["name"].as<string_list>(), (string_list{ "a", "b" }));
}

BOOST_AUTO_TEST_CASE(store__repeated_scalar__throws)
{
    options_description options{};
    options.add_options()("name", value<int32_t>(), "");

    variables_map variables{};
    BOOST_REQUIRE_EXCEPTION(store(make(options, { { "name", { "1" } }, { "name", { "2" } } }), variables), validation_exception, [](const validation_exception& ex) { return std::string{ ex.what() } == "option '--name' cannot be specified more than once"; });
}

BOOST_AUTO_TEST_CASE(store__invalid_undashed__named_undashed)
{
    options_description options{};
    options.add_options()("section.name", value<int32_t>(), "");

    variables_map variables{};
    BOOST_REQUIRE_EXCEPTION(store(make(options, { { "section.name", { "x" } } }, false), variables), istream_exception, [](const istream_exception& ex) { return std::string{ ex.what() } == "the argument ('x') for option 'section.name' is invalid"; });
}

BOOST_AUTO_TEST_CASE(store__undeclared__throws)
{
    const options_description options{};
    variables_map variables{};
    BOOST_REQUIRE_THROW(store(make(options, { { "bogus", { "1" } } }), variables), validation_exception);
}

BOOST_AUTO_TEST_CASE(store__unnamed_positional__skipped)
{
    const options_description options{};
    variables_map variables{};
    store(make(options, { { "", { "1" } } }), variables);
    BOOST_REQUIRE(variables.empty());
}

// notify

BOOST_AUTO_TEST_CASE(notify__bound__stored_and_notified)
{
    int32_t stored{};
    int32_t notified{};
    options_description options{};
    options.add_options()
        ("stored", value<int32_t>(&stored), "")
        ("notified", value<int32_t>()->notifier([&](const int32_t& value) { notified = value; }), "");

    variables_map variables{};
    store(make(options, { { "stored", { "1" } }, { "notified", { "2" } } }), variables);
    notify(variables);
    BOOST_REQUIRE_EQUAL(stored, 1);
    BOOST_REQUIRE_EQUAL(notified, 2);
}

BOOST_AUTO_TEST_CASE(notify__default__stored)
{
    int32_t stored{};
    options_description options{};
    options.add_options()("stored", value<int32_t>(&stored)->default_value(7), "");

    variables_map variables{};
    store(make(options, {}), variables);
    notify(variables);
    BOOST_REQUIRE_EQUAL(stored, 7);
}

BOOST_AUTO_TEST_CASE(notify__required_missing__throws_dashed)
{
    options_description options{};
    options.add_options()("name", value<int32_t>()->required(), "");

    variables_map variables{};
    store(make(options, {}, false), variables);
    store(make(options, {}, true), variables);
    BOOST_REQUIRE_EXCEPTION(notify(variables), validation_exception, [](const validation_exception& ex) { return std::string{ ex.what() } == "the option '--name' is required but missing"; });
}

BOOST_AUTO_TEST_CASE(notify__required_present__does_not_throw)
{
    options_description options{};
    options.add_options()("name", value<int32_t>()->required(), "");

    variables_map variables{};
    store(make(options, { { "name", { "1" } } }), variables);
    BOOST_REQUIRE_NO_THROW(notify(variables));
}

BOOST_AUTO_TEST_SUITE_END()
