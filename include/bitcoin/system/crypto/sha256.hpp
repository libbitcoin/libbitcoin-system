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
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace sha256 {
    
constexpr size_t digest_size = 32;
constexpr size_t block_size  = 64;
constexpr size_t state_size  = 8;

using digest = std_array<uint8_t,  digest_size>;
using block  = std_array<uint8_t,  block_size>;
using state  = std_array<uint32_t, state_size>;
using buffer = std_array<uint32_t, block_size>;

using block1 = std_array<block, 1>;
using block2 = std_array<block, 2>;
using block4 = std_array<block, 4>;
using block8 = std_array<block, 8>;

using digest1 = std_array<digest, 1>;
using digest2 = std_array<digest, 2>;
using digest4 = std_array<digest, 4>;
using digest8 = std_array<digest, 8>;

using blocks  = std::vector<block>;
using digests = std::vector<digest>;

constexpr state initial
{
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
    0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

constexpr block stream_pad
{
    0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

constexpr state pad32
{
    0x80000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000100
};

constexpr buffer expanded_pad64
{
    0x80000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000200,
    0x80000000, 0x01400000, 0x00205000, 0x00005088,
    0x22000800, 0x22550014, 0x05089742, 0xa0000020,
    0x5a880000, 0x005c9400, 0x0016d49d, 0xfa801f00,
    0xd33225d0, 0x11675959, 0xf6e6bfda, 0xb30c1549,
    0x08b2b050, 0x9d7c4c27, 0x0ce2a393, 0x88e6e1ea,
    0xa52b4335, 0x67a16f49, 0xd732016f, 0x4eeb2e91,
    0x5dbf55e5, 0x8eee2335, 0xe2bc5ec2, 0xa83f4394,
    0x45ad78f7, 0x36f3d0cd, 0xd99c05e8, 0xb0511dc7,
    0x69bc7ac4, 0xbd11375b, 0xe3ba71e5, 0x3b209ff2,
    0x18feee17, 0xe25ad9e7, 0x13375046, 0x0515089d,
    0x4f0d0f04, 0x2627484e, 0x310128d2, 0xc668b434,
    0x420841cc, 0x62d311b8, 0xe59ba771, 0x85a7a484
};

/// Finalized hash of any sized data, out32 must be at least 32 bytes.
BC_API bool hash(uint8_t* out32, size_t size, const uint8_t* in) NOEXCEPT;

/// Compute the merkle root of any rvalue collection of hashes.
BC_API digest merkle_root(digests&& hashes) NOEXCEPT;

// Removal pending hash generalization.
// ============================================================================

/// Bit count is encoded into message block as 64 bit big-endian.
/// This is by convention and not subject to platform endianness.
/// The padding sentinel is effectively a single byte (not a full word).
constexpr auto xxxx = 0xff_u8; // unused space
constexpr auto pppp = 0x80_u8; // pad sentinel
constexpr auto zzzz = 0x00_u8; // zeros of bit count
constexpr auto full = to_bits(block_size) >> byte_bits;  // bit count >> 8
constexpr auto half = to_bits(digest_size) >> byte_bits; // bit count >> 8

/// Padding for full block hash round (64 bytes of pad/count).
/// The buffer is prefilled with padding and a count of 512 bits.
alignas(16) constexpr block pad_64
{
    pppp, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    zzzz, zzzz, zzzz, zzzz, zzzz, zzzz, full, zzzz
};

/// Padding for a half block hash round (32 bytes of pad/count).
/// The buffer is prefilled with padding and a count of 256 bits.
alignas(16) constexpr block pad_32
{
    xxxx, xxxx, xxxx, xxxx, xxxx, xxxx, xxxx, xxxx,
    xxxx, xxxx, xxxx, xxxx, xxxx, xxxx, xxxx, xxxx,
    xxxx, xxxx, xxxx, xxxx, xxxx, xxxx, xxxx, xxxx,
    xxxx, xxxx, xxxx, xxxx, xxxx, xxxx, xxxx, xxxx,

    pppp, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    zzzz, zzzz, zzzz, zzzz, zzzz, zzzz, half, zzzz
};

// TODO: exposed for streaming until convert all to context object.
/// Hash a count of 64 byte blocks of data into state.
BC_API void update(state& state, size_t blocks, const uint8_t* in) NOEXCEPT;
/// Finalize a fully padded state vector.
BC_API void finalize(const state& state, uint8_t* out32) NOEXCEPT;

////// TODO: exposed only for test.
/////// Double hash a count of 2x32 byte pairs of data, such as hashes (finalized).
/////// out[] is populated to half the size of in[], and can be the same buffer.
////BC_API void merkle_hash(uint8_t* out, size_t blocks,
////    const uint8_t* in) NOEXCEPT;

// ----------------------------------------------------------------------------

} // namespace sha256
} // namespace system
} // namespace libbitcoin

#endif
