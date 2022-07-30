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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_HPP
#define LIBBITCOIN_SYSTEM_HASH_SHA_ALGORITHM_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace sha {

template <size_t Strength, size_t Rounds,
    bool_if<Strength == 160 || Strength == 256 || Strength == 512> = true,
    bool_if<Rounds == 64 || Rounds == 80> = true>
struct k
{
    static constexpr auto strength = Strength;
    static constexpr auto rounds = Rounds;
    static constexpr auto size = (strength == 160 ? 256_size : strength);
    using constants_t = std_array<
        iif<strength == 512, uint64_t, uint32_t>, rounds>;
};

struct k160
  : public k<160, 80>
{
    static constexpr constants_t get
    {
        // rounds 0..19
        0x5a827999, 0x5a827999, 0x5a827999, 0x5a827999,
        0x5a827999, 0x5a827999, 0x5a827999, 0x5a827999,
        0x5a827999, 0x5a827999, 0x5a827999, 0x5a827999,
        0x5a827999, 0x5a827999, 0x5a827999, 0x5a827999,
        0x5a827999, 0x5a827999, 0x5a827999, 0x5a827999,

        // rounds 20..39
        0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1,
        0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1,
        0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1,
        0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1,
        0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1, 0x6ed9eba1,

        // rounds 40..59
        0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc,
        0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc,
        0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc,
        0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc,
        0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc, 0x8f1bbcdc,

        // rounds 60..79
        0xca62c1d6, 0xca62c1d6, 0xca62c1d6, 0xca62c1d6,
        0xca62c1d6, 0xca62c1d6, 0xca62c1d6, 0xca62c1d6,
        0xca62c1d6, 0xca62c1d6, 0xca62c1d6, 0xca62c1d6,
        0xca62c1d6, 0xca62c1d6, 0xca62c1d6, 0xca62c1d6,
        0xca62c1d6, 0xca62c1d6, 0xca62c1d6, 0xca62c1d6
    };
};

struct k256
  : public k<256, 64>
{
    static constexpr constants_t get
    {
        // rounds 0..15
        0x428a2f98,	0x71374491,	0xb5c0fbcf,	0xe9b5dba5,
        0x3956c25b,	0x59f111f1,	0x923f82a4,	0xab1c5ed5,
        0xd807aa98,	0x12835b01,	0x243185be,	0x550c7dc3,
        0x72be5d74,	0x80deb1fe,	0x9bdc06a7,	0xc19bf174,

        // rounds 15..31
        0xe49b69c1,	0xefbe4786,	0x0fc19dc6,	0x240ca1cc,
        0x2de92c6f,	0x4a7484aa,	0x5cb0a9dc,	0x76f988da,
        0x983e5152,	0xa831c66d,	0xb00327c8,	0xbf597fc7,
        0xc6e00bf3,	0xd5a79147,	0x06ca6351,	0x14292967,

        // rounds 32..47
        0x27b70a85,	0x2e1b2138,	0x4d2c6dfc,	0x53380d13,
        0x650a7354,	0x766a0abb,	0x81c2c92e,	0x92722c85,
        0xa2bfe8a1,	0xa81a664b,	0xc24b8b70,	0xc76c51a3,
        0xd192e819,	0xd6990624,	0xf40e3585,	0x106aa070,

        // rounds 48..63
        0x19a4c116,	0x1e376c08,	0x2748774c,	0x34b0bcb5,
        0x391c0cb3,	0x4ed8aa4a,	0x5b9cca4f,	0x682e6ff3,
        0x748f82ee,	0x78a5636f,	0x84c87814,	0x8cc70208,
        0x90befffa,	0xa4506ceb,	0xbef9a3f7,	0xc67178f2
    };
};

