/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_DATA_EXTERNAL_PTR_HPP
#define LIBBITCOIN_SYSTEM_DATA_EXTERNAL_PTR_HPP

#include <iterator>
#include <memory>
#include <utility>
#include <vector>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
    
// Smart pointer of const T*, where T has an externally-guaranteed lifetime.
// Copy/assignable, non-nullable, does not create/destroy its reference.
// This models C const pointer with the exception of non-nullability.
// The default value is T* to a static T{}, allowing deferred assignment.
// The address of the default value remains consistent for a given type T.
// The bool cast is false only if the pointer is initialized to default.
// external_ptr<data_chunk>{} models bitcoin value zero (data_chunk{}).
template <typename Type, if_default_constructible<Type> = true>
class external_ptr
{
public:
    inline external_ptr() NOEXCEPT
      : pointer_(get_unassigned())
    {
    }

    /// Defaults.
    inline external_ptr(external_ptr&&) = default;
    inline external_ptr(const external_ptr&) = default;
    inline external_ptr& operator=(external_ptr&&) = default;
    inline external_ptr& operator=(const external_ptr&) = default;
    inline ~external_ptr() = default;

    /// External ownership is required.
    inline external_ptr(Type&&) = delete;

    inline explicit external_ptr(const Type& instance) NOEXCEPT
      : external_ptr(&instance)
    {
    }

    inline external_ptr(const Type* pointer) NOEXCEPT
      : pointer_(is_null(pointer) ? get_unassigned() : pointer)
    {
    }

    inline external_ptr(const std::shared_ptr<const Type>& shared) NOEXCEPT
      : pointer_(shared ? shared.get() : get_unassigned())
    {
    }

    inline external_ptr& operator=(const std::shared_ptr<const Type>& shared)
    {
        pointer_ = shared ? shared.get() : get_unassigned();
        return *this;
    }

    inline operator bool() const NOEXCEPT
    {
        return pointer_ != get_unassigned();
    }

    inline const Type& operator*() const NOEXCEPT
    {
        return *get();
    }

    inline const Type* operator->() const NOEXCEPT
    {
        return get();
    }

    inline const Type* operator[](size_t index) const NOEXCEPT
    {
        return std::next(get(), index);
    }

    inline const Type* get() const NOEXCEPT
    {
        return pointer_;
    }

    inline void reset() const NOEXCEPT
    {
        pointer_ = get_unassigned();
    }

private:
    const Type* pointer_{};

    static inline const Type* get_unassigned() NOEXCEPT
    {
        static Type unassigned{};
        return &unassigned;
    }
};

template <typename Type, if_default_constructible<Type> = true>
inline bool operator==(const external_ptr<Type>& left,
    const external_ptr<Type>& right) NOEXCEPT
{
    return *left == *right;
}

template <typename Type, if_default_constructible<Type> = true>
inline bool operator!=(const external_ptr<Type>& left,
    const external_ptr<Type>& right) NOEXCEPT
{
    return !(left == right);
}

/// Form of external store required by make_external.
template <typename Type, if_default_constructible<Type> = true>
using tether = std_vector<std::shared_ptr<Type>>;

/// Move instance to shared_ptr external ownership and return external_ptr.
template <typename Type, if_default_constructible<Type> = true>
inline external_ptr<Type> make_external(Type&& instance,
    tether<Type>& external) NOEXCEPT
{
    external.push_back(std::make_shared<Type>(std::forward<Type>(instance)));
    return { external.back().get() };
}

/// Same as external_ptr{}.
template <typename Type, if_default_constructible<Type> = true>
inline external_ptr<Type> make_external() NOEXCEPT
{
    return {};
}

/// Same as external_ptr{ pointer }.
template <typename Type, if_default_constructible<Type> = true>
inline external_ptr<Type> make_external(const Type* pointer) NOEXCEPT
{
    return { pointer };
}

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/data/external_ptr.ipp>

#endif
