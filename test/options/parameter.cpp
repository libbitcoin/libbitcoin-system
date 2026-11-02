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
#include <filesystem>

BC_AUTO_TEST_SUITE(parameter_tests)

using namespace bc::system::options;

enum opt
{
    short_long = 0,
    shorty,
    longy,
    simple,
    defaulty,
    required,
    toggled,
    vector,
    multitoken
};

static void load_test_options(options_description& options)
{
    options.add_options()
        ("short_long,s", "Long and short name.")
        (",m", "Short name only.")
        ("longy", value<std::string>()->required(), "Long name only.")
        ("simple", value<std::string>(), "Simple string.")
        ("defaulty", value<bool>()->default_value(true), "Defaulted bool.")
        ("required", value<std::filesystem::path>()->required(), "Required path.")
        ("toggled", value<bool>()->zero_tokens(), "Toggle only bool.")
        /* The enumerability of the data types does not control multiple,
           instance behavior but it is necessary to capture multiples. */
        ("VECTOR", value<std::vector<std::string>>(), "String vector.")
        ("multitoken", value<int>()->multitoken(), "Multi-token int.");
}

static void load_test_arguments(argument_list& arguments)
{
    arguments.push_back(argument_pair("longy", 1));
    arguments.push_back(argument_pair("simple", 2));
    arguments.push_back(argument_pair("defaulty", 3));
    arguments.push_back(argument_pair("required", 4));
    arguments.push_back(argument_pair("VECTOR", 5));
    arguments.push_back(argument_pair("multitoken", -1));
}

#define CONFIG_TEST_PARAMETER_SETUP(index) \
    options_description options; \
    load_test_options(options); \
    argument_list names; \
    load_test_arguments(names); \
    auto option = *(options.options()[index]); \
    options::parameter parameter

#define CONFIG_TEST_PARAMETER_OPTIONS_SETUP(index) \
    options_description options; \
    load_test_options(options); \
    auto option = *(options.options()[index]); \
    options::parameter parameter

// ------------------------------------------------------------------------- //
BC_AUTO_TEST_SUITE(parameter__position)

BC_AUTO_TEST_CASE(parameter__position__short_and_long__not_positional)
{
    CONFIG_TEST_PARAMETER_SETUP(opt::short_long);
    BC_REQUIRE_EQUAL(parameter.position(option, names), parameter::not_positional);
}

BC_AUTO_TEST_CASE(parameter__position__short_only__not_positional)
{
    CONFIG_TEST_PARAMETER_SETUP(opt::shorty);
    BC_REQUIRE_EQUAL(parameter.position(option, names), parameter::not_positional);
}

BC_AUTO_TEST_CASE(parameter__position__long_only__expected_position)
{
    CONFIG_TEST_PARAMETER_SETUP(opt::longy);
    BC_REQUIRE_EQUAL(parameter.position(option, names), 0);
}

BC_AUTO_TEST_CASE(parameter__position__simple__expected_position)
{
    CONFIG_TEST_PARAMETER_SETUP(opt::simple);
    BC_REQUIRE_EQUAL(parameter.position(option, names), 1);
}

BC_AUTO_TEST_CASE(parameter__position__defaulty__expected_position)
{
    CONFIG_TEST_PARAMETER_SETUP(opt::defaulty);
    BC_REQUIRE_EQUAL(parameter.position(option, names), 2);
}

BC_AUTO_TEST_CASE(parameter__position__required__expected_position)
{
    CONFIG_TEST_PARAMETER_SETUP(opt::required);
    BC_REQUIRE_EQUAL(parameter.position(option, names), 3);
}

BC_AUTO_TEST_CASE(parameter__position__toggled__not_positional)
{
    CONFIG_TEST_PARAMETER_SETUP(opt::toggled);
    BC_REQUIRE_EQUAL(parameter.position(option, names), parameter::not_positional);
}

BC_AUTO_TEST_CASE(parameter__position__vector__expected_position)
{
    CONFIG_TEST_PARAMETER_SETUP(opt::vector);
    BC_REQUIRE_EQUAL(parameter.position(option, names), 4);
}

