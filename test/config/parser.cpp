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

namespace po = boost::program_options;
using namespace bc::system::config;

BOOST_AUTO_TEST_SUITE(parser_tests)

namespace {

// Minimal concrete parser exposing load_configuration_variables for testing.
class mock_parser
  : public parser
{
public:
    options_metadata load_options() THROWS override
    {
        options_metadata description;
        description.add_options()
            ("config,c", po::value<std::filesystem::path>(&configured_), "")
            ("with_default", po::value<bool>()->default_value(false), "")
            ("without_default", po::value<bool>(), "");
        return description;
    }

    arguments_metadata load_arguments() THROWS override
    {
        return {};
    }

    options_metadata load_environment() THROWS override
    {
        return {};
    }

    options_metadata load_settings() THROWS override
    {
        return {};
    }

    using parser::load_command_variables;
    using parser::load_configuration_variables;

private:
    std::filesystem::path configured_;
};

// Load --config <path> into the instance, as the command line would.
void configure(mock_parser& instance, const std::filesystem::path& path)
{
    const auto value = path.string();
    const char* argv[]{ "test", "--config", value.c_str() };
    instance.load_command_variables(3, argv);
}

} // namespace

BOOST_FIXTURE_TEST_SUITE(parser__load_configuration_variables, test::directory_setup_fixture)

BOOST_AUTO_TEST_CASE(parser__load_configuration_variables__directory__throws)
{
    mock_parser instance;
    configure(instance, TEST_DIRECTORY);
    BOOST_REQUIRE_THROW(instance.load_configuration_variables("config"), ifstream_exception);
}

BOOST_AUTO_TEST_CASE(parser__load_configuration_variables__nonexistent_file__throws)
{
    mock_parser instance;
    configure(instance, TEST_PATH);
    BOOST_REQUIRE_THROW(instance.load_configuration_variables("config"), ifstream_exception);
}

BOOST_AUTO_TEST_CASE(parser__load_configuration_variables__regular_file__returns_true)
{
    mock_parser instance;
    BOOST_REQUIRE(test::create(TEST_PATH));
    configure(instance, TEST_PATH);
    BOOST_REQUIRE(instance.load_configuration_variables("config"));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(parser__is_configured)

BOOST_AUTO_TEST_CASE(parser__is_configured__unparsed__false)
{
    const mock_parser instance;
    BOOST_REQUIRE(!instance.is_configured("config"));
}

BOOST_AUTO_TEST_CASE(parser__is_configured__unregistered__false)
{
    mock_parser instance;
    const char* argv[]{ "test" };
    instance.load_command_variables(1, argv);
    BOOST_REQUIRE(!instance.is_configured("nonexistent"));
}

BOOST_AUTO_TEST_CASE(parser__is_configured__unspecified_without_default__false)
{
    mock_parser instance;
    const char* argv[]{ "test" };
    instance.load_command_variables(1, argv);
    BOOST_REQUIRE(!instance.is_configured("without_default"));
}

BOOST_AUTO_TEST_CASE(parser__is_configured__unspecified_with_default__false)
{
    mock_parser instance;
    const char* argv[]{ "test" };
    instance.load_command_variables(1, argv);
    BOOST_REQUIRE(!instance.is_configured("with_default"));
}

BOOST_AUTO_TEST_CASE(parser__is_configured__specified_without_default__true)
{
    mock_parser instance;
    const char* argv[]{ "test", "--without_default", "true" };
    instance.load_command_variables(3, argv);
    BOOST_REQUIRE(instance.is_configured("without_default"));
}

BOOST_AUTO_TEST_CASE(parser__is_configured__specified_with_default__true)
{
    mock_parser instance;
    const char* argv[]{ "test", "--with_default", "true" };
    instance.load_command_variables(3, argv);
    BOOST_REQUIRE(instance.is_configured("with_default"));
}

BOOST_AUTO_TEST_CASE(parser__is_configured__specified_as_default_value__true)
{
    mock_parser instance;
    const char* argv[]{ "test", "--with_default", "false" };
    instance.load_command_variables(3, argv);
    BOOST_REQUIRE(instance.is_configured("with_default"));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()
