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
#include "../test.hpp"

BOOST_AUTO_TEST_SUITE(rmd_tests)

template <size_t Strength,
    bool_if<Strength == 128 || Strength == 160> = true>
struct k
{
    // The two sets of rounds are treated as one contiguous set.
    static constexpr auto strength = Strength;
    static constexpr auto rounds = strength;
    static constexpr auto size = 256_size;
    static constexpr auto columns = 16_size;
    static constexpr auto rows = Strength / columns;
    using constants_t = std_array<uint32_t, rows>;
    using rounds_t = std_array<size_t, rounds>;
};

struct k128
  : public k<128>
{
    using base = k<128>;

    /// K values (constants).
    static constexpr base::constants_t get
    {
        0x00000000,
        0x5a827999,
        0x6ed9eba1,
        0x8f1bbcdc,
        0x50a28be6,
        0x5c4dd124,
        0x6d703ef3,
        0x00000000,
    };

    static constexpr base::rounds_t word
    {
        // parallel one
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
        7, 4, 13, 1, 10, 6, 15, 3, 12, 0, 9, 5, 2, 14, 11, 8,
        3, 10, 14, 4, 9, 15, 8, 1, 2, 7, 0, 6, 13, 11, 5, 12,
        1, 9, 11, 10, 0, 8, 12, 4, 13, 3, 7, 15, 14, 5, 6, 2,

        // parallel two
        5, 14, 7, 0, 9, 2, 11, 4, 13, 6, 15, 8, 1, 10, 3, 12,
        6, 11, 3, 7, 0, 13, 5, 10, 14, 15, 8, 12, 4, 9, 1, 2,
        15, 5, 1, 3, 7, 14, 6, 9, 11, 8, 12, 2, 10, 0, 4, 13,
        8, 6, 4, 1, 3, 11, 15, 0, 5, 12, 2, 13, 9, 7, 10, 14
    };

    static constexpr base::rounds_t rot
    {
        // parallel one
        11, 14, 15, 12, 5, 8, 7, 9, 11, 13, 14, 15, 6, 7, 9, 8,
        7, 6, 8, 13, 11, 9, 7, 15, 7, 12, 15, 9, 11, 7, 13, 12,
        11, 13, 6, 7, 14, 9, 13, 15, 14, 8, 13, 6, 5, 12, 7, 5,
        11, 12, 14, 15, 14, 15, 9, 8, 9, 14, 5, 6, 8, 6, 5, 12,

        // parallel two
        8, 9, 9, 11, 13, 15, 15, 5, 7, 7, 8, 11, 14, 14, 12, 6,
        9, 13, 15, 7, 12, 8, 9, 11, 7, 7, 12, 7, 6, 15, 13, 11,
        9, 7, 15, 11, 8, 6, 6, 14, 12, 13, 5, 14, 13, 13, 7, 5,
        15, 5, 8, 11, 14, 14, 6, 14, 6, 9, 12, 9, 12, 5, 15, 8
    };
};

struct k160
  : public k<160>
{
    using base = k<160>;

    /// K values (constants).
    static constexpr base::constants_t get
    {
        0x00000000,
        0x5a827999,
        0x6ed9eba1,
        0x8f1bbcdc,
        0xa953fd4e,
        0x50a28be6,
        0x5c4dd124,
        0x6d703ef3,
        0x7a6d76e9,
        0x00000000
    };

    static constexpr base::rounds_t word
    {
        // parallel one
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
        7, 4, 13, 1, 10, 6, 15, 3, 12, 0, 9, 5, 2, 14, 11, 8,
        3, 10, 14, 4, 9, 15, 8, 1, 2, 7, 0, 6, 13, 11, 5, 12,
        1, 9, 11, 10, 0, 8, 12, 4, 13, 3, 7, 15, 14, 5, 6, 2,
        4, 0, 5, 9, 7, 12, 2, 10, 14, 1, 3, 8, 11, 6, 15, 13,

        // parallel two
        5, 14, 7, 0, 9, 2, 11, 4, 13, 6, 15, 8, 1, 10, 3, 12,
        6, 11, 3, 7, 0, 13, 5, 10, 14, 15, 8, 12, 4, 9, 1, 2,
        15, 5, 1, 3, 7, 14, 6, 9, 11, 8, 12, 2, 10, 0, 4, 13,
        8, 6, 4, 1, 3, 11, 15, 0, 5, 12, 2, 13, 9, 7, 10, 14,
        12, 15, 10, 4, 1, 5, 8, 7, 6, 2, 13, 14, 0, 3, 9, 11
    };

    static constexpr base::rounds_t rot
    {
        // parallel one
        11, 14, 15, 12, 5, 8, 7, 9, 11, 13, 14, 15, 6, 7, 9, 8,
        7, 6, 8, 13, 11, 9, 7, 15, 7, 12, 15, 9, 11, 7, 13, 12,
        11, 13, 6, 7, 14, 9, 13, 15, 14, 8, 13, 6, 5, 12, 7, 5,
        11, 12, 14, 15, 14, 15, 9, 8, 9, 14, 5, 6, 8, 6, 5, 12,
        9, 15, 5, 11, 6, 8, 13, 12, 5, 12, 13, 14, 11, 8, 5, 6,

        // parallel two
        8, 9, 9, 11, 13, 15, 15, 5, 7, 7, 8, 11, 14, 14, 12, 6,
        9, 13, 15, 7, 12, 8, 9, 11, 7, 7, 12, 7, 6, 15, 13, 11,
        9, 7, 15, 11, 8, 6, 6, 14, 12, 13, 5, 14, 13, 13, 7, 5,
        15, 5, 8, 11, 14, 14, 6, 14, 6, 9, 12, 9, 12, 5, 15, 8,
        8, 5, 12, 9, 12, 5, 14, 6, 8, 13, 6, 5, 15, 13, 11, 11,
    };
};

