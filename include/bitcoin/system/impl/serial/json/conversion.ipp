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
#ifndef LIBBITCOIN_SYSTEM_SERIAL_JSON_CONVERSION_IPP
#define LIBBITCOIN_SYSTEM_SERIAL_JSON_CONVERSION_IPP

#include <optional>
#include <ranges>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace json {

// Traits.
// ----------------------------------------------------------------------------

template <typename Type>
constexpr bool unsupported_conversion = false;

template <typename Type>
constexpr bool has_from_tag = requires(value& to, const Type& from)
{
    tag_invoke(value_from_tag{}, to, from);
};

template <typename Type>
constexpr bool has_to_tag = requires(const value& from)
{
    { tag_invoke(value_to_tag<Type>{}, from) } -> std::convertible_to<Type>;
};

template <typename Type>
constexpr bool is_json_type = is_same_type<Type, value> ||
    is_same_type<Type, array> || is_same_type<Type, object>;

template <typename Type>
constexpr bool is_json_null = is_same_type<Type, std::nullptr_t> ||
    is_same_type<Type, std::monostate>;

template <typename Type>
constexpr bool is_json_text = !is_json_null<Type> &&
    std::is_convertible_v<const Type&, std::string_view>;

template <typename Type>
struct is_optional_t : std::false_type {};
template <typename Type>
struct is_optional_t<std::optional<Type>> : std::true_type {};
template <typename Type>
constexpr bool is_json_optional = is_optional_t<std::decay_t<Type>>::value;

template <typename Type>
struct is_variant_t : std::false_type {};
template <typename ...Types>
struct is_variant_t<std::variant<Types...>> : std::true_type {};
template <typename Type>
constexpr bool is_json_variant = is_variant_t<std::decay_t<Type>>::value;

template <typename Type>
constexpr bool is_json_map = requires
{
    typename Type::key_type;
    typename Type::mapped_type;
    requires std::is_convertible_v<const typename Type::key_type&,
        std::string_view>;
};

template <typename Type>
constexpr bool is_json_sequence = std::ranges::range<Type> &&
    !is_json_text<Type> && !is_json_map<Type>;

template <typename Type>
constexpr bool is_json_tuple = requires
{
    std::tuple_size<Type>::value;
} && !is_json_sequence<Type>;

template <typename Type>
struct is_std_array_t : std::false_type {};
template <typename Type, size_t Size>
struct is_std_array_t<std::array<Type, Size>> : std::true_type {};

// value_from
// ----------------------------------------------------------------------------

template <typename Type>
void value_from(Type&& from, value& to) THROWS
{
    using type = std::remove_cvref_t<Type>;

    if constexpr (has_from_tag<type>)
    {
        tag_invoke(value_from_tag{}, to, std::as_const(from));
    }
    else if constexpr (is_json_type<type> || is_same_type<type, string>)
    {
        to = std::forward<Type>(from);
    }
    else if constexpr (is_same_type<type, bool> || is_json_number<type>)
    {
        to = from;
    }
    else if constexpr (is_json_null<type>)
    {
        to = nullptr;
    }
    else if constexpr (is_json_text<type>)
    {
        to = std::string_view{ from };
    }
    else if constexpr (is_json_optional<type>)
    {
        if (from.has_value())
            value_from(*std::forward<Type>(from), to);
        else
            to = nullptr;
    }
    else if constexpr (is_json_variant<type>)
    {
        std::visit([&to](auto&& alternative) THROWS
        {
            value_from(std::forward<decltype(alternative)>(alternative), to);
        }, std::forward<Type>(from));
    }
    else if constexpr (is_json_map<type>)
    {
        auto& out = to.emplace_object();
        out.reserve(std::size(from));
        for (const auto& member: from)
            out.emplace(member.first, value_from(member.second));
    }
    else if constexpr (is_json_sequence<type>)
    {
        auto& out = to.emplace_array();
        if constexpr (std::ranges::sized_range<type>)
            out.reserve(std::ranges::size(from));

        for (const auto& element: from)
            out.push_back(value_from(element));
    }
    else if constexpr (is_json_tuple<type>)
    {
        auto& out = to.emplace_array();
        out.reserve(std::tuple_size_v<type>);
        std::apply([&out](const auto&... elements) THROWS
        {
            (out.push_back(value_from(elements)), ...);
        }, from);
    }
    else
    {
        static_assert(unsupported_conversion<type>, "no json conversion");
    }
}

