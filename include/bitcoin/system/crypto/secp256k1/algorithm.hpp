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

    /// Affine point, never infinity.
    template <typename Word>
    struct affine_t
    {
        field_t<Word> x{};
        field_t<Word> y{};
    };

    /// Jacobian point (x / z^2, y / z^3), infinity as a mask.
    template <typename Word>
    struct jacobian_t
    {
        field_t<Word> x{};
        field_t<Word> y{};
        field_t<Word> z{};
        Word infinity{};
    };

    /// Scalar, four 64 bit limbs.
    using scalar_t = std_array<uint64_t, 4>;

    /// Scalar product, eight 64 bit limbs.
    using wide_t = std_array<uint64_t, 8>;

    /// Signed odd window digits, least significant first.
    template <size_t Count>
    using digits_t = std_array<int16_t, Count>;

    /// Big-endian element encoding.
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

    /// Curve y^2 = x^3 + b.
    static constexpr field_t<uint64_t> curve_b{ 7 };

    /// Endomorphism eigenvalue, beta^3 = 1 mod p.
    static constexpr field_t<uint64_t> beta
    {
        0x00096c28719501ee, 0x0007512f58995c13, 0x000c3434e99cf049,
        0x00007106e64479ea, 0x00007ae96a2b657c
    };

    /// Generator.
    static constexpr affine_t<uint64_t> generator
    {
        {
            0x0002815b16f81798, 0x000db2dce28d959f, 0x000e870b07029bfc,
            0x000bbac55a06295c, 0x000079be667ef9dc
        },
        {
            0x0007d08ffb10d4b8, 0x00048a68554199c4, 0x000e1108a8fd17b4,
            0x000c4655da4fbfc0, 0x0000483ada7726a3
        }
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

    /// r = a - b (loose from loose a and weak b).
    template <typename Word>
    static constexpr void subtract(field_t<Word>& r, const field_t<Word>& a,
        const field_t<Word>& b) NOEXCEPT;

    /// r = Factor * a (loose from weak).
    template <size_t Factor, typename Word>
    static constexpr void scale(field_t<Word>& r,
        const field_t<Word>& a) NOEXCEPT;

    /// r = mask ? a : b, per lane.
    template <typename Word>
    static constexpr void select(field_t<Word>& r, Word mask,
        const field_t<Word>& a, const field_t<Word>& b) NOEXCEPT;

    /// Constant expanded to all lanes.
    template <typename Word>
    static constexpr field_t<Word> broadcast(
        const field_t<uint64_t>& a) NOEXCEPT;

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

    /// Scalar constants.
    /// -----------------------------------------------------------------------

    /// n (group order), 2^256 - n, and n / 2.
    static constexpr scalar_t order
    {
        0xbfd25e8cd0364141, 0xbaaedce6af48a03b, 0xfffffffffffffffe,
        0xffffffffffffffff
    };

    static constexpr scalar_t order_complement
    {
        0x402da1732fc9bebf, 0x4551231950b75fc4, 0x0000000000000001,
        0x0000000000000000
    };

    static constexpr scalar_t half_order
    {
        0xdfe92f46681b20a0, 0x5d576e7357a4501d, 0xffffffffffffffff,
        0x7fffffffffffffff
    };

    /// Endomorphism eigenvalue, lambda^3 = 1 mod n.
    static constexpr scalar_t lambda
    {
        0xdf02967c1b23bd72, 0x122e22ea20816678, 0xa5261c028812645a,
        0x5363ad4cc05c30e0
    };

    /// Lattice basis terms -b1 and -b2 mod n.
    static constexpr scalar_t minus_b1
    {
        0x6f547fa90abfe4c3, 0xe4437ed6010e8828, 0x0000000000000000,
        0x0000000000000000
    };

    static constexpr scalar_t minus_b2
    {
        0xd765cda83db1562c, 0x8a280ac50774346d, 0xfffffffffffffffe,
        0xffffffffffffffff
    };

    /// round(2^384 * b2 / n) and round(2^384 * -b1 / n).
    static constexpr scalar_t g1
    {
        0xe893209a45dbb031, 0x3daa8a1471e8ca7f, 0xe86c90e49284eb15,
        0x3086d221a7d46bcd
    };

    static constexpr scalar_t g2
    {
        0x1571b4ae8ac47f71, 0x221208ac9df506c6, 0x6f547fa90abfe4c4,
        0xe4437ed6010e8828
    };

    /// Scalar arithmetic (less than n).
    /// -----------------------------------------------------------------------

    /// r = a + b mod n.
    static constexpr void add(scalar_t& r, const scalar_t& a,
        const scalar_t& b) NOEXCEPT;

    /// r = -a mod n.
    static constexpr void negate(scalar_t& r, const scalar_t& a) NOEXCEPT;

    /// r = a * b mod n.
    static constexpr void multiply(scalar_t& r, const scalar_t& a,
        const scalar_t& b) NOEXCEPT;

    /// r = a^-1 mod n, zero for zero.
    static constexpr void inverse(scalar_t& r, const scalar_t& a) NOEXCEPT;

    /// k = k1 + k2 * lambda mod n, with k1 and k2 of magnitude below 2^128.
    static constexpr void split(scalar_t& k1, scalar_t& k2,
        const scalar_t& k) NOEXCEPT;

    /// Odd value below 2^(Bits * Count) as sum of digit[i] * 2^(Bits * i).
    template <size_t Bits, size_t Count>
    static constexpr void recode(digits_t<Count>& digits,
        const scalar_t& value) NOEXCEPT;

    /// Scalar predicates.
    /// -----------------------------------------------------------------------

    static constexpr bool is_zero(const scalar_t& a) NOEXCEPT;

    /// a greater than n / 2 (negative in signed interpretation).
    static constexpr bool is_high(const scalar_t& a) NOEXCEPT;

    /// Scalar encoding.
    /// -----------------------------------------------------------------------

    /// r from bytes mod n, false if bytes not less than n.
    static constexpr bool from_bytes(scalar_t& r,
        const bytes_t& bytes) NOEXCEPT;

    /// bytes from a.
    static constexpr void to_bytes(bytes_t& out, const scalar_t& a) NOEXCEPT;

    /// Scalar internals.
    /// -----------------------------------------------------------------------

    static constexpr bool is_overflow(const scalar_t& a) NOEXCEPT;

    static constexpr void reduce(scalar_t& r, bool overflow) NOEXCEPT;

    static constexpr void reduce(scalar_t& r, const wide_t& value) NOEXCEPT;

    static constexpr void multiply_add(wide_t& r, size_t position,
        uint64_t a, uint64_t b) NOEXCEPT;

    static constexpr void product(wide_t& r, const scalar_t& a,
        const scalar_t& b) NOEXCEPT;

    static constexpr void multiply_shift(scalar_t& r, const scalar_t& a,
        const scalar_t& b) NOEXCEPT;

    /// Group arithmetic (weak coordinates).
    /// -----------------------------------------------------------------------

    /// r = 2a.
    template <typename Word>
    static constexpr void double_(jacobian_t<Word>& r,
        const jacobian_t<Word>& a) NOEXCEPT;

    /// r = a + b, mask of lanes not computed (a infinite or a = b or -b).
    template <typename Word>
    static constexpr Word add(jacobian_t<Word>& r, const jacobian_t<Word>& a,
        const affine_t<Word>& b) NOEXCEPT;

    /// r = a + b, mask of lanes not computed (a or b infinite, a = b or -b).
    template <typename Word>
    static constexpr Word add(jacobian_t<Word>& r, const jacobian_t<Word>& a,
        const jacobian_t<Word>& b) NOEXCEPT;

    /// r = a + b, all cases.
    static constexpr void add_complete(jacobian_t<uint64_t>& r,
        const jacobian_t<uint64_t>& a, const affine_t<uint64_t>& b) NOEXCEPT;

    /// r = a + b, all cases.
    static constexpr void add_complete(jacobian_t<uint64_t>& r,
        const jacobian_t<uint64_t>& a, const jacobian_t<uint64_t>& b) NOEXCEPT;

    /// r = -a.
    template <typename Word>
    static constexpr void negate(affine_t<Word>& r,
        const affine_t<Word>& a) NOEXCEPT;

    /// r = -a.
    template <typename Word>
    static constexpr void negate(jacobian_t<Word>& r,
        const jacobian_t<Word>& a) NOEXCEPT;

    /// r = lambda * a, as (beta * x, y).
    template <typename Word>
    static constexpr void endomorphism(affine_t<Word>& r,
        const affine_t<Word>& a) NOEXCEPT;

    /// Group conversion.
    /// -----------------------------------------------------------------------

    /// r = a.
    template <typename Word>
    static constexpr void to_jacobian(jacobian_t<Word>& r,
        const affine_t<Word>& a) NOEXCEPT;

    /// r = a (normal), a not infinite.
    template <typename Word>
    static constexpr void to_affine(affine_t<Word>& r,
        const jacobian_t<Word>& a) NOEXCEPT;

    /// r = a (normal) by one inversion, no element infinite.
    template <size_t Count, typename Word>
    static constexpr void to_affine(std_array<affine_t<Word>, Count>& r,
        const std_array<jacobian_t<Word>, Count>& a) NOEXCEPT;

    /// r = (x, y) with y parity of odd mask, mask of x on the curve (normal).
    template <typename Word>
    static constexpr Word lift(affine_t<Word>& r, const field_t<Word>& x,
        Word odd) NOEXCEPT;

    /// Mask of a on the curve (normal).
    template <typename Word>
    static constexpr Word is_on_curve(const affine_t<Word>& a) NOEXCEPT;

    /// Group internals.
    /// -----------------------------------------------------------------------

    template <typename Word>
    static constexpr void to_affine(affine_t<Word>& r,
        const jacobian_t<Word>& a, const field_t<Word>& inverse_z) NOEXCEPT;
};

} // namespace secp256k1
} // namespace system
} // namespace libbitcoin

BC_PUSH_WARNING(NO_ARRAY_INDEXING)

#include <bitcoin/system/impl/crypto/secp256k1/algorithm_field.ipp>
#include <bitcoin/system/impl/crypto/secp256k1/algorithm_scalar.ipp>
#include <bitcoin/system/impl/crypto/secp256k1/algorithm_group.ipp>

BC_POP_WARNING()

#endif
