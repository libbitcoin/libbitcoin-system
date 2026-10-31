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

BC_AUTO_TEST_SUITE(unit_test_settings_tests)

static bool parse(unit_test::settings& out, std::string& error,
    const std::vector<const char*>& arguments)
{
    std::vector<const char*> argv{ "test" };
    argv.insert(argv.end(), arguments.begin(), arguments.end());
    const auto argc = possible_narrow_sign_cast<int>(argv.size());
    return out.parse(error, argc, argv.data());
}

static const std::vector<std::pair<const char*, log_level>> log_levels
{
    { "--log_level=all", log_level::all },
    { "--log_level=success", log_level::all },
    { "--log_level=test_suite", log_level::test_suite },
    { "--log_level=message", log_level::message },
    { "--log_level=warning", log_level::warning },
    { "--log_level=error", log_level::error },
    { "--log_level=cpp_exception", log_level::cpp_exception },
    { "--log_level=system_error", log_level::system_error },
    { "--log_level=fatal_error", log_level::fatal_error },
    { "--log_level=nothing", log_level::nothing }
};

static const std::vector<std::pair<const char*, report_level>> report_levels
{
    { "--report_level=no", report_level::no },
    { "--report_level=confirm", report_level::confirm },
    { "--report_level=short", report_level::summary },
    { "--report_level=detailed", report_level::detailed }
};

BC_AUTO_TEST_CASE(unit_test_settings__parse__no_arguments__defaults)
{
    unit_test::settings instance{};
    std::string error{};
    BC_REQUIRE(parse(instance, error, {}));
    BC_REQUIRE(error.empty());
    BC_REQUIRE(instance.log == log_level::error);
    BC_REQUIRE(instance.report == report_level::confirm);
    BC_REQUIRE(!instance.show_progress);
    BC_REQUIRE(!instance.build_info);
    BC_REQUIRE(!instance.list_content);
    BC_REQUIRE(!instance.help);
    BC_REQUIRE_EQUAL(instance.detect_memory_leaks, 1u);
    BC_REQUIRE(instance.run_test.empty());
}

BC_AUTO_TEST_CASE(unit_test_settings__parse__all_options__expected)
{
    unit_test::settings instance{};
    std::string error{};
    BC_REQUIRE(parse(instance, error, { "--log_level=warning", "--report_level=short", "--show_progress=yes", "--build_info=yes", "--detect_memory_leaks=0", "--run_test=*" }));
    BC_REQUIRE(instance.log == log_level::warning);
    BC_REQUIRE(instance.report == report_level::summary);
    BC_REQUIRE(instance.show_progress);
    BC_REQUIRE(instance.build_info);
    BC_REQUIRE_EQUAL(instance.detect_memory_leaks, 0u);
    BC_REQUIRE_EQUAL(instance.run_test.size(), 1u);
    BC_REQUIRE_EQUAL(instance.run_test.front(), "*");
}

BC_AUTO_TEST_CASE(unit_test_settings__parse__log_levels__expected)
{
    for (const auto& level: log_levels)
    {
        unit_test::settings instance{};
        std::string error{};
        BC_REQUIRE(parse(instance, error, { level.first }));
        BC_REQUIRE(instance.log == level.second);
    }
}

BC_AUTO_TEST_CASE(unit_test_settings__parse__report_levels__expected)
{
    for (const auto& level: report_levels)
    {
        unit_test::settings instance{};
        std::string error{};
        BC_REQUIRE(parse(instance, error, { level.first }));
        BC_REQUIRE(instance.report == level.second);
    }
}

BC_AUTO_TEST_CASE(unit_test_settings__parse__flag_values__expected)
{
    unit_test::settings instance{};
    std::string error{};
    BC_REQUIRE(parse(instance, error, { "--show_progress" }));
    BC_REQUIRE(instance.show_progress);
    BC_REQUIRE(parse(instance, error, { "--show_progress=no" }));
    BC_REQUIRE(!instance.show_progress);
    BC_REQUIRE(parse(instance, error, { "--show_progress=true" }));
    BC_REQUIRE(instance.show_progress);
    BC_REQUIRE(parse(instance, error, { "--show_progress=0" }));
    BC_REQUIRE(!instance.show_progress);
}

BC_AUTO_TEST_CASE(unit_test_settings__parse__run_test_colons__split)
{
    unit_test::settings instance{};
    std::string error{};
    BC_REQUIRE(parse(instance, error, { "--run_test=a:b/c::!d" }));
    BC_REQUIRE_EQUAL(instance.run_test.size(), 3u);
    BC_REQUIRE_EQUAL(instance.run_test.at(0), "a");
    BC_REQUIRE_EQUAL(instance.run_test.at(1), "b/c");
    BC_REQUIRE_EQUAL(instance.run_test.at(2), "!d");
}

BC_AUTO_TEST_CASE(unit_test_settings__parse__short_run_test__appended)
{
    unit_test::settings instance{};
    std::string error{};
    BC_REQUIRE(parse(instance, error, { "-t", "a", "--run_test=b" }));
    BC_REQUIRE_EQUAL(instance.run_test.size(), 2u);
    BC_REQUIRE_EQUAL(instance.run_test.at(0), "a");
    BC_REQUIRE_EQUAL(instance.run_test.at(1), "b");
}

BC_AUTO_TEST_CASE(unit_test_settings__parse__short_run_test_missing_value__false)
{
    unit_test::settings instance{};
    std::string error{};
    BC_REQUIRE(!parse(instance, error, { "-t" }));
    BC_REQUIRE_EQUAL(error, "missing value for option '-t'");
}

BC_AUTO_TEST_CASE(unit_test_settings__parse__memory_leak_spellings__expected)
{
    unit_test::settings instance{};
    std::string error{};
    BC_REQUIRE(parse(instance, error, { "--detect_memory_leak=0" }));
    BC_REQUIRE_EQUAL(instance.detect_memory_leaks, 0u);
    BC_REQUIRE(parse(instance, error, { "--detect_memory_leaks=42" }));
    BC_REQUIRE_EQUAL(instance.detect_memory_leaks, 42u);
}

BC_AUTO_TEST_CASE(unit_test_settings__parse__list_content_help__true)
{
    unit_test::settings instance{};
    std::string error{};
    BC_REQUIRE(parse(instance, error, { "--list_content", "--help" }));
    BC_REQUIRE(instance.list_content);
    BC_REQUIRE(instance.help);
}

BC_AUTO_TEST_CASE(unit_test_settings__parse__unrecognized__false)
{
    unit_test::settings instance{};
    std::string error{};
    BC_REQUIRE(!parse(instance, error, { "--bogus=1" }));
    BC_REQUIRE_EQUAL(error, "unrecognized option '--bogus=1'");
}

BC_AUTO_TEST_CASE(unit_test_settings__parse__invalid_values__false)
{
    unit_test::settings instance{};
    std::string error{};
    BC_REQUIRE(!parse(instance, error, { "--log_level=loud" }));
    BC_REQUIRE_EQUAL(error, "invalid value for option '--log_level=loud'");
    BC_REQUIRE(!parse(instance, error, { "--report_level=long" }));
    BC_REQUIRE(!parse(instance, error, { "--show_progress=maybe" }));
    BC_REQUIRE(!parse(instance, error, { "--detect_memory_leaks=-1" }));
    BC_REQUIRE(!parse(instance, error, { "--list_content=DOT" }));
}

BC_AUTO_TEST_SUITE_END()
