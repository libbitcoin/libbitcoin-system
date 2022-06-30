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

#include <bitcoin/system/crypto/siphash.hpp>

#include <cstdint>
#include <tuple>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/serial/serial.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {

constexpr uint64_t siphash_magic_0 = 0x736f6d6570736575;
constexpr uint64_t siphash_magic_1 = 0x646f72616e646f6d;
constexpr uint64_t siphash_magic_2 = 0x6c7967656e657261;
constexpr uint64_t siphash_magic_3 = 0x7465646279746573;
constexpr uint64_t finalization = 0x00000000000000ff;
constexpr uint64_t max_encoded_byte_count = (1 << byte_bits);

// C++14: can make constexpr.
static void sip_round(uint64_t& v0, uint64_t& v1, uint64_t& v2,
    uint64_t& v3) NOEXCEPT
{
    v0 += v1;
    v2 += v3;
    rotate_left_into(v1, 13);
    rotate_left_into(v3, 16);
    v1 ^= v0;
    v3 ^= v2;

    rotate_left_into(v0, 32);

    v2 += v1;
    v0 += v3;
    rotate_left_into(v1, 17);
    rotate_left_into(v3, 21);
    v1 ^= v2;
    v3 ^= v0;

    rotate_left_into(v2, 32);
}

// C++14: can make constexpr.
static void compression_round(uint64_t& v0, uint64_t& v1, uint64_t& v2,
    uint64_t& v3, uint64_t word) NOEXCEPT
{
    v3 ^= word;
    sip_round(v0, v1, v2, v3);
    sip_round(v0, v1, v2, v3);
    v0 ^= word;
}

uint64_t siphash(const half_hash& hash, const data_slice& message) NOEXCEPT
{
    return siphash(to_siphash_key(hash), message);
}

uint64_t siphash(const siphash_key& key, const data_slice& message) NOEXCEPT
{
    uint64_t v0 = siphash_magic_0 ^ std::get<0>(key);
    uint64_t v1 = siphash_magic_1 ^ std::get<1>(key);
    uint64_t v2 = siphash_magic_2 ^ std::get<0>(key);
    uint64_t v3 = siphash_magic_3 ^ std::get<1>(key);

    constexpr auto eight = sizeof(uint64_t);
    const auto bytes = message.size();
    read::bytes::copy source(message);

    for (size_t index = eight; index <= bytes; index += eight)
        compression_round(v0, v1, v2, v3,
            source.read_8_bytes_little_endian());

    auto last = is_zero(bytes % eight) ? 0ll :
        source.read_8_bytes_little_endian();

    last ^= ((bytes % max_encoded_byte_count) << to_bits(sub1(eight)));
    compression_round(v0, v1, v2, v3, last);

    v2 ^= finalization;
    sip_round(v0, v1, v2, v3);
    sip_round(v0, v1, v2, v3);
    sip_round(v0, v1, v2, v3);
    sip_round(v0, v1, v2, v3);

    return v0 ^ v1 ^ v2 ^ v3;
}

siphash_key to_siphash_key(const half_hash& hash) NOEXCEPT
{
    const auto part = split(hash);
    const auto hi = from_little_endian<uint64_t>(part.first);
    const auto lo = from_little_endian<uint64_t>(part.second);
    return std::make_tuple(hi, lo);
}

} // namespace system
} // namespace libbitcoin
