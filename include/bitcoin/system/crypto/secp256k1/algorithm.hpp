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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_ALGORITHM_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_ALGORITHM_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/intrinsics.hpp>
#include <bitcoin/system/math/math.hpp>

// Based on:
// secg.org/sec2-v2.pdf
// github.com/bitcoin-core/secp256k1 (5x52 field representation)

namespace libbitcoin {
namespace system {
namespace secp256k1 {

/// secp256k1 arithmetic over integral or extended integral words.
/// Extended words compute one element per 64 bit lane (requires ifma).
class algorithm
{
protected:
    /// Types.
    /// -----------------------------------------------------------------------

    /// Field element, five 52 bit limbs.
    template <typename Word>
    using field_t = std_array<Word, 5>;

    /// Field product, ten columns of 52 bit limb products.
    template <typename Word>
    using product_t = std_array<Word, 10>;

    /// Big-endian field element encoding.
    using bytes_t = data_array<32>;

    /// Constants.
    /// -----------------------------------------------------------------------

    static constexpr auto limb_bits = 52u;
    static constexpr auto top_bits = 48u;
    static constexpr uint64_t limb_mask = 0x000fffffffffffff;
    static constexpr uint64_t top_mask = 0x0000ffffffffffff;

    /// 2^256 mod p and 2^260 mod p.
    static constexpr uint64_t fold_256 = 0x00000001000003d1;
    static constexpr uint64_t fold_260 = 0x0000001000003d10;

    /// p = 2^256 - 2^32 - 977.
    static constexpr field_t<uint64_t> prime
    {
        0x000ffffefffffc2f, limb_mask, limb_mask, limb_mask, top_mask
    };

    /// Field arithmetic.
    /// -----------------------------------------------------------------------
    /// loose: limbs below 2^62.
    /// weak: limbs below 2^52, fifth limb below 2^49.
    /// normal: weak and less than p.

    /// r = a + b (loose from loose).
    template <typename Word>
    static constexpr void add(field_t<Word>& r, const field_t<Word>& a,
        const field_t<Word>& b) NOEXCEPT;

    /// r = -a (loose from weak).
    template <typename Word>
    static constexpr void negate(field_t<Word>& r,
        const field_t<Word>& a) NOEXCEPT;

    /// a = a (weak from loose).
    template <typename Word>
    static constexpr void carry(field_t<Word>& a) NOEXCEPT;

    /// a = a mod p (normal from loose).
    template <typename Word>
    static constexpr void normalize(field_t<Word>& a) NOEXCEPT;

    /// r = a * b (weak from weak).
    template <typename Word>
    static constexpr void multiply(field_t<Word>& r, const field_t<Word>& a,
        const field_t<Word>& b) NOEXCEPT;

    /// r = a^2 (weak from weak).
    template <typename Word>
    static constexpr void square(field_t<Word>& r,
        const field_t<Word>& a) NOEXCEPT;

    /// r = a^(2^Count) (weak from weak).
    template <size_t Count, typename Word>
    static constexpr void square(field_t<Word>& r,
        const field_t<Word>& a) NOEXCEPT;

    /// r = a^-1, zero for zero (weak from weak).
    template <typename Word>
    static constexpr void inverse(field_t<Word>& r,
        const field_t<Word>& a) NOEXCEPT;

    /// r = sqrt(a), mask of a being square (weak from weak).
    template <typename Word>
    static constexpr Word square_root(field_t<Word>& r,
        const field_t<Word>& a) NOEXCEPT;

    /// Field predicates (normal), masks of all bits set for true.
    /// -----------------------------------------------------------------------

    template <typename Word>
    static constexpr Word is_zero(const field_t<Word>& a) NOEXCEPT;

    template <typename Word>
    static constexpr Word is_odd(const field_t<Word>& a) NOEXCEPT;

    template <typename Word>
    static constexpr Word equal(const field_t<Word>& a,
        const field_t<Word>& b) NOEXCEPT;

    /// Field encoding.
    /// -----------------------------------------------------------------------

    /// r from bytes (normal), false if bytes not less than p.
    static constexpr bool from_bytes(field_t<uint64_t>& r,
        const bytes_t& bytes) NOEXCEPT;

    /// bytes from a (normal).
    static constexpr void to_bytes(bytes_t& out,
        const field_t<uint64_t>& a) NOEXCEPT;

    /// Field internals.
    /// -----------------------------------------------------------------------

    template <size_t Index, typename Word>
    INLINE static constexpr void propagate(auto& limbs, Word mask) NOEXCEPT;

    template <size_t Left, size_t Right, typename Word>
    INLINE static constexpr void accumulate(product_t<Word>& c,
        const field_t<Word>& a, const field_t<Word>& b) NOEXCEPT;

    template <size_t Index, typename Word>
    INLINE static constexpr void fold(product_t<Word>& c, Word value,
        Word factor) NOEXCEPT;

    template <typename Word>
    INLINE static constexpr void reduce(field_t<Word>& r,
        product_t<Word>& c) NOEXCEPT;

    template <typename Word>
    static constexpr void powers(field_t<Word>& x2, field_t<Word>& x22,
        field_t<Word>& x223, const field_t<Word>& a) NOEXCEPT;
};

} // namespace secp256k1
} // namespace system
} // namespace libbitcoin

BC_PUSH_WARNING(NO_ARRAY_INDEXING)

#include <bitcoin/system/impl/crypto/secp256k1/algorithm_field.ipp>

BC_POP_WARNING()

#endif
