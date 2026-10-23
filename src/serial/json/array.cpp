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

array::array() NOEXCEPT
  : values_{}
{
}

array::array(const array& other) THROWS
  : values_(other.values_)
{
}

array::array(array&& other) NOEXCEPT
  : values_(std::move(other.values_))
{
}

array::array(size_t count) THROWS
  : values_(count)
{
}

array::array(size_t count, const value& fill) THROWS
  : values_(count, fill)
{
}

array::array(std::initializer_list<value_ref> list) THROWS
  : array(value_ref::make_array(list))
{
}

array::~array() NOEXCEPT
{
}

array& array::operator=(const array& other) THROWS
{
    values_ = other.values_;
    return *this;
}

array& array::operator=(array&& other) NOEXCEPT
{
    values_ = std::move(other.values_);
    return *this;
}

array& array::operator=(std::initializer_list<value_ref> list) THROWS
{
    *this = value_ref::make_array(list);
    return *this;
}

void array::reserve(size_t count) THROWS
{
    values_.reserve(count);
}

void array::shrink_to_fit() THROWS
{
    values_.shrink_to_fit();
}

value& array::at(size_t index) THROWS
{
    if (index >= values_.size())
        throw_error(error::json_out_of_range);

    return (*this)[index];
}

const value& array::at(size_t index) const THROWS
{
    if (index >= values_.size())
        throw_error(error::json_out_of_range);

    return (*this)[index];
}

value* array::if_contains(size_t index) NOEXCEPT
{
    return index < values_.size() ? &(*this)[index] : nullptr;
}

const value* array::if_contains(size_t index) const NOEXCEPT
{
    return index < values_.size() ? &(*this)[index] : nullptr;
}

void array::clear() NOEXCEPT
{
    values_.clear();
}

void array::push_back(const value& element) THROWS
{
    values_.push_back(element);
}

void array::push_back(value&& element) THROWS
{
    values_.push_back(std::move(element));
}

array::iterator array::insert(const_iterator position,
    const value& element) THROWS
{
    return emplace(position, element);
}

array::iterator array::insert(const_iterator position,
    value&& element) THROWS
{
    return emplace(position, std::move(element));
}

array::iterator array::erase(const_iterator position) NOEXCEPT
{
    return erase(position, std::next(position));
}

array::iterator array::erase(const_iterator first,
    const_iterator last) NOEXCEPT
{
    const auto from = std::distance(cbegin(), first);
    const auto to = std::distance(cbegin(), last);
    values_.erase(std::next(values_.cbegin(), from),
        std::next(values_.cbegin(), to));
    return std::next(begin(), from);
}

void array::pop_back() NOEXCEPT
{
    values_.pop_back();
}

void array::resize(size_t count) THROWS
{
    values_.resize(count);
}

void array::resize(size_t count, const value& fill) THROWS
{
    values_.resize(count, fill);
}

void array::swap(array& other) NOEXCEPT
{
    values_.swap(other.values_);
}

bool array::operator==(const array& other) const NOEXCEPT
{
    return values_ == other.values_;
}

void swap(array& left, array& right) NOEXCEPT
{
    left.swap(right);
}

} // namespace json
} // namespace system
} // namespace libbitcoin
