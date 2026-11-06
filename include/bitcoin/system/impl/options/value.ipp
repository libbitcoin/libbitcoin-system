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
#ifndef LIBBITCOIN_SYSTEM_IMPL_OPTIONS_VALUE_IPP
#define LIBBITCOIN_SYSTEM_IMPL_OPTIONS_VALUE_IPP

#include <charconv>
#include <filesystem>
#include <locale>
#include <sstream>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/unicode/unicode.hpp>
#include <bitcoin/system/unicode/utf8_everywhere/utf8_everywhere.hpp>

namespace libbitcoin {
namespace system {
namespace options {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

template <typename Type>
typed_value<Type>::typed_value(Type* store) NOEXCEPT
  : store_(store)
{
}

template <typename Type>
typed_value<Type>* typed_value<Type>::default_value(
    const Type& value) NOEXCEPT
{
    return default_value(value, to_text(value));
}

template <typename Type>
typed_value<Type>* typed_value<Type>::default_value(const Type& value,
    const std::string& text) NOEXCEPT
{
    default_ = value;
    default_text_ = text;
    return this;
}

template <typename Type>
typed_value<Type>* typed_value<Type>::implicit_value(
    const Type& value) NOEXCEPT
{
    implicit_ = value;
    implicit_text_ = to_text(value);
    return this;
}

template <typename Type>
typed_value<Type>* typed_value<Type>::notifier(
    const handler& notify) NOEXCEPT
{
    notifier_ = notify;
    return this;
}

template <typename Type>
typed_value<Type>* typed_value<Type>::composing() NOEXCEPT
{
    composing_ = true;
    return this;
}

template <typename Type>
typed_value<Type>* typed_value<Type>::multitoken() NOEXCEPT
{
    multitoken_ = true;
    return this;
}

template <typename Type>
typed_value<Type>* typed_value<Type>::zero_tokens() NOEXCEPT
{
    zero_tokens_ = true;
    return this;
}

template <typename Type>
typed_value<Type>* typed_value<Type>::required() NOEXCEPT
{
    required_ = true;
    return this;
}

template <typename Type>
std::string typed_value<Type>::name() const NOEXCEPT
{
    const std::string argument{ "arg" };
    const auto defaulted = default_.has_value() && !default_text_.empty();
    const auto suffix = defaulted ? " (=" + default_text_ + ")" : "";

    if (implicit_.has_value() && !implicit_text_.empty())
        return "[=" + argument + "(=" + implicit_text_ + ")]" + suffix;

    return argument + suffix;
}

template <typename Type>
size_t typed_value<Type>::min_tokens() const NOEXCEPT
{
    return (zero_tokens_ || implicit_.has_value()) ? zero : one;
}

template <typename Type>
size_t typed_value<Type>::max_tokens() const NOEXCEPT
{
    return multitoken_ ? max_size_t : (zero_tokens_ ? zero : one);
}

template <typename Type>
bool typed_value<Type>::is_composing() const NOEXCEPT
{
    return composing_;
}

template <typename Type>
bool typed_value<Type>::is_required() const NOEXCEPT
{
    return required_;
}

template <typename Type>
void typed_value<Type>::parse(std::any& value, const string_list& tokens,
    const std::string& option) const THROWS
{
    if (tokens.empty() && implicit_.has_value())
    {
        value = implicit_.value();
        return;
    }

    if constexpr (is_std_vector<Type>)
    {
        using element = typename Type::value_type;
        if (!value.has_value())
            value = Type{};

        auto& list = std::any_cast<Type&>(value);
        for (const auto& token: tokens)
            list.push_back(parse_token<element>(token, option));
    }
    else
    {
        if (value.has_value())
            throw validation_exception{ "option '" + option +
                "' cannot be specified more than once" };

        if (tokens.size() > one)
            throw validation_exception{ "option '" + option +
                "' only takes a single argument" };

        if constexpr (is_same_type<Type, bool>)
        {
            value = tokens.empty() || parse_token<bool>(tokens.front(), option);
        }
        else
        {
            if (tokens.empty())
                throw validation_exception{ "option '" + option +
                    "' requires at least one argument" };

            value = parse_token<Type>(tokens.front(), option);
        }
    }
}

template <typename Type>
bool typed_value<Type>::apply_default(std::any& value) const NOEXCEPT
{
    if (!default_.has_value())
        return false;

    value = default_.value();
    return true;
}

template <typename Type>
void typed_value<Type>::notify(const std::any& value) const THROWS
{
    if (!value.has_value())
        return;

    const auto& typed = std::any_cast<const Type&>(value);

    if (!is_null(store_))
        *store_ = typed;

    if (notifier_)
        notifier_(typed);
}

// private
// ----------------------------------------------------------------------------

template <typename Type>
template <typename Element>
std::string typed_value<Type>::to_text(const Element& value) NOEXCEPT
{
    if constexpr (is_std_optional<Element>)
    {
        return value.has_value() ? to_text(value.value()) : std::string{};
    }
    else if constexpr (is_integral<Element> && is_one(sizeof(Element)))
    {
        // Single byte integers are numbers, not characters.
        return std::to_string(value);
    }
    else if constexpr (requires (std::ostream& out) { out << value; })
    {
        std::ostringstream stream{};
        stream << value;
        return stream.str();
    }
    else
    {
        return {};
    }
}

// A token is converted whole, so that it may be empty or contain spaces.
template <typename Type>
template <typename Element>
Element typed_value<Type>::parse_token(const std::string& token,
    const std::string& option) THROWS
{
    const auto invalid = [&]() THROWS
    {
        return istream_exception{ token, option };
    };

    if constexpr (is_std_optional<Element>)
    {
        return parse_token<typename Element::value_type>(token, option);
    }
    else if constexpr (is_same_type<Element, std::string>)
    {
        return token;
    }
    else if constexpr (is_same_type<Element, std::filesystem::path>)
    {
        return to_path(token);
    }
    else if constexpr (is_same_type<Element, bool>)
    {
        const auto text = ascii_to_lower(token);
        if (text.empty() || text == "on" || text == "yes" || text == "1" ||
            text == "true")
            return true;

        if (text == "off" || text == "no" || text == "0" || text == "false")
            return false;

        throw istream_exception{ token, option, ". Valid choices are "
            "'on|off', 'yes|no', '1|0' and 'true|false'" };
    }
    else if constexpr (is_integral<Element>)
    {
        // Single byte integers are numbers, not characters.
        constexpr auto byte = is_one(sizeof(Element));
        using integer = std::conditional_t<byte,
            std::conditional_t<is_signed<Element>, int16_t, uint16_t>,
            Element>;

        // A sign is optional, but a negative unsigned is invalid.
        const auto signed_ = token.starts_with('+');
        const auto start = std::next(token.data(), signed_ ? one : zero);
        const auto end = std::next(token.data(), token.size());

        integer number{};
        const auto [next, ec] = std::from_chars(start, end, number);
        if (ec != std::errc{} || next != end ||
            (signed_ && token.starts_with("+-")))
            throw invalid();

        if constexpr (byte)
        {
            if (number < std::numeric_limits<Element>::min() ||
                number > std::numeric_limits<Element>::max())
                throw invalid();
        }

        return possible_narrow_cast<Element>(number);
    }
    else
    {
        try
        {
            if constexpr (is_whole_token<Element>)
            {
                return Element{ token };
            }
            else if constexpr (requires (std::istream& in, Element& out)
                { in >> out; })
            {
                Element element{};
                std::istringstream stream{ token };
                stream.imbue(std::locale::classic());
                stream >> element;
                if (stream.fail() || stream.peek() !=
                    std::istringstream::traits_type::eof())
                    throw invalid();

                return element;
            }
            else
            {
                return Element{ token };
            }
        }
        catch (const std::exception&)
        {
            throw invalid();
        }
    }
}

BC_POP_WARNING()

template <typename Type>
typed_value<Type>* value(Type* store) NOEXCEPT
{
    BC_PUSH_WARNING(NO_NEW_OR_DELETE)
    return new typed_value<Type>(store);
    BC_POP_WARNING()
}

} // namespace options
} // namespace system
} // namespace libbitcoin

#endif
