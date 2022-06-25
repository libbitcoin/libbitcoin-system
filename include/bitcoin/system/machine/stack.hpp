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
    constexpr void drop() noexcept;
    constexpr bool empty() const noexcept;
    constexpr size_t size() const noexcept;
    constexpr void push(data_chunk&& value) noexcept;
    constexpr void push(stack_variant&& value) noexcept;
    constexpr void push(const stack_variant& value) noexcept;
    constexpr void emplace_boolean(bool value) noexcept;
    constexpr void emplace_integer(int64_t value) noexcept;
    constexpr void emplace_chunk(const chunk_xptr& value) noexcept;

    /// Positional (stack cheats).
    constexpr void erase(size_t index) noexcept;
    constexpr void swap(size_t left_index, size_t right_index) noexcept;
    inline const stack_variant& peek(size_t index) const noexcept;

    /// Variant data conversions.
    template<size_t Bytes, typename Integer,
        if_not_greater<Bytes, sizeof(Integer)> = true,
        if_signed_integral_integer<Integer> = true>
    constexpr bool peek_signed(Integer& value) const noexcept;
    constexpr bool peek_bool() const noexcept;
    constexpr bool peek_strict_bool() const noexcept;
    inline chunk_xptr peek_chunk() const noexcept;
    static constexpr bool equal_chunks(const stack_variant& left,
        const stack_variant& right) noexcept;

private:
    template<class... Overload>
    struct overload : Overload... { using Overload::operator()...; };

    static constexpr auto linked_ = is_same<Container, linked_stack>();
    static constexpr auto vector_ = is_same<Container, contiguous_stack>();
    static_assert(linked_ || vector_, "unsupported stack container");

    Container container_;

    // Mutable as this is updated by peek_chunk.
    mutable tether<data_chunk> tether_;
};

} // namespace machine
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/machine/stack.ipp>

#endif