template <typename Constants, size_t Digest = Constants::strength>
struct h
{
    using K = Constants;

    /// These constants are exposed though H on the algorithm.
    static constexpr auto digest       = Digest;
    static constexpr auto size         = K::size;
    static constexpr auto rounds       = K::rounds;
    static constexpr auto word_bits    = bytes<size>;
    static constexpr auto word_bytes   = bytes<word_bits>;
    static constexpr auto block_words  = bytes<size> / to_half(word_bytes);
    static constexpr auto chunk_words  = to_half(block_words);
    static constexpr auto buffer_words = rounds;
    static constexpr auto state_words  = Digest == 128 ? 4 : 5;

    /// These types are exposed though H on the algorithm.
    using byte_t   = uint8_t;
    using word_t   = unsigned_type<word_bytes>;
    using state_t  = std_array<word_t, state_words>;  // IV/state
    using words_t  = std_array<word_t, block_words>;  // one block pad
    using chunk_t  = std_array<word_t, chunk_words>;  // half block pad
    using statep_t = std_array<word_t, block_words - state_words>; // state pad
    using stream_t = std_array<byte_t, block_words * word_bytes>; // stream pad
};

// Digest 256 changes IV.
template <size_t Digest = 128,
    bool_if<Digest == 128 || Digest == 256> = true>
struct h128
  : public h<k128, Digest>
{
    using base = h<k128, Digest>;

    struct H
    {
        /// H values (initial state).
        static constexpr const base::state_t get
        {
            0x67452301,
            0xefcdab89,
            0x98badcfe,
            0x10325476
        };
    };

    struct pad
    {
        /// chunk (8 words)
        static constexpr base::chunk_t chunk
        {
            0x80000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000100
        };

        /// state (12 [16-4] words)
        static constexpr base::statep_t state
        {
            0x80000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x000000c8
        };

        /// block (16 words)
        static constexpr base::words_t block
        {
            0x80000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000200
        };

        /// block (64 bytes)
        static constexpr base::stream_t stream
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
    };
};

// Digest 256 changes IV.
template <size_t Digest = 160>
struct h160
  : public h<k160, Digest>
{
    using base = h<k160, Digest>;

    struct H
    {
        /// H values (initial state).
        static constexpr const base::state_t get
        {
            0x67452301,
            0xefcdab89,
            0x98badcfe,
            0x10325476,
            0xc3d2e1f0
        };
    };

    struct pad
    {
        /// chunk (8 words)
        static constexpr base::chunk_t chunk = h128<>::pad::chunk;

        /// state (11 [16-5] words)
        static constexpr base::statep_t state
        {
            0x80000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x00000000, 0x00000000,
            0x00000000, 0x00000000, 0x000000fa
        };

        /// block (16 words)
        static constexpr base::words_t block = h128<>::pad::block;

        /// block (64 bytes)
        static constexpr base::stream_t stream = h128<>::pad::stream;
    };
};

