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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHAX_ALGORITHM_HPP
#define LIBBITCOIN_SYSTEM_HASH_SHAX_ALGORITHM_HPP

#include "../../../test.hpp"

namespace shax {

/// SHA hashing algorithm.
/// Compression not yet implemented.
/// Vectorization of message schedules and merkle hashes.
template <typename SHA, bool Compressed = true, bool Vectorized = true,
    bool Cached = true, if_same<typename SHA::T, sha::shah_t> = true>
class algorithm : algorithm_t
{
public:
    /// Types.
    /// -----------------------------------------------------------------------

    /// Aliases.
    using H = SHA;
    using K = typename SHA::K;
    using word_t = typename SHA::word_t;
    using state_t = typename SHA::state_t;

    /// Word-based types.
    using chunk_t = std_array<word_t, SHA::chunk_words>;
    using words_t = std_array<word_t, SHA::block_words>;
    using buffer_t = std_array<word_t, K::rounds>;

    /// Byte-based types.
    using byte_t = uint8_t;
    using half_t = std_array<byte_t, SHA::chunk_words* SHA::word_bytes>;
    using block_t = std_array<byte_t, SHA::block_words* SHA::word_bytes>;
    using digest_t = std_array<byte_t, bytes<SHA::digest>>;

    /// Collection types.
    template <size_t Size>
    using ablocks_t = std_array<block_t, Size>;
    using iblocks_t = iterable<block_t>;
    using digests_t = std_vector<digest_t>;

    /// Constants (and count_t).
    /// -----------------------------------------------------------------------
    /// count_t is uint64_t (sha160/256) or uint128_t (sha512).
    /// All extended integer intrinsics currently have a "64 on 32" limit.

    static constexpr auto count_bits = SHA::block_words * SHA::word_bytes;
    static constexpr auto count_bytes = bytes<count_bits>;
    using count_t = unsigned_exact_type<bytes<count_bits>>;

    static constexpr auto caching = Cached;
    static constexpr auto limit_bits = maximum<count_t> -count_bits;
    static constexpr auto limit_bytes = to_floored_bytes(limit_bits);
    static constexpr auto big_end_count = true;

    /// Single hashing.
    /// -----------------------------------------------------------------------
    template <size_t Size>
    static constexpr digest_t hash(const ablocks_t<Size>& blocks) NOEXCEPT;
    static constexpr digest_t hash(const block_t& block) NOEXCEPT;
    static constexpr digest_t hash(const state_t& state) NOEXCEPT;
    static constexpr digest_t hash(const half_t& half) NOEXCEPT;
    static constexpr digest_t hash(const half_t& left, const half_t& right) NOEXCEPT;
    static digest_t hash(iblocks_t&& blocks) NOEXCEPT;

    /// Double hashing (sha256/512).
    /// -----------------------------------------------------------------------
    template <size_t Size>
    static constexpr digest_t double_hash(const ablocks_t<Size>& blocks) NOEXCEPT;
    static constexpr digest_t double_hash(const block_t& block) NOEXCEPT;
    static constexpr digest_t double_hash(const half_t& half) NOEXCEPT;
    static constexpr digest_t double_hash(const half_t& left, const half_t& right) NOEXCEPT;
    static digest_t double_hash(iblocks_t&& blocks) NOEXCEPT;

    /// Merkle hashing (sha256/512).
    /// -----------------------------------------------------------------------
    static VCONSTEXPR digest_t merkle_root(digests_t&& digests) NOEXCEPT;
    static VCONSTEXPR digests_t& merkle_hash(digests_t& digests) NOEXCEPT;

    /// Streamed hashing (unfinalized).
    /// -----------------------------------------------------------------------
    static void accumulate(state_t& state, iblocks_t&& blocks) NOEXCEPT;
    static constexpr void accumulate(state_t& state, const block_t& block) NOEXCEPT;
    static constexpr digest_t finalize(state_t& state, size_t blocks) NOEXCEPT;
    static constexpr digest_t finalize_double(state_t& state, size_t blocks) NOEXCEPT;
    static constexpr digest_t normalize(const state_t& state) NOEXCEPT;

protected:
    /// Functions
    /// -----------------------------------------------------------------------
    using uint = unsigned int;

    template <uint A, uint B, uint C>
    INLINE static constexpr auto sigma(word_t x) NOEXCEPT;
    template <uint A, uint B, uint C>
    INLINE static constexpr auto Sigma(auto x) NOEXCEPT;

    INLINE static constexpr auto parity(auto x, auto y, auto z) NOEXCEPT;
    INLINE static constexpr auto choice(auto x, auto y, auto z) NOEXCEPT;
    INLINE static constexpr auto majority(auto x, auto y, auto z) NOEXCEPT;

