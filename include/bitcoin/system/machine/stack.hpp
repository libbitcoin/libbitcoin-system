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
#ifndef LIBBITCOIN_SYSTEM_MACHINE_STACK_HPP
#define LIBBITCOIN_SYSTEM_MACHINE_STACK_HPP

#include <cstddef>
#include <list>
#include <type_traits>
#include <variant>
#include <vector>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace machine {

// Primary and alternate stacks have variant elements.
typedef std::variant<bool, int64_t, chunk_xptr> stack_variant;

// Primary stack options.
typedef std::list<stack_variant> linked_stack;
typedef std::vector<stack_variant> contiguous_stack;

// Alternate stack requires no stack<T> abstraction.
typedef std::vector<stack_variant> alternate_stack;

// Possibly space-efficient bit vector, optimized by std lib.
typedef std::vector<bool> condition_stack;

template <typename Container>
class stack
{
public:
    /// Defaults.
    inline stack(stack&&) = default;
    inline stack(const stack&) = default;
    inline stack& operator=(stack&&) = default;
    inline stack& operator=(const stack&) = default;
    inline ~stack() = default;

    /// Construct.
    inline stack() noexcept;
    inline stack(Container&& container) noexcept;

    /// Pure stack abstraction.
    inline const stack_variant& top() const noexcept;
    inline stack_variant pop() noexcept;
    inline void drop() noexcept;
    inline bool empty() const noexcept;
    inline size_t size() const noexcept;
    inline void push(data_chunk&& value) noexcept;
    inline void push(stack_variant&& value) noexcept;
    inline void push(const stack_variant& value) noexcept;
    inline void emplace_boolean(bool value) noexcept;
    inline void emplace_integer(int64_t value) noexcept;
    inline void emplace_chunk(const chunk_xptr& value) noexcept;

    /// Positional (stack cheats).
    inline void erase(size_t index) noexcept;
    inline void swap(size_t left_index, size_t right_index) noexcept;
    inline const stack_variant& peek(size_t index) const noexcept;

    /// Variant data conversions.
    inline bool peek_signed4(int32_t& value) const noexcept;
    inline bool peek_signed5(int64_t& value) const noexcept;
    inline bool peek_bool() const noexcept;
    inline bool peek_strict_bool() const noexcept;
    inline chunk_xptr peek_chunk() const noexcept;

    static inline bool equal_chunks(const stack_variant& left,
        const stack_variant& right) noexcept;

private:
    template<size_t Bytes, typename Integer,
        if_not_greater<Bytes, sizeof(Integer)> = true,
        if_signed_integral_integer<Integer> = true>
    inline bool peek_signed(Integer& value) const noexcept;

    static constexpr auto linked_ = is_same<Container, linked_stack>();
    static constexpr auto vector_ = is_same<Container, contiguous_stack>();
    static_assert(linked_ || vector_, "unsupported stack container");

    Container container_;

    // Mutable as this is updated by peek_chunk.
    mutable tether<data_chunk> tether_;
};

// For use with std::visit can otherwise be provate to stack<>.
template<class... Overload>
struct overload : Overload... { using Overload::operator()...; };

// Explicit deduction guide, should not be required in C++20 (namespace scope).
template<class... Overload> overload(Overload...) -> overload<Overload...>;

} // namespace machine
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/machine/stack.ipp>

#endif