/// RIPEMD 128/160/256/320 variants.
using rmd128     = h128<>;
using rmd128_256 = h128<256>;
using rmd160     = h160<>;
using rmd160_320 = h160<320>;

/// RMD hashing algorithm.
template <typename RMD, bool Concurrent = true>
class algorithm
{
public:
    /// RMD alises.
    /// -----------------------------------------------------------------------

    using H        = typename RMD::H;
    using K        = typename RMD::K;
    using pad      = typename RMD::pad;
    using byte_t   = typename RMD::byte_t;
    using word_t   = typename RMD::word_t;
    using state_t  = typename RMD::state_t;
    using chunk_t  = typename RMD::chunk_t;
    using words_t  = typename RMD::words_t;
    using statep_t = typename RMD::statep_t;
    using stream_t = typename RMD::stream_t;

    /// I/O (local) types.
    /// -----------------------------------------------------------------------

    /// Chunk is 1/2 block, not same as state/digest (sha160).
    static constexpr auto block_bytes   = RMD::block_words * RMD::word_bytes;
    static constexpr auto chunk_bytes   = RMD::chunk_words * RMD::word_bytes;
    static constexpr auto digest_bytes  = bytes<RMD::digest>;
    static constexpr auto count_bits    = block_bytes;
    static constexpr auto count_bytes   = bytes<count_bits>;
    static constexpr auto big_end_count = false;

    /// Blocks is a vector of cref (use emplace(block)).
    /// count_t is 64/128 bit for 64/128 byte blocks (sha512 uses uintx_t).
    using block_t   = std_array<byte_t, block_bytes>;
    using half_t    = std_array<byte_t, chunk_bytes>;
    using digest_t  = std_array<byte_t, digest_bytes>;
    using count_t   = unsigned_exact_type<count_bytes>;
    using blocks_t  = std_vector<cref<block_t>>;
    using digests_t = std_vector<digest_t>;

    /// Limits incorporate requirement to encode counter in final block.
    static constexpr auto limit_bits = maximum<count_t> - count_bits;
    static constexpr auto limit_bytes = to_floored_bytes(limit_bits);

    /// Streaming (single hash, one/multiple full blocks).
    static constexpr void accumulate(state_t& state, const block_t& block) NOEXCEPT;
    static VCONSTEXPR void accumulate(state_t& state, const blocks_t& blocks) NOEXCEPT;
    static constexpr digest_t finalize(const state_t& state) NOEXCEPT;

    /// Finalized single hash (one half block or one/multiple full blocks).
    static constexpr digest_t hash(const half_t& half) NOEXCEPT;
    static constexpr digest_t hash(const block_t& block) NOEXCEPT;
    static VCONSTEXPR digest_t hash(const blocks_t& blocks) NOEXCEPT;

protected:
    /// Functions
    /// -----------------------------------------------------------------------
    
    static constexpr auto f0(auto x, auto y, auto z) NOEXCEPT;
    static constexpr auto f1(auto x, auto y, auto z) NOEXCEPT;
    static constexpr auto f2(auto x, auto y, auto z) NOEXCEPT;
    static constexpr auto f3(auto x, auto y, auto z) NOEXCEPT;
    static constexpr auto f4(auto x, auto y, auto z) NOEXCEPT;

    /// Rounds
    /// -----------------------------------------------------------------------

    template<size_t Round>
    static constexpr auto functor() NOEXCEPT;

    template<size_t Round>
    static constexpr auto round(auto a, auto& b, auto c, auto d, auto& e,
        auto w) NOEXCEPT;

    template<size_t Round>
    static constexpr void round(auto& out, const auto& in) NOEXCEPT;

    template<bool First>
    static constexpr void batch(state_t& out, const words_t& in) NOEXCEPT;
    static constexpr void rounding(state_t& state, const words_t& buffer) NOEXCEPT;
    static constexpr void summarize(state_t& out, const state_t& in1,
        const state_t& in2) NOEXCEPT;

    /// Padding
    /// -----------------------------------------------------------------------
    static constexpr void pad_one(words_t& out) NOEXCEPT;
    static constexpr void pad_half(words_t& out) NOEXCEPT;
    static constexpr void pad_count(words_t& out, count_t blocks) NOEXCEPT;
    
    /// Parsing
    /// -----------------------------------------------------------------------
    static constexpr void little_one(words_t& out, const block_t& in) NOEXCEPT;
    static constexpr void little_half(words_t& out, const half_t& in) NOEXCEPT;
    static constexpr void little_state(digest_t& out, const state_t& in) NOEXCEPT;
};

