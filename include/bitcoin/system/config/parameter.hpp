/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CONFIG_PARAMETER_HPP
#define LIBBITCOIN_SYSTEM_CONFIG_PARAMETER_HPP

#include <string>
#include <utility>
#include <vector>
#include <boost/program_options.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace config {

/// Shorthand for property declarations in parameter class.
#define BC_PROPERTY(type, name) \
    public: virtual type name() const { return name##_; } \
    public: virtual void set_##name(type value) { name##_ = value; } \
    private: type name##_

/// A tuple to represent a positional argument name count.
typedef std::pair<const std::string, int> argument_pair;

/// A type to represent the list of positional argument name counts.
typedef std::vector<argument_pair> argument_list;

/// A type to represent a list of parameter structures.
class parameter;
typedef std::vector<parameter> parameter_list;

/// Normalized storage for command line arguments and options.
/// TEST: option_metadata does not provide virtual methods so must wrap to mock.
class BC_API parameter
{
private:

    /// Enumerated options for selecting the canonical name.
    enum search_options : int
    {
        /// --name/-n
        dashed_both_prefer_long = 1,

        /// name/-n
        dashed_short_prefer_long = 2,

        /// -n/name
        dashed_short_prefer_short = 4
    };

public:

    /// Sentinel - the option is not a positional argument.
    static const int not_positional;

    /// Sentinel - there is no short name.
    static const char no_short_name;

    /// The character used to prefix command line options.
    static const char option_prefix_char;

    virtual ~parameter();

    /// Populate with normalized parameter data.
    /// option     The metadata of the option to test.
    /// arguments  The list of supported positional arguments.
    virtual void initialize(
        const boost::program_options::option_description& option,
        const argument_list& arguments);

    /// Determine if the option is an argument by testing for it by name in the
    /// positional options collection and if so return the position.
    /// option     The metadata of the option to position.
    /// arguments  The list of supported positional arguments.
    /// return     Relative position or -1 if not positional.
    virtual int position(
        const boost::program_options::option_description& option,
        const argument_list& arguments) const;

    /// Get the value for the args_limit property.
    /// position   The option's position (or -1 of arg).
    /// arguments  The argument names list.
    /// return     The arguments limit value for the option.
    unsigned arguments_limit(int position,
        const boost::program_options::option_description& option,
        const argument_list& arguments) const;

    /// Get the option's short name character or zero.
    /// option  The metadata of the option to test.
    /// return  The short name character or null character.
    virtual char short_name(
        const boost::program_options::option_description& option) const;

    /// Virtual property declarations.
    BC_PROPERTY(int, position);
    BC_PROPERTY(bool, required);
    BC_PROPERTY(char, short_name);
    BC_PROPERTY(unsigned, args_limit);
    BC_PROPERTY(std::string, long_name);
    BC_PROPERTY(std::string, description);
    BC_PROPERTY(std::string, format_name);
    BC_PROPERTY(std::string, format_parameter);
};

#undef BC_PROPERTY

} // namespace config
} // namespace system
} // namespace libbitcoin

#endif
