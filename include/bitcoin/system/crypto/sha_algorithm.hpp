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

template <size_t Strength, size_t Digest = Strength,
    bool_if<Strength == 256u || Strength == 512u> = true>
class algorithm
{
public:
    /// Constants.
    static constexpr auto strength     = Strength;
    static constexpr auto digest       = Digest;
    static constexpr auto word_bits    = bytes<strength>;
    static constexpr auto word_bytes   = bytes<word_bits>;
    static constexpr auto digest_words = word_bits / word_bytes;
    static constexpr auto digest_bytes = digest_words * word_bytes;
    static constexpr auto block_words  = digest_words * two;
    static constexpr auto block_bytes  = block_words * word_bytes;
    static constexpr auto buffer_words = strength / word_bytes;
    static constexpr auto buffer_bytes = buffer_words * word_bytes;

    // 64/128 bit counter for 64/128 byte blocks.
    static constexpr auto count_bits  = block_bytes;
    static constexpr auto count_bytes = bytes<count_bits>;

    /// Types.
    using byte_t   = uint8_t;
    using word_t   = unsigned_type<word_bytes>;
    using digest_t = std_array<byte_t, digest_bytes>;
    using state_t  = std_array<word_t, digest_words>;
    using block_t  = std_array<byte_t, block_bytes>;
    using words_t  = std_array<word_t, block_words>;
    using buffer_t = std_array<word_t, buffer_words>;
    using count_t  = unsigned_exact_type<count_bytes>;

    // TODO: bytes<> is limited by size_t so dividing by byte_bits.
    /// Limit incorporates requirement to encode counter in final block.
    static constexpr auto limit_bits  = maximum<count_t> - count_bits;
    static constexpr auto limit_bytes = limit_bits / byte_bits;

    /// For vectorization optimization.
    using digests = std_vector<digest_t>;
    using blocks  = std_vector<cref<block_t>>;

    /// For optimizing common scenario.
    static constexpr auto chunk_bytes = to_half(block_bytes);
    using chunk_t = std_array<uint8_t, chunk_bytes>;

    /// Streaming (any number of full blocks).
    static constexpr  void accumulate(state_t& state, const block_t& block) NOEXCEPT;
    static VCONSTEXPR void accumulate(state_t& state, const blocks& blocks) NOEXCEPT;
    static constexpr digest_t finalize(const state_t& state) NOEXCEPT;

    /// Finalized single hash (full block or half block only).
    static constexpr digest_t hash(const chunk_t& half) NOEXCEPT;
    static constexpr digest_t hash(const block_t& block) NOEXCEPT;
    static VCONSTEXPR digests hash(const blocks& blocks) NOEXCEPT;

    /// This is an optimization for merkle root computation. It leverages fixed
    /// message size to avoid padding computations, fixed iteration count to avoid
    /// internal endian conversions, and a common buffer to avoid stack calls and
    /// buffer reallocations.

    /// Finalized merkle hash (N blocks produces N/2 independent hashes).
    static constexpr digest_t merkle(const block_t& block) NOEXCEPT;
    static VCONSTEXPR digests merkle(const blocks& blocks) NOEXCEPT;

protected:
    static constexpr auto choice(auto a, auto b, auto c) NOEXCEPT;
    static constexpr auto majory(auto a, auto b, auto c) NOEXCEPT;
    static constexpr auto SIGMA0(auto a) NOEXCEPT;
    static constexpr auto SIGMA1(auto a) NOEXCEPT;
    static constexpr auto sigma0(auto a) NOEXCEPT;
    static constexpr auto sigma1(auto a) NOEXCEPT;

    template<size_t Index>
    static constexpr void expand(buffer_t& out) NOEXCEPT;
    static constexpr void expand48(buffer_t& out) NOEXCEPT;

    // 64 or 80 rounds.
    template<uint8_t Round, uint32_t K>
    static constexpr void round(state_t& out, const buffer_t& in) NOEXCEPT;
    static constexpr void rounds(state_t& out, const buffer_t& in) NOEXCEPT;
    static constexpr void round1(auto a, auto b, auto c, auto& d, auto e, auto f,
        auto g, auto& h, auto k) NOEXCEPT;

    static constexpr void summary8(state_t& out, const state_t& in) NOEXCEPT;
    static constexpr void copyin08(buffer_t& out, const state_t& in) NOEXCEPT;
    static constexpr void copyin64(buffer_t& out, const buffer_t& in) NOEXCEPT;
    static constexpr void paddin08(buffer_t& out) NOEXCEPT;
    static constexpr void paddin16(buffer_t& out, size_t blocks) NOEXCEPT;
    static constexpr void tobigs08(buffer_t& out, const chunk_t& in) NOEXCEPT;
    static constexpr void tobigs16(buffer_t& out, const block_t& in) NOEXCEPT;
    static constexpr void frbigs08(digest_t& out, const state_t& in) NOEXCEPT;

protected:
    static constexpr size_t r_64  = 64;
    static constexpr size_t r_80  = 80;
    static constexpr size_t s_256 = 256;
    static constexpr size_t s_512 = 512;

    // 64/80 constants/rounds for sha-256/215 respectively.
    static constexpr auto count_round = (strength == s_256 ? r_64 : r_80);
    using const_t = std_array<word_t, count_round>;

