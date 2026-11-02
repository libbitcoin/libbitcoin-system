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

BC_AUTO_TEST_SUITE(parsers_tests)

struct parsers_setup_fixture
{
    parsers_setup_fixture()
    {
        options.add_options()
            ("help,h", value<bool>()->zero_tokens(), "")
            ("verbose,v", value<bool>()->zero_tokens(), "")
            ("number,n", value<int32_t>(), "")
            ("list,l", value<string_list>()->multitoken(), "")
            ("daemon,d", value<bool>()->implicit_value(true), "")
            ("DATA", value<string_list>(), "")
            ("FILE", value<std::string>(), "");

        arguments.add("FILE", 1).add("DATA", -1);
    }

    variables_map parse(const std::vector<const char*>& arguments_) const
    {
        std::vector<const char*> argv{ "test" };
        argv.insert(argv.end(), arguments_.begin(), arguments_.end());

        variables_map variables{};
        store(parse_command_line(possible_narrow_and_sign_cast<int>(argv.size()), argv.data(), options, arguments), variables);
        return variables;
    }

    options_description options{};
    positional_options_description arguments{};
};

BC_FIXTURE_TEST_SUITE(command_line_tests, parsers_setup_fixture)

BC_AUTO_TEST_CASE(parse_command_line__empty__defaults_only)
{
    BC_REQUIRE(parse({}).empty());
}

BC_AUTO_TEST_CASE(parse_command_line__long_switch__true)
{
    BC_REQUIRE(parse({ "--help" })["help"].as<bool>());
}

BC_AUTO_TEST_CASE(parse_command_line__long_abbreviated__true)
{
    BC_REQUIRE(parse({ "--he" })["help"].as<bool>());
}

BC_AUTO_TEST_CASE(parse_command_line__long_equals__value)
{
    BC_REQUIRE_EQUAL(parse({ "--number=42" })["number"].as<int32_t>(), 42);
}

BC_AUTO_TEST_CASE(parse_command_line__long_next__value)
{
    BC_REQUIRE_EQUAL(parse({ "--number", "42" })["number"].as<int32_t>(), 42);
}

BC_AUTO_TEST_CASE(parse_command_line__long_equals_empty__throws)
{
    BC_REQUIRE_EXCEPTION(parse({ "--number=" }), validation_exception, [](const validation_exception& ex) { return std::string{ ex.what() } == "the argument for option '--number' should follow immediately after the equal sign"; });
}

BC_AUTO_TEST_CASE(parse_command_line__short_adjacent__value)
{
    BC_REQUIRE_EQUAL(parse({ "-n42" })["number"].as<int32_t>(), 42);
}

BC_AUTO_TEST_CASE(parse_command_line__short_next__value)
{
    BC_REQUIRE_EQUAL(parse({ "-n", "-42" })["number"].as<int32_t>(), -42);
}

BC_AUTO_TEST_CASE(parse_command_line__grouped_switches__all_set)
{
    const auto variables = parse({ "-hv" });
    BC_REQUIRE(variables["help"].as<bool>());
    BC_REQUIRE(variables["verbose"].as<bool>());
}

BC_AUTO_TEST_CASE(parse_command_line__grouped_switches_value__value)
{
    const auto variables = parse({ "-hvn42" });
    BC_REQUIRE(variables["help"].as<bool>());
    BC_REQUIRE(variables["verbose"].as<bool>());
    BC_REQUIRE_EQUAL(variables["number"].as<int32_t>(), 42);
}

BC_AUTO_TEST_CASE(parse_command_line__multitoken__following_arguments)
{
    const auto variables = parse({ "--list", "a", "b", "c", "-h" });
    BC_REQUIRE_EQUAL(variables["list"].as<string_list>(), (string_list{ "a", "b", "c" }));
    BC_REQUIRE(variables["help"].as<bool>());
}

BC_AUTO_TEST_CASE(parse_command_line__implicit_alone__implicit)
{
    BC_REQUIRE(parse({ "--daemon" })["daemon"].as<bool>());
}

BC_AUTO_TEST_CASE(parse_command_line__implicit_next__value)
{
    BC_REQUIRE(!parse({ "-d", "false" })["daemon"].as<bool>());
}

BC_AUTO_TEST_CASE(parse_command_line__positional__named_by_position)
{
    const auto variables = parse({ "file", "-h", "one", "two" });
    BC_REQUIRE_EQUAL(variables["FILE"].as<std::string>(), "file");
    BC_REQUIRE_EQUAL(variables["DATA"].as<string_list>(), (string_list{ "one", "two" }));
}

BC_AUTO_TEST_CASE(parse_command_line__dash__positional)
{
    BC_REQUIRE_EQUAL(parse({ "-" })["FILE"].as<std::string>(), "-");
}

BC_AUTO_TEST_CASE(parse_command_line__terminator__following_positional)
{
    const auto variables = parse({ "--", "-h", "--number" });
    BC_REQUIRE_EQUAL(variables["FILE"].as<std::string>(), "-h");
    BC_REQUIRE_EQUAL(variables["DATA"].as<string_list>(), (string_list{ "--number" }));
    BC_REQUIRE(!variables.contains("help"));
}

BC_AUTO_TEST_CASE(parse_command_line__unknown_long__throws)
{
    BC_REQUIRE_EXCEPTION(parse({ "--bogus" }), validation_exception, [](const validation_exception& ex) { return std::string{ ex.what() } == "unrecognised option '--bogus'"; });
}

