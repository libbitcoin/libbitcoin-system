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
#include <bitcoin/system/serial/json/value.hpp>

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace json {

value_ref::value_ref(std::string_view text) NOEXCEPT
  : text_(text), what_(what::text)
{
}

value_ref::value_ref(const char* text) NOEXCEPT
  : text_(text), what_(what::text)
{
}

value_ref::value_ref(const string& text) NOEXCEPT
  : text_(text), what_(what::text)
{
}

value_ref::value_ref(string&& text) NOEXCEPT
  : moved_(&text), what_(what::moved)
{
}

value_ref::value_ref(init_list list) NOEXCEPT
  : list_(list), what_(what::list)
{
}

value_ref::value_ref(std::nullptr_t) NOEXCEPT
  : boolean_(false), what_(what::null)
{
}

value_ref::value_ref(bool boolean) NOEXCEPT
  : boolean_(boolean), what_(what::boolean)
{
}

value_ref::value_ref(double number) NOEXCEPT
  : number_(number), what_(what::number)
{
}

value_ref::value_ref(const value& other) NOEXCEPT
  : copied_(&other), what_(what::copied)
{
}

value_ref::value_ref(value&& other) NOEXCEPT
  : converter_{ &other, &from_rvalue<value> }, what_(what::function)
{
}

bool value_ref::is_key_value_pair() const NOEXCEPT
{
    if (what_ != what::list || list_.size() != two)
        return false;

    const auto& first = *list_.begin();
    return first.what_ == what::text || first.what_ == what::moved;
}

std::string_view value_ref::get_string() const NOEXCEPT
{
    BC_ASSERT(what_ == what::text || what_ == what::moved);
    return what_ == what::moved ? std::string_view{ *moved_ } : text_;
}

BC_PUSH_WARNING(NO_CONST_CAST)
value value_ref::make_value() const THROWS
{
    switch (what_)
    {
        case what::text:
            return value(text_);
        case what::moved:
            return value(std::move(*const_cast<string*>(moved_)));
        case what::list:
            return make_value(list_);
        case what::null:
            return {};
        case what::boolean:
            return value(boolean_);
        case what::int64:
            return value(int64_);
        case what::uint64:
            return value(uint64_);
        case what::number:
            return value(number_);
        case what::copied:
            return value(*copied_);
        case what::function:
        default:
            return converter_.make(converter_.source);
    }
}
BC_POP_WARNING()

bool value_ref::maybe_object(init_list list) NOEXCEPT
{
    return std::all_of(list.begin(), list.end(),
        [](const value_ref& element) NOEXCEPT
        {
            return element.is_key_value_pair();
        });
}

value value_ref::make_value(init_list list) THROWS
{
    if (maybe_object(list))
        return make_object(list);

    return make_array(list);
}

array value_ref::make_array(init_list list) THROWS
{
    array out{};
    out.reserve(list.size());
    for (const auto& element: list)
        out.push_back(element.make_value());

    return out;
}

object value_ref::make_object(init_list list) THROWS
{
    object out{};
    out.reserve(list.size());
    for (const auto& element: list)
    {
        const auto pair = element.list_.begin();
        out.emplace(pair->get_string(), std::next(pair)->make_value());
    }

    return out;
}

} // namespace json
} // namespace system
} // namespace libbitcoin
