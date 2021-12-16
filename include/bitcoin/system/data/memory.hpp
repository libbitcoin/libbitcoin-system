/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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

/// Create empty shared pointer to const.
template <typename Type>
inline std::shared_ptr<Type> to_shared()
{
    return std::make_shared<Type>();
}

/// Create shared pointer to const from the moved instance.
template <typename Type>
inline std::shared_ptr<const Type> to_shared(Type&& value)
{
    return std::make_shared<const Type>(std::forward<Type>(value));
}

/// Create shared pointer to const from the copied instance.
template <typename Type>
inline std::shared_ptr<const Type> to_shared(const Type& value)
{
    return std::make_shared<const Type>(value);
}

/// Create shared pointer to vector of const shared ptr from the moved vector.
template <typename Type>
std::shared_ptr<std::vector<std::shared_ptr<const Type>>> to_shareds(
    std::vector<Type>&& values);

/// Create shared pointer to vector of const shared ptr from the copied vector.
template <typename Type>
std::shared_ptr<std::vector<std::shared_ptr<const Type>>> to_shareds(
    const std::vector<Type>& values);

// bit.ly/3vdbF17
// Convert value initialization into default initialization.
template <typename Type, typename Allocator = std::allocator<Type>>
class default_allocator
  : public Allocator
{
public:
    template <typename T>
    struct rebind
    {
        // en.cppreference.com/w/cpp/memory/allocator_traits
        using other = default_allocator<T, typename
            std::allocator_traits<Allocator>::template rebind_alloc<T>>;
    };

    using Allocator::Allocator;

    template <typename T>
    void construct(T* ptr)
        noexcept(std::is_nothrow_default_constructible<T>::value)
    {
        // en.cppreference.com/w/cpp/memory/allocator
        // Base class (std::allocator) owns memory deallocation.
        ::new(static_cast<void*>(ptr)) T;
    }

    template <typename T, typename...Args>
    void construct(T* ptr, Args&&... args)
        noexcept(std::is_nothrow_default_constructible<Allocator>::value)
    {
        std::allocator_traits<Allocator>::construct(
            static_cast<Allocator&>(*this), ptr, std::forward<Args>(args)...);
    }
};

// C++14: std::vector(size_t, allocator) construction.
static default_allocator<uint8_t> no_fill_byte_allocator{};

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/data/memory.ipp>

#endif