    INLINE static constexpr auto Sigma0(auto x) NOEXCEPT;
    INLINE static constexpr auto Sigma1(auto x) NOEXCEPT;
    INLINE static constexpr auto sigma0(auto x) NOEXCEPT;
    INLINE static constexpr auto sigma1(auto x) NOEXCEPT;

    /// Rounds
    /// -----------------------------------------------------------------------

    template<size_t Round, typename Auto>
    static CONSTEVAL auto functor() NOEXCEPT;

    template <size_t Round>
    INLINE static constexpr void round(auto a, auto& b, auto c, auto d,
        auto& e, auto wk) NOEXCEPT;

    template <size_t Round>
    INLINE static constexpr void round(auto a, auto b, auto c, auto& d,
        auto e, auto f, auto g, auto& h, auto wk) NOEXCEPT;

    template <size_t Round, size_t Lane>
    INLINE static constexpr void round(auto& state, const auto& wk) NOEXCEPT;
    INLINE static constexpr void summarize(auto& out, const auto& in) NOEXCEPT;

    template <size_t Lane = zero>
    static constexpr void compress(auto& state, const auto& buffer) NOEXCEPT;

    template <size_t Round>
    INLINE static constexpr void prepare(auto& buffer) NOEXCEPT;
    INLINE static constexpr void add_k(auto& buffer) NOEXCEPT;
    INLINE static constexpr void schedule_(auto& buffer) NOEXCEPT;
    static constexpr void schedule(auto& buffer) NOEXCEPT;

    INLINE static constexpr void input(auto& buffer, const auto& state) NOEXCEPT;

    /// Parsing
    /// -----------------------------------------------------------------------
    INLINE static constexpr void input(buffer_t& buffer, const block_t& block) NOEXCEPT;
    INLINE static constexpr void input1(buffer_t& buffer, const half_t& half) NOEXCEPT;
    INLINE static constexpr void input2(buffer_t& buffer, const half_t& half) NOEXCEPT;
    INLINE static constexpr digest_t output(const state_t& state) NOEXCEPT;

    /// Padding
    /// -----------------------------------------------------------------------
    template <size_t Blocks>
    static constexpr void schedule_n(buffer_t& buffer) NOEXCEPT;
    static constexpr void schedule_n(buffer_t& buffer, size_t blocks) NOEXCEPT;
    static constexpr void schedule_1(buffer_t& buffer) NOEXCEPT;
    static constexpr void pad_half(buffer_t& buffer) NOEXCEPT;
    static constexpr void pad_n(buffer_t& buffer, count_t blocks) NOEXCEPT;

    /// Block iteration.
    /// -----------------------------------------------------------------------

    template <size_t Size>
    INLINE static constexpr void iterate(state_t& state,
        const ablocks_t<Size>& blocks) NOEXCEPT;
    INLINE static void iterate(state_t& state, iblocks_t& blocks) NOEXCEPT;

    template <size_t Size>
    INLINE static constexpr void iterate_(state_t& state,
        const ablocks_t<Size>& blocks) NOEXCEPT;
    INLINE static void iterate_(state_t& state, iblocks_t& blocks) NOEXCEPT;

    /// Merkle iteration.
    /// -----------------------------------------------------------------------
    VCONSTEXPR static void merkle_hash_(digests_t& digests,
        size_t offset = zero) NOEXCEPT;

private:
    using pad_t = std_array<word_t, subtract(SHA::block_words,
        count_bytes / SHA::word_bytes)>;

    template <size_t Blocks>
    static CONSTEVAL buffer_t scheduled_pad() NOEXCEPT;
    static CONSTEVAL chunk_t chunk_pad() NOEXCEPT;
    static CONSTEVAL pad_t stream_pad() NOEXCEPT;

    /// Compression.
    /// -----------------------------------------------------------------------
protected:
public:
    static constexpr auto have_shani = Compressed && system::with_shani;
    static constexpr auto have_neon = Compressed && system::with_neon;
    static constexpr auto compression = have_shani || have_neon;

    /// Vectorization.
    /// -----------------------------------------------------------------------
protected:
    /// Extended integer capacity for uint32_t/uint64_t is 2/4/8/16 only.
    template <size_t Lanes>
    static constexpr auto is_valid_lanes =
        (Lanes == 16u || Lanes == 8u || Lanes == 4u || Lanes == 2u);

    template <size_t Lanes, bool_if<is_valid_lanes<Lanes>> = true>
    using wblock_t = std_array<words_t, Lanes>;
    template <typename xWord, if_extended<xWord> = true>
    using xbuffer_t = std_array<xWord, SHA::rounds>;
    template <typename xWord, if_extended<xWord> = true>
    using xstate_t = std_array<xWord, SHA::state_words>;
    template <typename xWord, if_extended<xWord> = true>
    using xchunk_t = std_array<xWord, SHA::state_words>;
    using idigests_t = mutable_iterable<digest_t>;

