/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/config/parser.hpp>

#include <filesystem>
#include <sstream>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/unicode/utf8_everywhere/ifstream.hpp>

namespace libbitcoin {
namespace system {
namespace config {

using namespace boost::program_options;

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// The error is obtained from boost, which circumvents our localization.
// English-only hack to patch missing arg name in boost exception message.
std::string parser::format_invalid_parameter(
    const std::string& message) NOEXCEPT
{
    std::string clean_message(message);
    replace(clean_message, "for option is invalid", "is invalid");
    return "Error: " + clean_message;
}

std::filesystem::path parser::get_config_option(variables_map& variables,
    const std::string& name) NOEXCEPT
{
    // read config from the map so we don't require an early notify
    const auto& config = variables[name];

    // prevent exception in the case where the config variable is not set
    if (config.empty())
        return {};

    return config.as<std::filesystem::path>();
}

bool parser::get_option(variables_map& variables,
    const std::string& name) NOEXCEPT
{
    // Read settings from the map so we don't require an early notify call.
    const auto& variable = variables[name];

    // prevent exception in the case where the settings variable is not set.
    if (variable.empty())
        return false;

    return variable.as<bool>();
}

void parser::load_command_variables(variables_map& variables, int argc,
    const char* argv[]) THROWS
{
    const auto options = load_options();
    const auto arguments = load_arguments();
    auto command_parser = command_line_parser(argc, argv).options(options)
        /*.allow_unregistered()*/.positional(arguments);
    store(command_parser.run(), variables);
}

void parser::load_environment_variables(variables_map& variables,
    const std::string& prefix) THROWS
{
    const auto& environment_variables = load_environment();
    const auto environment = parse_environment(environment_variables, prefix);
    store(environment, variables);
}

bool parser::load_configuration_variables(variables_map& variables,
    const std::string& option_name) THROWS
{
    const auto config_settings = load_settings();
    const auto config_path = get_config_option(variables, option_name);

    // If the existence test errors out we pretend there's no file :/.
    std::error_code code;
    if (!config_path.empty() && exists(config_path, code))
    {
        const auto& path = config_path.string();
        ifstream file(path);

        if (!file.good())
            throw ifstream_exception(path.c_str());

        const auto config = parse_config_file(file, config_settings);
        store(config, variables);
        return true;
    }

    // Loading from an empty stream causes the defaults to populate.
    std::istringstream stream;
    const auto config = parse_config_file(stream, config_settings);
    store(config, variables);
    return false;
}

BC_POP_WARNING()

} // namespace config
} // namespace system
} // namespace libbitcoin
