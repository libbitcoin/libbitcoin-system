/**
 * Copyright (c) 2011-2024 libbitcoin developers (see AUTHORS)
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

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/hash/algorithm.hpp>
#include <bitcoin/system/intrinsics/intrinsics.hpp>
#include <bitcoin/system/math/math.hpp>

// algorithm.hpp file is the common include for sha.
#include <bitcoin/system/hash/sha/sha.hpp>
#include <bitcoin/system/hash/sha/sha160.hpp>
#include <bitcoin/system/hash/sha/sha256.hpp>
#include <bitcoin/system/hash/sha/sha512.hpp>

// Based on:
// FIPS PUB 180-4 [Secure Hash Standard (SHS)].
// All aspects of FIPS180 are supported within the implmentation.
// nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.180-4.pdf

namespace libbitcoin {
namespace system {
namespace sha {

/// SHA hashing algorithm.
/// Native not yet implemented.
/// Vectorization of message schedules and merkle hashes.
template <typename SHA, bool Native = true, bool Vector = true, bool Cached = true,
    if_same<typename SHA::T, sha::shah_t> = true>
class algorithm
  : algorithm_t
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
    using quart_t   = std_array<byte_t, to_half(SHA::chunk_words) * SHA::word_bytes>;
    using half_t    = std_array<byte_t, SHA::chunk_words * SHA::word_bytes>;
    using block_t   = std_array<byte_t, SHA::block_words * SHA::word_bytes>;
    using digest_t  = std_array<byte_t, bytes<SHA::digest>>;

    /// Collection types.
    template <size_t Size>
    using ablocks_t = std_array<block_t, Size>;
    using iblocks_t = iterable<block_t>;
    using digests_t = std::vector<digest_t>;

    /// Count types.
    /// -----------------------------------------------------------------------

    /// count_t is uint64_t (sha160/256) or uint128_t (sha512).
    /// All extended integer intrinsics currently have a "64 on 32" limit.
    static constexpr auto count_bits    = SHA::block_words * SHA::word_bytes;
    static constexpr auto count_bytes   = bytes<count_bits>;
    using count_t = unsigned_exact_type<bytes<count_bits>>;

    static constexpr auto limit_bits    = maximum<count_t> - count_bits;
    static constexpr auto limit_bytes   = to_floored_bytes(limit_bits);
    static constexpr auto big_end_count = true;

    /// Single hashing.
    /// -----------------------------------------------------------------------

    template <size_t Size>
    static constexpr digest_t hash(const ablocks_t<Size>& blocks) NOEXCEPT;
    static constexpr digest_t hash(const block_t& block) NOEXCEPT;
    static constexpr digest_t hash(const half_t& half) NOEXCEPT;
    static constexpr digest_t hash(const half_t& left, const half_t& right) NOEXCEPT;
    static constexpr digest_t hash(const quart_t& left, const quart_t& right) NOEXCEPT;
    static constexpr digest_t hash(uint8_t byte) NOEXCEPT;
    static digest_t hash(iblocks_t&& blocks) NOEXCEPT;

    /// Double hashing (sha256/512).
    /// -----------------------------------------------------------------------

    template <size_t Size>
    static constexpr digest_t double_hash(const ablocks_t<Size>& blocks) NOEXCEPT;
    static constexpr digest_t double_hash(const block_t& block) NOEXCEPT;
    static constexpr digest_t double_hash(const half_t& half) NOEXCEPT;
    static constexpr digest_t double_hash(const half_t& left, const half_t& right) NOEXCEPT;
    static digest_t double_hash(iblocks_t&& blocks) NOEXCEPT;

    /// Streamed hashing (explicitly finalized).
    /// -----------------------------------------------------------------------
    static void accumulate(state_t& state, iblocks_t&& blocks) NOEXCEPT;
    static constexpr void accumulate(state_t& state, const block_t& block) NOEXCEPT;
    static constexpr digest_t normalize(const state_t& state) NOEXCEPT;

    template <size_t Blocks>
    static constexpr digest_t finalize(state_t& state) NOEXCEPT;
    static constexpr digest_t finalize(state_t& state, size_t blocks) NOEXCEPT;
    static constexpr digest_t finalize_second(const state_t& state) NOEXCEPT;
    static constexpr digest_t finalize_double(state_t& state, size_t blocks) NOEXCEPT;

    /// Merkle hashing (sha256/512).
    /// -----------------------------------------------------------------------
    static VCONSTEXPR digests_t& merkle_hash(digests_t& digests) NOEXCEPT;
    static VCONSTEXPR digest_t merkle_root(digests_t&& digests) NOEXCEPT;

protected:
    /// Intrinsics constants.
    /// -----------------------------------------------------------------------

    static constexpr auto use_shani = Native && system::with_shani;
    static constexpr auto use_neon = Native && system::with_neon;
    static constexpr auto use_x128 = Vector && system::with_sse41;
    static constexpr auto use_x256 = Vector && system::with_avx2;
    static constexpr auto use_x512 = Vector && system::with_avx512;

    template <size_t Lanes>
    static constexpr auto is_valid_lanes =
        (Lanes == 16u || Lanes == 8u || Lanes == 4u || Lanes == 2u);

    static constexpr auto min_lanes =
        (use_x128 ? bytes<128> :
            (use_x256 ? bytes<256> :
                (use_x512 ? bytes<512> : 0))) / SHA::word_bytes;

    /// Intrinsics types.
    /// -----------------------------------------------------------------------

    /// Expand is multiple of buffer/state for Lane concurrent blocks.
    /// Multiple blocks are "striped" across the expanded buffer in xWords.
    template <size_t Lanes, bool_if<is_valid_lanes<Lanes>> = true>
    using xblock_t = std_array<words_t, Lanes>;

    template <typename xWord, if_extended<xWord> = true>
    using xbuffer_t = std_array<xWord, SHA::rounds>;

    template <typename xWord, if_extended<xWord> = true>
    using xstate_t = std_array<xWord, SHA::state_words>;

    template <typename xWord, if_extended<xWord> = true>
    using xchunk_t = std_array<xWord, SHA::state_words>;

    /// Wide is casting of buffer_t to xWord for single block concurrency.
    /// This is not multi-block or block striping, just larger words.
    template <typename xWord, if_extended<xWord> = true>
    using wbuffer_t = std_array<xWord, sizeof(buffer_t) / sizeof(xWord)>;

    template <typename xWord, if_extended<xWord> = true>
    using wstate_t = std_array<xWord, sizeof(state_t) / sizeof(xWord)>;

    /// Other types.
    /// -----------------------------------------------------------------------

    using uint = unsigned int;
    using idigests_t = mutable_iterable<digest_t>;
    using pad_t = std_array<word_t, subtract(SHA::block_words,
        count_bytes / SHA::word_bytes)>;

    /// Functions.
    /// -----------------------------------------------------------------------

    INLINE static constexpr auto parity(auto x, auto y, auto z) NOEXCEPT;
    INLINE static constexpr auto choice(auto x, auto y, auto z) NOEXCEPT;
    INLINE static constexpr auto majority(auto x, auto y, auto z) NOEXCEPT;

    template <uint A, uint B, uint C>
    INLINE static constexpr auto sigma(auto x) NOEXCEPT;
    INLINE static constexpr auto sigma0(auto x) NOEXCEPT;
    INLINE static constexpr auto sigma1(auto x) NOEXCEPT;

    template <uint A, uint B, uint C>
    INLINE static constexpr auto Sigma(auto x) NOEXCEPT;
    INLINE static constexpr auto Sigma0(auto x) NOEXCEPT;
    INLINE static constexpr auto Sigma1(auto x) NOEXCEPT;

    /// Compression.
    /// -----------------------------------------------------------------------

    template <typename Word, size_t Lane>
    INLINE static constexpr auto extract(Word a) NOEXCEPT;

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
    static constexpr void compress_(auto& state, const auto& buffer) NOEXCEPT;
    template <size_t Lane = zero>
    static constexpr void compress(state_t& state, const buffer_t& buffer) NOEXCEPT;

    /// Message scheduling.
    /// -----------------------------------------------------------------------

    template <size_t Round>
    INLINE static constexpr void prepare(auto& buffer) NOEXCEPT;
    static constexpr void schedule_(auto& buffer) NOEXCEPT;
    static constexpr void schedule(buffer_t& buffer) NOEXCEPT;

    /// Parsing (endian sensitive).
    /// -----------------------------------------------------------------------

    INLINE static constexpr void input(buffer_t& buffer, const block_t& block) NOEXCEPT;
    INLINE static constexpr void input_left(auto& buffer, const quart_t& quarter) NOEXCEPT;
    INLINE static constexpr void input_right(auto& buffer, const quart_t& quarter) NOEXCEPT;
    INLINE static constexpr void input_left(auto& buffer, const half_t& half) NOEXCEPT;
    INLINE static constexpr void input_right(auto& buffer, const half_t& half) NOEXCEPT;
    INLINE static constexpr void inject_left(auto& buffer, const auto& left) NOEXCEPT;
    INLINE static constexpr void inject_right(auto& buffer, const auto& right) NOEXCEPT;
    INLINE static constexpr digest_t output(const state_t& state) NOEXCEPT;

    /// Padding.
    /// -----------------------------------------------------------------------

    /// Scheduled padding (new and existing buffer objects).
    template <size_t Blocks>
    static CONSTEVAL buffer_t scheduled_pad() NOEXCEPT;
    template <size_t Blocks>
    static constexpr void schedule_n(buffer_t& buffer) NOEXCEPT;
    static constexpr void schedule_n(buffer_t& buffer, size_t blocks) NOEXCEPT;
    static constexpr void schedule_1(buffer_t& buffer) NOEXCEPT;

    /// Unscheduled padding (new objects).
    static words_t pad_block() NOEXCEPT;
    static words_t pad_blocks(count_t blocks) NOEXCEPT;
    static CONSTEVAL chunk_t chunk_pad() NOEXCEPT;
    static CONSTEVAL pad_t stream_pad() NOEXCEPT;

    /// Unscheduled padding (update block or buffer object).
    static constexpr void pad_half(auto& buffer) NOEXCEPT;
    static constexpr void pad_n(auto& buffer, count_t blocks) NOEXCEPT;

    /// Iteration (message scheduling vectorized for multiple blocks).
    /// -----------------------------------------------------------------------

    template <size_t Word, size_t Lanes>
    INLINE static auto pack(const xblock_t<Lanes>& xblock) NOEXCEPT;

    template <typename xWord>
    INLINE static void xinput(xbuffer_t<xWord>& xbuffer,
        iblocks_t& blocks) NOEXCEPT;

    template <typename Word, size_t Lane, typename xWord,
        if_not_same<Word, xWord> = true>
    INLINE static Word extract(xWord a) NOEXCEPT;

    template <typename xWord>
    INLINE static void sequential_compress(state_t& state,
        const xbuffer_t<xWord>& xbuffer) NOEXCEPT;

    template <typename xWord, if_extended<xWord> = true>
    INLINE static void vector_schedule_sequential_compress(state_t& state,
        iblocks_t& blocks) NOEXCEPT;

    template <size_t Size>
    INLINE static void iterate_vector(state_t& state,
        const ablocks_t<Size>& blocks) NOEXCEPT;
    INLINE static void iterate_vector(state_t& state,
        iblocks_t& blocks) NOEXCEPT;

    template <size_t Size>
    INLINE static void iterate_native(state_t& state,
        const ablocks_t<Size>& blocks) NOEXCEPT;
    INLINE static void iterate_native(state_t& state, iblocks_t& blocks) NOEXCEPT;

    template <size_t Size>
    INLINE static constexpr void iterate_(state_t& state,
        const ablocks_t<Size>& blocks) NOEXCEPT;
    INLINE static void iterate_(state_t& state, iblocks_t& blocks) NOEXCEPT;

    template <size_t Size>
    INLINE static constexpr void iterate(state_t& state,
        const ablocks_t<Size>& blocks) NOEXCEPT;
    INLINE static void iterate(state_t& state, iblocks_t& blocks) NOEXCEPT;

    /// Merkle hashing (fully vectorized for multiple blocks).
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
    INLINE static void xoutput(idigests_t& digests,
        const xstate_t<xWord>& xstate) NOEXCEPT;

    template <typename xWord, if_extended<xWord> = true>
    INLINE static void merkle_hash_vector(idigests_t& digests,
        iblocks_t& blocks) NOEXCEPT;
    INLINE static void merkle_hash_vector(digests_t& digests) NOEXCEPT;
    VCONSTEXPR static void merkle_hash_(digests_t& digests,
        size_t offset=zero) NOEXCEPT;

    /// sigma0 vectorization (single blocks).
    /// -----------------------------------------------------------------------

    template <typename xWord, if_extended<xWord> = true>
    INLINE static auto sigma0_8(auto x1, auto x2, auto x3, auto x4, auto x5,
        auto x6, auto x7, auto x8) NOEXCEPT;

    template<size_t Round, size_t Offset>
    INLINE static void prepare_1(buffer_t& buffer, const auto& xsigma0) NOEXCEPT;

    template<size_t Round>
    INLINE static void prepare_8(buffer_t& buffer) NOEXCEPT;

    template <typename xWord>
    INLINE static void schedule_sigma(xbuffer_t<xWord>& xbuffer) NOEXCEPT;
    INLINE static void schedule_sigma(buffer_t& buffer) NOEXCEPT;

    /// [K]onstant vectorization (single and multiple blocks).
    /// -----------------------------------------------------------------------

    template <size_t Round>
    INLINE static constexpr void konstant(auto& buffer) NOEXCEPT;

    template<size_t Round, typename xWord>
    INLINE static void vector_konstant(wbuffer_t<xWord>& wbuffer) NOEXCEPT;
    INLINE static void vector_konstant(buffer_t& buffer) NOEXCEPT;

    template <typename xWord>
    static constexpr void konstant(xbuffer_t<xWord>& xbuffer) NOEXCEPT;
    static constexpr void konstant(buffer_t& buffer) NOEXCEPT;
    static constexpr void konstant_(auto& buffer) NOEXCEPT;

    /// Native SHA optimizations (single blocks).
    /// -----------------------------------------------------------------------

    template <bool Swap>
    INLINE static xint128_t bytes(xint128_t message) NOEXCEPT;
    INLINE static void shuffle(xint128_t& state0, xint128_t& state1) NOEXCEPT;
    INLINE static void unshuffle(xint128_t& state0, xint128_t& state1) NOEXCEPT;
    INLINE static void prepare(xint128_t& message0, xint128_t message1) NOEXCEPT;
    INLINE static void prepare(xint128_t& message0, xint128_t message1,
        xint128_t& message2) NOEXCEPT;

    template <size_t Round>
    INLINE static void round_4(xint128_t& state0, xint128_t& state1,
        xint128_t message) NOEXCEPT;

    template <bool Swap>
    INLINE static void native_rounds(xint128_t& lo, xint128_t& hi,
        const block_t& block) NOEXCEPT;

    INLINE static void native_rounds(xint128_t& lo, xint128_t& hi,
        const half_t& left, const chunk_t& pad) NOEXCEPT;

    template <bool Swap>
    static void native_transform(state_t& state, const auto& block) NOEXCEPT;
    static void native_transform(state_t& state, iblocks_t& blocks) NOEXCEPT;

    template <size_t Blocks>
    static digest_t native_finalize(state_t& state) NOEXCEPT;
    static digest_t native_finalize(state_t& state, size_t blocks) NOEXCEPT;
    static digest_t native_finalize(state_t& state, const words_t& pad) NOEXCEPT;

    static digest_t native_finalize_second(const state_t& half) NOEXCEPT;
    static digest_t native_finalize_double(state_t& half, size_t blocks) NOEXCEPT;

    static digest_t native_hash(const block_t& block) NOEXCEPT;
    static digest_t native_hash(const half_t& half) NOEXCEPT;
    static digest_t native_hash(const half_t& left, const half_t& right) NOEXCEPT;
    static digest_t native_hash(uint8_t byte) NOEXCEPT;

    static digest_t native_double_hash(const block_t& block) NOEXCEPT;
    static digest_t native_double_hash(const half_t& half) NOEXCEPT;
    static digest_t native_double_hash(const half_t& left, const half_t& right) NOEXCEPT;

public:
    /// Summary public values.
    /// -----------------------------------------------------------------------
    static constexpr auto caching = Cached;
    static constexpr auto native = (use_shani || use_neon)
        && (SHA::strength == 256 || SHA::strength == 160);
    static constexpr auto vector = (use_x128 || use_x256 || use_x512)
        && !(build_x32 && is_same_size<word_t, uint64_t>);
};

} // namespace sha
} // namespace system
} // namespace libbitcoin

#define TEMPLATE template <typename SHA, bool Native, bool Vector, bool Cached, \
    if_same<typename SHA::T, sha::shah_t> If>
#define CLASS algorithm<SHA, Native, Vector, Cached, If>

// Bogus warning suggests constexpr when declared consteval.
BC_PUSH_WARNING(USE_CONSTEXPR_FOR_FUNCTION)
BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)

#include <bitcoin/system/impl/hash/sha/algorithm_compress.ipp>
#include <bitcoin/system/impl/hash/sha/algorithm_konstant.ipp>
#include <bitcoin/system/impl/hash/sha/algorithm_double.ipp>
#include <bitcoin/system/impl/hash/sha/algorithm_functions.ipp>
#include <bitcoin/system/impl/hash/sha/algorithm_iterate.ipp>
#include <bitcoin/system/impl/hash/sha/algorithm_merkle.ipp>
#include <bitcoin/system/impl/hash/sha/algorithm_native.ipp>
#include <bitcoin/system/impl/hash/sha/algorithm_padding.ipp>
#include <bitcoin/system/impl/hash/sha/algorithm_parsing.ipp>
#include <bitcoin/system/impl/hash/sha/algorithm_schedule.ipp>
#include <bitcoin/system/impl/hash/sha/algorithm_sigma.ipp>
#include <bitcoin/system/impl/hash/sha/algorithm_single.ipp>
#include <bitcoin/system/impl/hash/sha/algorithm_stream.ipp>

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

#undef CLASS
#undef TEMPLATE

#endif
