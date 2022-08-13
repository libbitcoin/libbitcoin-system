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

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/algorithm.hpp>
#include <bitcoin/system/math/math.hpp>

 // This file is a common include for sha.
#include <bitcoin/system/hash/sha/sha.hpp>
#include <bitcoin/system/hash/sha/sha160.hpp>
#include <bitcoin/system/hash/sha/sha256.hpp>
#include <bitcoin/system/hash/sha/sha512.hpp>

namespace libbitcoin {
namespace system {
namespace sha {

#if defined(HAVE_MSC)
// XCPU extended integrals.
using mint128_t = __m128i;
using mint256_t = __m256i;
using mint512_t = __m512i;
#endif

/// SHA hashing algorithm.
/// Presently Compressed enables non-vector sigma optimization.
/// Presently Vectorized enables vector-friendly sigma optimization.
/// The former is a 10% performance optimization, the latter -10% because
/// it is not being run vectorized, so is wasteful (disabled by default).
template <typename SHA, bool Compressed = true, bool Vectorized = false,
    bool Cached = true, if_same<typename SHA::T, shah_t> = true>
class algorithm : algorithm_t
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

    /// Vectorization types.
    using blocks_t  = std_vector<cref<block_t>>;
    using digests_t = std_vector<digest_t>;

    /// Constants (and count_t).
    /// -----------------------------------------------------------------------
    /// count_t is uint64_t (sha160/256) or uint128_t (sha512).

    static constexpr auto count_bits    = SHA::block_words * SHA::word_bytes;
    static constexpr auto count_bytes   = bytes<count_bits>;
    using count_t = unsigned_exact_type<bytes<count_bits>>;

    static constexpr auto limit_bits    = maximum<count_t> - count_bits;
    static constexpr auto limit_bytes   = to_floored_bytes(limit_bits);
    static constexpr auto compressed    = Compressed;
    static constexpr auto vectorized    = Vectorized;
    static constexpr auto cached        = Cached;
    static constexpr auto big_end_count = true;

    /// Single hashing.
    /// -----------------------------------------------------------------------

    template <size_t Size>
    static constexpr digest_t hash(const std_array<block_t, Size>& blocks) NOEXCEPT;
    static constexpr digest_t hash(const blocks_t& blocks) NOEXCEPT;
    static constexpr digest_t hash(const block_t& block) NOEXCEPT;
    static constexpr digest_t hash(const state_t& state) NOEXCEPT;
    static constexpr digest_t hash(const half_t& half) NOEXCEPT;
    static constexpr digest_t hash(const half_t& left, const half_t& right) NOEXCEPT;
    static digest_t hash(const iterable<block_t>& blocks) NOEXCEPT;

    /// Double hashing optimizations (sha256/512).
    /// -----------------------------------------------------------------------

    template <size_t Size>
    static constexpr digest_t double_hash(const std_array<block_t, Size>& blocks) NOEXCEPT;
    static constexpr digest_t double_hash(const blocks_t& blocks) NOEXCEPT;
    static constexpr digest_t double_hash(const block_t& block) NOEXCEPT;
    static constexpr digest_t double_hash(const half_t& half) NOEXCEPT;
    static constexpr digest_t double_hash(const half_t& left, const half_t& right) NOEXCEPT;
    static digest_t double_hash(const iterable<block_t>& blocks) NOEXCEPT;

    /// Merkle hashing (sha256/512).
    /// -----------------------------------------------------------------------

    static VCONSTEXPR digest_t merkle_root(digests_t&& digests) NOEXCEPT;
    static VCONSTEXPR digests_t& merkle_hash(digests_t& digests) NOEXCEPT;

    /// Streamed hashing (unfinalized).
    /// -----------------------------------------------------------------------

    static VCONSTEXPR void accumulate(state_t& state, const blocks_t& blocks) NOEXCEPT;
    static constexpr void accumulate(state_t& state, const block_t& block) NOEXCEPT;
    static void accumulate(state_t& state, const iterable<block_t>& blocks) NOEXCEPT;

    /// Finalize streaming state (pad and normalize, updates state).
    static constexpr digest_t finalize(state_t& state, size_t blocks) NOEXCEPT;

    /// Double finalize streaming state (pad, rehash, and normalize, updates state1).
    static constexpr digest_t finalize_double(state_t& state, size_t blocks) NOEXCEPT;

    /// Normalize streaming state (big-endian bytes).
    static constexpr digest_t normalize(const state_t& state) NOEXCEPT;

protected:
    /// Functions
    /// -----------------------------------------------------------------------
    using uint = unsigned int;

    template <uint V, uint W, uint X, uint Y, uint Z>
    INLINE static constexpr auto sigma_(auto x) NOEXCEPT;
    template <uint A, uint B, uint C, if_equal<C, 7> = true>
    INLINE static constexpr auto sigma_(auto x) NOEXCEPT;
    template <uint A, uint B, uint C, if_not_equal<C, 7> = true>
    INLINE static constexpr auto sigma_(auto x) NOEXCEPT;

