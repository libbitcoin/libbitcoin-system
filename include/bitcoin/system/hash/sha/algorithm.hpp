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
#include <bitcoin/system/hash/sha/sha.hpp>
#include <bitcoin/system/hash/sha/sha160.hpp>
#include <bitcoin/system/hash/sha/sha256.hpp>
#include <bitcoin/system/hash/sha/sha512.hpp>

namespace libbitcoin {
namespace system {
namespace sha {

/// SHA hashing algorithm.
template <typename SHA, bool Concurrent = true,
    if_same<typename SHA::T, shah_t> = true>
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
    static constexpr void finalize(digest_t& digest,
        const state_t& state) NOEXCEPT;

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

/// bc::system sha algorithm aliases.
using sha160     = sha::algorithm<sha::h160>;
using sha256_224 = sha::algorithm<sha::h256<224>>; // not fully implemented
using sha256     = sha::algorithm<sha::h256<>>;
using sha512_256 = sha::algorithm<sha::h512<256>>; // not fully implemented
using sha512_224 = sha::algorithm<sha::h512<224>>; // not fully implemented
using sha512_384 = sha::algorithm<sha::h512<384>>; // not fully implemented
using sha512     = sha::algorithm<sha::h512<>>;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/hash/sha/algorithm.ipp>

#endif
