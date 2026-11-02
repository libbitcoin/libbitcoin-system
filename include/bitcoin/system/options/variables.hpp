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
#ifndef LIBBITCOIN_SYSTEM_OPTIONS_VARIABLES_HPP
#define LIBBITCOIN_SYSTEM_OPTIONS_VARIABLES_HPP

#include <any>
#include <map>
#include <set>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/options/description.hpp>
#include <bitcoin/system/options/value.hpp>

namespace libbitcoin {
namespace system {
namespace options {

/// One occurrence of an option, as parsed from a source.
struct BC_API parsed_option
{
    /// The option key, empty for an unnamed positional argument.
    std::string key;

    /// The value tokens of the occurrence.
    string_list tokens;

    /// Positional, following the "--" terminator.
    bool terminated{};
};

/// The occurrences parsed from one source, against its declarations.
struct BC_API parsed_options
{
    /// The occurrences, in source order.
    std::vector<parsed_option> options;

    /// The declarations against which the source was parsed.
    const options_description* description;

    /// Options are named in messages as dashed on the command line.
    bool dashed;
};

/// Not thread safe, non-virtual.
/// The value of a variable, and whether it was declared as a default.
class BC_API variable_value
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(variable_value);

    variable_value() NOEXCEPT = default;
    variable_value(const std::any& value, bool defaulted,
        const std::shared_ptr<const value_semantic>& semantic) NOEXCEPT;

    /// The value as its type, throws std::bad_any_cast if not the type.
    template <typename Type>
    const Type& as() const THROWS
    {
        return std::any_cast<const Type&>(value_);
    }

    /// The value as its type, throws std::bad_any_cast if not the type.
    template <typename Type>
    Type& as() THROWS
    {
        return std::any_cast<Type&>(value_);
    }

    /// True if there is no value.
    bool empty() const NOEXCEPT;

    /// True if the value is the declared default.
    bool defaulted() const NOEXCEPT;

    /// The value.
    const std::any& value() const NOEXCEPT;
    std::any& value() NOEXCEPT;

    /// The semantic of the option that set the value, or null.
    const std::shared_ptr<const value_semantic>& semantic() const NOEXCEPT;

private:
    std::any value_{};
    bool defaulted_{};
    std::shared_ptr<const value_semantic> semantic_{};
};

/// Not thread safe, non-virtual.
/// Variables by name, stored from parsed sources in order of precedence.
class BC_API variables_map
  : public std::map<std::string, variable_value>
{
public:
    /// The named variable, empty if not present (does not insert).
    const variable_value& operator[](const std::string& name) const NOEXCEPT;

private:
    friend BC_API void store(const parsed_options& options,
        variables_map& variables) THROWS;
    friend BC_API void notify(variables_map& variables) THROWS;

    // Variables set explicitly by a non-composing option, which preclude
    // their assignment by subsequently stored sources.
    std::set<std::string> final_{};

    // Required variables, by name for display.
    std::map<std::string, std::string> required_{};
};

/// Store the parsed options, not overriding final values of preceding
/// sources, then store the defaults of declared options not yet present.
BC_API void store(const parsed_options& options,
    variables_map& variables) THROWS;

/// Throw if a required option is not present, then notify each value.
BC_API void notify(variables_map& variables) THROWS;

} // namespace options
} // namespace system

typedef system::options::variables_map variables_map;

} // namespace libbitcoin

#endif
