/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHA3_ALGORITHM_HPP
#define LIBBITCOIN_SYSTEM_HASH_SHA3_ALGORITHM_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/hash/algorithm.hpp>
#include <bitcoin/system/math/math.hpp>

// Sponge parameters.
#include <bitcoin/system/hash/sha3/sha3.hpp>

// Based on:
// FIPS PUB 202 [SHA-3 Standard: Permutation-Based Hash and Extendable-Output].
// Keccak (pre-standard) differs only in the padding domain separation byte.
// nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.202.pdf

namespace libbitcoin {
namespace system {
namespace sha3 {

// Hacking our way around an MSVC compiler bug (see sha::algorithm).
#define CLASSIF algorithm<SHA3, Vector, true>

/// SHA3 and Keccak hashing algorithm (sponge over Keccak-f[1600]).
/// Vectorization (lane-parallel independent messages) is reserved, not
/// implemented, so the Vector parameter is currently inert.
template <typename SHA3, bool Vector = true,
    if_same<typename SHA3::T, sha3h_t> = true>
class algorithm
  : algorithm_t
{
public:
    /// Types.
    /// -----------------------------------------------------------------------

    /// Aliases.
    using H         = SHA3;
    using K         = typename SHA3::K;
    using word_t    = typename SHA3::word_t;
    using state_t   = typename SHA3::state_t;

    /// Word-based types.
    using words_t   = std_array<word_t, SHA3::block_words>;

    /// Byte-based types.
    using byte_t    = uint8_t;
    template <size_t Size>
    using bytes_t   = std_array<byte_t, Size>;
    using half_t    = bytes_t<to_half(SHA3::block_words) * SHA3::word_bytes>;
    using block_t   = bytes_t<SHA3::block_words * SHA3::word_bytes>;
    using digest_t  = bytes_t<bytes<SHA3::digest>>;

    /// Collection types.
    template <size_t Size>
    using ablocks_t = std_array<block_t, Size>;
    using iblocks_t = iterable<block_t>;

    /// Constants (and count/limit related aliases).
    /// -----------------------------------------------------------------------

    /// Bytes absorbed per permutation (the rate).
    static constexpr auto block_bytes = array_count<block_t>;

    /// Maximum size of a final (padded) partial block.
    static constexpr auto space = sub1(block_bytes);

    /// Hashing (finalized).
    /// -----------------------------------------------------------------------

    template <size_t Size>
    static constexpr digest_t hash(const ablocks_t<Size>& blocks) NOEXCEPT;
    static constexpr digest_t hash(const block_t& block) NOEXCEPT;
    static constexpr digest_t hash(const half_t& half) NOEXCEPT;
    static constexpr digest_t hash(uint8_t byte) NOEXCEPT;
    static digest_t hash(iblocks_t&& blocks) NOEXCEPT;

    /// Finalized hash of a single partial block (any size less than a block).
    template <size_t Size, if_not_greater<Size, CLASSIF::space> = true>
    static constexpr digest_t simple_hash(const bytes_t<Size>& bytes) NOEXCEPT;

    /// Streamed hashing (explicitly finalized).
    /// -----------------------------------------------------------------------

    static void accumulate(state_t& state, iblocks_t&& blocks) NOEXCEPT;
    static constexpr void accumulate(state_t& state,
        const block_t& block) NOEXCEPT;

    /// Finalize block-aligned message (empty tail).
    static constexpr digest_t finalize(state_t& state) NOEXCEPT;

    /// Finalize with the trailing partial block of the message.
    template <size_t Size, if_not_greater<Size, CLASSIF::space> = true>
    static constexpr digest_t finalize(state_t& state,
        const bytes_t<Size>& tail) NOEXCEPT;

protected:
    /// Keccak-f[1600] functions.
    /// -----------------------------------------------------------------------

    static constexpr void theta(state_t& state) NOEXCEPT;
    static constexpr void rho_pi(state_t& state) NOEXCEPT;
    static constexpr void chi(state_t& state) NOEXCEPT;

    template <size_t Round>
    static constexpr void iota(state_t& state) NOEXCEPT;

    template <size_t Round>
    static constexpr void round(state_t& state) NOEXCEPT;

    template <size_t ...Rounds>
    static constexpr void permute_(state_t& state,
        std::index_sequence<Rounds...>) NOEXCEPT;
    static constexpr void permute(state_t& state) NOEXCEPT;

    /// Sponge functions.
    /// -----------------------------------------------------------------------

    template <size_t ...Words>
    static constexpr void input_(words_t& words, const block_t& block,
        std::index_sequence<Words...>) NOEXCEPT;
    static constexpr void input(words_t& words, const block_t& block) NOEXCEPT;

    template <size_t ...Words>
    static constexpr void output_(block_t& bytes, const state_t& state,
        std::index_sequence<Words...>) NOEXCEPT;
    static constexpr digest_t output(const state_t& state) NOEXCEPT;

    template <size_t Size>
    static constexpr block_t pad(const bytes_t<Size>& tail) NOEXCEPT;
    static constexpr block_t pad(size_t size, const byte_t* tail) NOEXCEPT;

    static constexpr void absorb(state_t& state, const block_t& block) NOEXCEPT;

    /// Iteration.
    /// -----------------------------------------------------------------------

    template <size_t Size>
    static constexpr void iterate(state_t& state,
        const ablocks_t<Size>& blocks) NOEXCEPT;
    static void iterate(state_t& state, iblocks_t& blocks) NOEXCEPT;

public:
    /// Summary public values.
    /// -----------------------------------------------------------------------
    static constexpr auto vector = false;
};

} // namespace sha3
} // namespace system
} // namespace libbitcoin

#define TEMPLATE template <typename SHA3, bool Vector, \
    if_same<typename SHA3::T, sha3::sha3h_t> If>
#define CLASS algorithm<SHA3, Vector, If>

BC_PUSH_WARNING(NO_UNGUARDED_POINTERS)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)

#include <bitcoin/system/impl/hash/sha3/algorithm_functions.ipp>
#include <bitcoin/system/impl/hash/sha3/algorithm_iterate.ipp>
#include <bitcoin/system/impl/hash/sha3/algorithm_padding.ipp>
#include <bitcoin/system/impl/hash/sha3/algorithm_parsing.ipp>
#include <bitcoin/system/impl/hash/sha3/algorithm_single.ipp>
#include <bitcoin/system/impl/hash/sha3/algorithm_stream.ipp>

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

#undef CLASSIF
#undef CLASS
#undef TEMPLATE

#endif