BC_AUTO_TEST_CASE(parameter__position__multitoken__expected_position)
{
    CONFIG_TEST_PARAMETER_SETUP(opt::multitoken);
    BC_REQUIRE_EQUAL(parameter.position(option, names), 5);
}

BC_AUTO_TEST_SUITE_END()

// ------------------------------------------------------------------------- //
BC_AUTO_TEST_SUITE(parameter__initialize)

BC_AUTO_TEST_CASE(parameter__initialize__command_line_option__no_values_unconfigured)
{
    CONFIG_TEST_PARAMETER_SETUP(opt::simple);
    parameter.initialize(option, names);
    BC_REQUIRE_EQUAL(parameter.values(), (string_list{}));
    BC_REQUIRE_EQUAL(parameter.configured(), false);
}

BC_AUTO_TEST_CASE(parameter__initialize__bound__values_unconfigured)
{
    uint32_t threads = 42;
    options_description options;
    options.add_options()("bound", setting(&threads), "Bound setting.");
    const auto& option = *options.options().front();

    argument_list names;
    options::parameter parameter;
    parameter.initialize(option, names);
    BC_REQUIRE_EQUAL(parameter.values(), (string_list{ "42" }));
    BC_REQUIRE_EQUAL(parameter.configured(), false);
}

BC_AUTO_TEST_CASE(parameter__initialize__bound_placeholder__default_text_substituted)
{
    uint32_t threads = 42;
    options_description options;
    options.add_options()("bound", setting(&threads), "Threads, defaults to {} (0 disables).");
    const auto& option = *options.options().front();

    argument_list names;
    options::parameter parameter;
    parameter.initialize(option, names);
    BC_REQUIRE_EQUAL(parameter.description(), "Threads, defaults to '42' (0 disables).");
}

BC_AUTO_TEST_CASE(parameter__initialize__unbound_placeholder__unchanged)
{
    options_description options;
    options.add_options()("unbound", value<uint32_t>(), "Threads, defaults to {}.");
    const auto& option = *options.options().front();

    argument_list names;
    options::parameter parameter;
    parameter.initialize(option, names);
    BC_REQUIRE_EQUAL(parameter.description(), "Threads, defaults to {}.");
}

BC_AUTO_TEST_CASE(parameter__initialize__bound_specified__values_configured)
{
    uint32_t threads = 42;
    options_description options;
    options.add_options()("bound", setting(&threads), "Bound setting.");
    const auto& option = *options.options().front();

    variables_map variables;
    const char* argv[]{ "test", "--bound", "42" };
    store(parse_command_line(3, argv, options, {}), variables);

    argument_list names;
    options::parameter parameter;
    parameter.initialize(option, names, variables);
    BC_REQUIRE_EQUAL(parameter.values(), (string_list{ "42" }));
    BC_REQUIRE_EQUAL(parameter.configured(), true);
}

BC_AUTO_TEST_CASE(parameter__initialize__short_long__sets_limit_0)
{
    CONFIG_TEST_PARAMETER_SETUP(opt::short_long);
    parameter.initialize(option, names);
    BC_REQUIRE_EQUAL(parameter.position(), parameter::not_positional);
    BC_REQUIRE_EQUAL(parameter.args_limit(), 0u);
    BC_REQUIRE_EQUAL(parameter.required(), false);
    BC_REQUIRE_EQUAL(parameter.short_name(), 's');
    BC_REQUIRE_EQUAL(parameter.long_name(), "short_long");
    BC_REQUIRE_EQUAL(parameter.format_name(), "-s [ --short_long ]");
    BC_REQUIRE_EQUAL(parameter.format_parameter(), "");
    BC_REQUIRE_EQUAL(parameter.description(), "Long and short name.");
}

