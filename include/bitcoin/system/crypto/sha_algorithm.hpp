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
#include <type_traits>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace sha {

template <typename Word, size_t Rounds,
    bool_if<is_same_type<Word, uint32_t> ||
            is_same_type<Word, uint64_t>> = true,
    bool_if<Rounds == 64 || Rounds == 80> = true,
    if_integral_integer<Word> = true>
struct k
{
    static constexpr size_t rounds = Rounds;
    using constants_t = std_array<Word, Rounds>;
};

struct k160 : public k<uint32_t, 80>
{
    using base = k<uint32_t, 80>;
    static constexpr base::constants_t get
    {
        0x5a827999, // rounds 0..19
        0x5a827999,
        0x5a827999,
        0x5a827999,
        0x5a827999,
        0x5a827999,
        0x5a827999,
        0x5a827999,
        0x5a827999,
        0x5a827999,
        0x5a827999,
        0x5a827999,
        0x5a827999,
        0x5a827999,
        0x5a827999,
        0x5a827999,
        0x5a827999,
        0x5a827999,
        0x5a827999,
        0x5a827999,
        0x6ed9eba1, // rounds 20..39
        0x6ed9eba1,
        0x6ed9eba1,
        0x6ed9eba1,
        0x6ed9eba1,
        0x6ed9eba1,
        0x6ed9eba1,
        0x6ed9eba1,
        0x6ed9eba1,
        0x6ed9eba1,
        0x6ed9eba1,
        0x6ed9eba1,
        0x6ed9eba1,
        0x6ed9eba1,
        0x6ed9eba1,
        0x6ed9eba1,
        0x6ed9eba1,
        0x6ed9eba1,
        0x6ed9eba1,
        0x6ed9eba1,
        0x8f1bbcdc, // rounds 40..59
        0x8f1bbcdc,
        0x8f1bbcdc,
        0x8f1bbcdc,
        0x8f1bbcdc,
        0x8f1bbcdc,
        0x8f1bbcdc,
        0x8f1bbcdc,
        0x8f1bbcdc,
        0x8f1bbcdc,
        0x8f1bbcdc,
        0x8f1bbcdc,
        0x8f1bbcdc,
        0x8f1bbcdc,
        0x8f1bbcdc,
        0x8f1bbcdc,
        0x8f1bbcdc,
        0x8f1bbcdc,
        0x8f1bbcdc,
        0x8f1bbcdc,
        0xca62c1d6, // rounds 60..79
        0xca62c1d6,
        0xca62c1d6,
        0xca62c1d6,
        0xca62c1d6,
        0xca62c1d6,
        0xca62c1d6,
        0xca62c1d6,
        0xca62c1d6,
        0xca62c1d6,
        0xca62c1d6,
        0xca62c1d6,
        0xca62c1d6,
        0xca62c1d6,
        0xca62c1d6,
        0xca62c1d6,
        0xca62c1d6,
        0xca62c1d6,
        0xca62c1d6,
        0xca62c1d6
    };
};

struct k256 : public k<uint32_t, 64>
{
    using base = k<uint32_t, 64>;
    static constexpr constants_t get
    {
        0x428a2f98,
        0x71374491,
        0xb5c0fbcf,
        0xe9b5dba5,
        0x3956c25b,
        0x59f111f1,
        0x923f82a4,
        0xab1c5ed5,
        0xd807aa98,
        0x12835b01,
        0x243185be,
        0x550c7dc3,
        0x72be5d74,
        0x80deb1fe,
        0x9bdc06a7,
        0xc19bf174,
        0xe49b69c1,
        0xefbe4786,
        0x0fc19dc6,
        0x240ca1cc,
        0x2de92c6f,
        0x4a7484aa,
        0x5cb0a9dc,
        0x76f988da,
        0x983e5152,
        0xa831c66d,
        0xb00327c8,
        0xbf597fc7,
        0xc6e00bf3,
        0xd5a79147,
        0x06ca6351,
        0x14292967,
        0x27b70a85,
        0x2e1b2138,
        0x4d2c6dfc,
        0x53380d13,
        0x650a7354,
        0x766a0abb,
        0x81c2c92e,
        0x92722c85,
        0xa2bfe8a1,
        0xa81a664b,
        0xc24b8b70,
        0xc76c51a3,
        0xd192e819,
        0xd6990624,
        0xf40e3585,
        0x106aa070,
        0x19a4c116,
        0x1e376c08,
        0x2748774c,
        0x34b0bcb5,
        0x391c0cb3,
        0x4ed8aa4a,
        0x5b9cca4f,
        0x682e6ff3,
        0x748f82ee,
        0x78a5636f,
        0x84c87814,
        0x8cc70208,
        0x90befffa,
        0xa4506ceb,
        0xbef9a3f7,
        0xc67178f2
    };
};

