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
#include <bitcoin/system/options/variables.hpp>

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/options/description.hpp>
#include <bitcoin/system/options/value.hpp>

namespace libbitcoin {
namespace system {
namespace options {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// variable_value
// ----------------------------------------------------------------------------

variable_value::variable_value(const std::any& value, bool defaulted,
    const std::shared_ptr<const value_semantic>& semantic) NOEXCEPT
  : value_(value), defaulted_(defaulted), semantic_(semantic)
{
}

bool variable_value::empty() const NOEXCEPT
{
    return !value_.has_value();
}

bool variable_value::defaulted() const NOEXCEPT
{
    return defaulted_;
}

const std::any& variable_value::value() const NOEXCEPT
{
    return value_;
}

std::any& variable_value::value() NOEXCEPT
{
    return value_;
}

const std::shared_ptr<const value_semantic>&
variable_value::semantic() const NOEXCEPT
{
    return semantic_;
}

// variables_map
// ----------------------------------------------------------------------------

const variable_value& variables_map::operator[](
    const std::string& name) const NOEXCEPT
{
    static const variable_value empty{};
    const auto it = find(name);
    return it == cend() ? empty : it->second;
}

// store/notify
// ----------------------------------------------------------------------------

void store(const parsed_options& options, variables_map& variables) THROWS
{
    BC_ASSERT(!is_null(options.description));
    const auto& description = *options.description;
    std::set<std::string> final{};

    for (const auto& option: options.options)
    {
        if (option.key.empty() || variables.final_.contains(option.key))
            continue;

        const auto& declared = description.find(option.key, false);
        const auto semantic = declared.semantic();
        auto& variable = variables.try_emplace(option.key).first->second;

        // An explicit value replaces a default.
        if (variable.defaulted())
            variable = {};

        semantic->parse(variable.value(), option.tokens,
            declared.display_name(options.dashed));

        variable = { variable.value(), false, semantic };

        // Repetition is allowed within one source, but not across sources.
        if (!semantic->is_composing())
            final.insert(option.key);
    }

    variables.final_.insert(final.begin(), final.end());

    for (const auto& declared: description.options())
    {
        const auto& key = declared->key();
        const auto semantic = declared->semantic();

        std::any value{};
        if (!variables.contains(key) && semantic->apply_default(value))
            variables.try_emplace(key, value, true, semantic);

        // The dashed (command line) name is preferred for display.
        if (semantic->is_required())
        {
            const auto name = declared->display_name(options.dashed);
            auto& required = variables.required_[key];
            if (name.size() > required.size())
                required = name;
        }
    }
}

void notify(variables_map& variables) THROWS
{
    for (const auto& [key, name]: variables.required_)
        if (variables[key].empty())
            throw validation_exception{ "the option '" + name +
                "' is required but missing" };

    for (const auto& [key, variable]: variables)
        if (variable.semantic())
            variable.semantic()->notify(variable.value());
}

BC_POP_WARNING()

} // namespace options
} // namespace system
} // namespace libbitcoin