#ifndef TESTS
static_assert(k<128>::strength == 128);
static_assert(k<128>::rounds == 128);
static_assert(k<128>::size == 256);
static_assert(is_same_type<k<128>::constants_t, std_array<uint32_t, 8>>);
static_assert(is_same_type<k<128>::rounds_t, std_array<size_t, 128>>);

static_assert(k<160>::strength == 160);
static_assert(k<160>::rounds == 160);
static_assert(k<160>::size == 256);
static_assert(is_same_type<k<160>::constants_t, std_array<uint32_t, 10>>);
static_assert(is_same_type<k<160>::rounds_t, std_array<size_t, 160>>);

static_assert(k128::strength == 128);
static_assert(k128::rounds == 128);
static_assert(k128::size == 256);
static_assert(k128::get[0] == 0x00000000);
static_assert(k128::get[3] == 0x8f1bbcdc);
static_assert(k128::get[4] == 0x50a28be6);
static_assert(k128::get[7] == 0x00000000);
static_assert(k128::word[0] == 0u);
static_assert(k128::word[127] == 14u);
static_assert(k128::rot[0] == 11u);
static_assert(k128::rot[127] == 8u);
static_assert(is_same_type<k128::constants_t, std_array<uint32_t, 8>>);
static_assert(is_same_type<k128::rounds_t, std_array<size_t, 128>>);

static_assert(k160::strength == 160);
static_assert(k160::rounds == 160);
static_assert(k160::size == 256);
static_assert(k160::get[0] == 0x00000000);
static_assert(k160::get[4] == 0xa953fd4e);
static_assert(k160::get[5] == 0x50a28be6);
static_assert(k160::get[9] == 0x00000000);
static_assert(k160::word[0] == 0u);
static_assert(k160::word[159] == 11u);
static_assert(k160::rot[0] == 11u);
static_assert(k160::rot[159] == 11u);
static_assert(is_same_type<k160::constants_t, std_array<uint32_t, 10>>);
static_assert(is_same_type<k160::rounds_t, std_array<size_t, 160>>);

// h<k128,...>
static_assert(h<k128>::size == 256);
static_assert(h<k128>::digest == 128);
static_assert(h<k128, 128>::word_bits == 32);
static_assert(h<k128, 128>::word_bytes == 4);
static_assert(h<k128, 128>::chunk_words == 8);
static_assert(h<k128, 128>::block_words == 16);
static_assert(h<k128, 128>::state_words == 4);
static_assert(is_same_type<h<k128, 128>::K, k128>);
static_assert(is_same_type<h<k128, 128>::byte_t, uint8_t>);
static_assert(is_same_type<h<k128, 128>::word_t, uint32_t>);
static_assert(is_same_type<h<k128, 128>::state_t, std_array<uint32_t, 4>>);
static_assert(is_same_type<h<k128, 128>::chunk_t, std_array<uint32_t, 8>>);
static_assert(is_same_type<h<k128, 128>::words_t, std_array<uint32_t, 16>>);
static_assert(is_same_type<h<k128, 128>::statep_t, std_array<uint32_t, 12>>);
static_assert(is_same_type<h<k128, 128>::stream_t, std_array<uint8_t, 64>>);

// h<k160,...>
static_assert(h<k160>::size == 256);
static_assert(h<k160>::digest == 160);
static_assert(h<k160, 160>::word_bits == 32);
static_assert(h<k160, 160>::word_bytes == 4);
static_assert(h<k160, 160>::chunk_words == 8);
static_assert(h<k160, 160>::block_words == 16);
static_assert(h<k160, 160>::state_words == 5);
static_assert(is_same_type<h<k160, 160>::K, k160>);
static_assert(is_same_type<h<k160, 160>::byte_t, uint8_t>);
static_assert(is_same_type<h<k160, 160>::word_t, uint32_t>);
static_assert(is_same_type<h<k160, 160>::state_t, std_array<uint32_t, 5>>);
static_assert(is_same_type<h<k160, 160>::chunk_t, std_array<uint32_t, 8>>);
static_assert(is_same_type<h<k160, 160>::words_t, std_array<uint32_t, 16>>);
static_assert(is_same_type<h<k160, 160>::statep_t, std_array<uint32_t, 11>>);
static_assert(is_same_type<h<k160, 160>::stream_t, std_array<uint8_t, 64>>);

