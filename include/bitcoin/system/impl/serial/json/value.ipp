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
#ifndef LIBBITCOIN_SYSTEM_SERIAL_JSON_VALUE_IPP
#define LIBBITCOIN_SYSTEM_SERIAL_JSON_VALUE_IPP

#include <cmath>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace json {

// value_ref
// ----------------------------------------------------------------------------

template <typename Type>
value value_ref::from_const(const void* source) THROWS
{
    return value_from(*pointer_cast<const Type>(source));
}

BC_PUSH_WARNING(NO_CONST_CAST)
template <typename Type>
value value_ref::from_rvalue(const void* source) THROWS
{
    // Initializer lists are const, the referenced rvalue is not.
    return value_from(std::move(*const_cast<Type*>(
        pointer_cast<const Type>(source))));
}
BC_POP_WARNING()

template <typename Integer, if_signed_integral_integer<Integer>>
inline value_ref::value_ref(Integer number) NOEXCEPT
  : int64_(number), what_(what::int64)
{
}

template <typename Integer, if_unsigned_integral_integer<Integer>>
inline value_ref::value_ref(Integer number) NOEXCEPT
  : uint64_(number), what_(what::uint64)
{
}

template <typename Type, bool_if<is_json_convertible<Type>>>
inline value_ref::value_ref(const Type& other) NOEXCEPT
  : converter_{ &other, &from_const<Type> }, what_(what::function)
{
}

template <typename Type, bool_if<is_json_convertible<Type> &&
    !std::is_lvalue_reference_v<Type>>>
inline value_ref::value_ref(Type&& other) NOEXCEPT
  : converter_{ &other, &from_rvalue<std::decay_t<Type>> },
    what_(what::function)
{
}

// array
// ----------------------------------------------------------------------------

inline bool array::empty() const NOEXCEPT
{
    return values_.empty();
}

inline size_t array::size() const NOEXCEPT
{
    return values_.size();
}

inline size_t array::capacity() const NOEXCEPT
{
    return values_.capacity();
}

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
inline value& array::operator[](size_t index) NOEXCEPT
{
    return values_[index];
}

inline const value& array::operator[](size_t index) const NOEXCEPT
{
    return values_[index];
}
BC_POP_WARNING()

inline value& array::front() NOEXCEPT
{
    return values_.front();
}

inline const value& array::front() const NOEXCEPT
{
    return values_.front();
}

inline value& array::back() NOEXCEPT
{
    return values_.back();
}

inline const value& array::back() const NOEXCEPT
{
    return values_.back();
}

inline value* array::data() NOEXCEPT
{
    return values_.data();
}

inline const value* array::data() const NOEXCEPT
{
    return values_.data();
}

BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
inline array::iterator array::begin() NOEXCEPT
{
    return values_.data();
}

inline array::const_iterator array::begin() const NOEXCEPT
{
    return values_.data();
}

inline array::const_iterator array::cbegin() const NOEXCEPT
{
    return values_.data();
}

inline array::iterator array::end() NOEXCEPT
{
    return std::next(values_.data(), values_.size());
}

inline array::const_iterator array::end() const NOEXCEPT
{
    return std::next(values_.data(), values_.size());
}

inline array::const_iterator array::cend() const NOEXCEPT
{
    return std::next(values_.data(), values_.size());
}
BC_POP_WARNING()

inline array::reverse_iterator array::rbegin() NOEXCEPT
{
    return reverse_iterator{ end() };
}

inline array::const_reverse_iterator array::rbegin() const NOEXCEPT
{
    return const_reverse_iterator{ end() };
}

inline array::const_reverse_iterator array::crbegin() const NOEXCEPT
{
    return const_reverse_iterator{ cend() };
}

inline array::reverse_iterator array::rend() NOEXCEPT
{
    return reverse_iterator{ begin() };
}

inline array::const_reverse_iterator array::rend() const NOEXCEPT
{
    return const_reverse_iterator{ begin() };
}

inline array::const_reverse_iterator array::crend() const NOEXCEPT
{
    return const_reverse_iterator{ cbegin() };
}

template <typename ...Args>
inline value& array::emplace_back(Args&&... args) THROWS
{
    return values_.emplace_back(std::forward<Args>(args)...);
}

template <typename ...Args>
inline array::iterator array::emplace(const_iterator position,
    Args&&... args) THROWS
{
    const auto offset = std::distance(cbegin(), position);
    const auto it = values_.emplace(std::next(values_.cbegin(), offset),
        std::forward<Args>(args)...);
    return std::next(begin(), std::distance(values_.begin(), it));
}

// object
// ----------------------------------------------------------------------------

inline bool object::empty() const NOEXCEPT
{
    return members_.empty();
}

inline size_t object::size() const NOEXCEPT
{
    return members_.size();
}

inline size_t object::capacity() const NOEXCEPT
{
    return members_.capacity();
}

BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
inline object::iterator object::begin() NOEXCEPT
{
    return members_.data();
}

inline object::const_iterator object::begin() const NOEXCEPT
{
    return members_.data();
}

