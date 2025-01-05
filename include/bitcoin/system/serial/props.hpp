/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_SERIAL_PROPS_HPP
#define LIBBITCOIN_SYSTEM_SERIAL_PROPS_HPP

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
class BC_API props
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(props);

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

    /// "null" property value.
    props() NOEXCEPT;

    /// "true" or "false" property value.
    props(truth boolean) NOEXCEPT;

    /// number property value. 
    props(int64_t number) NOEXCEPT;

    /// string property value, whitespace is not trimmed.
    props(const std::string& text) NOEXCEPT;

    /// Serialize the properties to the specified format.
    std::ostream& write(std::ostream& stream, format format,
        bool flat=true) const NOEXCEPT;

protected:
    typedef std::pair<std::string, props> named_props;
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

    static props from_number(int64_t number) NOEXCEPT;
    props(type type, const std::string& text={}) NOEXCEPT;
    bool is_complex() const NOEXCEPT;
    std::ostream& write(std::ostream& stream, format format,
        size_t depth, bool flat) const NOEXCEPT;
    std::ostream& write(std::ostream& stream, format format,
        size_t depth, bool flat, const std::string& name) const NOEXCEPT;

    type type_;
    std::string value_;
    std::vector<named_props> children_;
};

class BC_API array_props
  : public props
{
public:
    /// Construct an array properties value, name applied to all children.
    array_props(const std::string& name) NOEXCEPT;
    array_props(const std::string& name,
        std::initializer_list<props> values) NOEXCEPT;

    /// Add child array property values.
    array_props& add(const props& value) NOEXCEPT;
    array_props& add(std::initializer_list<props> values) NOEXCEPT;
};

class BC_API object_props
  : public props
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(object_props);

    /// Construct an object properties value, each child is explicitly named.
    object_props() NOEXCEPT;
    object_props(std::initializer_list<named_props> values) NOEXCEPT;

    /// Add child object property values.
    object_props& add(const named_props& value) NOEXCEPT;
    object_props& add(std::initializer_list<named_props> values) NOEXCEPT;
};

} // namespace system
} // namespace libbitcoin

#endif
