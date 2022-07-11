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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_INTRINSICS_INTRINSICS_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_INTRINSICS_INTRINSICS_HPP

#include <array>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace sha256 {
    
constexpr size_t state_size = 8;
constexpr size_t hash_size  = 32;
constexpr size_t block_size = 64;

using state = std::array<uint32_t, state_size>;
using hash  = std::array<uint8_t, hash_size>;
using block = std::array<uint8_t, block_size>;

using block1 = std::array<block, 1>;
using block2 = std::array<block, 2>;
using block4 = std::array<block, 4>;
using block8 = std::array<block, 8>;

using hash1 = std::array<hash, 1>;
using hash2 = std::array<hash, 2>;
using hash4 = std::array<hash, 4>;
using hash8 = std::array<hash, 8>;

template <size_t Blocks>
using blocks = std::array<block, Blocks>;

constexpr state initial
{
    0x6a09e667_u32, 0xbb67ae85_u32,
    0x3c6ef372_u32, 0xa54ff53a_u32,
    0x510e527f_u32, 0x9b05688c_u32,
    0x1f83d9ab_u32, 0x5be0cd19_u32
};

/// General purpose pad, the counter will always fill the last 8 bytes of the
/// final block, so the last row here is never used.
constexpr block pad
{
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // <= pad start
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // <= row unused.
};

/// Padding for double hash first round. Since doubling supports only single
/// full blocks there is always a full block of padding with count of 512 bits.
constexpr block double_pad
{
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // <= pad start
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00  // <= 512 bits
};

/// Padding for double has second round. Since the second round always hashes
/// a half block, the buffer is prefilled with padding and a count of 256 bits.
constexpr block double_buffer
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // <= final hash target
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // <= pad start
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00  // <= 256 bits
};

/// Single hash of data of any size.
/// out[] is populated to 32 bytes, and and can be the same buffer as in[].
BC_API void sha256_single(uint8_t* out, size_t size, const uint8_t* in) NOEXCEPT;

/// Double hash 64 byte blocks of data, such as pairs of hashes.
/// out[] is populated to half the size of in[], and can be the same buffer.
BC_API void sha256_double(uint8_t* out, size_t blocks, const uint8_t* in) NOEXCEPT;

} // namespace sha256
} // namespace system
} // namespace libbitcoin

#endif