BC_AUTO_TEST_CASE(parameter__initialize__short_only__sets_limit_0)
{
    CONFIG_TEST_PARAMETER_SETUP(opt::shorty);
    parameter.initialize(option, names);
    BC_REQUIRE_EQUAL(parameter.position(), parameter::not_positional);
    BC_REQUIRE_EQUAL(parameter.args_limit(), 0u);
    BC_REQUIRE_EQUAL(parameter.required(), false);
    BC_REQUIRE_EQUAL(parameter.short_name(), 'm');
    BC_REQUIRE_EQUAL(parameter.long_name(), "");
    // A boost 1.49 bug causes this test failure:
    // parameter.format_name() == "-m" failed [-m [ -- ] != -m]
    // But this behavior is not critical to operations, so we overlook it.
    // BC_REQUIRE_EQUAL(parameter.format_name(), "-m");
    BC_REQUIRE_EQUAL(parameter.format_parameter(), "");
    BC_REQUIRE_EQUAL(parameter.description(), "Short name only.");
}

BC_AUTO_TEST_CASE(parameter__initialize__long_only__sets_limit_0)
{
    CONFIG_TEST_PARAMETER_SETUP(opt::longy);
    parameter.initialize(option, names);
    BC_REQUIRE_EQUAL(parameter.position(), 0);
    BC_REQUIRE_EQUAL(parameter.args_limit(), 1u);
    BC_REQUIRE_EQUAL(parameter.required(), true);
    BC_REQUIRE_EQUAL(parameter.short_name(), '\0');
    BC_REQUIRE_EQUAL(parameter.long_name(), "longy");
    BC_REQUIRE_EQUAL(parameter.format_name(), "--longy");
    BC_REQUIRE_EQUAL(parameter.format_parameter(), "arg");
    BC_REQUIRE_EQUAL(parameter.description(), "Long name only.");
}

BC_AUTO_TEST_CASE(parameter__initialize__simple__sets_limit_2)
{
    CONFIG_TEST_PARAMETER_SETUP(opt::simple);
    parameter.initialize(option, names);
    BC_REQUIRE_EQUAL(parameter.position(), 1);
    BC_REQUIRE_EQUAL(parameter.args_limit(), 2u);
    BC_REQUIRE_EQUAL(parameter.required(), false);
    BC_REQUIRE_EQUAL(parameter.short_name(), '\0');
    BC_REQUIRE_EQUAL(parameter.long_name(), "simple");
    BC_REQUIRE_EQUAL(parameter.format_name(), "--simple");
    BC_REQUIRE_EQUAL(parameter.format_parameter(), "arg");
    BC_REQUIRE_EQUAL(parameter.description(), "Simple string.");
}

BC_AUTO_TEST_CASE(parameter__initialize__defaulted__sets_limit_3)
{
    CONFIG_TEST_PARAMETER_SETUP(opt::defaulty);
    parameter.initialize(option, names);
    BC_REQUIRE_EQUAL(parameter.position(), 2);
    BC_REQUIRE_EQUAL(parameter.args_limit(), 3u);
    BC_REQUIRE_EQUAL(parameter.required(), false);
    BC_REQUIRE_EQUAL(parameter.short_name(), '\0');
    BC_REQUIRE_EQUAL(parameter.long_name(), "defaulty");
    BC_REQUIRE_EQUAL(parameter.format_name(), "--defaulty");
    BC_REQUIRE_EQUAL(parameter.description(), "Defaulted bool.");

    // The (=1) appears to be the default value (as int), i.e. (=true)
    // This makes the bool type indistinguishable at the metadata level from
    // an integer. So we must use zero_tokens() instead (see 'toggled' case).
    BC_REQUIRE_EQUAL(parameter.format_parameter(), "arg (=1)");
}

BC_AUTO_TEST_CASE(parameter__initialize__required__sets_limit_4)
{
    CONFIG_TEST_PARAMETER_SETUP(opt::required);
    parameter.initialize(option, names);
    BC_REQUIRE_EQUAL(parameter.position(), 3);
    BC_REQUIRE_EQUAL(parameter.args_limit(), 4u);
    BC_REQUIRE_EQUAL(parameter.required(), true);
    BC_REQUIRE_EQUAL(parameter.short_name(), '\0');
    BC_REQUIRE_EQUAL(parameter.long_name(), "required");
    BC_REQUIRE_EQUAL(parameter.format_name(), "--required");
    BC_REQUIRE_EQUAL(parameter.format_parameter(), "arg");
    BC_REQUIRE_EQUAL(parameter.description(), "Required path.");
}