// h128
static_assert(h128<>::size == 256);
static_assert(h128<>::digest == 128);
static_assert(h128<>::rounds == 128);
static_assert(h128<>::K::rounds == 128);
static_assert(h128<>::K::get[0] == 0x00000000);
static_assert(h128<>::K::get[7] == 0x00000000);
static_assert(h128<>::K::get.size() == 8);
static_assert(h128<>::H::get[0] == 0x67452301);
static_assert(h128<>::H::get[3] == 0x10325476);
static_assert(h128<>::H::get.size() == 4);
static_assert(h128<>::pad::chunk.size() == 8);
static_assert(h128<>::pad::chunk[0] == 0x80000000);
static_assert(h128<>::pad::chunk[7] == 0x00000100);
static_assert(h128<>::pad::state.size() == 12);
static_assert(h128<>::pad::state[0] == 0x80000000);
static_assert(h128<>::pad::state[11] == 0x000000c8);
static_assert(h128<>::pad::block.size() == 16);
static_assert(h128<>::pad::block[0] == 0x80000000);
static_assert(h128<>::pad::block[15] == 0x00000200);
static_assert(h128<>::pad::stream.size() == 64);
static_assert(h128<>::pad::stream[0] == 0x80);
static_assert(h128<>::pad::stream[63] == 0x00);

// h160
static_assert(h160<>::size == 256);
static_assert(h160<>::digest == 160);
static_assert(h160<>::rounds == 160);
static_assert(h160<>::K::rounds == 160);
static_assert(h160<>::K::get[0] == 0x00000000);
static_assert(h160<>::K::get[4] == 0xa953fd4e);
static_assert(h160<>::K::get[5] == 0x50a28be6);
static_assert(h160<>::K::get[9] == 0x00000000);
static_assert(h160<>::K::get.size() == 10);
static_assert(h160<>::H::get[0] == 0x67452301);
static_assert(h160<>::H::get[4] == 0xc3d2e1f0);
static_assert(h160<>::H::get.size() == 5);
static_assert(h160<>::pad::chunk.size() == 8);
static_assert(h160<>::pad::chunk[0] == 0x80000000);
static_assert(h160<>::pad::chunk[7] == 0x00000100);
static_assert(h160<>::pad::state.size() == 11);
static_assert(h160<>::pad::state[0] == 0x80000000);
static_assert(h160<>::pad::state[10] == 0x000000fa);
static_assert(h160<>::pad::block.size() == 16);
static_assert(h160<>::pad::block[0] == 0x80000000);
static_assert(h160<>::pad::block[15] == 0x00000200);
static_assert(h160<>::pad::stream.size() == 64);
static_assert(h160<>::pad::stream[0] == 0x80);
static_assert(h160<>::pad::stream[63] == 0x00);

// rmd128
static_assert(rmd128::size == 256);
static_assert(rmd128::digest == 128);
static_assert(rmd128::K::rounds == 128);
static_assert(rmd128::K::get.size() == 8);
static_assert(rmd128::K::get[0] == 0x00000000);
static_assert(rmd128::K::get[7] == 0x00000000);
static_assert(rmd128::H::get.size() == 4);
static_assert(rmd128::H::get[0] == 0x67452301);
static_assert(rmd128::H::get[3] == 0x10325476);

// rmd160
static_assert(rmd160::size == 256);
static_assert(rmd160::digest == 160);
static_assert(rmd160::K::rounds == 160);
static_assert(rmd160::K::get.size() == 10);
static_assert(rmd160::K::get[0] == 0x00000000);
static_assert(rmd160::K::get[9] == 0x00000000);
static_assert(rmd160::H::get.size() == 5);
static_assert(rmd160::H::get[0] == 0x67452301);
static_assert(rmd160::H::get[4] == 0xc3d2e1f0);

// Expansions.
static_assert(rmd128::digest == 128);
static_assert(rmd128_256::digest == 256);
static_assert(rmd160::digest == 160);
static_assert(rmd160_320::digest == 320);

