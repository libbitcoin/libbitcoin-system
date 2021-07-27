/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_SERIAL_PROPERTIES_HPP
#define LIBBITCOIN_SYSTEM_SERIAL_PROPERTIES_HPP

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <string>
#include <vector>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

//// XML Preamble
////"<?xml version=\"1.0\" encoding=\"utf-8\"?>"

/// A very simple data structure for standard formats serialization.
/// XML attributes are not supported as there is no JSON analog. Stream
/// operator overrides are not provided because a format parameter is required.
/// Specialized types are not used for value because they must be stored in the
/// children collection, which then must be able to specialize serialization.
/// TODO: Escape sequences and JSON exponents and fractions are not supported.
class BC_API properties
{
public:
    typedef enum class truth
    {
        true_,
        false_
    } truth;

    typedef enum class format
    {
        json,
        info,
        xml
    } format;

    /// Move/copy construct.
    properties(properties&& other) noexcept;
    properties(const properties& other) noexcept;

    /// "null" property value.
    properties() noexcept;

    /// "true" or "false" property value.
    properties(truth boolean) noexcept;

    /// number property value. 
    properties(int64_t number) noexcept;

    /// string property value, whitespace is not trimmed.
    properties(const std::string& text) noexcept;

    /// Serialize the properties to the specified format.
    std::ostream& write(std::ostream& stream, format format,
        bool flat=true) const noexcept;

protected:
    typedef std::pair<std::string, properties> named_properties;
    typedef enum class type
    {
        null_,
        true_,
        false_,
        string_,
        number_,
        array_,
        object_,
        invalid_
    } type;

    static properties from_number(int64_t number) noexcept;
    properties(type type, const std::string& text={}) noexcept;
    bool is_complex() const noexcept;
    std::ostream& write(std::ostream& stream, format format,
        size_t depth, bool flat) const noexcept;
    std::ostream& write(std::ostream& stream, format format,
        size_t depth, bool flat, const std::string& name) const noexcept;

    const type type_;
    std::string value_;
    std::vector<named_properties> children_;
};

class BC_API array_properties
  : public properties
{
public:
    /// Construct an array properties value, name applied to all children.
    array_properties(const std::string& name) noexcept;
    array_properties(const std::string& name,
        std::initializer_list<properties> values) noexcept;

    /// Add child array property values.
    array_properties& add(const properties& value) noexcept;
    array_properties& add(std::initializer_list<properties> values) noexcept;
};

class BC_API object_properties
  : public properties
{
public:
    /// Construct an object properties value, each child is explicitly named.
    object_properties() noexcept;
    object_properties(std::initializer_list<named_properties> values) noexcept;

    /// Add child object property values.
    object_properties& add(const named_properties& value) noexcept;
    object_properties& add(
        std::initializer_list<named_properties> values) noexcept;
};

} // namespace system
} // namespace libbitcoin

#endif
