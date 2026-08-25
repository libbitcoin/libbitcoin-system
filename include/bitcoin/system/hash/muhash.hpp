/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#ifndef LIBBITCOIN_SYSTEM_HASH_MUHASH_HPP
#define LIBBITCOIN_SYSTEM_HASH_MUHASH_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/functions.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

/// Rolling multiplicative set hash.
/// Elements may be inserted and removed in any order, the hash is invariant.
class BC_API muhash3072
{
public:
    /// The set representation is 3072 bits.
    static constexpr size_t byte_size = 384;

    /// The empty set.
    muhash3072() NOEXCEPT;

    /// The set containing one element.
    muhash3072(const data_slice& element) NOEXCEPT;

    /// Add an element to the set.
    void insert(const data_slice& element) NOEXCEPT;

    /// Remove an element from the set (must have been inserted).
    void remove(const data_slice& element) NOEXCEPT;

    /// The set hash, the set remains valid for continued mutation.
    hash_digest flush() NOEXCEPT;

private:
    static uintx to_element(const data_slice& element) NOEXCEPT;

    // These are not thread safe.
    uintx numerator_;
    uintx denominator_;
};

} // namespace system
} // namespace libbitcoin

#endif