BC_AUTO_TEST_CASE(parse_command_line__unknown_short__throws)
{
    BC_REQUIRE_EXCEPTION(parse({ "-hz" }), validation_exception, [](const validation_exception& ex) { return std::string{ ex.what() } == "unrecognised option '-z'"; });
}

BC_AUTO_TEST_CASE(parse_command_line__missing_value__throws)
{
    BC_REQUIRE_EXCEPTION(parse({ "--number" }), validation_exception, [](const validation_exception& ex) { return std::string{ ex.what() } == "the required argument for option '--number' is missing"; });
}

BC_AUTO_TEST_CASE(parse_command_line__short_option_as_value__throws)
{
    BC_REQUIRE_THROW(parse({ "--number", "-h" }), validation_exception);
}

BC_AUTO_TEST_CASE(parse_command_line__dashed_value__value)
{
    BC_REQUIRE_EQUAL(parse({ "--number", "-42" })["number"].as<int32_t>(), -42);
    BC_REQUIRE_EQUAL(parse({ "--FILE", "--help" })["FILE"].as<std::string>(), "--help");
}

BC_AUTO_TEST_CASE(parse_command_line__switch_value__throws)
{
    BC_REQUIRE_EXCEPTION(parse({ "--help=yes" }), validation_exception, [](const validation_exception& ex) { return std::string{ ex.what() } == "option '--help' does not take any arguments"; });
}

BC_AUTO_TEST_CASE(parse_command_line__repeated_scalar__throws)
{
    BC_REQUIRE_THROW(parse({ "-n1", "-n2" }), validation_exception);
}

BC_AUTO_TEST_CASE(parse_command_line__invalid_value__throws_named)
{
    BC_REQUIRE_EXCEPTION(parse({ "-nx" }), istream_exception, [](const istream_exception& ex) { return std::string{ ex.what() } == "the argument ('x') for option '--number' is invalid"; });
}

BC_AUTO_TEST_CASE(parse_command_line__too_many_positional__throws)
{
    const options_description none{};
    const positional_options_description nothing{};
    const char* argv[]{ "test", "extra" };
    BC_REQUIRE_THROW(parse_command_line(2, argv, none, nothing), validation_exception);
}

BC_AUTO_TEST_SUITE_END()

BC_AUTO_TEST_SUITE(config_file_tests)

BC_AUTO_TEST_CASE(parse_config_file__sections__prefixed_names)
{
    options_description options{};
    options.add_options()
        ("top.value", value<std::string>(), "")
        ("table.header.rate", value<uint32_t>(), "")
        ("table.header.list", value<string_list>(), "");

    std::istringstream stream{ "[top]\nvalue = a b  # comment\n\n# ignored\n[table.header]\n  rate=42\nlist = x\nlist =\n" };
    variables_map variables{};
    store(parse_config_file(stream, options), variables);
    BC_REQUIRE_EQUAL(variables["top.value"].as<std::string>(), "a b");
    BC_REQUIRE_EQUAL(variables["table.header.rate"].as<uint32_t>(), 42u);
    BC_REQUIRE_EQUAL(variables["table.header.list"].as<string_list>(), (string_list{ "x", "" }));
}

BC_AUTO_TEST_CASE(parse_config_file__crlf__trimmed)
{
    options_description options{};
    options.add_options()("section.value", value<std::string>(), "");

    std::istringstream stream{ "[section]\r\nvalue = text\r\n" };
    variables_map variables{};
    store(parse_config_file(stream, options), variables);
    BC_REQUIRE_EQUAL(variables["section.value"].as<std::string>(), "text");
}

BC_AUTO_TEST_CASE(parse_config_file__empty__defaults)
{
    options_description options{};
    options.add_options()("section.value", value<uint32_t>()->default_value(42), "");

    std::istringstream stream{};
    variables_map variables{};
    store(parse_config_file(stream, options), variables);
    BC_REQUIRE_EQUAL(variables["section.value"].as<uint32_t>(), 42u);
    BC_REQUIRE(variables["section.value"].defaulted());
}

BC_AUTO_TEST_CASE(parse_config_file__undeclared__throws)
{
    const options_description options{};
    std::istringstream stream{ "[section]\nbogus = 1\n" };
    BC_REQUIRE_EXCEPTION(parse_config_file(stream, options), validation_exception, [](const validation_exception& ex) { return std::string{ ex.what() } == "unrecognised option 'section.bogus'"; });
}

BC_AUTO_TEST_CASE(parse_config_file__invalid_line__throws)
{
    const options_description options{};
    std::istringstream stream{ "[section]\nbogus\n" };
    BC_REQUIRE_THROW(parse_config_file(stream, options), validation_exception);
}

BC_AUTO_TEST_CASE(parse_config_file__invalid_value__throws_undashed)
{
    options_description options{};
    options.add_options()("section.value", value<uint32_t>(), "");

    std::istringstream stream{ "[section]\nvalue = x\n" };
    variables_map variables{};
    BC_REQUIRE_EXCEPTION(store(parse_config_file(stream, options), variables), istream_exception, [](const istream_exception& ex) { return std::string{ ex.what() } == "the argument ('x') for option 'section.value' is invalid"; });
}

BC_AUTO_TEST_SUITE_END()

BC_AUTO_TEST_SUITE(environment_tests)

BC_AUTO_TEST_CASE(parse_environment__undeclared__empty)
{
    const options_description options{};
    BC_REQUIRE(parse_environment(options, "BC_TEST_UNDECLARED_").options.empty());
}

BC_AUTO_TEST_SUITE_END()

BC_AUTO_TEST_SUITE_END()
