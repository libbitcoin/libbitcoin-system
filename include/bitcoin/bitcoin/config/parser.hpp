/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_CONFIG_PARSER_HPP
#define LIBBITCOIN_CONFIG_PARSER_HPP

#include <string>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <bitcoin/bitcoin/define.hpp>

namespace libbitcoin {

// These are just annoyingly long.
typedef boost::program_options::variables_map variables_map;
typedef boost::program_options::option_description option_metadata;
typedef boost::program_options::options_description options_metadata;
typedef boost::program_options::positional_options_description
    arguments_metadata;

namespace config {

#define PROPERTY(type, property) \
    value<type>()->notifier([&](type value) { property(value); })

/// Parse configurable values from environment variables, settings file, and
/// command line positional and non-positional options.
class BC_API parser
{
public:
    static std::string format_invalid_parameter(const std::string& message);
    static bool get_option(variables_map& variables, const std::string& name);
    static boost::filesystem::path get_config_option(variables_map& variables,
        const std::string& name);

    /// Load command line options (named).
    virtual options_metadata load_options() = 0;

    /// Load command line arguments (positional).
    virtual arguments_metadata load_arguments() = 0;

    /// Load environment variable settings.
    virtual options_metadata load_environment() = 0;

    /// Load configuration file settings.
    virtual options_metadata load_settings() = 0;

protected:
    virtual void load_command_variables(variables_map& variables,
        int argc, const char* argv[]);

    virtual bool load_configuration_variables(variables_map& variables,
        const std::string& option_name);

    virtual void load_environment_variables(variables_map& variables,
        const std::string& prefix);
};

} // namespace config
} // namespace libbitcoin

#endif
