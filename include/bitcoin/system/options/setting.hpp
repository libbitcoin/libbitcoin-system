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
#ifndef LIBBITCOIN_SYSTEM_OPTIONS_SETTING_HPP
#define LIBBITCOIN_SYSTEM_OPTIONS_SETTING_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/options/value.hpp>
#include <bitcoin/system/options/variables.hpp>

namespace libbitcoin {
namespace system {
namespace options {

/// True if set by command line, environment, or configuration file.
/// False if unset or obtained from a declared option default value.
BC_API bool is_configured(const variables_map& variables,
    const std::string& name) NOEXCEPT;

/// The integer of the text, with optional comma digit grouping (1,000,000),
/// as decimal text, or the text unchanged if it is not such an integer.
BC_API std::string ungroup(const std::string& text) NOEXCEPT;

/// Not thread safe, virtual.
/// Interface for obtaining the current value of a bound option.
class BC_API printable
{
public:
    virtual ~printable() NOEXCEPT = default;

    /// The bound value(s), one per entry, empty only for a collection.
    virtual string_list values() const NOEXCEPT = 0;

    /// The declared default value, formatted for display.
    virtual std::string default_text() const NOEXCEPT = 0;
};

/// Not thread safe, virtual.
/// Option value that reports the current value of its store, and declares
/// the value of its store at construction as its default.
template <typename Type>
class setting_value
  : public typed_value<Type>,
    public printable
{
public:
    setting_value(Type* store, bool secret=false) THROWS;

    /// The bound value(s), one per entry, empty only for a collection.
    /// A secret reports one empty value, disclosing only that it is set.
    string_list values() const NOEXCEPT override;

    /// The declared default value, quoted, or 'empty' if it has no text.
    /// A secret reports empty text, disclosing nothing.
    std::string default_text() const NOEXCEPT override;

    /// Parse the tokens, accepting comma digit grouping of an integer.
    void parse(std::any& value, const string_list& tokens,
        const std::string& option) const THROWS override;

private:
    const Type* store_;
    bool secret_;
    std::string default_text_;
};

/// Declare a configuration file setting bound to store, defaulted to its value.
/// The default text replaces "{}" in the setting description.
template <typename Type>
setting_value<Type>* setting(Type* store) THROWS;

/// Declare a setting whose value is withheld from display.
template <typename Type>
setting_value<Type>* secret(Type* store) THROWS;

} // namespace options
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/options/setting.ipp>

#endif
