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
#ifndef LIBBITCOIN_SYSTEM_MACHINE_STACK_IPP
#define LIBBITCOIN_SYSTEM_MACHINE_STACK_IPP

#include <iterator>
#include <utility>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace machine {

TEMPLATE
INLINE CLASS::
stack() NOEXCEPT
  : container_{}, tether_{}
{
}

TEMPLATE
INLINE CLASS::
stack(Container&& container) NOEXCEPT
  : container_(std::move(container)), tether_{}
{
}

// Pure stack abstraction.
// ----------------------------------------------------------------------------

TEMPLATE
INLINE const stack_variant& CLASS::
top() const NOEXCEPT
{
    BC_ASSERT(!empty());
    return container_.back();
}

TEMPLATE
INLINE stack_variant CLASS::
pop() NOEXCEPT
{
    BC_ASSERT(!empty());
    stack_variant temporary{ std::move(container_.back()) };
    container_.pop_back();
    return temporary;
}

TEMPLATE
INLINE void CLASS::
drop() NOEXCEPT
{
    BC_ASSERT(!empty());
    container_.pop_back();
}

TEMPLATE
INLINE bool CLASS::
empty() const NOEXCEPT
{
    return container_.empty();
}

TEMPLATE
INLINE size_t CLASS::
size() const NOEXCEPT
{
    return container_.size();
}

TEMPLATE
INLINE void CLASS::
push(data_chunk&& value) NOEXCEPT
{
    container_.push_back(make_external(std::move(value), tether_));
}

TEMPLATE
INLINE void CLASS::
push(stack_variant&& value) NOEXCEPT
{
    container_.push_back(std::move(value));
}

TEMPLATE
INLINE void CLASS::
push(const stack_variant& value) NOEXCEPT
{
    container_.push_back(value);
}

TEMPLATE
INLINE void CLASS::
emplace_boolean(bool value) NOEXCEPT
{
    container_.emplace_back(value);
}

TEMPLATE
INLINE void CLASS::
emplace_integer(int64_t value) NOEXCEPT
{
    container_.emplace_back(value);
}

TEMPLATE
INLINE void CLASS::
emplace_chunk(const chunk_xptr& value) NOEXCEPT
{
    container_.emplace_back(value.get());
}

// Positional (stack cheats).
// ----------------------------------------------------------------------------
// These optimizations prevent used of std::stack.

TEMPLATE
INLINE void CLASS::
erase(size_t index) NOEXCEPT
{
    BC_ASSERT(index < size());
    container_.erase(std::prev(container_.end(), add1(index)));
}

TEMPLATE
INLINE void CLASS::
swap(size_t left_index, size_t right_index) NOEXCEPT
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

TEMPLATE
INLINE const stack_variant& CLASS::
peek(size_t index) const NOEXCEPT
{
    BC_ASSERT(index < size());

    if constexpr (vector_)
        return container_[sub1(size()) - index];

    if constexpr (linked_)
        return *std::prev(container_.end(), add1(index));
}

TEMPLATE
INLINE bool CLASS::
peek_signed4(int32_t& value) const NOEXCEPT
{
    return peek_signed<4>(value);
}

TEMPLATE
INLINE bool CLASS::
peek_signed5(int64_t& value) const NOEXCEPT
{
    return peek_signed<5>(value);
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