inline object::const_iterator object::cbegin() const NOEXCEPT
{
    return members_.data();
}

inline object::iterator object::end() NOEXCEPT
{
    return std::next(members_.data(), members_.size());
}

inline object::const_iterator object::end() const NOEXCEPT
{
    return std::next(members_.data(), members_.size());
}

inline object::const_iterator object::cend() const NOEXCEPT
{
    return std::next(members_.data(), members_.size());
}

inline object::iterator object::find(std::string_view key) NOEXCEPT
{
    const auto position = locate(key);
    return position == npos ? end() : std::next(begin(), position);
}

inline object::const_iterator object::find(
    std::string_view key) const NOEXCEPT
{
    const auto position = locate(key);
    return position == npos ? end() : std::next(begin(), position);
}
BC_POP_WARNING()

inline object::reverse_iterator object::rbegin() NOEXCEPT
{
    return reverse_iterator{ end() };
}

inline object::const_reverse_iterator object::rbegin() const NOEXCEPT
{
    return const_reverse_iterator{ end() };
}

inline object::const_reverse_iterator object::crbegin() const NOEXCEPT
{
    return const_reverse_iterator{ cend() };
}

inline object::reverse_iterator object::rend() NOEXCEPT
{
    return reverse_iterator{ begin() };
}

inline object::const_reverse_iterator object::rend() const NOEXCEPT
{
    return const_reverse_iterator{ begin() };
}

inline object::const_reverse_iterator object::crend() const NOEXCEPT
{
    return const_reverse_iterator{ cbegin() };
}

inline bool object::contains(std::string_view key) const NOEXCEPT
{
    return locate(key) != npos;
}

inline size_t object::count(std::string_view key) const NOEXCEPT
{
    return to_int<size_t>(contains(key));
}

template <typename Type>
inline std::pair<object::iterator, bool> object::emplace(
    std::string_view key, Type&& element) THROWS
{
    if (const auto it = find(key); it != end())
        return { it, false };

    return { append({ key, json::value(std::forward<Type>(element)) }), true };
}

template <typename Type>
inline std::pair<object::iterator, bool> object::insert_or_assign(
    std::string_view key, Type&& element) THROWS
{
    if (const auto it = find(key); it != end())
    {
        it->value_ = std::forward<Type>(element);
        return { it, false };
    }

    return { append({ key, json::value(std::forward<Type>(element)) }), true };
}

template <typename Pair>
inline std::pair<object::iterator, bool> object::insert(Pair&& pair) THROWS
{
    return emplace(pair.first, std::forward<Pair>(pair).second);
}

// value
// ----------------------------------------------------------------------------

template <typename Integer, if_signed_integral_integer<Integer>>
inline value::value(Integer number) NOEXCEPT
  : inner_(std::in_place_type<int64_t>, number)
{
}

template <typename Integer, if_unsigned_integral_integer<Integer>>
inline value::value(Integer number) NOEXCEPT
  : inner_(std::in_place_type<uint64_t>, number)
{
}

template <typename Integer, if_signed_integral_integer<Integer>>
inline value& value::operator=(Integer number) NOEXCEPT
{
    inner_.emplace<int64_t>(number);
    return *this;
}

template <typename Integer, if_unsigned_integral_integer<Integer>>
inline value& value::operator=(Integer number) NOEXCEPT
{
    inner_.emplace<uint64_t>(number);
    return *this;
}

inline json::kind value::kind() const NOEXCEPT
{
    return static_cast<json::kind>(inner_.index());
}

inline bool value::is_null() const NOEXCEPT
{
    return std::holds_alternative<std::nullptr_t>(inner_);
}

inline bool value::is_bool() const NOEXCEPT
{
    return std::holds_alternative<bool>(inner_);
}

inline bool value::is_int64() const NOEXCEPT
{
    return std::holds_alternative<int64_t>(inner_);
}

inline bool value::is_uint64() const NOEXCEPT
{
    return std::holds_alternative<uint64_t>(inner_);
}

inline bool value::is_double() const NOEXCEPT
{
    return std::holds_alternative<double>(inner_);
}

inline bool value::is_number() const NOEXCEPT
{
    return is_int64() || is_uint64() || is_double();
}

inline bool value::is_string() const NOEXCEPT
{
    return std::holds_alternative<string>(inner_);
}

inline bool value::is_array() const NOEXCEPT
{
    return std::holds_alternative<array>(inner_);
}

inline bool value::is_object() const NOEXCEPT
{
    return std::holds_alternative<object>(inner_);
}

inline bool value::is_primitive() const NOEXCEPT
{
    return !is_structured();
}

inline bool value::is_structured() const NOEXCEPT
{
    return is_array() || is_object();
}

inline bool* value::if_bool() NOEXCEPT
{
    return std::get_if<bool>(&inner_);
}

inline const bool* value::if_bool() const NOEXCEPT
{
    return std::get_if<bool>(&inner_);
}

inline int64_t* value::if_int64() NOEXCEPT
{
    return std::get_if<int64_t>(&inner_);
}

