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

BC_PUSH_WARNING(NO_ARRAY_INDEXING)

object::object() NOEXCEPT
  : members_{}, buckets_{}
{
}

object::object(const object& other) THROWS
  : members_(other.members_),
    buckets_(other.buckets_ ?
        std::make_unique<std::vector<size_t>>(*other.buckets_) : nullptr)
{
}

object::object(object&& other) NOEXCEPT
  : members_(std::move(other.members_)),
    buckets_(std::move(other.buckets_))
{
    other.members_.clear();
}

object::object(init_list list) THROWS
  : object()
{
    insert(list);
}

object::~object() NOEXCEPT
{
}

object& object::operator=(const object& other) THROWS
{
    if (this != &other)
        object(other).swap(*this);

    return *this;
}

object& object::operator=(object&& other) NOEXCEPT
{
    members_ = std::move(other.members_);
    buckets_ = std::move(other.buckets_);
    other.members_.clear();
    return *this;
}

object& object::operator=(init_list list) THROWS
{
    object(list).swap(*this);
    return *this;
}

void object::reserve(size_t count) THROWS
{
    members_.reserve(count);
}

value& object::at(std::string_view key) THROWS
{
    const auto position = locate(key);
    if (position == npos)
        throw_error(error::json_out_of_range);

    return members_[position].value_;
}

const value& object::at(std::string_view key) const THROWS
{
    const auto position = locate(key);
    if (position == npos)
        throw_error(error::json_out_of_range);

    return members_[position].value_;
}

value& object::operator[](std::string_view key) THROWS
{
    return emplace(key, nullptr).first->value_;
}

value* object::if_contains(std::string_view key) NOEXCEPT
{
    const auto position = locate(key);
    return position == npos ? nullptr : &members_[position].value_;
}

const value* object::if_contains(std::string_view key) const NOEXCEPT
{
    const auto position = locate(key);
    return position == npos ? nullptr : &members_[position].value_;
}

void object::clear() NOEXCEPT
{
    members_.clear();
    buckets_.reset();
}

void object::insert(init_list list) THROWS
{
    reserve(size() + list.size());
    for (const auto& pair: list)
        emplace(pair.first, pair.second.make_value());
}

object::iterator object::erase(const_iterator position) NOEXCEPT
{
    const auto offset = std::distance(cbegin(), position);
    const auto index = possible_narrow_and_sign_cast<size_t>(offset);
    if (index != sub1(members_.size()))
        members_[index] = std::move(members_.back());

    members_.pop_back();

    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    reindex();
    BC_POP_WARNING()

    return std::next(begin(), offset);
}

size_t object::erase(std::string_view key) NOEXCEPT
{
    const auto it = find(key);
    if (it == end())
        return zero;

    erase(it);
    return one;
}

object::iterator object::stable_erase(const_iterator position) NOEXCEPT
{
    const auto offset = std::distance(cbegin(), position);
    members_.erase(std::next(members_.cbegin(), offset));

    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    reindex();
    BC_POP_WARNING()

    return std::next(begin(), offset);
}

size_t object::stable_erase(std::string_view key) NOEXCEPT
{
    const auto it = find(key);
    if (it == end())
        return zero;

    stable_erase(it);
    return one;
}

void object::swap(object& other) NOEXCEPT
{
    members_.swap(other.members_);
    buckets_.swap(other.buckets_);
}

bool object::operator==(const object& other) const NOEXCEPT
{
    if (size() != other.size())
        return false;

    return std::all_of(cbegin(), cend(),
        [&other](const key_value_pair& member) NOEXCEPT
        {
            const auto found = other.if_contains(member.key());
            return !is_null(found) && (*found == member.value());
        });
}

// private
// ----------------------------------------------------------------------------

size_t object::locate(std::string_view key) const NOEXCEPT
{
    if (!buckets_)
    {
        for (size_t position{}; position < members_.size(); ++position)
            if (members_[position].key_ == key)
                return position;

        return npos;
    }

    const auto& buckets = *buckets_;
    const auto bucket = std::hash<std::string_view>{}(key) % buckets.size();
    for (auto position = buckets[bucket]; position != npos;
        position = members_[position].next_)
        if (members_[position].key_ == key)
            return position;

    return npos;
}

object::iterator object::append(key_value_pair&& member) THROWS
{
    members_.push_back(std::move(member));
    const auto position = sub1(members_.size());

    if (!buckets_)
    {
        if (members_.size() > indexed)
            reindex();
    }
    else if (members_.size() > buckets_->size())
    {
        reindex();
    }
    else
    {
        link(position);
    }

    return std::next(begin(), position);
}

void object::link(size_t position) NOEXCEPT
{
    auto& buckets = *buckets_;
    auto& member = members_[position];
    const auto bucket = std::hash<std::string_view>{}(member.key_) %
        buckets.size();

    member.next_ = buckets[bucket];
    buckets[bucket] = position;
}

void object::reindex() THROWS
{
    if (members_.size() <= indexed)
    {
        buckets_.reset();
        return;
    }

    const auto count = two * members_.size();
    if (!buckets_)
        buckets_ = std::make_unique<std::vector<size_t>>();

    buckets_->assign(count, npos);
    for (size_t position{}; position < members_.size(); ++position)
        link(position);
}

BC_POP_WARNING()

// key_value_pair
// ----------------------------------------------------------------------------

key_value_pair::key_value_pair(std::string_view key,
    json::value&& element) THROWS
  : key_(key), value_(std::move(element)), next_{}
{
}

key_value_pair::key_value_pair(string&& key, json::value&& element) NOEXCEPT
  : key_(std::move(key)), value_(std::move(element)), next_{}
{
}

void swap(object& left, object& right) NOEXCEPT
{
    left.swap(right);
}

} // namespace json
} // namespace system
} // namespace libbitcoin
