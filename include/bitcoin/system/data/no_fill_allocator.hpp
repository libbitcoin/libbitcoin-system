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
#ifndef LIBBITCOIN_SYSTEM_DATA_NO_FILL_ALLOCATOR_HPP
#define LIBBITCOIN_SYSTEM_DATA_NO_FILL_ALLOCATOR_HPP

#include <memory>
#include <type_traits>
#include <utility>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

// bit.ly/3vdbF17
// Convert value initialization into default initialization.
template <typename Type, typename Allocator = std::allocator<Type>>
class no_fill_allocator
  : public Allocator
{
public:
    template <typename T>
    struct rebind
    {
        // en.cppreference.com/w/cpp/memory/allocator_traits
        using other = no_fill_allocator<T, typename
            std::allocator_traits<Allocator>::template rebind_alloc<T>>;
    };

    using Allocator::Allocator;

    template <typename T>
    void construct(T* ptr) noexcept(
        std::is_nothrow_default_constructible_v<T>)
    {
        // en.cppreference.com/w/cpp/memory/allocator
        // Base class (std::allocator) owns memory deallocation.
        // Default fill is bypassed here.
        ::new(static_cast<void*>(ptr)) T;
    }

    template <typename T, typename ...Args>
    void construct(T* ptr, Args&&... args) noexcept(
        std::is_nothrow_default_constructible_v<Allocator>)
    {
        // Explicit fill args are forwarded to type copy constructor here.
        std::allocator_traits<Allocator>::construct(
            static_cast<Allocator&>(*this), ptr, std::forward<Args>(args)...);
    }
};

} // namespace system
} // namespace libbitcoin

#endif