inline const int64_t* value::if_int64() const NOEXCEPT
{
    return std::get_if<int64_t>(&inner_);
}

inline uint64_t* value::if_uint64() NOEXCEPT
{
    return std::get_if<uint64_t>(&inner_);
}

inline const uint64_t* value::if_uint64() const NOEXCEPT
{
    return std::get_if<uint64_t>(&inner_);
}

inline double* value::if_double() NOEXCEPT
{
    return std::get_if<double>(&inner_);
}

inline const double* value::if_double() const NOEXCEPT
{
    return std::get_if<double>(&inner_);
}

inline string* value::if_string() NOEXCEPT
{
    return std::get_if<string>(&inner_);
}

inline const string* value::if_string() const NOEXCEPT
{
    return std::get_if<string>(&inner_);
}

inline array* value::if_array() NOEXCEPT
{
    return std::get_if<array>(&inner_);
}

inline const array* value::if_array() const NOEXCEPT
{
    return std::get_if<array>(&inner_);
}

inline object* value::if_object() NOEXCEPT
{
    return std::get_if<object>(&inner_);
}

inline const object* value::if_object() const NOEXCEPT
{
    return std::get_if<object>(&inner_);
}

inline bool& value::get_bool() NOEXCEPT
{
    BC_ASSERT(is_bool());
    return *if_bool();
}

inline bool value::get_bool() const NOEXCEPT
{
    BC_ASSERT(is_bool());
    return *if_bool();
}

inline int64_t& value::get_int64() NOEXCEPT
{
    BC_ASSERT(is_int64());
    return *if_int64();
}

inline int64_t value::get_int64() const NOEXCEPT
{
    BC_ASSERT(is_int64());
    return *if_int64();
}

inline uint64_t& value::get_uint64() NOEXCEPT
{
    BC_ASSERT(is_uint64());
    return *if_uint64();
}

inline uint64_t value::get_uint64() const NOEXCEPT
{
    BC_ASSERT(is_uint64());
    return *if_uint64();
}

inline double& value::get_double() NOEXCEPT
{
    BC_ASSERT(is_double());
    return *if_double();
}

inline double value::get_double() const NOEXCEPT
{
    BC_ASSERT(is_double());
    return *if_double();
}

inline string& value::get_string() NOEXCEPT
{
    BC_ASSERT(is_string());
    return *if_string();
}

inline const string& value::get_string() const NOEXCEPT
{
    BC_ASSERT(is_string());
    return *if_string();
}

inline array& value::get_array() NOEXCEPT
{
    BC_ASSERT(is_array());
    return *if_array();
}

inline const array& value::get_array() const NOEXCEPT
{
    BC_ASSERT(is_array());
    return *if_array();
}

inline object& value::get_object() NOEXCEPT
{
    BC_ASSERT(is_object());
    return *if_object();
}

inline const object& value::get_object() const NOEXCEPT
{
    BC_ASSERT(is_object());
    return *if_object();
}

template <typename Number, bool_if<is_json_number<Number>>>
Number value::to_number(code& ec) const NOEXCEPT
{
    ec = error::json_success;
    if constexpr (is_floating_point<Number>)
    {
        static_assert(is_same_type<Number, double>);
        if (const auto number = if_int64())
            return to_floating<Number>(*number);
        if (const auto number = if_uint64())
            return to_floating<Number>(*number);
        if (const auto number = if_double())
            return *number;

        ec = error::json_not_number;
        return {};
    }
    else
    {
        if (const auto signed_ = if_int64())
        {
            if (!is_limited<Number>(*signed_))
                return limit<Number>(*signed_);
        }
        else if (const auto unsigned_ = if_uint64())
        {
            if (!is_limited<Number>(*unsigned_))
                return limit<Number>(*unsigned_);
        }
        else if (const auto real = if_double())
        {
            // Bounds are powers of two, exact in double.
            constexpr auto digits = std::numeric_limits<Number>::digits;
            const auto upper = std::ldexp(1.0, digits);
            const auto lower = is_signed<Number> ? -upper : 0.0;
            const auto whole = std::trunc(*real) == *real;
            if (whole && *real >= lower && *real < upper)
                return to_integer<Number>(*real);
        }
        else
        {
            ec = error::json_not_number;
            return {};
        }

        ec = error::json_not_exact;
        return {};
    }
}

template <typename Number, bool_if<is_json_number<Number>>>
Number value::to_number() const THROWS
{
    code ec{};
    const auto number = to_number<Number>(ec);
    if (ec)
        throw_error(ec);

    return number;
}

// key_value_pair
// ----------------------------------------------------------------------------

inline std::string_view key_value_pair::key() const NOEXCEPT
{
    return key_;
}

inline const char* key_value_pair::key_c_str() const NOEXCEPT
{
    return key_.c_str();
}

inline json::value& key_value_pair::value() NOEXCEPT
{
    return value_;
}

inline const json::value& key_value_pair::value() const NOEXCEPT
{
    return value_;
}

} // namespace json
} // namespace system
} // namespace libbitcoin

#endif
