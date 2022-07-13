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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SHA256_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SHA256_HPP

#include <array>
#include <vector>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace sha256 {
    
constexpr size_t state_size  = 8;
constexpr size_t block_size  = 64;
constexpr size_t digest_size = 32;

using state  = std::array<uint32_t, state_size>;
using block  = std::array<uint8_t,  block_size>;
using digest = std::array<uint8_t,  digest_size>;

using block1 = std::array<block, 1>;
using block2 = std::array<block, 2>;
using block4 = std::array<block, 4>;
using block8 = std::array<block, 8>;

using digest1 = std::array<digest, 1>;
using digest2 = std::array<digest, 2>;
using digest4 = std::array<digest, 4>;
using digest8 = std::array<digest, 8>;

using blocks  = std::vector<block>;
using digests = std::vector<digest>;

constexpr state initial
{
    0x6a09e667_u32, 0xbb67ae85_u32, 0x3c6ef372_u32, 0xa54ff53a_u32,
    0x510e527f_u32, 0x9b05688c_u32, 0x1f83d9ab_u32, 0x5be0cd19_u32
};

/// Padding for any size hash round (truncated to fill block).
/// The count will be applied to any position after the first byte of padding,
/// which is the 0x80 sentinel byte. As such the last row is never padding.
constexpr block pad_any
{
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // <= pad start
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff  // <= row unused.
};

/// Padding for full block hash round (64 bytes of pad/count).
/// The buffer is prefilled with padding and a count of 256 bits.
constexpr alignas(16) block pad_64
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

/// Padding for a half block hash round (32 bytes of pad/count).
/// The buffer is prefilled with padding and a count of 256 bits.
constexpr alignas(16) block padded_32
{
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // <= hash target
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,

    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // <= pad start
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00  // <= 256 bits
};

/// Compute the merkle root of any collection of hashes (in place).
/// Resulting hash vector is always one element (empty returns null_hash).
BC_API void merkle_root(digests& hashes) NOEXCEPT;

/// Single hash a count of 64 byte blocks of data into state (unfinalized).
BC_API void transform(state& state, size_t blocks, const uint8_t* in) NOEXCEPT;

/// Double hash a count of 2x32 byte pairs of data, such as hashes (finalized).
/// out[] is populated to half the size of in[], and can be the same buffer.
BC_API void transform(uint8_t* out, size_t blocks, const uint8_t* in) NOEXCEPT;

} // namespace sha256
} // namespace system
} // namespace libbitcoin

#endif
