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
#ifndef LIBBITCOIN_SYSTEM_MACHINE_STACK_IPP
#define LIBBITCOIN_SYSTEM_MACHINE_STACK_IPP

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <list>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/machine/number.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace machine {

template <typename Container>
inline stack<Container>::stack() noexcept
  : container_{}, tether_{}
{
}

template <typename Container>
inline stack<Container>::stack(Container&& container) noexcept
  : container_(std::move(container)), tether_{}
{
}

// Pure stack abstraction.
// ----------------------------------------------------------------------------

template <typename Container>
inline const stack_variant& stack<Container>::top() const noexcept
{
    BC_ASSERT(!empty());
    return container_.back();
}

template <typename Container>
inline stack_variant stack<Container>::pop() noexcept
{
    BC_ASSERT(!empty());
    stack_variant temporary{ std::move(container_.back()) };
    container_.pop_back();
    return temporary;
}

template <typename Container>
inline void stack<Container>::drop() noexcept
{
    BC_ASSERT(!empty());
    container_.pop_back();
}

template <typename Container>
inline bool stack<Container>::empty() const noexcept
{
    return container_.empty();
}

template <typename Container>
inline size_t stack<Container>::size() const noexcept
{
    return container_.size();
}

template <typename Container>
inline void stack<Container>::push(data_chunk&& value) noexcept
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    container_.push_back(make_external(std::move(value), tether_));
    BC_POP_WARNING()
}

template <typename Container>
inline void stack<Container>::push(stack_variant&& value) noexcept
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    container_.push_back(std::move(value));
    BC_POP_WARNING()
}

template <typename Container>
inline void stack<Container>::push(const stack_variant& value) noexcept
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    container_.push_back(value);
    BC_POP_WARNING()
}

template <typename Container>
inline void stack<Container>::emplace_boolean(bool value) noexcept
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    container_.emplace_back(value);
    BC_POP_WARNING()
}

template <typename Container>
inline void stack<Container>::emplace_integer(int64_t value) noexcept
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    container_.emplace_back(value);
    BC_POP_WARNING()
}

template <typename Container>
inline void stack<Container>::emplace_chunk(const chunk_xptr& value) noexcept
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    container_.emplace_back(value.get());
    BC_POP_WARNING()
}

// Positional (stack cheats).
// ----------------------------------------------------------------------------
// These optimizations prevent used of std::stack.

template <typename Container>
inline void stack<Container>::erase(size_t index) noexcept
{
    BC_ASSERT(index < size());
    container_.erase(std::prev(container_.end(), add1(index)));
}

template <typename Container>
inline void stack<Container>::swap(size_t left_index,
    size_t right_index) noexcept
{
    BC_ASSERT(left_index < size() && right_index < size());

    if constexpr (vector_)
    {
        const auto back = sub1(size());
        std::swap(
            container_[back - left_index],
            container_[back - right_index]);
    }

    if constexpr (linked_)
    {
        const auto end = container_.end();
        std::swap(
            *std::prev(end, add1(left_index)),
            *std::prev(end, add1(right_index)));
    }
}

template <typename Container>
inline const stack_variant& stack<Container>::peek(size_t index) const noexcept
{
    BC_ASSERT(index < size());

    if constexpr (vector_)
        return container_[sub1(size()) - index];

    if constexpr (linked_)
        return *std::prev(container_.end(), add1(index));
}

/// Variant data conversions.
// ----------------------------------------------------------------------------

// Generalized integer peek for varying bit widths up to 8 bytes.
// Chunk to integer conversions are constrained by caller (4 or 5 bytes).
template <typename Container>
template<size_t Bytes, typename Integer,
    if_not_greater<Bytes, sizeof(Integer)>,
    if_signed_integral_integer<Integer>>
inline bool stack<Container>::peek_signed(Integer& value) const noexcept
{
    using namespace number;
    auto result{ true };

    std::visit(overload
    {
        [&](bool vary) noexcept
        {
            // This is never executed in standard scripts.
            value = boolean::to_integer<Bytes>(vary);
        },
        [&](int64_t vary) noexcept
        {
            // This is the canonical use case (bounds check only).
            result = integer<Bytes>::from_integer(value, vary);
        },
        [&](const chunk_xptr& vary) noexcept
        {
            // This is never executed in standard scripts.
            result = integer<Bytes>::from_chunk(value , *vary);
        }
    }, top());

    return result;
}

template <typename Container>
inline bool stack<Container>::peek_signed4(int32_t& value) const noexcept
{
    return peek_signed<4>(value);
}

template <typename Container>
inline bool stack<Container>::peek_signed5(int64_t& value) const noexcept
{
    return peek_signed<5>(value);
}

