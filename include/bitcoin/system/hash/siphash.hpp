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

// Sponsored in part by Digital Contract Design, LLC

#ifndef LIBBITCOIN_SYSTEM_HASH_SIPHASH
#define LIBBITCOIN_SYSTEM_HASH_SIPHASH

#include <tuple>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/functions.hpp>

namespace libbitcoin {
namespace system {

typedef std::tuple<uint64_t, uint64_t> siphash_key;

BC_API siphash_key to_siphash_key(const half_hash& hash) NOEXCEPT;
BC_API uint64_t siphash(const half_hash& hash,
    const data_slice& message) NOEXCEPT;
BC_API uint64_t siphash(const siphash_key& key,
    const data_slice& message) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#endif
