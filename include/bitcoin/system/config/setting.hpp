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
#ifndef LIBBITCOIN_SYSTEM_CONFIG_SETTING_HPP
#define LIBBITCOIN_SYSTEM_CONFIG_SETTING_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/unicode/unicode.hpp>

namespace libbitcoin {
namespace system {
namespace config {

/// True if set by command line, environment, or configuration file.
/// False if unset or obtained from a declared option default value.
BC_API bool is_configured(const variables_map& variables,
    const std::string& name) NOEXCEPT;

/// Not thread safe, virtual.
/// Interface for obtaining the current value of a bound option.
class BC_API printable
{
public:
    virtual ~printable() NOEXCEPT = default;

    /// The bound value(s), one per entry, empty only for a collection.
    virtual string_list values() const NOEXCEPT = 0;
};

/// Not thread safe, virtual.
/// Option value that reports the current value of its store.
template <typename Type>
class setting_value
  : public boost::program_options::typed_value<Type>,
    public printable
{
public:
    setting_value(Type* store, bool secret=false) THROWS;

    /// The bound value(s), one per entry, empty only for a collection.
    /// A secret reports one empty value, disclosing only that it is set.
    string_list values() const NOEXCEPT override;

private:
    const Type* store_;
    bool secret_;
};

/// Declare a configuration file setting bound to store.
template <typename Type>
setting_value<Type>* setting(Type* store) THROWS;

/// Declare a setting whose value is withheld from display.
template <typename Type>
setting_value<Type>* secret(Type* store) THROWS;

} // namespace config
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/config/setting.ipp>

#endif
