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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_POLY1305_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_POLY1305_HPP

#include <span>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/intrinsics.hpp>

namespace libbitcoin {
namespace system {

/// Poly1305 one-time authenticator (rfc8439).
/// The key must be used to authenticate no more than one message.
class BC_API poly1305
{
public:
    /// Poly1305 block is always 128 bits.
    static constexpr size_t block_size = 16;

    /// Poly1305 secret is always 256 bits.
    static constexpr size_t secret_size = 32;
    typedef data_array<secret_size> secret;

    /// Poly1305 tag is always 128 bits.
    static constexpr size_t tag_size = 16;
    typedef data_array<tag_size> tag;

    /// Poly1305 accumulator, not resettable.
    poly1305(const secret& key) NOEXCEPT;

    void write(const_byte_span data) NOEXCEPT;
    void flush(tag& out) NOEXCEPT;

protected:
    /// The accumulator and multipliers are five 26 bit limbs, over integral
    /// or extended integral words, the latter computing one block per 64 bit
    /// lane.
    static constexpr size_t limb_count = 5;

    template <typename Word>
    using limbs_t = std_array<Word, limb_count>;

    /// The number of concurrent blocks computed by an extended word.
    template <typename xWord>
    static constexpr auto lanes = capacity<xWord, uint64_t>;

    /// The ascending scalar powers of r, one for each lane.
    template <typename xWord>
    using ladder_t = std_array<limbs_t<uint64_t>, lanes<xWord>>;

    /// multiplication
    /// -----------------------------------------------------------------------

    template <size_t Limb>
    static uint64_t limb(const uint8_t* data, uint32_t hibit) NOEXCEPT;

    template <typename Word>
    static constexpr void scale(limbs_t<Word>& s,
        const limbs_t<Word>& r) NOEXCEPT;

    template <typename Word>
    static constexpr Word dot(const limbs_t<Word>& a, Word w0, Word w1,
        Word w2, Word w3, Word w4) NOEXCEPT;

    template <typename Word>
    static constexpr void multiply(limbs_t<Word>& d, const limbs_t<Word>& a,
        const limbs_t<Word>& r, const limbs_t<Word>& s) NOEXCEPT;

    template <typename Word>
    static constexpr void reduce(limbs_t<Word>& a,
        const limbs_t<Word>& d) NOEXCEPT;

    /// vectorization
    /// -----------------------------------------------------------------------

    template <size_t Limb, typename xWord>
    static xWord load(const uint8_t* data, uint32_t hibit,
        uint64_t extra) NOEXCEPT;

    template <typename xWord>
    static xWord descend(const ladder_t<xWord>& ladder, size_t index) NOEXCEPT;

    template <typename xWord>
    static uint64_t sum(xWord a) NOEXCEPT;

    template <typename xWord>
    static void xblocks(const uint8_t*& data, size_t& blocks, uint32_t hibit,
        const limbs_t<uint64_t>& r, const limbs_t<uint64_t>& s,
        limbs_t<uint64_t>& h) NOEXCEPT;

    void blocks(const uint8_t* data, size_t blocks, uint32_t hibit) NOEXCEPT;

private:
    limbs_t<uint32_t> r_{};
    limbs_t<uint32_t> h_{};
    std_array<uint32_t, 4> pad_{};
    data_array<block_size> buffer_{};
    size_t offset_{};
};

} // namespace system
} // namespace libbitcoin

#endif