    /// Vector-optimized sigma.
    /// -----------------------------------------------------------------------
    template <uint V, uint W, uint X, uint Y, uint Z>
    INLINE static constexpr auto sigma_(auto x) NOEXCEPT;
    template <uint A, uint B, uint C, if_equal<C, 7> = true>
    INLINE static constexpr auto sigma_(auto x) NOEXCEPT;
    template <uint A, uint B, uint C, if_not_equal<C, 7> = true>
    INLINE static constexpr auto sigma_(auto x) NOEXCEPT;
    template <uint A, uint B, uint C, typename xWord, if_extended<xWord> = true>
    INLINE static constexpr auto sigma(xWord x) NOEXCEPT;

    /// Common.
    /// -----------------------------------------------------------------------

    template <size_t Word, size_t Lanes>
    INLINE static auto pack(const wblock_t<Lanes>& wblock) NOEXCEPT;

    template <typename xWord>
    INLINE static void input(xbuffer_t<xWord>& xbuffer,
        iblocks_t& blocks) NOEXCEPT;

    /// Merkle Hash.
    /// -----------------------------------------------------------------------

    template <typename xWord>
    INLINE static auto pack_pad_half() NOEXCEPT;

    template <typename xWord>
    INLINE static auto pack_schedule_1() NOEXCEPT;

    template <typename xWord>
    INLINE static void pad_half(xbuffer_t<xWord>& xbuffer) NOEXCEPT;

    template <typename xWord>
    INLINE static void schedule_1(xbuffer_t<xWord>& xbuffer) NOEXCEPT;

    template <typename xWord>
    INLINE static auto pack(const state_t& state) NOEXCEPT;

    template <size_t Lane, typename xWord>
    INLINE static digest_t unpack(const xstate_t<xWord>& xstate) NOEXCEPT;

    template <typename xWord>
    INLINE static void output(idigests_t& digests,
        const xstate_t<xWord>& xstate) NOEXCEPT;

    template <typename xWord, if_extended<xWord> = true>
    INLINE static void merkle_hash_v_(idigests_t& digests,
        iblocks_t& blocks) NOEXCEPT;

    INLINE static void merkle_hash_v(digests_t& digests) NOEXCEPT;

    /// Message Schedule (block vectorization).
    /// -----------------------------------------------------------------------

    template <typename Word, size_t Lane>
    INLINE static constexpr auto extract(Word a) NOEXCEPT;

    template <typename Word, size_t Lane, typename xWord,
        if_not_same<Word, xWord> = true>
    INLINE static Word extract(xWord a) NOEXCEPT;

    template <typename xWord>
    INLINE static void compress_v(state_t& state,
        const xbuffer_t<xWord>& xbuffer) NOEXCEPT;

    template <typename xWord, if_extended<xWord> = true>
    INLINE static void iterate_v_(state_t& state, iblocks_t& blocks) NOEXCEPT;

    template <size_t Size>
    INLINE static void iterate_v(state_t& state, const ablocks_t<Size>& blocks) NOEXCEPT;
    INLINE static void iterate_v(state_t& state, iblocks_t& blocks) NOEXCEPT;

    /// Message Schedule (sigma vectorization).
    /// -----------------------------------------------------------------------

    template <typename xWord, if_extended<xWord> = true>
    INLINE static auto sigma1_2(auto x1, auto x2) NOEXCEPT;
    template <typename xWord, if_extended<xWord> = true>
    INLINE static auto sigma0_8(auto x1, auto x2, auto x3, auto x4, auto x5,
        auto x6, auto x7, auto x8) NOEXCEPT;

    template<size_t Round>
    INLINE static void prepare_v(buffer_t& buffer) NOEXCEPT;
    INLINE static void schedule_v(auto& buffer) NOEXCEPT;

public:
    static constexpr auto have_x128 = Vectorized && system::with_sse41;
    static constexpr auto have_x256 = Vectorized && system::with_avx2;
    static constexpr auto have_x512 = Vectorized && system::with_avx512;
    static constexpr auto min_lanes = (have_x128 ? 16 : (have_x256 ? 32 :
        (have_x128 ? 64 : 0))) / SHA::word_bytes;
    static constexpr auto vectorization = (have_x128 || have_x256 || have_x512) &&
        !(build_x32 && is_same_size<word_t, uint64_t>);
};

} // namespace shax

#define TEMPLATE template <typename SHA, bool Compressed, bool Vectorized, \
    bool Cached, if_same<typename SHA::T, sha::shah_t> If>
#define CLASS algorithm<SHA, Compressed, Vectorized, Cached, If>

#include "algorithm.ipp"
#include "algorithm_compression.ipp"
#include "algorithm_vectorization.ipp"

#undef CLASS
#undef TEMPLATE

#endif