template <typename Type>
value value_from(Type&& from) THROWS
{
    value out{};
    value_from(std::forward<Type>(from), out);
    return out;
}

// value_to
// ----------------------------------------------------------------------------

template <typename Variant, size_t Index = zero>
Variant to_variant(const value& from) THROWS
{
    if constexpr (Index == std::variant_size_v<Variant>)
    {
        throw_error(error::json_exhausted_variants);
    }
    else
    {
        using alternative = std::variant_alternative_t<Index, Variant>;

        try
        {
            return Variant{ std::in_place_index<Index>,
                value_to<alternative>(from) };
        }
        catch (const exception&)
        {
            return to_variant<Variant, add1(Index)>(from);
        }
    }
}

template <typename Tuple, size_t ...Index>
Tuple to_tuple(const array& from, std::index_sequence<Index...>) THROWS
{
    return Tuple{ value_to<std::tuple_element_t<Index, Tuple>>(
        from.at(Index))... };
}

template <typename Type>
Type value_to(const value& from) THROWS
{
    using type = std::remove_cvref_t<Type>;

    if constexpr (has_to_tag<type>)
    {
        return tag_invoke(value_to_tag<type>{}, from);
    }
    else if constexpr (is_same_type<type, value>)
    {
        return from;
    }
    else if constexpr (is_same_type<type, array>)
    {
        return from.as_array();
    }
    else if constexpr (is_same_type<type, object>)
    {
        return from.as_object();
    }
    else if constexpr (is_same_type<type, bool>)
    {
        return from.as_bool();
    }
    else if constexpr (is_json_number<type>)
    {
        return from.to_number<type>();
    }
    else if constexpr (is_same_type<type, string> ||
        is_same_type<type, std::string_view>)
    {
        return type{ from.as_string() };
    }
    else if constexpr (is_json_null<type>)
    {
        if (!from.is_null())
            throw_error(error::json_not_null);

        return type{};
    }
    else if constexpr (is_json_optional<type>)
    {
        if (from.is_null())
            return type{};

        return type{ value_to<typename type::value_type>(from) };
    }
    else if constexpr (is_json_variant<type>)
    {
        return to_variant<type>(from);
    }
    else if constexpr (is_json_map<type>)
    {
        type out{};
        for (const auto& member: from.as_object())
            out.emplace(typename type::key_type{ member.key() },
                value_to<typename type::mapped_type>(member.value()));

        return out;
    }
    else if constexpr (is_json_sequence<type>)
    {
        using element = std::ranges::range_value_t<type>;
        const auto& values = from.as_array();

        if constexpr (is_std_array_t<type>::value)
        {
            if (values.size() != std::tuple_size_v<type>)
                throw_error(error::json_size_mismatch);

            type out{};
            std::transform(values.begin(), values.end(), out.begin(),
                [](const value& item) THROWS
                {
                    return value_to<element>(item);
                });

            return out;
        }
        else
        {
            type out{};
            if constexpr (requires { out.reserve(values.size()); })
                out.reserve(values.size());

            for (const auto& item: values)
                out.insert(out.end(), value_to<element>(item));

            return out;
        }
    }
    else if constexpr (is_json_tuple<type>)
    {
        const auto& values = from.as_array();
        if (values.size() != std::tuple_size_v<type>)
            throw_error(error::json_size_mismatch);

        return to_tuple<type>(values,
            std::make_index_sequence<std::tuple_size_v<type>>{});
    }
    else
    {
        static_assert(unsupported_conversion<type>, "no json conversion");
    }
}

} // namespace json
} // namespace system
} // namespace libbitcoin

#endif