struct k512
  : public k<512, 80>
{
    static constexpr constants_t get
    {
        // rounds 0..15
        0x428a2f98d728ae22,	0x7137449123ef65cd,
        0xb5c0fbcfec4d3b2f,	0xe9b5dba58189dbbc,
        0x3956c25bf348b538,	0x59f111f1b605d019,
        0x923f82a4af194f9b,	0xab1c5ed5da6d8118,
        0xd807aa98a3030242,	0x12835b0145706fbe,
        0x243185be4ee4b28c,	0x550c7dc3d5ffb4e2,
        0x72be5d74f27b896f,	0x80deb1fe3b1696b1,
        0x9bdc06a725c71235,	0xc19bf174cf692694,

        // rounds 15..31
        0xe49b69c19ef14ad2,	0xefbe4786384f25e3,
        0x0fc19dc68b8cd5b5,	0x240ca1cc77ac9c65,
        0x2de92c6f592b0275,	0x4a7484aa6ea6e483,
        0x5cb0a9dcbd41fbd4,	0x76f988da831153b5,
        0x983e5152ee66dfab,	0xa831c66d2db43210,
        0xb00327c898fb213f,	0xbf597fc7beef0ee4,
        0xc6e00bf33da88fc2,	0xd5a79147930aa725,
        0x06ca6351e003826f,	0x142929670a0e6e70,

        // rounds 32..47
        0x27b70a8546d22ffc,	0x2e1b21385c26c926,
        0x4d2c6dfc5ac42aed,	0x53380d139d95b3df,
        0x650a73548baf63de,	0x766a0abb3c77b2a8,
        0x81c2c92e47edaee6,	0x92722c851482353b,
        0xa2bfe8a14cf10364,	0xa81a664bbc423001,
        0xc24b8b70d0f89791,	0xc76c51a30654be30,
        0xd192e819d6ef5218,	0xd69906245565a910,
        0xf40e35855771202a,	0x106aa07032bbd1b8,

        // rounds 48..63
        0x19a4c116b8d2d0c8,	0x1e376c085141ab53,
        0x2748774cdf8eeb99,	0x34b0bcb5e19b48a8,
        0x391c0cb3c5c95a63,	0x4ed8aa4ae3418acb,
        0x5b9cca4f7763e373,	0x682e6ff3d6b2b8a3,
        0x748f82ee5defb2fc,	0x78a5636f43172f60,
        0x84c87814a1f0ab72,	0x8cc702081a6439ec,
        0x90befffa23631e28,	0xa4506cebde82bde9,
        0xbef9a3f7b2c67915,	0xc67178f2e372532b,

        // rounds 64..79
        0xca273eceea26619c,	0xd186b8c721c0c207,
        0xeada7dd6cde0eb1e,	0xf57d4f7fee6ed178,
        0x06f067aa72176fba,	0x0a637dc5a2c898a6,
        0x113f9804bef90dae,	0x1b710b35131c471b,
        0x28db77f523047d84,	0x32caab7b40c72493,
        0x3c9ebe0a15c9bebc,	0x431d67c49c100d4c,
        0x4cc5d4becb3e42b6,	0x597f299cfc657e2a,
        0x5fcb6fab3ad6faec,	0x6c44198c4a475817
    };
};

template <typename Constants, size_t Digest = Constants::strength,
    if_not_greater<Digest, Constants::size> = true>
struct h
{
    using K = Constants;

    static constexpr auto digest       = Digest;
    static constexpr auto size         = K::size;
    static constexpr auto rounds       = K::rounds;
    static constexpr auto word_bits    = bytes<size>;
    static constexpr auto word_bytes   = bytes<word_bits>;
    static constexpr auto block_words  = bytes<size> / to_half(word_bytes);
    static constexpr auto chunk_words  = to_half(block_words);
    static constexpr auto state_words  = Digest == 160 ? 5 : 8;

    using word_t  = unsigned_type<word_bytes>;
    using state_t = std_array<word_t, state_words>;
};

struct h160
  : public h<k160>
{
    using state_t = typename h<k160>::state_t;

    static constexpr state_t get
    {
        0x67452301,
        0xefcdab89,
        0x98badcfe,
        0x10325476,
        0xc3d2e1f0
    };
};

