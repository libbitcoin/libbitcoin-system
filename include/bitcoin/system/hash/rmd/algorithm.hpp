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
#ifndef LIBBITCOIN_SYSTEM_HASH_RMD_ALGORITHM_HPP
#define LIBBITCOIN_SYSTEM_HASH_RMD_ALGORITHM_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/algorithm.hpp>
#include <bitcoin/system/math/math.hpp>

 // This file is a common include for rmd.
#include <bitcoin/system/hash/rmd/rmd.hpp>
#include <bitcoin/system/hash/rmd/rmd128.hpp>
#include <bitcoin/system/hash/rmd/rmd160.hpp>

namespace libbitcoin {
namespace system {
namespace rmd {

/// RMD hashing algorithm.
template <typename RMD, if_same<typename RMD::T, rmdh_t> = true>
class algorithm : algorithm_t
{
public:
    /// Types.
    /// -----------------------------------------------------------------------

    /// Aliases.
    using H         = RMD;
    using K         = typename RMD::K;
    using word_t    = typename RMD::word_t;
    using state_t   = typename RMD::state_t;

    /// Word-based types.
    using chunk_t   = std_array<word_t, RMD::chunk_words>;
    using words_t   = std_array<word_t, RMD::block_words>;
    using buffer_t  = std_array<word_t, K::rounds>;

    /// Byte-based types.
    using byte_t    = uint8_t;
    using half_t    = std_array<byte_t, RMD::chunk_words * RMD::word_bytes>;
    using block_t   = std_array<byte_t, RMD::block_words * RMD::word_bytes>;
    using digest_t  = std_array<byte_t, bytes<RMD::digest>>;

    /// Block collection types.
    template <size_t Size>
    using ablocks_t = std_array<block_t, Size>;
    using iblocks_t = iterable<block_t>;

    /// Constants (and count_t).
    /// -----------------------------------------------------------------------
    /// count_t is always uint64_t for rmd.

    static constexpr auto count_bits    = RMD::block_words * RMD::word_bytes;
    static constexpr auto count_bytes   = bytes<count_bits>;
    using count_t = unsigned_exact_type<bytes<count_bits>>;

    static constexpr auto limit_bits    = maximum<count_t> - count_bits;
    static constexpr auto limit_bytes   = to_floored_bytes(limit_bits);
    static constexpr auto big_end_count = false;

    /// Hashing (finalized).
    /// -----------------------------------------------------------------------

    template <size_t Size>
    static constexpr digest_t hash(const ablocks_t<Size>& blocks) NOEXCEPT;
    static constexpr digest_t hash(const block_t& block) NOEXCEPT;
    static constexpr digest_t hash(const half_t& half) NOEXCEPT;
    static digest_t hash(iblocks_t&& blocks) NOEXCEPT;

    /// Streamed hashing (unfinalized).
    /// -----------------------------------------------------------------------

    static constexpr void accumulate(state_t& state, const block_t& block) NOEXCEPT;
    static void accumulate(state_t& state, iblocks_t&& blocks) NOEXCEPT;

    /// Finalize streaming state (pad and normalize, updates state).
    static constexpr digest_t finalize(state_t& state, size_t blocks) NOEXCEPT;

    /// Normalize streaming state (big-endian bytes).
    static constexpr digest_t normalize(const state_t& state) NOEXCEPT;

protected:
    /// Functions
    /// -----------------------------------------------------------------------
    
    INLINE static constexpr auto f0(auto x, auto y, auto z) NOEXCEPT;
    INLINE static constexpr auto f1(auto x, auto y, auto z) NOEXCEPT;
    INLINE static constexpr auto f2(auto x, auto y, auto z) NOEXCEPT;
    INLINE static constexpr auto f3(auto x, auto y, auto z) NOEXCEPT;
    INLINE static constexpr auto f4(auto x, auto y, auto z) NOEXCEPT;

    /// Rounds
    /// -----------------------------------------------------------------------

    template<size_t Round, typename Auto>
    static CONSTEVAL auto functor() NOEXCEPT;

    template<size_t Round>
    INLINE static constexpr auto round(auto& a, auto b, auto c, auto d,
        auto x) NOEXCEPT;

    template<size_t Round>
    INLINE static constexpr auto round(auto& a, auto b, auto& c, auto d,
        auto e, auto x) NOEXCEPT;

    template<size_t Round>
    INLINE static constexpr void round(auto& state, const auto& words) NOEXCEPT;
    INLINE static constexpr void summarize(state_t& out, const state_t& batch1,
        const state_t& batch2) NOEXCEPT;
    static constexpr void compress(state_t& state, const words_t& words) NOEXCEPT;
    
    /// Parsing
    /// -----------------------------------------------------------------------
    INLINE static constexpr void input(words_t& words, const block_t& block) NOEXCEPT;
    INLINE static constexpr void input(words_t& words, const half_t& half) NOEXCEPT;
    INLINE static constexpr digest_t output(const state_t& state) NOEXCEPT;

    /// Padding
    /// -----------------------------------------------------------------------
    static constexpr void pad_one(words_t& words) NOEXCEPT;
    static constexpr void pad_half(words_t& words) NOEXCEPT;
    static constexpr void pad_n(words_t& words, count_t blocks) NOEXCEPT;

private:
    using pad_t = std_array<word_t, subtract(RMD::block_words,
        count_bytes / RMD::word_bytes)>;

    static CONSTEVAL words_t block_pad() NOEXCEPT;
    static CONSTEVAL chunk_t chunk_pad() NOEXCEPT;
    static CONSTEVAL pad_t stream_pad() NOEXCEPT;
};

} // namespace rmd
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/hash/rmd/algorithm.ipp>

#endif