    template <uint A, uint B, uint C>
    INLINE static constexpr auto sigma(auto x) NOEXCEPT;
    template <uint A, uint B, uint C>
    INLINE static constexpr auto Sigma(auto x) NOEXCEPT;

    INLINE static constexpr auto parity(auto x, auto y, auto z) NOEXCEPT;
    INLINE static constexpr auto choice(auto x, auto y, auto z) NOEXCEPT;
    INLINE static constexpr auto majority(auto x, auto y, auto z) NOEXCEPT;

    /// Aggregate functions
    /// -----------------------------------------------------------------------

    INLINE static constexpr auto Sigma0(auto x) NOEXCEPT;
    INLINE static constexpr auto Sigma1(auto x) NOEXCEPT;
    INLINE static constexpr auto sigma0(auto x) NOEXCEPT;
    INLINE static constexpr auto sigma1(auto x) NOEXCEPT;

    /// Rounds
    /// -----------------------------------------------------------------------

    template<size_t Round, typename Auto>
    static CONSTEVAL auto functor() NOEXCEPT;

    template<size_t Round>
    INLINE static constexpr void round(auto a, auto& b, auto c, auto d,
        auto& e, auto wk) NOEXCEPT;

    template<size_t Round>
    INLINE static constexpr void round(auto a, auto b, auto c, auto& d,
        auto e, auto f, auto g, auto& h, auto wk) NOEXCEPT;

    template<size_t Round>
    INLINE static constexpr void round(auto& state, const auto& wk) NOEXCEPT;
    INLINE static constexpr void summarize(auto& out, const auto& in) NOEXCEPT;
    static constexpr void compress(auto& state, const auto& buffer) NOEXCEPT;

    template<size_t Round>
    INLINE static constexpr void prepare(auto& buffer) NOEXCEPT;
    static constexpr void schedule(auto& buffer) NOEXCEPT;

    /// Parsing
    /// -----------------------------------------------------------------------
    INLINE static constexpr void input(buffer_t& buffer, const state_t& state) NOEXCEPT;
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

private:
    using pad_t = std_array<word_t, subtract(SHA::block_words,
        count_bytes / SHA::word_bytes)>;

    template <size_t Blocks>
    static CONSTEVAL buffer_t scheduled_pad() NOEXCEPT;
    static CONSTEVAL chunk_t chunk_pad() NOEXCEPT;
    static CONSTEVAL pad_t stream_pad() NOEXCEPT;

#if defined(HAVE_MSC)
protected:
    using block_it = typename iterable<block_t>::iterator;
    template <size_t Lanes,
        bool_if<!(Lanes == 16 && is_same_size<word_t, uint64_t>)> = true,
        bool_if<Lanes == 16 || Lanes == 8 || Lanes == 4 || Lanes == 2> = true>
    using wordex_t = iif<is_same_size<word_t, uint32_t>,
        iif<Lanes == 16, mint512_t, iif<Lanes == 8, mint256_t, mint128_t>>,
        iif<Lanes ==  8, mint512_t, iif<Lanes == 4, mint256_t, mint128_t>>>;

    template <size_t Lanes>
    using bufferex_t = std_array<wordex_t<Lanes>, K::rounds>;
    ////template <size_t Lanes>
    ////using statex_t = std_array<wordex_t<Lanes>, H::state_words>;

    template <size_t Lanes,
        bool_if<Lanes == 16 || Lanes == 8 || Lanes == 4 || Lanes == 2> = true>
    INLINE bool have() NOEXCEPT;

    template <size_t Lanes>
    static void input(bufferex_t<Lanes>& buffer, block_it& block) NOEXCEPT;
    ////template <size_t Lanes>
    ////static digest_t outputex(const statex_t<Lanes>& state) NOEXCEPT;

    template <size_t Lanes, if_equal<Lanes, 16> = true>
    inline static void vectorize(state_t& state, block_it& blocks, size_t& count) NOEXCEPT;
    template <size_t Lanes, if_equal<Lanes, 8> = true>
    inline static void vectorize(state_t& state, block_it& blocks, size_t& count) NOEXCEPT;
    template <size_t Lanes, if_equal<Lanes, 4> = true>
    inline static void vectorize(state_t& state, block_it& blocks, size_t& count) NOEXCEPT;
    template <size_t Lanes, if_equal<Lanes, 2> = true>
    inline static void vectorize(state_t& state, block_it& blocks, size_t& count) NOEXCEPT;
    template <size_t Lanes, if_equal<Lanes, 1> = true>
    inline static void vectorize(state_t& state, block_it& blocks, size_t& count) NOEXCEPT;
    template <size_t Lanes, if_equal<Lanes, 0> = true>
    inline static void vectorize(state_t& state, block_it& blocks, size_t& count) NOEXCEPT;
#endif //HAVE_MSC
};

} // namespace sha
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/hash/sha/algorithm.ipp>

#endif
