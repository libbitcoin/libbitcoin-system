/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/serial/props.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <string>
#include <vector>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/serial/serial.hpp>
#include <bitcoin/system/stream/stream.hpp>
#include <bitcoin/system/unicode/unicode.hpp>

namespace libbitcoin {
namespace system {

// locals
static const std::string null_text{ "null" };
static const std::string true_text{ "true" };
static const std::string false_text{ "false" };

props::props(props&& other) noexcept
  : type_(other.type_),
    value_(std::move(other.value_)),
    children_(std::move(other.children_))
{
}

props::props(const props& other) noexcept
  : type_(other.type_),
    value_(other.value_),
    children_(other.children_)
{
}

props::props() noexcept
  : props(type::null_)
{
}

props::props(truth boolean) noexcept
  : props(boolean == truth::true_ ? type::true_ : type::false_)
{
}

props::props(int64_t number) noexcept
  : props(from_number(number))
{
}

props::props(const std::string& text) noexcept
  : props(type::string_, text)
{
}

// private
props::props(type type, const std::string& text) noexcept
  : type_(type),
    value_(text),
    children_()
{
}

// private static
props props::from_number(int64_t number) noexcept
{
    // datatracker.ietf.org/doc/html/rfc7159#section-6
    // limit to [-(2**53) + 1, (2**53) - 1] for interoperability.
    constexpr auto power = power2<int64_t>(53);

    if (number <= -add1(power) || number >= sub1(power))
        return { type::invalid_ };

    return { type::number_, serialize(number) };
}

std::ostream& props::write(std::ostream& stream, format format,
    bool flat) const noexcept
{
    return write(stream, format, zero, flat);
}

constexpr size_t tab = 4;

bool props::is_complex() const noexcept
{
    return type_ == type::array_ || type_ == type::object_;
}

// private
std::ostream& props::write(std::ostream& stream, format format, size_t depth,
    bool flat, const std::string& name) const noexcept
{
    const auto end = flat ? "" : ("\n" + std::string(depth * tab, ' '));
    const auto start = flat ? "" : (is_zero(depth) ? "" : end);

    // Keep simple values on same line.
    const auto is_simple = [&]() noexcept
    {
        return type_ != type::array_ && type_ != type::object_;
    };

    switch (format)
    {
        case props::format::json:
        {
            // JSON quotes names and uses colon for property name delimiter.
            stream << start << "\"" << name << "\":";
            return write(stream, format, is_simple() ? zero : depth, flat);
        }
        case props::format::info:
        {
            // Info does not quote and uses space for property name delimiter.
            stream << start << name << " ";
            return write(stream, format, is_simple() ? zero : depth, flat);
        }
        case props::format::xml:
        {
            stream << start << "<" << name << ">";

            if (is_simple())
            {
                write(stream, format, zero, flat);
                stream << "</" << name << ">";
            }
            else
            {
                write(stream, format, add1(depth), flat);
                stream << end << "</" << name << ">";
            }

            return stream;
        }
        default:
        {
            return stream_result(stream, false);
        }
    }
}

// private
std::ostream& props::write(std::ostream& stream,
    format format, size_t depth, bool flat) const noexcept
{
    const auto end = flat ? "" : ("\n" + std::string(depth * tab, ' '));
    const auto start = flat ? "" : (is_zero(depth) ? "" : end);

    switch (type_)
    {
        case type::null_:
        {
            stream << start;
            switch (format)
            {
                case props::format::json:
                case props::format::info:
                {
                    stream << null_text;
                    return stream;
                }
                case props::format::xml:
                {
                    // This no-value is distinct from empty quotes (string).
                    return stream;
                }
                default:
                {
                    return stream_result(stream, false);
                }
            }
        }
        case type::true_:
        {
            stream << start;
            switch (format)
            {
                case props::format::json:
                case props::format::info:
                case props::format::xml:
                {
                    // For XML this is unique to our implementation.
                    stream << true_text;
                    return stream;
                }
                default:
                {
                    return stream_result(stream, false);
                }
            }
        }
        case type::false_:
        {
            stream << start;
            switch (format)
            {
                case props::format::json:
                case props::format::info:
                case props::format::xml:
                {
                    // For XML this is unique to our implementation.
                    stream << false_text;
                    return stream;
                }
                default:
                {
                    return stream_result(stream, false);
                }
            }
        }
        case type::string_:
        {
            stream << start;
            switch (format)
            {
                case props::format::json:
                {
                    stream << "\"" << value_ << "\"";
                    return stream;
                }
                case props::format::info:
                {
                    // Unquoted rendering for contiguous text.
                    if (has_whitespace(value_))
                        stream << "\"" << value_ << "\"";
                    else
                        stream << value_;

                    return stream;
                }
                case props::format::xml:
                {
                    stream << value_;
                    return stream;
                }
                default:
                {
                    return stream_result(stream, false);
                }
            }
        }
        case type::number_:
        {
            stream << start;
            switch (format)
            {
                case props::format::json:
                case props::format::info:
                case props::format::xml:
                {
                    stream << value_;
                    return stream;
                }
                default:
                {
                    return stream_result(stream, false);
                }
            }
        }
        case type::array_:
        {
            switch (format)
            {
                case props::format::json:
                {
                    stream << start << "[";

                    if (!children_.empty())
                    {
                        for (auto child = children_.begin();
                            child != std::prev(children_.end()); ++child)
                            child->second.write(stream, format, add1(depth),
                                flat) << ",";

                        children_.back().second.write(stream, format,
                            add1(depth), flat);
                    }

                    stream << end << "]";
                    return stream;
                }
                case props::format::info:
                {
                    // Info array uses curved array braces, not square.
                    stream << start << "{";

                    if (!children_.empty())
                    {
                        // Info uses space or line feed array element sep.
                        for (auto child = children_.begin();
                            child != std::prev(children_.end()); ++child)
                            child->second.write(stream, format, add1(depth),
                                flat) << (flat ? " " : "");

                        children_.back().second.write(stream, format,
                            add1(depth), flat);
                    }

                    stream << end << "}";
                    return stream;
                }
                case props::format::xml:
                {
                    for (const auto& child: children_)
                        child.second.write(stream, format, depth, flat,
                            child.first);

                    return stream;
                }
                default:
                {
                    return stream_result(stream, false);
                }
            }
        }
        case type::object_:
        {
            switch (format)
            {
                case props::format::json:
                {
                    stream << start << "{";

                    if (!children_.empty())
                    {
                        for (auto child = children_.begin();
                            child != std::prev(children_.end()); ++child)
                            child->second.write(stream, format, add1(depth),
                                flat, child->first) << ",";

                        children_.back().second.write(stream, format,
                            add1(depth), flat, children_.back().first);
                    }

                    stream << end << "}";
                    return stream;
                }
                case props::format::info:
                {
                    stream << start << "{";

                    if (!children_.empty())
                    {
                        // Info uses comma or line feed for named prop sep.
                        for (auto child = children_.begin();
                            child != std::prev(children_.end()); ++child)
                            child->second.write(stream, format, add1(depth),
                                flat, child->first) << (flat ? "," : "");

                        children_.back().second.write(stream, format,
                            add1(depth), flat, children_.back().first);
                    }

                    stream << end << "}";
                    return stream;
                }
                case props::format::xml:
                {
                    for (const auto& child: children_)
                        child.second.write(stream, format, depth, flat,
                            child.first);

                    return stream;
                }
                default:
                {
                    return stream_result(stream, false);
                }
            }
        }
        case type::invalid_:
        default:
        {
            return stream_result(stream, false);
        }
    }
}


// array_properties
// ----------------------------------------------------------------------------

array_props::array_props(const std::string& name) noexcept
  : props(type::array_, name)
{
    // Unused value_ member overloaded to store all-element name.
    // Name is then propagated to each child element upon add.
}

array_props::array_props(const std::string& name,
    std::initializer_list<props> values) noexcept
  : array_props(name)
{
    add(values);
}

array_props& array_props::add(const props& value) noexcept
{
    children_.emplace_back(value_, value);
    return *this;
}

array_props& array_props::add(std::initializer_list<props> values) noexcept
{
    std::for_each(values.begin(), values.end(), [&](const props& value)
    {
        add(value);
    });

    return *this;
}

// object_properties
// ----------------------------------------------------------------------------

object_props::object_props() noexcept
  : props(type::object_)
{
}

object_props::object_props(std::initializer_list<named_props> values) noexcept
  : object_props()
{
    add(values);
}

object_props& object_props::add(const named_props& value) noexcept
{
    children_.push_back(value);
    return *this;
}

object_props& object_props::add(
    std::initializer_list<named_props> values) noexcept
{
    std::for_each(values.begin(), values.end(), [&](const named_props& value)
    {
        add(value);
    });

    return *this;
}

} // namespace system
} // namespace libbitcoin
