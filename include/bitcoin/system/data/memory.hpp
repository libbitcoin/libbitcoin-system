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

// TODO: test.

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

// shared_ptr moves are avoided in vector population by using 'new' and passing
// to shared_pointer construct a raw pointer via std::vector.emplace_back:
// std::vector.emplace_back(new block{...}).
// Otherwise emplace_back copies the shared pointer, just as would push_back:
// std::vector.emplace_back(std::make_shared<block>(block{...})).
// Vector emplace constructs the instance using its allocator, invoking new.
// So in this case it constructs the shared_pointer which accepts the raw
// pointer as its constructor argument, passed by std::vector.emplace_back.

/// Create empty shared pointer.
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
template <typename Type, typename... Args>
inline std::shared_ptr<const Type> to_shared(Args&&... values) NOEXCEPT
{
    BC_PUSH_WARNING(NO_NEW_OR_DELETE)
    return std::shared_ptr<const Type>(
        new const Type(std::forward<Args>(values)...));
    BC_POP_WARNING()
}

/// Create shared pointer to vector of const shared pointers from moved vector.
template <typename Type>
std::shared_ptr<std::vector<std::shared_ptr<const Type>>>
to_shareds(std::vector<Type>&& values) NOEXCEPT;

/// Create shared pointer to vector of const shared pointers from copied vector.
template <typename Type>
std::shared_ptr<std::vector<std::shared_ptr<const Type>>>
to_shareds(const std::vector<Type>& values) NOEXCEPT;

BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/data/memory.ipp>

#endif
