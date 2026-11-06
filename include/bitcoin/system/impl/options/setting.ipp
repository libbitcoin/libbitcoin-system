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
#ifndef LIBBITCOIN_SYSTEM_IMPL_OPTIONS_SETTING_IPP
#define LIBBITCOIN_SYSTEM_IMPL_OPTIONS_SETTING_IPP

#include <filesystem>
#include <sstream>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/serial/serial.hpp>
#include <bitcoin/system/unicode/utf8_everywhere/utf8_everywhere.hpp>

namespace libbitcoin {
namespace system {
namespace options {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// Path insertion is quoted, and wide on win32.
inline std::string setting_text(const std::filesystem::path& value) NOEXCEPT
{
    return from_path(value);
}

template <typename Type>
std::string setting_text(const Type& value) NOEXCEPT
{
    if constexpr (is_std_optional<Type>)
    {
        return value.has_value() ? setting_text(value.value()) : std::string{};
    }
    else if constexpr (is_floating_point<Type>)
    {
        return serialize_fixed(value);
    }
    else if constexpr (is_integral<Type> && is_one(sizeof(Type)))
    {
        // Single byte integers are numbers, not characters.
        return std::to_string(value);
    }
    else
    {
        std::ostringstream stream{};
        stream << std::boolalpha << value;
        return stream.str();
    }
}

template <typename Type>
std::string format_default(const Type& value) NOEXCEPT
{
    string_list items{};
    if constexpr (is_std_vector<Type>)
    {
        items.reserve(value.size());
        for (const auto& element: value)
            items.push_back(setting_text(element));
    }
    else
    {
        items.push_back(setting_text(value));
    }

    if (items.empty() || (is_one(items.size()) && items.front().empty()))
        return "empty";

    for (auto& item: items)
        item = "'" + item + "'";

    return join(items, ", ");
}

template <typename Type>
setting_value<Type>::setting_value(Type* store, bool secret) THROWS
  : typed_value<Type>(store), store_(store),
    secret_(secret), default_text_(secret ? "" : format_default(*store))
{
    this->default_value(*store, default_text_);
}

template <typename Type>
string_list setting_value<Type>::values() const NOEXCEPT
{
    string_list out{};

    if constexpr (is_std_vector<Type>)
    {
        out.reserve(store_->size());

        for (const auto& element: *store_)
            out.push_back(setting_text(element));
    }
    else
    {
        out.push_back(setting_text(*store_));
    }

    // A set secret reports one empty value, so that it displays as configured
    // without disclosure, and an unset secret remains indistinguishable.
    if (secret_ && !out.empty())
        out = string_list{ {} };

    return out;
}

template <typename Type>
std::string setting_value<Type>::default_text() const NOEXCEPT
{
    return default_text_;
}

template <typename Type>
void setting_value<Type>::parse(std::any& value, const string_list& tokens,
    const std::string& option) const THROWS
{
    if constexpr (is_integral<Type> || is_std_optional<Type>)
    {
        string_list ungrouped{};
        ungrouped.reserve(tokens.size());
        for (const auto& token: tokens)
            ungrouped.push_back(ungroup(token));

        typed_value<Type>::parse(value, ungrouped, option);
    }
    else
    {
        typed_value<Type>::parse(value, tokens, option);
    }
}

BC_PUSH_WARNING(NO_NEW_OR_DELETE)

template <typename Type>
setting_value<Type>* setting(Type* store) THROWS
{
    return new setting_value<Type>(store);
}

template <typename Type>
setting_value<Type>* secret(Type* store) THROWS
{
    return new setting_value<Type>(store, true);
}

BC_POP_WARNING()

BC_POP_WARNING()

} // namespace options
} // namespace system
} // namespace libbitcoin

#endif
