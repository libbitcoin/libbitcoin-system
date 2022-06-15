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
#ifndef LIBBITCOIN_SYSTEM_DATA_MEMORY_HPP
#define LIBBITCOIN_SYSTEM_DATA_MEMORY_HPP

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>
#include <bitcoin/system/define.hpp>

// TODO: test.

namespace libbitcoin {
namespace system {

// shared_ptr moves are avoided in vector population by using 'new' and passing
// to shared_pointer constrcut a raw pointer via std::vector.emplace_back:
// std::vector.emplace_back(new block{...}).
// Otherwise emplace_back copies the shared pointer, just as would push_back:
// std::vector.emplace_back(std::make_shared<block>(block{...})).
// Vector emplace constructs the instance using its allocator, invoking new.
// So in this case it constructs the shared_pointer which accepts the raw
// pointer as its constructor argument, passed by std::vector.emplace_back.

/// Create empty shared pointer.
template <typename Type>
inline std::shared_ptr<Type> to_shared() noexcept
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    return std::make_shared<Type>();
    BC_POP_WARNING()
}

/// Create shared pointer to const from instance pointer.
template <typename Type>
inline std::shared_ptr<const Type> to_shared(Type* value) noexcept
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    return std::shared_ptr<const Type>(value);
    BC_POP_WARNING()
}

/// Create shared pointer to const from moved instance.
template <typename Type>
inline std::shared_ptr<const Type> to_shared(Type&& value) noexcept
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    return std::make_shared<const Type>(std::forward<Type>(value));
    BC_POP_WARNING()
}

/// Create shared pointer to const from copied instance.
template <typename Type>
inline std::shared_ptr<const Type> to_shared(const Type& value) noexcept
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    return std::make_shared<const Type>(value);
    BC_POP_WARNING()
}

/// Create shared pointer to vector of const shared pointers from moved vector.
template <typename Type>
std::shared_ptr<std::vector<std::shared_ptr<const Type>>>
to_shareds(std::vector<Type>&& values) noexcept;

/// Create shared pointer to vector of const shared pointers from copied vector.
template <typename Type>
std::shared_ptr<std::vector<std::shared_ptr<const Type>>>
to_shareds(const std::vector<Type>& values) noexcept;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/data/memory.ipp>

#endif