template <typename Container>
inline bool stack<Container>::peek_bool() const noexcept
{
    using namespace number;
    bool value{};

    std::visit(overload
    {
        [&](bool vary) noexcept
        {
            // This is the canonical use case.
            value = vary;
        },
        [&](int64_t vary) noexcept
        {
            // This is never executed in standard scripts.
            value = boolean::to_bool(vary);
        },
        [&](const chunk_xptr& vary) noexcept
        {
            // This is never executed in standard scripts.
            value = boolean::from_chunk(*vary);
        }
    }, top());

    return value;
}

// This differs from peek_bool in that a stack chunk must be empty.
template <typename Container>
inline bool stack<Container>::peek_strict_bool() const noexcept
{
    using namespace number;
    bool value{};

    std::visit(overload
    {
        [&](bool vary) noexcept
        {
            // This is the canonical use case (after bip147).
            value = vary;
        },
        [&](int64_t vary) noexcept
        {
            // This may be executed in standard scripts (before bip147).
            value = boolean::to_bool(vary);
        },
        [&](const chunk_xptr& vary) noexcept
        {
            // This may be executed in standard scripts (before bip147).
            value = boolean::strict_from_chunk(*vary);
        }
    }, top());

    return value;
}

// This is the only source of peek/pop (read) tethering.
template <typename Container>
inline chunk_xptr stack<Container>::peek_chunk() const noexcept
{
    using namespace number;
    chunk_xptr value{};

    std::visit(overload
    {
        [&, this](bool vary) noexcept
        {
            // This is never executed in standard scripts.
            value = make_external(chunk::from_bool(vary), tether_);
        },
        [&](int64_t vary) noexcept
        {
            // This is never executed in standard scripts.
            value = make_external(chunk::from_integer(vary), tether_);
        },
        [&](const chunk_xptr& vary) noexcept
        {
            // This is the canonical use case.
            value = vary;
        }
    }, top());

    return value;
}

/// Static variant compare with conversion.
/// Integers are unconstrained as these are stack chunk equality comparisons.
template <typename Container>
inline bool stack<Container>::equal_chunks(const stack_variant& left,
    const stack_variant& right) noexcept
{
    enum stack_type { bool_, int64_, pchunk_ };
    static_assert(std::variant_size<stack_variant>::value == 3u);
    const auto right_type = static_cast<stack_type>(right.index());

    using namespace number;
    auto same{ true };

    // Methods bound at compile time (free).
    // One runtime switch on variant index (cheap).
    // bool/int conversions are compile-time (free).
    // chunk conversions reduce to conventional bitcoin design.
    std::visit(overload
    {
        // This is never executed in standard scripts.
        [&](bool vary) noexcept
        {
            switch (right_type)
            {
                case bool_:
                    same = std::get<bool>(right) == vary;
                    break;
                case int64_:
                    same = std::get<int64_t>(right) == boolean::to_integer(vary);
                    break;
                case pchunk_:
                    same = *std::get<chunk_xptr>(right) == chunk::from_bool(vary);
            }
        },

        // This is never executed in standard scripts.
        [&](int64_t vary) noexcept
        {
            switch (right_type)
            {
                case bool_:
                    same = boolean::to_integer(std::get<bool>(right)) == vary;
                    break;
                case int64_:
                    same = std::get<int64_t>(right) == vary;
                    break;
                case pchunk_:
                    same = *std::get<chunk_xptr>(right) == chunk::from_integer(vary);
            }
        },

        // This is the canonical use case.
        [&](chunk_xptr vary) noexcept
        {
            switch (right_type)
            {
                case bool_:
                    // This is never executed in standard scripts.
                    same = chunk::from_bool(std::get<bool>(right)) == *vary;
                    break;
                case int64_:
                    // This is never executed in standard scripts.
                    same = chunk::from_integer(std::get<int64_t>(right)) == *vary;
                    break;
                case pchunk_:
                    // This is the canonical use case.
                    same = *std::get<chunk_xptr>(right) == *vary;
            }
        }
    }, left);

    return same;
}

} // namespace machine
} // namespace system
} // namespace libbitcoin

#endif

// Tethering Considerations
//
// Hash results and int/bool->chunks are saved using a shared_ptr vector.
// The tether is not garbage-collected (until destruct) as this is a space-
// time performance tradeoff. The maximum number of constructable chunks is
// bound by the script size limit. A standard in/out script pair tethers
// only one chunk, the computed hash.
//
// The following script operations will ALWAYS tether chunks, as the result
// of a computed hash is *pushed* to the weak pointer (xptr) variant stack.
//
// op_ripemd160         (1)
// op_sha1              (1)
// op_sha256            (1)
// op_hash160           (1)
// op_hash256           (1)
//
// The following script operations will ONLY tether chunks in the case where
// the *popped* element was originally bool/int64_t but required as chunk.
// This is never the case in standard scripts.
//
// op_ripemd160         (0..1)
// op_sha1              (0..1)
// op_sha256            (0..1)
// op_hash160           (0..1)
// op_hash256           (0..1)
// op_size              (0..1)
// op_check_sig         (0..2, and m (endorsements) + n (keys))
// op_check_sig_verify  (0..2, and m (endorsements) + n (keys))