// Digest 224 changes IV (specialize template).
template <size_t Digest = 256,
    bool_if<Digest == 224 || Digest == 256> = true>
struct h256
  : public h<k256, Digest>
{
    using state_t = typename h<k256, Digest>::state_t;

    static constexpr state_t get
    {
        0x6a09e667,
        0xbb67ae85,
        0x3c6ef372,
        0xa54ff53a,
        0x510e527f,
        0x9b05688c,
        0x1f83d9ab,
        0x5be0cd19
    };
};

// Digest 224/256/384 change IV (specialize template).
template <size_t Digest = 512,
    bool_if<Digest == 224 || Digest == 256 ||
            Digest == 384 || Digest == 512> = true>
struct h512
  : public h<k512, Digest>
{
    using state_t = typename h<k512, Digest>::state_t;

    static constexpr state_t get
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
};

/// FIPS180 SHA1/SHA256/SHA512 variants.
using sha160     = h160;
using sha256_224 = h256<224>; // not fully implemented
using sha256     = h256<>;
using sha256_256 = h256<>;
using sha512_256 = h512<256>; // not fully implemented
using sha512_224 = h512<224>; // not fully implemented
using sha512_384 = h512<384>; // not fully implemented
using sha512     = h512<>;
using sha512_512 = h512<>;

/// SHA hashing algorithm.
template <typename SHA, bool Concurrent = true>
class algorithm
{
public:
    /// Types.
    /// -----------------------------------------------------------------------

    /// Aliases.
    using H         = SHA;
    using K         = typename SHA::K;
    using word_t    = typename SHA::word_t;
    using state_t   = typename SHA::state_t;

    /// Word-based types.
    using chunk_t   = std_array<word_t, SHA::chunk_words>;
    using words_t   = std_array<word_t, SHA::block_words>;
    using buffer_t  = std_array<word_t, K::rounds>;

    /// Byte-based types.
    using byte_t    = uint8_t;
    using half_t    = std_array<byte_t, SHA::chunk_words * SHA::word_bytes>;
    using block_t   = std_array<byte_t, SHA::block_words * SHA::word_bytes>;
    using digest_t  = std_array<byte_t, bytes<SHA::digest>>;

    /// Vectorization types (set types are cref).
    template <size_t Size>
    using sets_t    = std_vector<cref<std_array<block_t, Size>>>;
    using set_t     = std_vector<cref<block_t>>;
    using states_t  = std_vector<state_t>;
    using digests_t = std_vector<digest_t>;

    /// Constants (and count_t).
    /// -----------------------------------------------------------------------

    /// count_t is 64 or 128 bit (sha512 is 128 bit and uses uint128_t).
    static constexpr auto count_bits    = SHA::block_words * SHA::word_bytes;
    static constexpr auto count_bytes   = bytes<count_bits>;
    using count_t = unsigned_exact_type<bytes<count_bits>>;

    /// Limits incorporate requirement to encode counter in final block.
    static constexpr auto limit_bits    = maximum<count_t> - count_bits;
    static constexpr auto limit_bytes   = to_floored_bytes(limit_bits);
    static constexpr auto big_end_count = true;

    /// Hashing (finalized).
    /// -----------------------------------------------------------------------

    /// Finalized single hash.
    static VCONSTEXPR digest_t hash(const set_t& blocks) NOEXCEPT;
    static constexpr digest_t hash(const block_t& block) NOEXCEPT;
    static constexpr digest_t hash(const half_t& half) NOEXCEPT;

    /// Finalized double hash (N independent blocks produces N/2 hashes).
    ////static VCONSTEXPR digest_t merkle_hash(const set_t& blocks) NOEXCEPT;
    static VCONSTEXPR digests_t double_hash(const set_t& blocks) NOEXCEPT;
    static constexpr digest_t double_hash(const block_t& block) NOEXCEPT;
    static constexpr digest_t double_hash(const digest_t& left,
        const digest_t& right) NOEXCEPT;