    /// Initialization constants (H).
    static constexpr state_t h() NOEXCEPT;

    /// Round constants (K).
    static constexpr const_t k() NOEXCEPT;
};

#ifndef TEST
template <size_t Strength, size_t Digest = Strength>
class accessory
  : public algorithm<Strength, Digest>
{
public:
    using const_t = algorithm<Strength, Digest>::const_t;
    static constexpr auto count_round = algorithm<Strength, Digest>::count_round;
};

static_assert(algorithm<256>::word_bits == 32u);
static_assert(algorithm<256>::word_bytes == 4u);
static_assert(algorithm<256>::digest_words == 8u);
static_assert(algorithm<256>::digest_bytes == 32u);
static_assert(algorithm<256>::block_words == 16u);
static_assert(algorithm<256>::block_bytes == 64u);
static_assert(algorithm<256>::chunk_bytes == 32u);
static_assert(algorithm<256>::buffer_words == 64u);
static_assert(algorithm<256>::buffer_bytes == 256u);
static_assert(algorithm<256>::count_bits == 64u);
static_assert(algorithm<256>::count_bytes == 8u);
static_assert(accessory<256>::count_round == 64u);
static_assert(algorithm<256>::limit_bits == std::numeric_limits<uint64_t>::max() - 64u);
static_assert(algorithm<256>::limit_bytes == algorithm<256>::limit_bits / byte_bits);
static_assert(is_same_type<algorithm<256>::byte_t, uint8_t>);
static_assert(is_same_type<algorithm<256>::word_t, uint32_t>);
static_assert(is_same_type<algorithm<256>::count_t, uint64_t>);
static_assert(is_same_type<accessory<256>::const_t, std_array<uint32_t, 64>>);
static_assert(is_same_type<algorithm<256>::digest_t, std_array<uint8_t, 32>>);
static_assert(is_same_type<algorithm<256>::block_t, std_array<uint8_t, 64>>);
static_assert(is_same_type<algorithm<256>::words_t, std_array<uint32_t, 16>>);
static_assert(is_same_type<algorithm<256>::chunk_t, std_array<uint8_t, 32>>);
static_assert(is_same_type<decltype(algorithm<256>::limit_bits), const uint64_t>);
static_assert(is_same_type<decltype(algorithm<256>::limit_bytes), const uint64_t>);

static_assert(algorithm<512>::word_bits == 64u);
static_assert(algorithm<512>::word_bytes == 8u);
static_assert(algorithm<512>::digest_words == 8u);
static_assert(algorithm<512>::digest_bytes == 64u);
static_assert(algorithm<512>::block_words == 16u);
static_assert(algorithm<512>::block_bytes == 128u);
static_assert(algorithm<512>::chunk_bytes == 64u);
static_assert(algorithm<512>::buffer_words == 64u);
static_assert(algorithm<512>::buffer_bytes == 512u);
static_assert(algorithm<512>::count_bits == 128u);
static_assert(algorithm<512>::count_bytes == 16u);
static_assert(accessory<512>::count_round == 80u);
static_assert(algorithm<512>::limit_bits == std::numeric_limits<uint128_t>::max() - 128u);
static_assert(algorithm<512>::limit_bytes == algorithm<512>::limit_bits / byte_bits);
static_assert(is_same_type<algorithm<512>::byte_t, uint8_t>);
static_assert(is_same_type<algorithm<512>::word_t, uint64_t>);
static_assert(is_same_type<algorithm<512>::count_t, uint128_t>);
static_assert(is_same_type<accessory<512>::const_t, std_array<uint64_t, 80>>);
static_assert(is_same_type<algorithm<512>::digest_t, std_array<uint8_t, 64>>);
static_assert(is_same_type<algorithm<512>::block_t, std_array<uint8_t, 128>>);
static_assert(is_same_type<algorithm<512>::words_t, std_array<uint64_t, 16>>);
static_assert(is_same_type<algorithm<512>::buffer_t, std_array<uint64_t, 64>>);
static_assert(is_same_type<algorithm<512>::chunk_t, std_array<uint8_t, 64>>);
static_assert(is_same_type<decltype(algorithm<512>::limit_bits), const uint128_t>);
static_assert(is_same_type<decltype(algorithm<512>::limit_bytes), const uint128_t>);
#endif

using word_t = uint32_t;
constexpr size_t state_words = 8u;
constexpr size_t digest_size = 32u;
constexpr size_t block_size  = digest_size * two;
constexpr size_t word_size   = sizeof(word_t);
constexpr size_t block_words = block_size / word_size;

using block   = std_array<uint8_t,  block_size>;  // input
using digest  = std_array<uint8_t,  digest_size>; // output
using padding = std_array<word_t, block_words>; // input as words
using state   = std_array<word_t, state_words>; // output as words
using buffer  = std_array<word_t, block_size>;  // word per block byte

constexpr size_t half_size = to_half(block_size);
using half = std_array<uint8_t, half_size>;   // common scenario

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
constexpr digest hash(const half& half) NOEXCEPT;
constexpr digest hash(const block& block) NOEXCEPT;
VCONSTEXPR digest hash(const blocks& blocks) NOEXCEPT;

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
