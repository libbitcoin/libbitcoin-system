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
#include <new>
#include <type_traits>
#include <utility>
#include <vector>
#include <bitcoin/system/arena.hpp>

namespace libbitcoin {

/// Default-filling polymorphic allocator.
/// Strictly conforms to std::pmr::polymorphic_allocator.
/// Does not default to std::pmr::get_default_resource() but
/// default_arena::get() exposes the same underlying global default allocators.
template <class Value = uint8_t>
class allocator
{
public:
    template <class>
    friend class allocator;
    using value_type = Value;

    /// construct/assign
    /// -----------------------------------------------------------------------

    template <class Type>
    allocator(const allocator<Type>& other) NOEXCEPT
      : allocator{ other.arena_ }
    {
    }

    allocator() NOEXCEPT
      : allocator{ default_arena::get() }
    {
    }

    allocator(arena* value) NOEXCEPT
      : arena_{ value }
    {
    }

    allocator(const allocator&) = default;
    allocator& operator=(const allocator&) = delete;

    /// allocate_bytes/deallocate_bytes
    /// -----------------------------------------------------------------------
    /// These allocate/deallocate bytes, without consideration of other types.

    NODISCARD ALLOCATOR void* allocate_bytes(size_t bytes,
        size_t align=alignof(max_align_t)) THROWS
    {
        return arena_->allocate(bytes, align);
    }

    void deallocate_bytes(void* ptr, size_t bytes,
        size_t align=alignof(max_align_t)) NOEXCEPT
    {
        arena_->deallocate(ptr, bytes, align);
    }

    /// allocate_object/deallocate_object
    /// -----------------------------------------------------------------------
    /// These allocate/deallocate bytes of Type size, for count of Type.

    template <class Type>
    NODISCARD ALLOCATOR Type* allocate_object(size_t count=1) THROWS
    {
        const auto bytes = get_byte_size<sizeof(Type)>(count);
        return static_cast<Type*>(allocate_bytes(bytes, alignof(Type)));
    }

    template <class Type>
    void deallocate_object(Type* ptr, size_t count=1) NOEXCEPT
    {
        deallocate_bytes(ptr, count * sizeof(Type), alignof(Type));
    }

    /// allocate/deallocate
    /// -----------------------------------------------------------------------
    /// These allocate/deallocate bytes of Value size, for count of Value.

    NODISCARD ALLOCATOR Value* allocate(size_t count) THROWS
    {
        return allocate_object<Value>(count);
    }

    void deallocate(Value* ptr, size_t count) NOEXCEPT
    {
        return deallocate_object<Value>(ptr, count);
    }

    /// new_object/delete_object
    /// -----------------------------------------------------------------------
    /// These allocate & construct / destruct & deallocate.

    template <class Type, class ...Args>
    NODISCARD ALLOCATOR Type* new_object(Args&&... args) THROWS
    {
        // construct_guard ensures deallocation if construct exception.
        auto ptr = allocate_object<Type>();
        construct_guard<Type> guard{ arena_, ptr };
        construct<Type>(ptr, std::forward<Args>(args)...);
        guard.arena_ = nullptr;
        return ptr;
    }

    template <class Type>
    void delete_object(Type* ptr) NOEXCEPT
    {
        destroy<Type>(ptr);
        deallocate_object(ptr);
    }

    /// construct/destroy
    /// -----------------------------------------------------------------------
    /// These neither allocate nor deallocate.

    // Clang is not yet C++20 compliant in terms of aggregate initialization.
    // See [reviews.llvm.org/D140327] for details, resolved in future releases.
    template <class Type, class ...Args>
    void construct(Type* ptr, Args&&... arguments) THROWS
    {
        BC_PUSH_WARNING(NO_IMPLICIT_CONVERTABLE_CAST)
        auto at = const_cast<void*>(static_cast<const volatile void*>(ptr));
        BC_POP_WARNING()

        std::apply
        (
            // std::apply forwards second argument (tuple) as args to lambda.
            [at](auto&&... args)
            {
                BC_PUSH_WARNING(NO_ARRAY_TO_POINTER_DECAY)
                BC_PUSH_WARNING(NO_RETURN_MOVEABLE_HEAP_OBJECT)

                // Construct Type(...) in previously-allocated address 'at'.
                return ::new(at) Type(std::forward<decltype(args)>(args)...);

                BC_POP_WARNING()
                BC_POP_WARNING()
            },

            // std::uses_allocator_construction_args merges *this as last arg
            // if exists Type(..., const Alloc& alloc), otherwise forwards args.
            std::uses_allocator_construction_args<Type>(*this,
                std::forward<Args>(arguments)...)
        );
    }

    // www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2875r0.pdf
    // To become undeprecated in C++26 (which basically means it is not now).
    template <class Type>
    static void destroy(Type* ptr) NOEXCEPT
    {
        if constexpr (std::is_array_v<Type>)
        {
            using element = std::iter_value_t<Type>;
            if constexpr (!std::is_trivially_destructible_v<element>)
            {
                const auto last = *ptr + std::extent_v<Type>;
                for (auto first = *ptr; first != last; ++first)
                {
                    // Recurse until non-array or trivially destructible.
                    destroy(first);
                }
            }
        }
        else
        {
            ptr->~Type();
        }
    }

    /// Utility for passing deleted to shared pointer construction.
    template <typename Type>
    static auto deleter(arena* arena) NOEXCEPT
    {
        return [arena](Type* ptr) NOEXCEPT
        {
            if (ptr != nullptr)
            {
                allocator<Type>::destroy(ptr);
                arena->deallocate(ptr, sizeof(Type), alignof(Type));
            }
        };
    }

    /// Utility for passing deleted to shared pointer construction.
    template <typename Type>
    auto deleter() NOEXCEPT
    {
        return [arena = arena_](Type* ptr) NOEXCEPT
        {
            deleter<Type>(arena)(ptr);
        };
    }

    /// Other
    /// -----------------------------------------------------------------------

    /// polymorphic allocators do not propagate on container copy construction!
    allocator select_on_container_copy_construction() const NOEXCEPT
    {
        return {};
    }

    /// Obtain the memory resource (arena).
    NODISCARD arena* resource() const NOEXCEPT
    {
        return arena_;
    }

    /// Allocated memory is interchangeable.
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
                throw bad_array_new_length();
        }

        return count * Size;
    }

private:
    // This is thread safe.
    arena* arena_;
};

/// Same as friend equality but allows conversion to allocator.
template<class Left, class Right>
inline bool operator==(const allocator<Left>& left,
    const allocator<Right>& right) NOEXCEPT
{
    return left == right;
}

using byte_allocator = allocator<uint8_t>;

#define CREATE(Type, allocate, ...) \
    allocate.new_object<Type>(__VA_ARGS__), allocate.deleter<Type>(), allocate
#define POINTER(Type, allocate, ptr) \
    ptr, allocate.deleter<Type>(), allocate
#define INPLACE(ptr, Type, allocate, ...) \
    allocate.construct<std::shared_ptr<const Type>>(ptr, __VA_ARGS__, \
        allocate.deleter<Type>(), allocate)

// TODO: replace above macros with parameter pack expansion.
////template <typename Type, typename Allocator, typename... Args>
////auto shared_ptr(Allocator allocator, Args&&... args) NOEXCEPT
////{
////    return std::make_tuple
////    (
////        allocator.new_object<Type>(std::forward<Args>(args)...),
////        allocator.deleter<Type>(),
////        allocator
////    );
////}

} // namespace libbitcoin

#endif
