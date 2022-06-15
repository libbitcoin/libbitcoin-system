/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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

#include <cstdlib>
#include <iterator>
#include <memory>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/constraints.hpp>

namespace libbitcoin {
namespace system {
    
// external_ptr<T> wraps const T* with externally-guaranteed lifetime.
// external_ptr models C pointer with the exception of non-nullability.
// Its default value is T* to a static T{}, allowing deferred assignment.
// external_ptr<data_chunk>{} models bitcoin value zero (data_chunk{}).
// Copy/assignable, non-nullable, does not create/destroy its reference.
template <typename Type, if_default_constructible<Type> = true>
class external_ptr
{
public:
    external_ptr() noexcept
    {
        static const auto default_value = Type{};
        pointer_ = &default_value;
    }

    /// Defaults (nullptr may be copied/moved).
    external_ptr(external_ptr&&) = default;
    external_ptr(const external_ptr&) = default;
    external_ptr& operator=(external_ptr&&) = default;
    external_ptr& operator=(const external_ptr&) = default;
    ~external_ptr() = default;

    /// External ownership is required.
    constexpr external_ptr(Type&&) = delete;
    constexpr external_ptr(std::shared_ptr<Type>&&) = delete;

    constexpr external_ptr(const Type& instance) noexcept
      : pointer_(&instance)
    {
    }

    // Abort if nullptr construct.
    constexpr external_ptr(const Type* pointer) noexcept
      : pointer_(pointer)
    {
        if (is_null(pointer)) std::abort();
    }

    // Abort if nullptr construct.
    constexpr external_ptr(const std::shared_ptr<Type>& shared) noexcept
      : external_ptr(shared.get())
    {
    }

    external_ptr& operator=(const std::shared_ptr<Type>& shared) const noexcept
    {
        pointer_ = external_ptr(shared);
    }

    constexpr const Type* operator*() const noexcept
    {
        return get();
    }

    constexpr const Type* get() const noexcept
    {
        return pointer_;
    }

    constexpr const Type* operator->() const noexcept
    {
        return get();
    }

    constexpr const Type& at() const noexcept
    {
        return *get();
    }

    constexpr Type* reset() const noexcept
    {
        pointer_ = external_ptr{};
    }

private:
    const Type* pointer_{};
};

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/data/external_ptr.ipp>

#endif
