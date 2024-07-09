/**
 * Copyright (c) 2011-2024 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_ALLOCATOR_HPP
#define LIBBITCOIN_SYSTEM_ALLOCATOR_HPP

#include <iterator>
#include <limits>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>
#include <vector>
#include <bitcoin/system/arena.hpp>

namespace libbitcoin {

/// No-default-fill polymorphic allocator.
template <class Value = uint8_t>
class allocator
{
public:
    template <class>
    friend class allocator;

    using value_type = Value;

    template <class Type>
    allocator(const allocator<Type>& other) NOEXCEPT
      : allocator{ other.arena_ }
    {
    }

    allocator() NOEXCEPT
      : allocator{ default_arena::get() }
    {
    }

    allocator(arena* const value) NOEXCEPT
      : arena_{ value }
    {
    }

    allocator& operator=(const allocator&) = delete;

    NODISCARD ALLOCATOR Value* allocate(size_t count)
    {
        const auto bytes = get_byte_size<sizeof(Value)>(count);
        return static_cast<Value*>(arena_->allocate(bytes, alignof(Value)));
    }

    void deallocate(Value* ptr, size_t count) NOEXCEPT
    {
        // No need to verify multiplication overflow.
        arena_->deallocate(ptr, count * sizeof(Value), alignof(Value));
    }

    NODISCARD ALLOCATOR void* allocate_bytes(size_t bytes,
        size_t align=alignof(max_align_t))
    {
        return arena_->allocate(bytes, align);
    }

    void deallocate_bytes(void* ptr, size_t bytes,
        size_t align=alignof(max_align_t)) NOEXCEPT
    {
        arena_->deallocate(ptr, bytes, align);
    }

    template <class Type>
    NODISCARD ALLOCATOR Type* allocate_object(size_t count=1)
    {
        const auto bytes = get_byte_size<sizeof(Type)>(count);
        return static_cast<Type*>(allocate_bytes(bytes, alignof(Type)));
    }

    template <class Type>
    void deallocate_object(Type* ptr, size_t count=1) NOEXCEPT
    {
        deallocate_bytes(ptr, count * sizeof(Type), alignof(Type));
    }

    ////template <class Type>
    ////NODISCARD ALLOCATOR Type* new_object()
    ////{
    ////    // Default construction fill is bypassed here.
    ////    return allocate_object<Type>();
    ////}

    template <class Type, class... Args>
    NODISCARD ALLOCATOR Type* new_object(Args&&... args)
    {
        // construct_guard ensures deallocation if construct exception.
        auto ptr = allocate_object<Type>();
        construct_guard<Type> guard{ arena_, ptr };
        construct(ptr, std::forward<Args>(args)...);
        guard.arena_ = nullptr;
        return ptr;
    }

    template <class Type>
    void delete_object(Type* ptr) NOEXCEPT
    {
        destroy_in_place(*ptr);
        deallocate_object(ptr);
    }

    ////template <class Type>
    ////void construct(Type*) NOEXCEPT
    ////{
    ////    // Default construction fill is bypassed here.
    ////}

    template <class Type, class... Args>
    void construct(Type* ptr, Args&&... arguments)
    {
        std::apply
        (
            [ptr](auto&&... args)
            {
                return ::new(
                    const_cast<void*>(static_cast<const volatile void*>(ptr)))
                    Type(std::forward<decltype(args)>(args)...);
            },
            std::uses_allocator_construction_args<Type>(*this,
                std::forward<Args>(arguments)...)
        );
    }

    // Container copy results in default arena!!!
    allocator select_on_container_copy_construction() const NOEXCEPT
    {
        return {};
    }

    NODISCARD arena* resource() const NOEXCEPT
    {
        return arena_;
    }

    // To become undeprecated in C++26.
    // www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2875r0.pdf
    template <class Type>
    DEPRECATED void destroy(Type* ptr) NOEXCEPT
    {
        destroy_in_place(*ptr);
    }

    friend bool operator==(const allocator& left,
        const allocator& right) NOEXCEPT
    {
        return left.arena_->is_equal(*right.arena_);
    }

protected:
    template <class Type>
    struct construct_guard
    {
        ~construct_guard() NOEXCEPT
        {
            if (arena_ != nullptr)
                arena_->deallocate(ptr_, sizeof(Type), alignof(Type));
        }

        construct_guard& operator=(const construct_guard&) = delete;
        construct_guard& operator=(construct_guard&&) = delete;

        arena* arena_;
        void* ptr_;
    };

    template <size_t Size>
    static constexpr size_t get_byte_size(size_t count) THROWS
    {
        if constexpr (Size > 1u)
        {
            if (count > (std::numeric_limits<size_t>::max() / Size))
                throw overflow_exception("allocation overflow");
        }

        return count * Size;
    }

    template <class Type>
    constexpr void destroy_in_place(Type& object_) NOEXCEPT
    {
        if constexpr (std::is_array_v<Type>)
        {
            destroy_range(object_, object_ + std::extent_v<Type>);
        }
        else
        {
            object_.~Type();
        }
    }

    template <class First, class Last>
    constexpr void destroy_range(First first, const Last last) NOEXCEPT
    {
        using element = std::iter_value_t<First>;

        // Optimization for debug mode, in release mode this is removed.
        if constexpr (!std::is_trivially_destructible_v<element>)
            for (; first != last; ++first)
                destroy_in_place(*first);
    }

private:
    arena* arena_;
};

} // namespace libbitcoin

#endif