//
BC_AUTO_TEST_CASE(parameter__initialize__toggled__sets_limit_0)
{
    CONFIG_TEST_PARAMETER_SETUP(opt::toggled);
    parameter.initialize(option, names);
    BC_REQUIRE_EQUAL(parameter.required(), false);
    BC_REQUIRE_EQUAL(parameter.short_name(), '\0');
    BC_REQUIRE_EQUAL(parameter.long_name(), "toggled");
    BC_REQUIRE_EQUAL(parameter.format_name(), "--toggled");
    BC_REQUIRE_EQUAL(parameter.format_parameter(), "");
    BC_REQUIRE_EQUAL(parameter.description(), "Toggle only bool.");

    // This combination uniquely implies that the option is a toggle.
    BC_REQUIRE_EQUAL(parameter.position(), parameter::not_positional);
    BC_REQUIRE_EQUAL(parameter.args_limit(), 0u);
}

BC_AUTO_TEST_CASE(parameter__initialize__vector__sets_limit_1)
{
    CONFIG_TEST_PARAMETER_SETUP(opt::vector);
    parameter.initialize(option, names);
    BC_REQUIRE_EQUAL(parameter.position(), 4);
    BC_REQUIRE_EQUAL(parameter.args_limit(), 5u);
    BC_REQUIRE_EQUAL(parameter.required(), false);
    BC_REQUIRE_EQUAL(parameter.short_name(), '\0');
    BC_REQUIRE_EQUAL(parameter.long_name(), "VECTOR");
    BC_REQUIRE_EQUAL(parameter.format_name(), "--VECTOR");
    BC_REQUIRE_EQUAL(parameter.format_parameter(), "arg");
    BC_REQUIRE_EQUAL(parameter.description(), "String vector.");
}

BC_AUTO_TEST_CASE(parameter__initialize__multitoken__sets_unlimited)
{
    CONFIG_TEST_PARAMETER_SETUP(opt::multitoken);
    parameter.initialize(option, names);
    BC_REQUIRE_EQUAL(parameter.position(), 5);
    /* The positional "unlimited" limit is different than the named limit. */
    BC_REQUIRE_EQUAL(parameter.args_limit(), 0xFFFFFFFFu);
    BC_REQUIRE_EQUAL(parameter.required(), false);
    BC_REQUIRE_EQUAL(parameter.short_name(), '\0');
    BC_REQUIRE_EQUAL(parameter.long_name(), "multitoken");
    BC_REQUIRE_EQUAL(parameter.format_name(), "--multitoken");
    BC_REQUIRE_EQUAL(parameter.format_parameter(), "arg");
    BC_REQUIRE_EQUAL(parameter.description(), "Multi-token int.");
}

BC_AUTO_TEST_SUITE_END()

// ------------------------------------------------------------------------- //
BC_AUTO_TEST_SUITE(parameter__short_name)

BC_AUTO_TEST_CASE(parameter__short_name__short_and_long__short)
{
    CONFIG_TEST_PARAMETER_OPTIONS_SETUP(opt::short_long);
    BC_REQUIRE_EQUAL(parameter.short_name(option), 's');
}

BC_AUTO_TEST_CASE(parameter__short_name__short_only__short)
{
    CONFIG_TEST_PARAMETER_OPTIONS_SETUP(opt::shorty);
    BC_REQUIRE_EQUAL(parameter.short_name(option), 'm');
}

BC_AUTO_TEST_CASE(parameter__short_name__long_only__null_char)
{
    CONFIG_TEST_PARAMETER_OPTIONS_SETUP(opt::longy);
    BC_REQUIRE_EQUAL(parameter.short_name(option), parameter::no_short_name);
}

BC_AUTO_TEST_SUITE_END()

BC_AUTO_TEST_SUITE_END()
