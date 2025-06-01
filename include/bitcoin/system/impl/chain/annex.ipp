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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_ANNEX_IPP
#define LIBBITCOIN_SYSTEM_CHAIN_ANNEX_IPP

#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

inline annex::annex() NOEXCEPT
  : data_(nullptr)
{
}

// Witness checks is_annex_pattern() and passes data, avoiding excess copying.
inline annex::annex(const chunk_cptr& data) NOEXCEPT
  : data_(data)
{
}

inline annex::annex(const chunk_cptrs& stack) NOEXCEPT
  : data_(from_stack(stack))
{
}

// static/protected
inline chunk_cptr annex::from_stack(const chunk_cptrs& stack) NOEXCEPT
{
    return is_annex_pattern(stack) ? stack.back() : nullptr;
}

// static/private
inline const data_chunk& annex::empty_annex() NOEXCEPT
{
    static const data_chunk empty{};
    return empty;
}

inline annex::operator bool() const NOEXCEPT
{
    return !is_null(data_);
}

inline size_t annex::size() const NOEXCEPT
{
    return data_ ? data_->size() : zero;
}

inline const data_chunk& annex::data() const NOEXCEPT
{
    return data_ ? *data_ : empty_annex();
}

inline const hash_digest annex::hash() const NOEXCEPT
{
    return sha256_hash(data());
}

// static
constexpr bool annex::is_annex_pattern(const chunk_cptrs& stack) NOEXCEPT
{
    // If at least two elements, discard annex if present.
    if (stack.size() <= one)
        return false;

    // If first byte of stack top is 0x50 it is the annex [bip341].
    const auto& top = stack.back();
    return !top->empty() && (top->front() == taproot_annex_prefix);
}

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif

