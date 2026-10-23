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

void throw_error(const code& ec) THROWS
{
    throw exception{ ec };
}

value::value() NOEXCEPT
  : inner_{ nullptr }
{
}

value::value(const value& other) THROWS
  : inner_(other.inner_)
{
}

value::value(value&& other) NOEXCEPT
  : inner_(std::move(other.inner_))
{
}

value::value(std::nullptr_t) NOEXCEPT
  : inner_{ nullptr }
{
}

value::value(bool boolean) NOEXCEPT
  : inner_(std::in_place_type<bool>, boolean)
{
}

value::value(double number) NOEXCEPT
  : inner_(std::in_place_type<double>, number)
{
}

value::value(std::string_view text) THROWS
  : inner_(std::in_place_type<string>, text)
{
}

value::value(const char* text) THROWS
  : inner_(std::in_place_type<string>, text)
{
}

value::value(const string& text) THROWS
  : inner_(std::in_place_type<string>, text)
{
}

value::value(string&& text) NOEXCEPT
  : inner_(std::in_place_type<string>, std::move(text))
{
}

value::value(const array& other) THROWS
  : inner_(std::in_place_type<array>, other)
{
}

value::value(array&& other) NOEXCEPT
  : inner_(std::in_place_type<array>, std::move(other))
{
}

value::value(const object& other) THROWS
  : inner_(std::in_place_type<object>, other)
{
}

value::value(object&& other) NOEXCEPT
  : inner_(std::in_place_type<object>, std::move(other))
{
}

value::value(std::initializer_list<value_ref> list) THROWS
  : inner_{ nullptr }
{
    if (value_ref::maybe_object(list))
        inner_.emplace<object>(value_ref::make_object(list));
    else if (list.size() == one)
        *this = list.begin()->make_value();
    else
        inner_.emplace<array>(value_ref::make_array(list));
}

value::~value() NOEXCEPT
{
}

value& value::operator=(const value& other) THROWS
{
    if (this != &other)
        value(other).swap(*this);

    return *this;
}

value& value::operator=(value&& other) NOEXCEPT
{
    // Self-contained values may be assigned from a member (e.g. v = v[0]).
    value(std::move(other)).swap(*this);
    return *this;
}

value& value::operator=(std::initializer_list<value_ref> list) THROWS
{
    value(list).swap(*this);
    return *this;
}

value& value::operator=(std::nullptr_t) NOEXCEPT
{
    inner_.emplace<std::nullptr_t>(nullptr);
    return *this;
}

value& value::operator=(bool boolean) NOEXCEPT
{
    inner_.emplace<bool>(boolean);
    return *this;
}

value& value::operator=(double number) NOEXCEPT
{
    inner_.emplace<double>(number);
    return *this;
}

value& value::operator=(std::string_view text) THROWS
{
    value(text).swap(*this);
    return *this;
}

value& value::operator=(const char* text) THROWS
{
    value(text).swap(*this);
    return *this;
}

value& value::operator=(const string& text) THROWS
{
    value(text).swap(*this);
    return *this;
}

value& value::operator=(string&& text) NOEXCEPT
{
    value(std::move(text)).swap(*this);
    return *this;
}

value& value::operator=(const array& other) THROWS
{
    value(other).swap(*this);
    return *this;
}

value& value::operator=(array&& other) NOEXCEPT
{
    value(std::move(other)).swap(*this);
    return *this;
}

value& value::operator=(const object& other) THROWS
{
    value(other).swap(*this);
    return *this;
}

value& value::operator=(object&& other) NOEXCEPT
{
    value(std::move(other)).swap(*this);
    return *this;
}

bool& value::as_bool() THROWS
{
    if (!is_bool()) throw_error(error::json_not_bool);
    return get_bool();
}

bool value::as_bool() const THROWS
{
    if (!is_bool()) throw_error(error::json_not_bool);
    return get_bool();
}

int64_t& value::as_int64() THROWS
{
    if (!is_int64()) throw_error(error::json_not_int64);
    return get_int64();
}

int64_t value::as_int64() const THROWS
{
    if (!is_int64()) throw_error(error::json_not_int64);
    return get_int64();
}

uint64_t& value::as_uint64() THROWS
{
    if (!is_uint64()) throw_error(error::json_not_uint64);
    return get_uint64();
}

uint64_t value::as_uint64() const THROWS
{
    if (!is_uint64()) throw_error(error::json_not_uint64);
    return get_uint64();
}

double& value::as_double() THROWS
{
    if (!is_double()) throw_error(error::json_not_double);
    return get_double();
}

double value::as_double() const THROWS
{
    if (!is_double()) throw_error(error::json_not_double);
    return get_double();
}

string& value::as_string() THROWS
{
    if (!is_string()) throw_error(error::json_not_string);
    return get_string();
}

const string& value::as_string() const THROWS
{
    if (!is_string()) throw_error(error::json_not_string);
    return get_string();
}

array& value::as_array() THROWS
{
    if (!is_array()) throw_error(error::json_not_array);
    return get_array();
}

const array& value::as_array() const THROWS
{
    if (!is_array()) throw_error(error::json_not_array);
    return get_array();
}

object& value::as_object() THROWS
{
    if (!is_object()) throw_error(error::json_not_object);
    return get_object();
}

const object& value::as_object() const THROWS
{
    if (!is_object()) throw_error(error::json_not_object);
    return get_object();
}

value& value::at(std::string_view key) THROWS
{
    return as_object().at(key);
}

const value& value::at(std::string_view key) const THROWS
{
    return as_object().at(key);
}

value& value::at(size_t index) THROWS
{
    return as_array().at(index);
}

const value& value::at(size_t index) const THROWS
{
    return as_array().at(index);
}

void value::emplace_null() NOEXCEPT
{
    inner_.emplace<std::nullptr_t>(nullptr);
}

bool& value::emplace_bool() NOEXCEPT
{
    return inner_.emplace<bool>();
}

int64_t& value::emplace_int64() NOEXCEPT
{
    return inner_.emplace<int64_t>();
}

uint64_t& value::emplace_uint64() NOEXCEPT
{
    return inner_.emplace<uint64_t>();
}

double& value::emplace_double() NOEXCEPT
{
    return inner_.emplace<double>();
}

string& value::emplace_string() NOEXCEPT
{
    return inner_.emplace<string>();
}

array& value::emplace_array() NOEXCEPT
{
    return inner_.emplace<array>();
}

object& value::emplace_object() NOEXCEPT
{
    return inner_.emplace<object>();
}

void value::swap(value& other) NOEXCEPT
{
    inner_.swap(other.inner_);
}

bool value::operator==(const value& other) const NOEXCEPT
{
    const auto signed_left = if_int64();
    const auto unsigned_right = other.if_uint64();
    if (!libbitcoin::is_null(signed_left) && !libbitcoin::is_null(unsigned_right))
        return !is_negative(*signed_left) &&
            to_unsigned(*signed_left) == *unsigned_right;

    const auto unsigned_left = if_uint64();
    const auto signed_right = other.if_int64();
    if (!libbitcoin::is_null(unsigned_left) && !libbitcoin::is_null(signed_right))
        return !is_negative(*signed_right) &&
            to_unsigned(*signed_right) == *unsigned_left;

    return inner_ == other.inner_;
}

void swap(value& left, value& right) NOEXCEPT
{
    left.swap(right);
}

} // namespace json
} // namespace system
} // namespace libbitcoin
