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

BC_AUTO_TEST_SUITE(description_tests)

// option_description

BC_AUTO_TEST_CASE(option_description__names__long_short__both)
{
    const option_description instance{ "long,s", value<int32_t>(), "text" };
    BC_REQUIRE_EQUAL(instance.long_name(), "long");
    BC_REQUIRE_EQUAL(instance.short_name(), "-s");
    BC_REQUIRE_EQUAL(instance.key(), "long");
    BC_REQUIRE_EQUAL(instance.description(), "text");
    BC_REQUIRE_EQUAL(instance.format_name(), "-s [ --long ]");
    BC_REQUIRE_EQUAL(instance.display_name(true), "--long");
    BC_REQUIRE_EQUAL(instance.display_name(false), "long");
}

BC_AUTO_TEST_CASE(option_description__names__long__long)
{
    const option_description instance{ "section.long", value<int32_t>(), "" };
    BC_REQUIRE_EQUAL(instance.long_name(), "section.long");
    BC_REQUIRE(instance.short_name().empty());
    BC_REQUIRE_EQUAL(instance.key(), "section.long");
    BC_REQUIRE_EQUAL(instance.format_name(), "--section.long");
}

BC_AUTO_TEST_CASE(option_description__names__short__short)
{
    const option_description instance{ ",s", value<int32_t>(), "" };
    BC_REQUIRE(instance.long_name().empty());
    BC_REQUIRE_EQUAL(instance.short_name(), "-s");
    BC_REQUIRE_EQUAL(instance.key(), "-s");
    BC_REQUIRE_EQUAL(instance.format_name(), "-s");
    BC_REQUIRE_EQUAL(instance.display_name(true), "-s");
}

BC_AUTO_TEST_CASE(option_description__format_parameter__tokens__value_name)
{
    const option_description instance{ "long", value<int32_t>()->default_value(42), "" };
    BC_REQUIRE_EQUAL(instance.format_parameter(), "arg (=42)");
}

BC_AUTO_TEST_CASE(option_description__format_parameter__zero_tokens__empty)
{
    const option_description instance{ "long", value<bool>()->zero_tokens(), "" };
    BC_REQUIRE(instance.format_parameter().empty());
}

BC_AUTO_TEST_CASE(option_description__matches__names__expected)
{
    const option_description instance{ "long,s", value<int32_t>(), "" };
    BC_REQUIRE(instance.matches("long"));
    BC_REQUIRE(instance.matches("-s"));
    BC_REQUIRE(!instance.matches("s"));
    BC_REQUIRE(!instance.matches("lon"));
    BC_REQUIRE(!instance.matches("--long"));
}

BC_AUTO_TEST_CASE(option_description__abbreviates__prefix__expected)
{
    const option_description instance{ "long,s", value<int32_t>(), "" };
    BC_REQUIRE(instance.abbreviates("l"));
    BC_REQUIRE(instance.abbreviates("lon"));
    BC_REQUIRE(!instance.abbreviates("long"));
    BC_REQUIRE(!instance.abbreviates(""));
    BC_REQUIRE(!instance.abbreviates("longer"));
}

// options_description

BC_AUTO_TEST_CASE(options_description__add_options__ordered)
{
    options_description instance{ "caption" };
    instance.add_options()
        ("first,f", value<int32_t>(), "one")
        ("second", "two");

    BC_REQUIRE_EQUAL(instance.caption(), "caption");
    BC_REQUIRE_EQUAL(instance.options().size(), 2u);
    BC_REQUIRE_EQUAL(instance.options().front()->key(), "first");
    BC_REQUIRE_EQUAL(instance.options().back()->key(), "second");
    BC_REQUIRE_EQUAL(instance.options().back()->semantic()->max_tokens(), 0u);
}

BC_AUTO_TEST_CASE(options_description__find_nothrow__exact__found)
{
    options_description instance{};
    instance.add_options()
        ("all", "")
        ("all-chroots", "");

    BC_REQUIRE_EQUAL(instance.find_nothrow("all", true)->key(), "all");
}

BC_AUTO_TEST_CASE(options_description__find_nothrow__unique_abbreviation__found)
{
    options_description instance{};
    instance.add_options()
        ("help,h", "")
        ("version", "");

    BC_REQUIRE_EQUAL(instance.find_nothrow("he", true)->key(), "help");
    BC_REQUIRE_EQUAL(instance.find_nothrow("-h", false)->key(), "help");
    BC_REQUIRE(is_null(instance.find_nothrow("he", false)));
    BC_REQUIRE(is_null(instance.find_nothrow("bogus", true)));
}

BC_AUTO_TEST_CASE(options_description__find_nothrow__ambiguous_abbreviation__throws)
{
    options_description instance{};
    instance.add_options()
        ("alpha", "")
        ("alps", "");

    BC_REQUIRE_THROW(instance.find_nothrow("al", true), validation_exception);
}

BC_AUTO_TEST_CASE(options_description__find__missing__throws)
{
    const options_description instance{};
    BC_REQUIRE_THROW(instance.find("bogus", false), validation_exception);
}

// positional_options_description

BC_AUTO_TEST_CASE(positional_options_description__limited__names_by_position)
{
    positional_options_description instance{};
    instance.add("first", 1).add("second", 2);
    BC_REQUIRE_EQUAL(instance.max_total_count(), 3u);
    BC_REQUIRE_EQUAL(instance.name_for_position(0), "first");
    BC_REQUIRE_EQUAL(instance.name_for_position(1), "second");
    BC_REQUIRE_EQUAL(instance.name_for_position(2), "second");
}

BC_AUTO_TEST_CASE(positional_options_description__unlimited__trailing_name)
{
    positional_options_description instance{};
    instance.add("first", 1).add("rest", -1);
    BC_REQUIRE_EQUAL(instance.max_total_count(), max_size_t);
    BC_REQUIRE_EQUAL(instance.name_for_position(0), "first");
    BC_REQUIRE_EQUAL(instance.name_for_position(1), "rest");
    BC_REQUIRE_EQUAL(instance.name_for_position(42), "rest");
}

BC_AUTO_TEST_CASE(positional_options_description__empty__zero)
{
    const positional_options_description instance{};
    BC_REQUIRE_EQUAL(instance.max_total_count(), 0u);
}

BC_AUTO_TEST_SUITE_END()
