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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_AES_ALGORITHM_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_AES_ALGORITHM_HPP

#include <span>
#include <bitcoin/system/crypto/aes/aes.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/intrinsics/intrinsics.hpp>
#include <bitcoin/system/math/math.hpp>

// Based on:
// FIPS PUB 197 [Advanced Encryption Standard (AES)].
// nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197-upd1.pdf
// NIST SP 800-38A [Recommendation for Block Cipher Modes of Operation].
// nvlpubs.nist.gov/nistpubs/Legacy/SP/nistspecialpublication800-38a.pdf

namespace libbitcoin {
namespace system {
namespace aes {

/// AES block cipher, with counter and cipher block chaining modes.
/// Native AES-NI/VAES (xcpu) or Crypto (arm), otherwise constant time
/// bitsliced blocks vectorized across 64 bit lanes.
template <typename AES, bool Native = true, bool Vector = true,
    if_same<typename AES::T, aes::aesk_t> = true>
class algorithm
{
public:
    /// Types.
    /// -----------------------------------------------------------------------

    using K = AES;
    static constexpr auto block_bytes = 16_size;
    static constexpr auto key_bytes = bytes<K::strength>;
    using block_t    = std_array<uint8_t, block_bytes>;
    using key_t      = std_array<uint8_t, key_bytes>;
    using schedule_t = std_array<block_t, K::round_keys>;

    /// Key expansion.
    /// -----------------------------------------------------------------------
    static constexpr schedule_t expand(const key_t& key) NOEXCEPT;

    /// Single block.
    /// -----------------------------------------------------------------------
    static constexpr void encrypt(block_t& block, const schedule_t& schedule) NOEXCEPT;
    static constexpr void decrypt(block_t& block, const schedule_t& schedule) NOEXCEPT;
    static constexpr void encrypt(block_t& block, const key_t& key) NOEXCEPT;
    static constexpr void decrypt(block_t& block, const key_t& key) NOEXCEPT;

    /// Modes.
    /// -----------------------------------------------------------------------

    /// XOR in with the keystream of successive counter blocks into out
    /// (in/out may be the same, empty in writes keystream). The counter is
    /// incremented in its low order 32 bits, big-endian (sp800-38d inc32),
    /// and is advanced past each block used, including a final partial block.
    static void ctr(byte_span out, const_byte_span in, block_t& counter,
        const schedule_t& schedule) NOEXCEPT;

    /// Decrypt whole blocks in cipher block chaining mode (in/out may be the
    /// same). The iv is advanced to the last cipher block, for continuation.
    static void cbc_decrypt(byte_span out, const_byte_span in, block_t& iv,
        const schedule_t& schedule) NOEXCEPT;

protected:
    /// Intrinsics constants.
    /// -----------------------------------------------------------------------

    static constexpr auto use_aes = Native && bc::have_aes && bc::have_128;
    static constexpr auto use_vaes = use_aes && bc::have_vaes;
    static constexpr auto use_128 = Vector && bc::have_128;
    static constexpr auto use_256 = Vector && bc::have_256;
    static constexpr auto use_512 = Vector && bc::have_512;

    /// Bitsliced types.
    /// -----------------------------------------------------------------------

    /// Eight bit planes of four blocks per 64 bit lane, over integral or
    /// extended integral words (lanes of four blocks each).
    static constexpr auto planes = 8_size;
    static constexpr auto slice_blocks = 4_size;
    using words_t = std_array<uint32_t, 4>;

    template <typename Word>
    using xplanes_t = std_array<Word, planes>;

    template <typename Word>
    using xkeys_t = std_array<xplanes_t<Word>, K::round_keys>;

    template <size_t Lanes>
    using xblocks_t = std_array<block_t, Lanes>;

    /// Native types.
    /// -----------------------------------------------------------------------

    /// Concurrent native blocks, interleaved to cover instruction latency.
    static constexpr auto native_words = 4_size;

    template <typename xWord>
    using native_keys_t = std_array<xWord, K::round_keys>;

    template <typename xWord>
    using native_state_t = std_array<xWord, native_words>;

    /// Bitslicing.
    /// -----------------------------------------------------------------------

    template <typename Word>
    INLINE static constexpr Word constant(uint64_t value) NOEXCEPT;

    template <typename Word, typename... Words>
    INLINE static constexpr Word xors(Word a, Words... b) NOEXCEPT;

    template <typename Word, typename... Words>
    INLINE static constexpr Word ors(Word a, Words... b) NOEXCEPT;

    template <uint64_t Lo, uint64_t Hi, size_t Shift, typename Word>
    INLINE static constexpr void swap(Word& x, Word& y) NOEXCEPT;

    template <typename Word>
    INLINE static constexpr void ortho(xplanes_t<Word>& q) NOEXCEPT;

    INLINE static constexpr void interleave_in(uint64_t& q0, uint64_t& q1,
        const words_t& w) NOEXCEPT;
    INLINE static constexpr void interleave_out(words_t& w, uint64_t q0,
        uint64_t q1) NOEXCEPT;

    INLINE static constexpr words_t to_words(const block_t& block) NOEXCEPT;
    INLINE static constexpr block_t from_words(const words_t& words) NOEXCEPT;

    /// Round functions.
    /// -----------------------------------------------------------------------

    template <typename Word>
    INLINE static constexpr void sbox(xplanes_t<Word>& q) NOEXCEPT;
    template <typename Word>
    INLINE static constexpr void inverse_sbox(xplanes_t<Word>& q) NOEXCEPT;
    template <typename Word>
    INLINE static constexpr void inverse_affine(xplanes_t<Word>& q) NOEXCEPT;