struct k512 : public k<uint64_t, 80>
{
    using base = k<uint64_t, 80>;
    static constexpr constants_t get
    {
        0x428a2f98d728ae22,
        0x7137449123ef65cd,
        0xb5c0fbcfec4d3b2f,
        0xe9b5dba58189dbbc,
        0x3956c25bf348b538,
        0x59f111f1b605d019,
        0x923f82a4af194f9b,
        0xab1c5ed5da6d8118,
        0xd807aa98a3030242,
        0x12835b0145706fbe,
        0x243185be4ee4b28c,
        0x550c7dc3d5ffb4e2,
        0x72be5d74f27b896f,
        0x80deb1fe3b1696b1,
        0x9bdc06a725c71235,
        0xc19bf174cf692694,
        0xe49b69c19ef14ad2,
        0xefbe4786384f25e3,
        0x0fc19dc68b8cd5b5,
        0x240ca1cc77ac9c65,
        0x2de92c6f592b0275,
        0x4a7484aa6ea6e483,
        0x5cb0a9dcbd41fbd4,
        0x76f988da831153b5,
        0x983e5152ee66dfab,
        0xa831c66d2db43210,
        0xb00327c898fb213f,
        0xbf597fc7beef0ee4,
        0xc6e00bf33da88fc2,
        0xd5a79147930aa725,
        0x06ca6351e003826f,
        0x142929670a0e6e70,
        0x27b70a8546d22ffc,
        0x2e1b21385c26c926,
        0x4d2c6dfc5ac42aed,
        0x53380d139d95b3df,
        0x650a73548baf63de,
        0x766a0abb3c77b2a8,
        0x81c2c92e47edaee6,
        0x92722c851482353b,
        0xa2bfe8a14cf10364,
        0xa81a664bbc423001,
        0xc24b8b70d0f89791,
        0xc76c51a30654be30,
        0xd192e819d6ef5218,
        0xd69906245565a910,
        0xf40e35855771202a,
        0x106aa07032bbd1b8,
        0x19a4c116b8d2d0c8,
        0x1e376c085141ab53,
        0x2748774cdf8eeb99,
        0x34b0bcb5e19b48a8,
        0x391c0cb3c5c95a63,
        0x4ed8aa4ae3418acb,
        0x5b9cca4f7763e373,
        0x682e6ff3d6b2b8a3,
        0x748f82ee5defb2fc,
        0x78a5636f43172f60,
        0x84c87814a1f0ab72,
        0x8cc702081a6439ec,
        0x90befffa23631e28,
        0xa4506cebde82bde9,
        0xbef9a3f7b2c67915,
        0xc67178f2e372532b,
        0xca273eceea26619c,
        0xd186b8c721c0c207,
        0xeada7dd6cde0eb1e,
        0xf57d4f7fee6ed178,
        0x06f067aa72176fba,
        0x0a637dc5a2c898a6,
        0x113f9804bef90dae,
        0x1b710b35131c471b,
        0x28db77f523047d84,
        0x32caab7b40c72493,
        0x3c9ebe0a15c9bebc,
        0x431d67c49c100d4c,
        0x4cc5d4becb3e42b6,
        0x597f299cfc657e2a,
        0x5fcb6fab3ad6faec,
        0x6c44198c4a475817
    };
};

