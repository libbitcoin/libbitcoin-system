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

BC_AUTO_TEST_SUITE(unit_test_registry_tests)

static void invoker()
{
}

static const unit* find(const unit& suite, const std::string& name)
{
    for (const auto& child: suite.children)
        if (child.name == name)
            return &child;

    return nullptr;
}

// unit

BC_AUTO_TEST_CASE(unit_test_registry__unit_is_suite__no_invoker__true)
{
    const unit instance{ "suite", "file", 42 };
    BC_REQUIRE(instance.is_suite());
}

BC_AUTO_TEST_CASE(unit_test_registry__unit_is_suite__invoker__false)
{
    const unit instance{ "case", "file", 42, &invoker };
    BC_REQUIRE(!instance.is_suite());
}

// registry

BC_AUTO_TEST_CASE(unit_test_registry__default__empty_master)
{
    registry instance{};
    BC_REQUIRE(instance.master().is_suite());
    BC_REQUIRE(instance.master().children.empty());
}

BC_AUTO_TEST_CASE(unit_test_registry__add_case__master__added)
{
    registry instance{};
    instance.add_case(&invoker, "case", "file", 42);

    const auto& children = instance.master().children;
    BC_REQUIRE_EQUAL(children.size(), 1u);
    BC_REQUIRE_EQUAL(children.front().name, "case");
    BC_REQUIRE_EQUAL(children.front().file, "file");
    BC_REQUIRE_EQUAL(children.front().line, 42u);
    BC_REQUIRE(!children.front().is_suite());
}

BC_AUTO_TEST_CASE(unit_test_registry__begin_suite__cases__added_to_open_suite)
{
    registry instance{};
    instance.begin_suite("suite", "file", 1);
    instance.add_case(&invoker, "one", "file", 2);
    instance.begin_suite("inner", "file", 3);
    instance.add_case(&invoker, "two", "file", 4);
    instance.end_suite();
    instance.add_case(&invoker, "three", "file", 5);
    instance.end_suite();
    instance.add_case(&invoker, "four", "file", 6);

    const auto& master = instance.master();
    const auto suite = find(master, "suite");
    BC_REQUIRE_EQUAL(master.children.size(), 2u);
    BC_REQUIRE(!is_null(find(master, "four")));
    BC_REQUIRE(!is_null(suite));
    BC_REQUIRE(suite->is_suite());
    BC_REQUIRE_EQUAL(suite->children.size(), 3u);
    BC_REQUIRE(!is_null(find(*suite, "one")));
    BC_REQUIRE(!is_null(find(*suite, "three")));

    const auto inner = find(*suite, "inner");
    BC_REQUIRE(!is_null(inner));
    BC_REQUIRE(!is_null(find(*inner, "two")));
}

BC_AUTO_TEST_CASE(unit_test_registry__begin_suite__same_name__merged)
{
    registry instance{};
    instance.begin_suite("suite", "first", 1);
    instance.add_case(&invoker, "one", "first", 2);
    instance.end_suite();
    instance.begin_suite("suite", "second", 1);
    instance.add_case(&invoker, "two", "second", 2);
    instance.end_suite();

    const auto& master = instance.master();
    BC_REQUIRE_EQUAL(master.children.size(), 1u);
    BC_REQUIRE_EQUAL(master.children.front().file, "first");
    BC_REQUIRE_EQUAL(master.children.front().children.size(), 2u);
}

BC_AUTO_TEST_CASE(unit_test_registry__end_suite__master__unchanged)
{
    registry instance{};
    instance.end_suite();
    instance.add_case(&invoker, "case", "file", 42);
    BC_REQUIRE_EQUAL(instance.master().children.size(), 1u);
}

BC_AUTO_TEST_CASE(unit_test_registry__instance__this_case__registered)
{
    const auto suite = find(registry::instance().master(), "unit_test_registry_tests");
    BC_REQUIRE(!is_null(suite));
    BC_REQUIRE(!is_null(find(*suite, "unit_test_registry__instance__this_case__registered")));
    BC_REQUIRE_EQUAL(unit_test::session::instance().case_name(), "unit_test_registry__instance__this_case__registered");
}

BC_AUTO_TEST_SUITE_END()