    template <typename Word>
    INLINE static constexpr Word shift_row(Word x) NOEXCEPT;
    template <typename Word>
    INLINE static constexpr Word inverse_shift_row(Word x) NOEXCEPT;
    template <typename Word>
    INLINE static constexpr void shift_rows(xplanes_t<Word>& q) NOEXCEPT;
    template <typename Word>
    INLINE static constexpr void inverse_shift_rows(xplanes_t<Word>& q) NOEXCEPT;

    template <typename Word>
    INLINE static constexpr void xtime(xplanes_t<Word>& q) NOEXCEPT;
    template <typename Word>
    INLINE static constexpr void mix_columns(xplanes_t<Word>& q) NOEXCEPT;
    template <typename Word>
    INLINE static constexpr void inverse_mix_columns(xplanes_t<Word>& q) NOEXCEPT;

    template <typename Word>
    INLINE static constexpr void add_key(xplanes_t<Word>& q,
        const xplanes_t<Word>& key) NOEXCEPT;

    template <typename Word>
    static constexpr void encrypt_planes(xplanes_t<Word>& q,
        const xkeys_t<Word>& keys) NOEXCEPT;
    template <typename Word>
    static constexpr void decrypt_planes(xplanes_t<Word>& q,
        const xkeys_t<Word>& keys) NOEXCEPT;

    /// Schedule.
    /// -----------------------------------------------------------------------

    static constexpr uint32_t sub_word(uint32_t word) NOEXCEPT;
    static constexpr xkeys_t<uint64_t> slice(const schedule_t& schedule) NOEXCEPT;

    template <typename xWord>
    INLINE static xkeys_t<xWord> broadcast(const xkeys_t<uint64_t>& keys) NOEXCEPT;

    /// Bitsliced blocks.
    /// -----------------------------------------------------------------------

    template <typename Word, size_t Lanes>
    INLINE static constexpr void xinput(xplanes_t<Word>& q,
        const xblocks_t<Lanes>& in) NOEXCEPT;
    template <typename Word, size_t Lanes>
    INLINE static constexpr void xoutput(xblocks_t<Lanes>& out,
        const xplanes_t<Word>& q) NOEXCEPT;

    template <typename Word, size_t Lanes>
    static constexpr void xencrypt(xblocks_t<Lanes>& xblocks,
        const xkeys_t<Word>& keys) NOEXCEPT;
    template <typename Word, size_t Lanes>
    static constexpr void xdecrypt(xblocks_t<Lanes>& xblocks,
        const xkeys_t<Word>& keys) NOEXCEPT;

    static constexpr void encrypt_sliced(block_t& block,
        const schedule_t& schedule) NOEXCEPT;
    static constexpr void decrypt_sliced(block_t& block,
        const schedule_t& schedule) NOEXCEPT;

    /// Native.
    /// -----------------------------------------------------------------------

    template <typename xWord>
    INLINE static native_keys_t<xWord> native_encrypt_keys(
        const schedule_t& schedule) NOEXCEPT;
    template <typename xWord>
    INLINE static native_keys_t<xWord> native_decrypt_keys(
        const schedule_t& schedule) NOEXCEPT;

    template <typename xWord>
    INLINE static void native_encrypt(native_state_t<xWord>& state,
        const native_keys_t<xWord>& keys) NOEXCEPT;

    static void encrypt_native(block_t& block,
        const schedule_t& schedule) NOEXCEPT;
    static void decrypt_native(block_t& block,
        const schedule_t& schedule) NOEXCEPT;

    /// Counter mode.
    /// -----------------------------------------------------------------------

    INLINE static void increment(block_t& counter) NOEXCEPT;
    INLINE static void apply(byte_span out, const_byte_span in, size_t start,
        const_byte_span stream) NOEXCEPT;

    template <typename xWord>
    INLINE static size_t native_ctr(byte_span out, const_byte_span in,
        size_t start, block_t& counter, const schedule_t& schedule) NOEXCEPT;

    template <typename Word>
    INLINE static size_t sliced_ctr(byte_span out, const_byte_span in,
        size_t start, block_t& counter, const xkeys_t<uint64_t>& keys) NOEXCEPT;

public:
    /// Summary public values.
    /// -----------------------------------------------------------------------
    static constexpr auto native = use_aes;
    static constexpr auto vector = (use_128 || use_256 || use_512);
};

} // namespace aes
} // namespace system
} // namespace libbitcoin

#define TEMPLATE template <typename AES, bool Native, bool Vector, \
    if_same<typename AES::T, aes::aesk_t> If>
#define CLASS algorithm<AES, Native, Vector, If>

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_POINTER_ARITHMETIC)
BC_PUSH_WARNING(NO_USE_OF_SPAN)

#include <bitcoin/system/impl/crypto/aes/algorithm_bitslice.ipp>
#include <bitcoin/system/impl/crypto/aes/algorithm_ctr.ipp>
#include <bitcoin/system/impl/crypto/aes/algorithm_native.ipp>
#include <bitcoin/system/impl/crypto/aes/algorithm_rounds.ipp>
#include <bitcoin/system/impl/crypto/aes/algorithm_schedule.ipp>
#include <bitcoin/system/impl/crypto/aes/algorithm_single.ipp>

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

#undef CLASS
#undef TEMPLATE

#endif