template <size_t Strength, size_t Digest,
    if_not_greater<Digest, Strength> = true,
    bool_if<(Strength == 256) || (Strength == 512)> = true>
struct h
{
    static constexpr auto digest     = Digest;
    static constexpr auto strength   = Strength;
    static constexpr auto word_bits  = bytes<strength>;
    static constexpr auto word_bytes = bytes<word_bits>;
    static constexpr auto state = (Strength == 256) && (Digest == 160) ? 5 : 8;

    using word_t   = unsigned_type<word_bytes>;
    using state_t  = std_array<word_t, state>; // initialization vector
    using chunk_t  = std_array<word_t, 8>;  // half block padding optimize
    using words_t  = std_array<word_t, 16>; // block padding optimize/streaming
    using buffer_t = std_array<word_t, 64>; // block padding with expansion
    using K = std::conditional_t<Strength == 512, k512,
        std::conditional_t<Digest == 160, k160, k256>>;
};

struct h160
  : public h<256, 160>
{
    using base = h<256, 160>;

    static constexpr const base::state_t get
    {
        0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476, 0xc3d2e1f0
    };

    // 256 bits (half block data) pad.
    static constexpr base::chunk_t chunk_pad
    {
        // TODO: populate.
    };

    // 512 bits (one block data) pad, without expansion.
    static constexpr base::words_t block_pad
    {
        // TODO: populate.
    };

    // 512 bits (one block data) pad, with expansion.
    static constexpr base::buffer_t buffer_pad
    {
        // TODO: generate/populate.
    };

    // Uncounted block-sized pad, used for streaming.
    static constexpr base::words_t stream_pad
    {
        // TODO: populate.
    };
};

template <size_t Digest>
struct h256
  : public h<256, Digest>
{
    using base = h<256, Digest>;

    // H values (initial state).
    static constexpr base::state_t get
    {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };

    // 256 bits (half block data) pad.
    static constexpr base::chunk_t chunk_pad
    {
        0x80000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000100,
    };

    // This is only used for the normalized merkle algorithm.
    // 512 bits (one block data) pad, without expansion.
    static constexpr base::words_t block_pad
    {
        0x80000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000200
    };

    // 512 bits (one block data) pad, with expansion.
    static constexpr base::buffer_t buffer_pad
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

    // Uncounted block-sized pad, used for streaming.
    static constexpr base::words_t stream_pad
    {
        0x80000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000
    };
};

template <size_t Digest>
struct h512
  : public h<512, Digest>
{
    using base = h<512, Digest>;

    // H values (initial state).
    static constexpr base::state_t get
    {
        0x6a09e667f3bcc908,
        0xbb67ae8584caa73b,
        0x3c6ef372fe94f82b,
        0xa54ff53a5f1d36f1,
        0x510e527fade682d1,
        0x9b05688c2b3e6c1f,
        0x1f83d9abfb41bd6b,
        0x5be0cd19137e2179
    };

    // 512 bits (half block data) pad.
    static constexpr base::chunk_t chunk_pad
    {
        // TODO: populate.
    };

    // This is only used for the normalized merkle algorithm.
    // 1024 bits (one block data) pad, without expansion.
    static constexpr base::words_t block_pad
    {
        // TODO: populate.
    };

    // 1024 bits (one block data) pad, with expansion.
    static constexpr base::buffer_t buffer_pad
    {
        // TODO: generate/populate.
    };

    // Uncounted block-sized pad, used for streaming.
    static constexpr base::words_t stream_pad
    {
        // TODO: populate.
    };
};

/// FIPS180 SHA256/512
using sha160     = h160;
using sha256_224 = h256<224>;
using sha256_256 = h256<256>;
using sha512_384 = h512<384>;
using sha512_224 = h512<224>;
using sha512_256 = h512<256>;
using sha512_512 = h512<512>;

