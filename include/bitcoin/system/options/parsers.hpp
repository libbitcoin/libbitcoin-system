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
#ifndef LIBBITCOIN_SYSTEM_OPTIONS_PARSERS_HPP
#define LIBBITCOIN_SYSTEM_OPTIONS_PARSERS_HPP

#include <istream>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/options/description.hpp>
#include <bitcoin/system/options/variables.hpp>

namespace libbitcoin {
namespace system {
namespace options {

/// Parse command line arguments (excluding the program name), as long options
/// (--name, --name=value, --name value, unique abbreviation of name), short
/// options (-n, -nvalue, -n value, grouped switches -abc), and positional
/// arguments, as are all arguments following a "--" terminator.
BC_API parsed_options parse_command_line(int argc, const char* argv[],
    const options_description& options,
    const positional_options_description& arguments) THROWS;

/// Parse configuration settings ("name = value" within "[section]" lines,
/// named "section.name"), where "#" begins a comment and every name must be
/// declared.
BC_API parsed_options parse_config_file(std::istream& stream,
    const options_description& options) THROWS;

/// Parse the environment variables that begin with prefix, named by the
/// remainder in lower case, ignoring those not declared.
BC_API parsed_options parse_environment(const options_description& options,
    const std::string& prefix) THROWS;

} // namespace options
} // namespace system
} // namespace libbitcoin

#endif
