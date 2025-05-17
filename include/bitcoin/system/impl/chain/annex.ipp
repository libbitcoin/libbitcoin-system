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
#include <bitcoin/system/chain/annex.hpp>

#include <bitcoin/system/chain/witness.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

static const data_chunk& empty_annex() NOEXCEPT
{
    static const data_chunk empty{};
    return empty;
}

inline annex::annex(const witness& owner) NOEXCEPT
  : witness_(owner)
{
}

inline annex::operator bool() const NOEXCEPT
{
    return witness_.get().is_annex_pattern();
}

inline size_t annex::size() const NOEXCEPT
{
    return data().size();
}

inline const data_chunk& annex::data() const NOEXCEPT
{
    const auto& stack = witness_.get().stack();
    return stack.empty() ? empty_annex() : *stack.back();
}

inline const hash_digest annex::hash() const NOEXCEPT
{
    return sha256_hash(data());
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
