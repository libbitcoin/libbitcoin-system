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
#ifndef LIBBITCOIN_SYSTEM_ARENA_HPP
#define LIBBITCOIN_SYSTEM_ARENA_HPP

#include <bitcoin/system/exceptions.hpp>

namespace libbitcoin {

/// Strictly conforms to std::pmr::memory_resource.
/// Thread safe resource interface, for use with our (polymorphic) allocator.
class arena
{
public:
    virtual ~arena() NOEXCEPT = default;

    /// Allocate bytes with alignment (align must be power of 2).
    /// Throws if the requested size and alignment cannot be obtained.
    NODISCARD ALLOCATOR void* allocate(size_t bytes,
        size_t align=alignof(max_align_t)) THROWS
    {
        return do_allocate(bytes, align);
    }

    /// Deallocate allocated bytes with alignment (align must be power of 2).
    void deallocate(void* ptr, const size_t bytes,
        size_t align=alignof(max_align_t)) NOEXCEPT
    {
        return do_deallocate(ptr, bytes, align);
    }

    /// Other can deallocate memory allocated by this and vice versa.
    NODISCARD bool is_equal(const arena& other) const NOEXCEPT
    {
        return do_is_equal(other);
    }

    /// Non-linear allocator just returns next address.
    /// Linear allocator resets to and returns buffer first address.
    /// Detachable linear allocator allocates buffer returns its first address.
    virtual void* start(size_t baseline) THROWS = 0;

    /// Non-linear allocator just returns zero.
    /// Linear allocator returns current allocation size.
    /// Detachable linear allocator detaches allocation and returns its size.
    virtual size_t detach() THROWS = 0;

    /// Non-linear and linear allocator is a nop.
    /// Detachable linear allocator frees the memory associated with memory.
    virtual void release(void* memory) NOEXCEPT = 0;

private:
    virtual void* do_allocate(size_t bytes, size_t align) THROWS = 0;
    virtual void do_deallocate(void* ptr, size_t bytes, size_t align) NOEXCEPT = 0;
    virtual bool do_is_equal(const arena& other) const NOEXCEPT = 0;
};

/// Left can deallocate memory allocated by right and vice versa.
bool operator==(const arena& left, const arena& right) NOEXCEPT;

/// ***************************************************************************
/// BE AWARE of the risks of memory relocation. Generally speaking a custom
/// resource [arena] must be used in strict isolation (avoiding relocation).
/// "In the polymorphic allocator model, 'transferring ownership' of a memory
/// allocation is possible only for objects within the same arena. If object
/// O1 lives in arena A1, and object O2 lives in arena A2, then an 'assignment'
/// like O1 = std::move(O2) cannot transfer any pointers from O2 into O1 - they
/// point into the wrong arena! The assignment must allocate a fresh copy of
/// O2's data in arena A1. This completely nerfs C++11 move semantics..."
/// Note also that move constructors are allocator extended and may result in
/// copies when transitioning arenas - correct but obviously inefficient.
/// quuxplusone.github.io/blog/2023/06/02/not-so-quick-pmr
/// quuxplusone.github.io/blog/2023/06/03/p1144-pmr-koans
/// ***************************************************************************
class BC_API default_arena final
  : public arena
{
public:
    static arena* get() NOEXCEPT;
    void* start(size_t baseline) THROWS override;
    size_t detach() THROWS override;
    void release(void* address) NOEXCEPT override;

private:
    void* do_allocate(size_t bytes, size_t align) THROWS override;
    void do_deallocate(void* ptr, size_t bytes, size_t align) NOEXCEPT override;
    bool do_is_equal(const arena& other) const NOEXCEPT override;
};

} // namespace libbitcoin

#endif
