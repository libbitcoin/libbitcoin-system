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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SHA_ALGORITHM_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SHA_ALGORITHM_HPP

#include <functional>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace sha {
    
constexpr size_t digest_size = 32;
constexpr size_t block_size  = 64;
constexpr size_t state_size  = 8;
constexpr size_t padding_size = block_size / sizeof(uint32_t);

using digest  = std_array<uint8_t,  digest_size>;
using block   = std_array<uint8_t,  block_size>;
using state   = std_array<uint32_t, state_size>;
using buffer  = std_array<uint32_t, block_size>;
using padding = std_array<uint32_t, padding_size>;

using digests = std_vector<digest>;
using blocks = std_vector<std::reference_wrapper<const block>>;

constexpr state initial
{
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
    0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

constexpr state pad32
{
    0x80000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000100
};

constexpr padding pad64
{
    0x80000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000200
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

/// Streaming (any number of full blocks).
constexpr void accumulate(state& state, const block& block) NOEXCEPT;
VCONSTEXPR void accumulate(state& state, const blocks& blocks) NOEXCEPT;
constexpr digest finalize(const state& state) NOEXCEPT;

/// Finalized single hash (full block or half block only).
constexpr digest hash(const block& data) NOEXCEPT;
constexpr digest hash(const digest& data) NOEXCEPT;
VCONSTEXPR digest hash(const blocks& data) NOEXCEPT;

/// This is an optimization for merkle root computation. It leverages fixed
/// message size to avoid padding computations, fixed iteration count to avoid
/// internal endian conversions, and a common buffer to avoid stack calls and
/// buffer reallocations.

/// Finalized merkle hash (N blocks produces N/2 independent hashes).
constexpr digest merkle(const block& block) NOEXCEPT;
VCONSTEXPR digests merkle(const blocks& blocks) NOEXCEPT;

} // namespace sha
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/crypto/sha_algorithm.ipp>

#endif
