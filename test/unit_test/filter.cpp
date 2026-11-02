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

BC_AUTO_TEST_SUITE(unit_test_filter_tests)

// matches

BC_AUTO_TEST_CASE(unit_test_filter__matches__exact__true)
{
    BC_REQUIRE(filter::matches("abc", "abc"));
    BC_REQUIRE(filter::matches("", ""));
}

BC_AUTO_TEST_CASE(unit_test_filter__matches__different__false)
{
    BC_REQUIRE(!filter::matches("abc", "abd"));
    BC_REQUIRE(!filter::matches("abc", "ab"));
    BC_REQUIRE(!filter::matches("ab", "abc"));
    BC_REQUIRE(!filter::matches("", "a"));
}

BC_AUTO_TEST_CASE(unit_test_filter__matches__wildcards__expected)
{
    BC_REQUIRE(filter::matches("*", ""));
    BC_REQUIRE(filter::matches("*", "anything"));
    BC_REQUIRE(filter::matches("abc*", "abcdef"));
    BC_REQUIRE(filter::matches("*def", "abcdef"));
    BC_REQUIRE(filter::matches("a*f", "abcdef"));
    BC_REQUIRE(filter::matches("a*c*f", "abcdef"));
    BC_REQUIRE(filter::matches("**", "abc"));
    BC_REQUIRE(filter::matches("a*a", "aaa"));
    BC_REQUIRE(!filter::matches("abc*", "abd"));
    BC_REQUIRE(!filter::matches("*def", "abcdeg"));
    BC_REQUIRE(!filter::matches("a*c*g", "abcdef"));
}

// selects

BC_AUTO_TEST_CASE(unit_test_filter__selects__no_filters__true)
{
    const filter instance{ {} };
    BC_REQUIRE(instance.selects({ "suite", "case" }));
}

BC_AUTO_TEST_CASE(unit_test_filter__selects__suite__subtree)
{
    const filter instance{ { "suite" } };
    BC_REQUIRE(instance.selects({ "suite", "case" }));
    BC_REQUIRE(instance.selects({ "suite", "inner", "case" }));
    BC_REQUIRE(!instance.selects({ "other", "case" }));
}

BC_AUTO_TEST_CASE(unit_test_filter__selects__levels__expected)
{
    const filter instance{ { "suite/case*" } };
    BC_REQUIRE(instance.selects({ "suite", "case_one" }));
    BC_REQUIRE(!instance.selects({ "suite", "other" }));
    BC_REQUIRE(!instance.selects({ "suite" }));
}

BC_AUTO_TEST_CASE(unit_test_filter__selects__alternatives__expected)
{
    const filter instance{ { "one,two/a,b" } };
    BC_REQUIRE(instance.selects({ "one", "a" }));
    BC_REQUIRE(instance.selects({ "two", "b" }));
    BC_REQUIRE(!instance.selects({ "three", "a" }));
    BC_REQUIRE(!instance.selects({ "one", "c" }));
}

BC_AUTO_TEST_CASE(unit_test_filter__selects__any_of_filters__expected)
{
    const filter instance{ { "one", "two/a" } };
    BC_REQUIRE(instance.selects({ "one", "x" }));
    BC_REQUIRE(instance.selects({ "two", "a" }));
    BC_REQUIRE(!instance.selects({ "two", "b" }));
}

BC_AUTO_TEST_CASE(unit_test_filter__selects__exclusion__expected)
{
    const filter instance{ { "!suite/slow*" } };
    BC_REQUIRE(instance.selects({ "suite", "fast" }));
    BC_REQUIRE(instance.selects({ "other", "slow" }));
    BC_REQUIRE(!instance.selects({ "suite", "slow_one" }));
}

BC_AUTO_TEST_CASE(unit_test_filter__selects__inclusion_and_exclusion__expected)
{
    const filter instance{ { "suite", "!suite/slow" } };
    BC_REQUIRE(instance.selects({ "suite", "fast" }));
    BC_REQUIRE(!instance.selects({ "suite", "slow" }));
    BC_REQUIRE(!instance.selects({ "other", "fast" }));
}

BC_AUTO_TEST_SUITE_END()
