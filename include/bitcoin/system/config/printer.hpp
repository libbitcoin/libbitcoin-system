/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CONFIG_PRINTER_HPP
#define LIBBITCOIN_SYSTEM_CONFIG_PRINTER_HPP

#include <bitcoin/system/config/parameter.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace config {

/// Shorthand for property declarations in printer class.
#define BC_PROPERTY(type, name) \
    public: const type& name() const NOEXCEPT { return name##_; } \
    private: type name##_

/// Shorthand for reference getter declarations in printer class.
#define BC_PROPERTY_GET_REF(type, name) \
    public: type& get_##name() NOEXCEPT { return name##_; } \
    private: type name##_

/// Class for managing the serialization of command line options and arguments.
class BC_API printer final
{
public:
    /// Number of arguments above which the argument is considered unlimited.
    static const int max_arguments;

    /// Construct an instance of the printer class.
    /// settings     Populated config file settings metadata.
    /// application  This application (e.g. 'bitcoin_server').
    /// description  This application description (e.g. 'Server').
    printer(const options_metadata& settings, const std::string& application,
        const std::string& description="") NOEXCEPT;

    /// Construct an instance of the printer class.
    /// options      Populated command line options metadata.
    /// arguments    Populated command line arguments metadata.
    /// application  This application (e.g. 'bx').
    /// description  This command description (e.g. 'Convert BTC').
    /// command      This command (e.g. 'btc').
    printer(const options_metadata& options,
        const arguments_metadata& arguments, const std::string& application,
        const std::string& description="",
        const std::string& command="") NOEXCEPT;

    /// Convert a paragraph of text into a column.
    /// This formats to 80 char width as: [ 23 | ' ' | 55 | '\n' ].
    /// If one word exceeds width it will cause a column overflow.
    /// This always sets at least one line and always collapses whitespace.
    /// paragraph  The paragraph to columnize.
    /// return     The column, as a list of fragments.
    std::vector<std::string> columnize(const std::string& paragraph,
        size_t width) NOEXCEPT;

    /// Format stuff.
    std::string format_description() NOEXCEPT;
    std::string format_parameters_table(bool positional) NOEXCEPT;
    std::string format_settings_table() NOEXCEPT;
    std::string format_paragraph(const std::string& paragraph) NOEXCEPT;
    std::string format_usage() NOEXCEPT;
    std::string format_usage_parameters() NOEXCEPT;

    /// Build the list of argument name/count tuples.
    void generate_argument_names() NOEXCEPT;

    /// Build the list of parameters.
    void generate_parameters() NOEXCEPT;

    /// Parse the arguments and options into the normalized parameter list.
    void initialize() NOEXCEPT;

    /// Serialize command line help (full details).
    void commandline(std::ostream& output) NOEXCEPT;

    /// Serialize as config settings (full details).
    void settings(std::ostream& output) NOEXCEPT;

    /// Virtual property declarations, passed on construct.
    BC_PROPERTY(options_metadata, options);
    BC_PROPERTY(arguments_metadata, arguments);
    BC_PROPERTY(std::string, application);
    BC_PROPERTY(std::string, description);
    BC_PROPERTY(std::string, command);

    /// Virtual property declarations, generated from metadata.
    BC_PROPERTY_GET_REF(argument_list, argument_names);
    BC_PROPERTY_GET_REF(parameter_list, parameters);
};

#undef BC_PROPERTY
#undef BC_PROPERTY_GET_REF

} // namespace config
} // namespace system
} // namespace libbitcoin

#endif
