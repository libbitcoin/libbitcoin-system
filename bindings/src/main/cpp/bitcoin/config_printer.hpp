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
#ifndef LIBBITCOIN__CONFIG_PRINTER_HPP
#define LIBBITCOIN__CONFIG_PRINTER_HPP

//#include <iostream>
//#include <string>
//#include <vector>
//#include <boost/format.hpp>
//#include <boost/program_options.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/config/printer.hpp>
#include <config_parameter.hpp>
#include <config_argument_list.hpp>
#include <config_parameter_list.hpp>
#include <config_options_description.hpp>
#include <config_positional_options_description.hpp>
#include <string_vector.hpp>
//#include <p_std_string.hpp>

/* NOTE: don't declare 'using namespace foo' in headers. */

namespace libbitcoin {
//namespace config {
namespace api {

/**
 * Shorthand for property declarations in printer class.
 */
//#define BC_PROPERTY_GET_REF(type, name) \
//    public: virtual type& get_##name() { return name##_; } \
//    private: type name##_

/**
 * Class for managing the serialization of command line options and arguments.
 */
class BC_API config_printer
{
public:

    /**
     * Number of arguments above which the argument is considered unlimited.
     */
//    BC_API static const int max_arguments;

    /**
     * Construct an instance of the printer class.
     * @param[in]  settings     Populated config file settings metadata.
     * @param[in]  application  This application (e.g. 'bitcoin_server').
     * @param[in]  description  This application description (e.g. 'Server').
     */
    config_printer(const config_options_description& settings,
        const std::string& application, const std::string& description="");

    /**
     * Construct an instance of the printer class.
     * @param[in]  options      Populated command line options metadata.
     * @param[in]  arguments    Populated command line arguments metadata.
     * @param[in]  application  This application (e.g. 'bx').
     * @param[in]  description  This command description (e.g. 'Convert BTC').
     * @param[in]  command      This command (e.g. 'btc').
     */
    config_printer(const config_options_description& options,
        const config_positional_options_description& arguments,
        const std::string& application, const std::string& description="",
        const std::string& command="");

    virtual ~config_printer();

    /**
     * Convert a paragraph of text into a column.
     * This formats to 80 char width as: [ 23 | ' ' | 55 | '\n' ].
     * If one word exceeds width it will cause a column overflow.
     * This always sets at least one line and always collapses whitespace.
     * @param[in]  paragraph  The paragraph to columnize.
     * @return                The column, as a list of fragments.
     */
    virtual string_vector columnize(const std::string& paragraph,
        size_t width);

    /**
     * Format the command description.
     * @return  The command description.
     */
    virtual std::string format_description();

    /**
     * Format the parameters table.
     * @param[in]  positional  True for positional otherwize named.
     * @return                 The formatted help arguments table.
     */
    virtual std::string format_parameters_table(bool positional);

    /**
     * Format the settings table.
     * @return  The formatted settings table.
     */
    virtual std::string format_settings_table();

    /**
     * Format a paragraph.
     * @param[in]  paragraph  The text to format.
     * @return                The formatted paragraph.
     */
    virtual std::string format_paragraph(const std::string& paragraph);

    /**
     * Format the command line usage.
     * @return  The formatted usage.
     */
    virtual std::string format_usage();

    /**
     * Format the command line parameters.
     * @return  The formatted command line parameters.
     */
    virtual std::string format_usage_parameters();

    /**
     * Build the list of argument name/count tuples.
     */
    virtual void generate_argument_names();

    /**
     * Build the list of parameters.
     */
    virtual void generate_parameters();

    /**
     * Parse the arguments and options into the normalized parameter list.
     */
    virtual void initialize();

    /**
     * Serialize command line help (full details).
     * @param[out] output  Stream that is sink for output.
     */
//    virtual void commandline(std::ostream& output);

    /**
     * Serialize as config settings (full details).
     * @param[out] output  Stream that is sink for output.
     */
//    virtual void settings(std::ostream& output);

    /**
     * Virtual property declarations, passed on construct.
     */
//    BC_PROPERTY_GET_REF(config_options_description, options);
//    BC_PROPERTY_GET_REF(config_positional_options_description, arguments);
//    BC_PROPERTY_GET_REF(p_std_string, application);
//    BC_PROPERTY_GET_REF(p_std_string, description);
//    BC_PROPERTY_GET_REF(p_std_string, command);

    /**
     * Virtual property declarations, generated from metadata.
     */
//    BC_PROPERTY_GET_REF(config_argument_list, argument_names);
//    BC_PROPERTY_GET_REF(config_parameter_list, parameters);

public:
    config::printer getValue() {
        return value;
    }

    void setValue(config::printer value) {
        this->value = value;
    }
private:
    config::printer value;

};

#undef PROPERTY_GET_REF

} // namespace api
//} // namespace config
} // namespace libbitcoin

#endif