// algorithm<rmd128>
static_assert(!algorithm<rmd128>::big_end_count);
static_assert(algorithm<rmd128>::block_bytes == 64u);
static_assert(algorithm<rmd128>::chunk_bytes == 32u);
static_assert(algorithm<rmd128>::digest_bytes == 16u);
static_assert(algorithm<rmd128>::count_bits == 64u);
static_assert(algorithm<rmd128>::count_bytes == 8u);
static_assert(algorithm<rmd128>::H::get.size() == 4u);
static_assert(algorithm<rmd128>::K::get.size() == 8u);
static_assert(algorithm<rmd128>::pad::chunk.size() == 8u);
static_assert(algorithm<rmd128>::limit_bits == std::numeric_limits<uint64_t>::max() - 64u);
static_assert(algorithm<rmd128>::limit_bytes == algorithm<rmd128>::limit_bits / byte_bits);
static_assert(is_same_type<algorithm<rmd128>::byte_t, uint8_t>);
static_assert(is_same_type<algorithm<rmd128>::word_t, uint32_t>);
static_assert(is_same_type<algorithm<rmd128>::state_t, std_array<uint32_t, 4>>);
static_assert(is_same_type<algorithm<rmd128>::chunk_t, std_array<uint32_t, 8>>);
static_assert(is_same_type<algorithm<rmd128>::words_t, std_array<uint32_t, 16>>);
static_assert(is_same_type<algorithm<rmd128>::statep_t, std_array<uint32_t, 12>>);
static_assert(is_same_type<algorithm<rmd128>::stream_t, std_array<uint8_t, 64>>);
static_assert(is_same_type<algorithm<rmd128>::block_t, std_array<uint8_t, 64>>);
static_assert(is_same_type<algorithm<rmd128>::half_t, std_array<uint8_t, 32>>);
static_assert(is_same_type<algorithm<rmd128>::digest_t, std_array<uint8_t, 16>>);
static_assert(is_same_type<algorithm<rmd128>::count_t, uint64_t>);
static_assert(is_same_type<algorithm<rmd128>::blocks_t, std_vector<cref<std_array<uint8_t, 64>>>>);
static_assert(is_same_type<algorithm<rmd128>::digests_t, std_vector<std_array<uint8_t, 16>>>);
static_assert(is_same_type<decltype(algorithm<rmd128>::limit_bits), const uint64_t>);
static_assert(is_same_type<decltype(algorithm<rmd128>::limit_bytes), const uint64_t>);

// algorithm<rmd160>
static_assert(!algorithm<rmd160>::big_end_count);
static_assert(algorithm<rmd160>::block_bytes == 64u);
static_assert(algorithm<rmd160>::chunk_bytes == 32u);
static_assert(algorithm<rmd160>::digest_bytes == 20u);
static_assert(algorithm<rmd160>::count_bits == 64u);
static_assert(algorithm<rmd160>::count_bytes == 8u);
static_assert(algorithm<rmd160>::H::get.size() == 5u);
static_assert(algorithm<rmd160>::K::get.size() == 10u);
static_assert(algorithm<rmd160>::pad::chunk.size() == 8u);
static_assert(algorithm<rmd160>::limit_bits == std::numeric_limits<uint64_t>::max() - 64u);
static_assert(algorithm<rmd160>::limit_bytes == algorithm<rmd160>::limit_bits / byte_bits);
static_assert(is_same_type<algorithm<rmd160>::byte_t, uint8_t>);
static_assert(is_same_type<algorithm<rmd160>::word_t, uint32_t>);
static_assert(is_same_type<algorithm<rmd160>::state_t, std_array<uint32_t, 5>>);
static_assert(is_same_type<algorithm<rmd160>::chunk_t, std_array<uint32_t, 8>>);
static_assert(is_same_type<algorithm<rmd160>::words_t, std_array<uint32_t, 16>>);
static_assert(is_same_type<algorithm<rmd160>::statep_t, std_array<uint32_t, 11>>);
static_assert(is_same_type<algorithm<rmd160>::stream_t, std_array<uint8_t, 64>>);
static_assert(is_same_type<algorithm<rmd160>::block_t, std_array<uint8_t, 64>>);
static_assert(is_same_type<algorithm<rmd160>::half_t, std_array<uint8_t, 32>>);
static_assert(is_same_type<algorithm<rmd160>::digest_t, std_array<uint8_t, 20>>);
static_assert(is_same_type<algorithm<rmd160>::count_t, uint64_t>);
static_assert(is_same_type<algorithm<rmd160>::blocks_t, std_vector<cref<std_array<uint8_t, 64>>>>);
static_assert(is_same_type<algorithm<rmd160>::digests_t, std_vector<std_array<uint8_t, 20>>>);
static_assert(is_same_type<decltype(algorithm<rmd160>::limit_bits), const uint64_t>);
static_assert(is_same_type<decltype(algorithm<rmd160>::limit_bytes), const uint64_t>);
#endif

BOOST_AUTO_TEST_SUITE_END()
