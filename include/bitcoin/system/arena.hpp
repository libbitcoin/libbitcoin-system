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

/// Memory arena interface, for use with our (polymorphic) allocator.
class arena
{
public:
    static constexpr auto max_align = alignof(max_align_t);
    virtual ~arena() NOEXCEPT = default;

    /// Allocate bytes with alignment (align must be power of 2).
    NODISCARD ALLOCATOR void* allocate(size_t bytes,
        size_t align=max_align) THROWS
    {
        // actual allocation.
        auto ptr = do_allocate(bytes, align);

        // non-allocating placement.
        // "The standard library implementation performs no action and returns
        // ptr unmodified. The behavior is undefined if this function is called
        // through a placement new expression and ptr is a null pointer."
        return ::operator new(bytes, ptr);
    }

    /// Deallocate allocated bytes with alignment (align must be power of 2).
    void deallocate(void* ptr, const size_t bytes,
        size_t align=max_align) NOEXCEPT
    {
        return do_deallocate(ptr, bytes, align);
    }

    /// Other can deallocate memory allocated by this.
    NODISCARD bool is_equal(const arena& other) const NOEXCEPT
    {
        return do_is_equal(other);
    }

private:
    virtual void* do_allocate(size_t bytes, size_t align) THROWS = 0;
    virtual void do_deallocate(void* ptr, size_t bytes, size_t align) NOEXCEPT = 0;
    virtual bool do_is_equal(const arena& other) const NOEXCEPT = 0;
};

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
class default_arena
  : public arena
{
public:
    static arena* get() NOEXCEPT;

private:
    void* do_allocate(size_t bytes, size_t align) THROWS override;
    void do_deallocate(void* ptr, size_t bytes, size_t align) NOEXCEPT override;
    bool do_is_equal(const arena& other) const NOEXCEPT override;
};

} // namespace libbitcoin

#endif