template <typename SHA>
class algorithm
{
public:
    /// Constants.
    static constexpr auto digest_words = SHA::word_bits / SHA::word_bytes;
    static constexpr auto block_words  = digest_words   * two;
    static constexpr auto buffer_words = SHA::strength  / SHA::word_bytes;
    static constexpr auto digest_bytes = digest_words   * SHA::word_bytes;
    static constexpr auto block_bytes  = block_words    * SHA::word_bytes;
    static constexpr auto buffer_bytes = buffer_words   * SHA::word_bytes;

    // 64/128 bit counter for 64/128 byte blocks.
    static constexpr auto count_bits  = block_bytes;
    static constexpr auto count_bytes = bytes<count_bits>;

    /// Types.
    using H        = SHA;
    using K        = SHA::K;
    using byte_t   = uint8_t;
    using word_t   = SHA::word_t;
    using digest_t = std_array<byte_t, digest_bytes>;
    using block_t  = std_array<byte_t, block_bytes>;
    using state_t  = std_array<word_t, digest_words>;
    using words_t  = std_array<word_t, block_words>;
    using buffer_t = std_array<word_t, buffer_words>;
    using count_t  = unsigned_exact_type<count_bytes>;

    // TODO: bytes<> is limited by size_t so dividing by byte_bits.
    /// Limit incorporates requirement to encode counter in final block.
    static constexpr auto limit_bits = maximum<count_t> - count_bits;
    static constexpr auto limit_bytes = limit_bits / byte_bits;

    /// For vectorization optimization.
    using digests = std_vector<digest_t>;
    using blocks = std_vector<cref<block_t>>;

    /// For optimizing common scenario.
    static constexpr auto chunk_words = to_half(block_words);
    static constexpr auto chunk_bytes = chunk_words * SHA::word_bytes;
    using chunk_t = std_array<word_t, chunk_words>;
    using half_t = std_array<uint8_t, chunk_bytes>;

    /// Streaming (any number of full blocks).
    static constexpr  void accumulate(state_t& state, const block_t& block) NOEXCEPT;
    static VCONSTEXPR void accumulate(state_t& state, const blocks& blocks) NOEXCEPT;
    static constexpr digest_t finalize(const state_t& state) NOEXCEPT;

    /// Finalized single hash (full block or half block only).
    static constexpr  digest_t hash(const half_t& half) NOEXCEPT;
    static constexpr  digest_t hash(const block_t& block) NOEXCEPT;
    static VCONSTEXPR digest_t hash(const blocks& blocks) NOEXCEPT;

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
    static constexpr void round1(auto a, auto b, auto c, auto& d, auto e, auto f,
        auto g, auto& h, auto k) NOEXCEPT;

    // 64 or 80 rounds.
    template<size_t Round>
    static constexpr void round(state_t& out, const buffer_t& in) NOEXCEPT;
    static constexpr void rounds(state_t& out, const buffer_t& in) NOEXCEPT;

    // 48 words.
    template<size_t Word>
    static constexpr void expand(buffer_t& out) NOEXCEPT;
    static constexpr void expand48(buffer_t& out) NOEXCEPT;
    static constexpr void copying8(buffer_t& out, const state_t& in) NOEXCEPT;
    static constexpr void summary8(state_t& out, const state_t& in) NOEXCEPT;
    static constexpr void paddin64(buffer_t& out) NOEXCEPT;
    static constexpr void padding8(buffer_t& out) NOEXCEPT;
    static constexpr void paddin16(buffer_t& out, count_t blocks) NOEXCEPT;
    static constexpr void bigend08(buffer_t& out, const half_t& in) NOEXCEPT;
    static constexpr void bigend16(buffer_t& out, const block_t& in) NOEXCEPT;
    static constexpr void bigend08(digest_t& out, const state_t& in) NOEXCEPT;
};

#ifndef REMOVE
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
#endif

} // namespace sha
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/crypto/sha_algorithm.ipp>

#endif