    /// Streaming (unfinalized).
    /// -----------------------------------------------------------------------

    /// N independent blocks produces N independently accumulated states.
    template <size_t Size>
    static VCONSTEXPR states_t accumulate(const sets_t<Size>& blocks) NOEXCEPT;

    /// One or more dependent blocks produces one state.
    static constexpr void accumulate(state_t& state, const block_t& block) NOEXCEPT;
    static VCONSTEXPR void accumulate(state_t& state, const set_t& blocks) NOEXCEPT;

    /// Finalize streaming state (converts to big-endian bytes).
    static constexpr digest_t finalize(const state_t& state) NOEXCEPT;

protected:
    /// Functions
    /// -----------------------------------------------------------------------

    static constexpr auto parity(auto x, auto y, auto z) NOEXCEPT;
    static constexpr auto choice(auto x, auto y, auto z) NOEXCEPT;
    static constexpr auto majority(auto x, auto y, auto z) NOEXCEPT;

    static constexpr auto SIGMA0(auto x) NOEXCEPT;
    static constexpr auto SIGMA1(auto x) NOEXCEPT;
    static constexpr auto sigma0(auto x) NOEXCEPT;
    static constexpr auto sigma1(auto x) NOEXCEPT;

    /// Rounds
    /// -----------------------------------------------------------------------

    template<size_t Round>
    static CONSTEVAL auto functor() NOEXCEPT;

    template<size_t Round>
    FORCE_INLINE static constexpr void round(auto a, auto& b, auto c, auto d,
        auto& e, auto w) NOEXCEPT;

    template<size_t Round>
    FORCE_INLINE static constexpr void round(auto a, auto b, auto c, auto& d,
        auto e, auto f, auto g, auto& h, auto w) NOEXCEPT;

    template<size_t Round>
    FORCE_INLINE static constexpr void round(auto& out, const auto& in) NOEXCEPT;
    static constexpr void rounding(state_t& out, const buffer_t& in) NOEXCEPT;

    template<size_t Word>
    FORCE_INLINE static constexpr void prepare(auto& out) NOEXCEPT;
    static constexpr void preparing(buffer_t& out) NOEXCEPT;
    static constexpr void summarize(state_t& out, const state_t& in) NOEXCEPT;
    static constexpr void input(buffer_t& out, const state_t& in) NOEXCEPT;

    /// Padding
    /// -----------------------------------------------------------------------
    static constexpr void pad_one(buffer_t& out) NOEXCEPT;
    static constexpr void pad_half(buffer_t& out) NOEXCEPT;
    static constexpr void pad_state(buffer_t& out) NOEXCEPT;
    static constexpr void pad_n(buffer_t& out, count_t blocks) NOEXCEPT;

    /// Parsing
    /// -----------------------------------------------------------------------
    static constexpr void input(buffer_t& out, const block_t& in) NOEXCEPT;
    static constexpr void input1(buffer_t& out, const half_t& in) NOEXCEPT;
    static constexpr void input2(buffer_t& out, const half_t& in) NOEXCEPT;
    static constexpr digest_t output(const state_t& in) NOEXCEPT;

private:
    // Specialized padding types.
    using state_pad_t = std_array<word_t, SHA::block_words - SHA::state_words>;
    using blocks_pad_t = std_array<word_t, subtract(SHA::block_words,
        count_bytes / SHA::word_bytes)>;

    static CONSTEVAL auto& concurrency() NOEXCEPT;
    static CONSTEVAL chunk_t chunk_pad() NOEXCEPT;
    static CONSTEVAL buffer_t block_pad() NOEXCEPT;
    static CONSTEVAL state_pad_t state_pad() NOEXCEPT;
    static CONSTEVAL blocks_pad_t blocks_pad() NOEXCEPT;
};

} // namespace sha
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/hash/sha_algorithm.ipp>

#endif
