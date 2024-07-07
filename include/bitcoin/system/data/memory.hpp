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
#ifndef LIBBITCOIN_SYSTEM_DATA_MEMORY_HPP
#define LIBBITCOIN_SYSTEM_DATA_MEMORY_HPP

#include <memory>
#include <utility>
#include <vector>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

/// shared_ptr
/// ---------------------------------------------------------------------------

/// Create default shared pointer.
template <typename Type>
inline std::shared_ptr<Type> to_shared() NOEXCEPT
{
    return std::make_shared<Type>();
}

/// Create shared pointer to const from instance pointer.
template <typename Type>
inline std::shared_ptr<const Type> to_shared(Type* value) NOEXCEPT
{
    return std::shared_ptr<const Type>(value);
}

/// Create shared pointer to const from moved instance.
template <typename Type>
inline std::shared_ptr<const Type> to_shared(Type&& value) NOEXCEPT
{
    return std::make_shared<const Type>(std::forward<Type>(value));
}

/// Create shared pointer to const from copied instance.
template <typename Type>
inline std::shared_ptr<const Type> to_shared(const Type& value) NOEXCEPT
{
    return std::make_shared<const Type>(value);
}

/// Construct shared pointer to const from moved constructor parameters.
template <typename Type, typename ...Args>
inline std::shared_ptr<const Type> to_shared(Args&&... values) NOEXCEPT
{
    return std::make_shared<const Type>(Type{ std::forward<Args>(values)... });
}

/// Obtain non constant pointer from shared_ptr to const.
/// This is useful when allocating objects to shared const before population.
template <typename Type>
inline Type* to_non_const_raw_ptr(const std::shared_ptr<const Type>& cptr) NOEXCEPT
{
    BC_PUSH_WARNING(NO_CONST_CAST)
    return const_cast<Type*>(cptr.get());
    BC_POP_WARNING()
}

/// Create shared pointer to vector of const shared pointers from moved vector.
template <typename Type>
std::shared_ptr<std_vector<std::shared_ptr<const Type>>>
to_shareds(std_vector<Type>&& values) NOEXCEPT;

/// Create shared pointer to vector of const shared pointers from copied vector.
template <typename Type>
std::shared_ptr<std_vector<std::shared_ptr<const Type>>>
to_shareds(const std_vector<Type>& values) NOEXCEPT;

/// Allocate a shared instance and construct with given arguments.
/// Allocator must be pointer to instance of std::pmr::polymorphic_allocator.
/// Allocator is passed as Type(allocator, args) and retained by Type instance.
template <typename Type, typename Allocator, typename ...Args>
std::shared_ptr<const Type> to_allocated(const Allocator& allocator,
    Args&&... args) NOEXCEPT;

/// unique_ptr
/// ---------------------------------------------------------------------------

/// Create default unique pointer.
template <typename Type>
inline std::unique_ptr<const Type> to_unique() NOEXCEPT
{
    return std::make_unique<const Type>();
}

/// Create unique pointer to const from moved instance.
template <typename Type>
inline std::unique_ptr<const Type> to_unique(Type&& value) NOEXCEPT
{
    return std::make_unique<const Type>(std::forward<Type>(value));
}

/// Create unique pointer to const from copied instance.
template <typename Type>
inline std::unique_ptr<const Type> to_unique(const Type& value) NOEXCEPT
{
    return std::make_unique<const Type>(value);
}

/// Construct unique pointer to const from moved constructor parameters.
template <typename Type, typename ...Args>
inline std::unique_ptr<const Type> to_unique(Args&&... values) NOEXCEPT
{
    return std::make_unique<const Type>(Type{ std::forward<Args>(values)... });
}

BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/data/memory.ipp>

#endif
