/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_MACHINE_STACK_HPP
#define LIBBITCOIN_SYSTEM_MACHINE_STACK_HPP

#include <list>
#include <variant>
#include <vector>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/machine/number_boolean.hpp>
#include <bitcoin/system/machine/number_chunk.hpp>
#include <bitcoin/system/machine/number_integer.hpp>

namespace libbitcoin {
namespace system {
namespace machine {

/// Primary and alternate stacks have variant elements.
typedef std::variant<bool, int64_t, chunk_xptr> stack_variant;

/// Primary stack options.
typedef std::list<stack_variant> linked_stack;
typedef std::vector<stack_variant> contiguous_stack;

/// Alternate stack requires no stack<T> abstraction.
typedef std::vector<stack_variant> alternate_stack;

/// Possibly space-efficient bit vector, optimized by std lib.
typedef std::vector<bool> condition_stack;

/// Stack queries are not guarded against stack empty, caller must guard.
template <typename Container>
class stack
{
public:
    /// Stack is copied in program construct.
    DEFAULT_COPY_MOVE_DESTRUCT(stack);

    /// Construct.
    INLINE stack() NOEXCEPT;
    INLINE stack(Container&& container) NOEXCEPT;

    /// Pure stack abstraction.
    INLINE const stack_variant& top() const NOEXCEPT;
    INLINE stack_variant pop() NOEXCEPT;
    INLINE void drop() NOEXCEPT;
    INLINE bool empty() const NOEXCEPT;
    INLINE size_t size() const NOEXCEPT;
    INLINE void push(data_chunk&& value) NOEXCEPT;
    INLINE void push(stack_variant&& value) NOEXCEPT;
    INLINE void push(const stack_variant& value) NOEXCEPT;
    INLINE void emplace_boolean(bool value) NOEXCEPT;
    INLINE void emplace_integer(int64_t value) NOEXCEPT;
    INLINE void emplace_chunk(const chunk_xptr& value) NOEXCEPT;

    /// Positional (stack cheats).
    INLINE void erase(size_t index) NOEXCEPT;
    INLINE void swap(size_t left_index, size_t right_index) NOEXCEPT;
    INLINE const stack_variant& peek(size_t index) const NOEXCEPT;
    INLINE bool peek_signed4(int32_t& value) const NOEXCEPT;
    INLINE bool peek_signed5(int64_t& value) const NOEXCEPT;

    /// Variant data conversions.
    /// -----------------------------------------------------------------------
    inline bool peek_bool() const NOEXCEPT;
    inline bool peek_strict_bool() const NOEXCEPT;
    inline bool peek_minimal_bool(bool& value) const NOEXCEPT;
    inline size_t peek_size() const NOEXCEPT;
    inline chunk_xptr peek_chunk() const NOEXCEPT;
    static inline bool equal_chunks(const stack_variant& left,
        const stack_variant& right) NOEXCEPT;

private:
    template<size_t Bytes, typename Integer,
        if_not_lesser<sizeof(Integer), Bytes> = true,
        if_signed_integral_integer<Integer> = true>
    inline bool peek_signed(Integer& value) const NOEXCEPT;

    static constexpr auto linked_ = is_same_type<Container, linked_stack>;
    static constexpr auto vector_ = is_same_type<Container, contiguous_stack>;
    static_assert(linked_ || vector_, "unsupported stack container");

    Container container_;

    // Mutable as this is updated by peek_chunk.
    mutable tether<data_chunk> tether_;
};

// For use with std::visit.
template<class... Overload>
struct overload : Overload... { using Overload::operator()...; };

// clang++16 still requires.
// Explicit deduction guide, should not be required in C++20 (namespace scope).
template<class... Overload> overload(Overload...) -> overload<Overload...>;

} // namespace machine
} // namespace system
} // namespace libbitcoin

#define TEMPLATE template <typename Container>
#define CLASS stack<Container>

#include <bitcoin/system/impl/machine/stack.ipp>
#include <bitcoin/system/impl/machine/stack_variant.ipp>

#undef CLASS
#undef TEMPLATE

#endif
