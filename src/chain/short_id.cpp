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
#include <bitcoin/system/chain/short_id.hpp>

#include <bitcoin/system/chain/header.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// The key is the first half of the sha256 of the header and nonce.
siphash_key short_id::to_key(const header& header, uint64_t nonce) NOEXCEPT
{
    hash_digest digest{};
    stream::out::fast stream{ digest };
    hash::sha256::fast sink{ stream };
    header.to_data(sink);
    sink.write_8_bytes_little_endian(nonce);
    sink.flush();
    return to_siphash_key(split(digest).first);
}

short_id::integer short_id::to_id(const siphash_key& key,
    const hash_digest& hash) NOEXCEPT
{
    return bit_and(siphash(key, hash), mask);
}

// The serialization is the low bytes of the little-endian integer.
short_id::integer short_id::from_mini(const mini_hash& id) NOEXCEPT
{
    data_array<sizeof(integer)> bytes{};
    std::copy(id.cbegin(), id.cend(), bytes.begin());
    return from_little_endian<integer>(bytes);
}

mini_hash short_id::to_mini(integer id) NOEXCEPT
{
    const auto bytes = to_little_endian(id);
    return array_cast<uint8_t, mini_hash_size>(bytes);
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
