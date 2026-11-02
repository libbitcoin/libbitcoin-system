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
#include <bitcoin/system/options/parser.hpp>

#include <filesystem>
#include <sstream>
#include <bitcoin/system/config/config.hpp>
#include <bitcoin/system/options/parsers.hpp>
#include <bitcoin/system/options/setting.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/unicode/unicode.hpp>
#include <bitcoin/system/unicode/utf8_everywhere/utf8_everywhere.hpp>

namespace libbitcoin {
namespace system {
namespace options {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// English-only patch of an exception message that does not name its option.
std::string parser::format_invalid_parameter(
    const std::string& message) NOEXCEPT
{
    std::string clean_message(message);
    replace(clean_message, "for option is invalid", "is invalid");
    return "Error: " + clean_message;
}

const variables_map& parser::variables() const NOEXCEPT
{
    return variables_;
}

// Defaulted implies the value was declared by an option, not specified.
bool parser::is_configured(const std::string& name) const NOEXCEPT
{
    return options::is_configured(variables_, name);
}

std::filesystem::path parser::get_config_option(
    const std::string& name) const NOEXCEPT
{
    // read config from the map so we don't require an early notify
    const auto& config = variables_[name];

    // prevent exception in the case where the config variable is not set
    if (config.empty())
        return {};

    return config.as<config::path>();
}

bool parser::get_option(const std::string& name) const NOEXCEPT
{
    // Read settings from the map so we don't require an early notify call.
    const auto& variable = variables_[name];

    // prevent exception in the case where the settings variable is not set.
    if (variable.empty())
        return false;

    return variable.as<bool>();
}

const options_metadata& parser::settings() THROWS
{
    if (!settings_)
        settings_ = std::make_shared<options_metadata>(load_settings());

    return *settings_;
}

void parser::load_command_variables(int argc, const char* argv[]) THROWS
{
    const auto options = load_options();
    const auto arguments = load_arguments();
    store(parse_command_line(argc, argv, options, arguments), variables_);
}

void parser::load_environment_variables(const std::string& prefix) THROWS
{
    const auto environment = load_environment();
    store(parse_environment(environment, prefix), variables_);
}

bool parser::load_configuration_variables(
    const std::string& option_name) THROWS
{
    const auto& metadata = settings();
    const auto path = get_config_option(option_name);
    const auto extended = extended_path(path);

    std::error_code code{};
    if (!path.empty())
    {
        if (!std::filesystem::is_regular_file(extended, code))
            throw ifstream_exception{ from_path(extended).c_str() };

        ifstream file{ path };
        if (!file.good())
            throw ifstream_exception{ from_path(extended).c_str() };

        store(parse_config_file(file, metadata), variables_);
        return true;
    }

    // Loading from an empty stream causes the defaults to populate.
    std::istringstream stream{};
    store(parse_config_file(stream, metadata), variables_);
    return false;
}

BC_POP_WARNING()

} // namespace options
} // namespace system
} // namespace libbitcoin
