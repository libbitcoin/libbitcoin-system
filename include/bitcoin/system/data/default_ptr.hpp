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
#ifndef LIBBITCOIN_SYSTEM_DATA_DEFAULT_PTR_HPP
#define LIBBITCOIN_SYSTEM_DATA_DEFAULT_PTR_HPP

#include <bitcoin/system/data/memory.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

// Smart pointer of shared_ptr<const T>, where T is shared and non-nullable.
// Copy/assignable, shares ownership of its reference, models shared_ptr with
// the exception of non-nullability. A null obtains a shared default instance,
// as does a moved-from instance, so both remain safe to dereference.
// The address of the default instance remains consistent for a given type T.
template <typename Type, if_default_constructible<Type> = true>
class default_ptr
{
public:
    using cptr = std::shared_ptr<const Type>;

    inline default_ptr() NOEXCEPT
      : pointer_(to_empty<Type>())
    {
    }

    inline default_ptr(const cptr& pointer) NOEXCEPT
      : pointer_(to_pointer(pointer))
    {
    }

    /// Defaults.
    inline default_ptr(const default_ptr&) = default;
    inline default_ptr& operator=(const default_ptr&) = default;
    inline ~default_ptr() = default;

    /// Move leaves the source holding the default instance.
    inline default_ptr(default_ptr&& other) NOEXCEPT
      : pointer_(std::move(other.pointer_))
    {
        other.pointer_ = to_empty<Type>();
    }

    inline default_ptr& operator=(default_ptr&& other) NOEXCEPT
    {
        pointer_ = std::move(other.pointer_);
        other.pointer_ = to_empty<Type>();
        return *this;
    }

    inline default_ptr& operator=(const cptr& pointer) NOEXCEPT
    {
        pointer_ = to_pointer(pointer);
        return *this;
    }

    inline operator const cptr&() const NOEXCEPT
    {
        return pointer_;
    }

    inline const Type& operator*() const NOEXCEPT
    {
        return *pointer_;
    }

    inline const Type* operator->() const NOEXCEPT
    {
        return pointer_.get();
    }

    inline const cptr& get() const NOEXCEPT
    {
        return pointer_;
    }

private:
    cptr pointer_;
};

template <typename Type, if_default_constructible<Type> = true>
inline bool operator==(const default_ptr<Type>& left,
    const default_ptr<Type>& right) NOEXCEPT
{
    return left.get() == right.get();
}

template <typename Type, if_default_constructible<Type> = true>
inline bool operator!=(const default_ptr<Type>& left,
    const default_ptr<Type>& right) NOEXCEPT
{
    return !(left == right);
}

} // namespace system
} // namespace libbitcoin

#endif
