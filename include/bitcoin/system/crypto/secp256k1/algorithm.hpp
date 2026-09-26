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

#include <bitcoin/system/crypto/secp256k1.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
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

    /// Unsigned 128 bit accumulator, as high and low words.
    struct unsigned128_t
    {
        uint64_t high{};
        uint64_t low{};
    };

    /// Field product, ten columns of 52 bit limb products, where integral
    /// words accumulate full limb products in 128 bit columns.
    template <typename Word>
    using product_t = iif<is_same_type<Word, uint64_t>,
        std_array<unsigned128_t, 10>, std_array<Word, 10>>;

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

    static constexpr size_t limb_bits = 52;
    static constexpr size_t  top_bits = 48;
    static constexpr uint64_t limb_mask = 0x000fffffffffffff;
    static constexpr uint64_t  top_mask = 0x0000ffffffffffff;

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
        0x00096c28719501ee,
        0x0007512f58995c13,
        0x000c3434e99cf049,
        0x00007106e64479ea,
        0x00007ae96a2b657c
    };

    /// Generator.
    static constexpr affine_t<uint64_t> generator
    {
        {
            0x0002815b16f81798,
            0x000db2dce28d959f,
            0x000e870b07029bfc,
            0x000bbac55a06295c,
            0x000079be667ef9dc
        },
        {
            0x0007d08ffb10d4b8,
            0x00048a68554199c4,
            0x000e1108a8fd17b4,
            0x000c4655da4fbfc0,
            0x0000483ada7726a3
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

    /// r from bytes at Offset (normal), false if bytes not less than p.
    template <size_t Offset = zero, size_t Size = array_count<bytes_t>>
    static constexpr bool from_bytes(field_t<uint64_t>& r,
        const data_array<Size>& bytes) NOEXCEPT;

    /// bytes from a (normal).
    static constexpr void to_bytes(bytes_t& out,
        const field_t<uint64_t>& a) NOEXCEPT;

    /// r from 256 bit words, least significant first (normal if below p).
    static constexpr void to_field(field_t<uint64_t>& r,
        const scalar_t& words) NOEXCEPT;

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

    INLINE static constexpr void multiply_add(unsigned128_t& r, uint64_t a,
        uint64_t b) NOEXCEPT;
    INLINE static constexpr void add(unsigned128_t& r,
        const unsigned128_t& a) NOEXCEPT;
    INLINE static constexpr uint64_t take(unsigned128_t& a) NOEXCEPT;

    template <typename Word>
    static constexpr void powers(field_t<Word>& x2, field_t<Word>& x22,
        field_t<Word>& x223, const field_t<Word>& a) NOEXCEPT;

    /// Scalar constants.
    /// -----------------------------------------------------------------------

    /// n (group order), 2^256 - n, and n / 2.
    static constexpr scalar_t order
    {
        0xbfd25e8cd0364141, 0xbaaedce6af48a03b,
        0xfffffffffffffffe, 0xffffffffffffffff
    };

    static constexpr scalar_t order_complement
    {
        0x402da1732fc9bebf, 0x4551231950b75fc4,
        0x0000000000000001, 0x0000000000000000
    };

    static constexpr scalar_t half_order
    {
        0xdfe92f46681b20a0, 0x5d576e7357a4501d,
        0xffffffffffffffff, 0x7fffffffffffffff
    };

    /// Endomorphism eigenvalue, lambda^3 = 1 mod n.
    static constexpr scalar_t lambda
    {
        0xdf02967c1b23bd72, 0x122e22ea20816678,
        0xa5261c028812645a, 0x5363ad4cc05c30e0
    };

    /// Lattice basis terms -b1 and -b2 mod n.
    static constexpr scalar_t minus_b1
    {
        0x6f547fa90abfe4c3, 0xe4437ed6010e8828,
        0x0000000000000000, 0x0000000000000000
    };

    static constexpr scalar_t minus_b2
    {
        0xd765cda83db1562c, 0x8a280ac50774346d,
        0xfffffffffffffffe, 0xffffffffffffffff
    };

    /// round(2^384 * b2 / n) and round(2^384 * -b1 / n).
    static constexpr scalar_t g1
    {
        0xe893209a45dbb031, 0x3daa8a1471e8ca7f,
        0xe86c90e49284eb15, 0x3086d221a7d46bcd
    };

    static constexpr scalar_t g2
    {
        0x1571b4ae8ac47f71, 0x221208ac9df506c6,
        0x6f547fa90abfe4c4, 0xe4437ed6010e8828
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
    static constexpr bool from_bytes(scalar_t& r, const bytes_t& bytes) NOEXCEPT;

    /// bytes from a.
    static constexpr void to_bytes(bytes_t& out, const scalar_t& a) NOEXCEPT;

    /// Scalar encoding internals.
    /// -----------------------------------------------------------------------

    /// r from 32 big-endian bytes at Offset.
    template <size_t Offset = zero, size_t Size = array_count<bytes_t>>
    static constexpr void decode(scalar_t& r,
        const data_array<Size>& bytes) NOEXCEPT;

    /// out as 32 big-endian bytes of a.
    static constexpr void encode(bytes_t& out, const scalar_t& a) NOEXCEPT;

    /// Scalar internals.
    /// -----------------------------------------------------------------------

    static constexpr bool is_overflow(const scalar_t& a) NOEXCEPT;
    static constexpr bool is_less(const scalar_t& a, const scalar_t& b) NOEXCEPT;
    static constexpr void reduce(scalar_t& r, bool overflow) NOEXCEPT;
    static constexpr void reduce(scalar_t& r, const wide_t& value) NOEXCEPT;

    static constexpr void multiply_add(wide_t& r, size_t position,
        uint64_t a, uint64_t b) NOEXCEPT;

    static constexpr void product(wide_t& r, const scalar_t& a,
        const scalar_t& b) NOEXCEPT;

    static constexpr void multiply_shift(scalar_t& r, const scalar_t& a,
        const scalar_t& b) NOEXCEPT;

    /// Inversion (variable time).
    /// -----------------------------------------------------------------------
    /// Bernstein-Yang safegcd in batches of 62 divsteps, over five signed 62
    /// bit limbs (value is the sum of v[i] * 2^(62 * i)).

    using signed62_t = std_array<int64_t, 5>;

    /// Signed 128 bit accumulator, as two's complement high and low words.
    struct signed128_t
    {
        uint64_t high{};
        uint64_t low{};
    };

    /// Transition matrix of 62 divsteps.
    struct transition_t
    {
        int64_t u{};
        int64_t v{};
        int64_t q{};
        int64_t r{};
    };

    /// Odd modulus and its inverse modulo 2^62.
    struct modulus_t
    {
        signed62_t value{};
        uint64_t inverse{};
    };

    static constexpr modulus_t prime_modulus
    {
        { -0x00000001000003d1, 0, 0, 0, 256 },
        0x27c7f6e22ddacacf
    };

    static constexpr modulus_t order_modulus
    {
        { 0x3fd25e8cd0364141, 0x2abb739abd2280ee, -0x0000000000000015, 0, 256 },
        0x34f20099aa774ec1
    };

    /// x = x^-1 mod m in [0, m), zero for zero, x in [0, m).
    static constexpr void invert(signed62_t& x, const modulus_t& m) NOEXCEPT;

    static constexpr int64_t divsteps(transition_t& t, int64_t eta,
        uint64_t f, uint64_t g) NOEXCEPT;
    static constexpr void update_de(signed62_t& d, signed62_t& e,
        const transition_t& t, const modulus_t& m) NOEXCEPT;
    static constexpr void update_fg(signed62_t& f, signed62_t& g,
        size_t length, const transition_t& t) NOEXCEPT;
    static constexpr void normalize(signed62_t& r, int64_t sign,
        const modulus_t& m) NOEXCEPT;

    static constexpr void multiply_add(signed128_t& r, int64_t a,
        int64_t b) NOEXCEPT;
    static constexpr void shift(signed128_t& r) NOEXCEPT;

    static constexpr void to_signed62(signed62_t& r,
        const field_t<uint64_t>& a) NOEXCEPT;
    static constexpr void to_signed62(signed62_t& r,
        const scalar_t& a) NOEXCEPT;
    static constexpr void from_signed62(field_t<uint64_t>& r,
        const signed62_t& a) NOEXCEPT;
    static constexpr void from_signed62(scalar_t& r,
        const signed62_t& a) NOEXCEPT;

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

    /// r = mask ? a : b, per lane.
    template <typename Word>
    static constexpr void select(jacobian_t<Word>& r, Word mask,
        const jacobian_t<Word>& a, const jacobian_t<Word>& b) NOEXCEPT;

    /// r = mask ? -a : a, per lane.
    template <typename Word>
    static constexpr void negate(affine_t<Word>& r, const affine_t<Word>& a,
        Word mask) NOEXCEPT;

    /// Window tables.
    /// -----------------------------------------------------------------------

    /// Window bits of generator and point tables.
    static constexpr size_t generator_bits = 7;
    static constexpr size_t point_bits = 5;

    /// Number of odd multiples (1, 3, ..., 2^Bits - 1) in a window table.
    template <size_t Bits>
    static constexpr size_t table_size = power2(sub1(Bits));

    /// Odd multiples as limb columns, for gather by index (normal).
    template <size_t Bits>
    struct table_t
    {
        std_array<std_array<uint64_t, table_size<Bits>>, 5> x{};
        std_array<std_array<uint64_t, table_size<Bits>>, 5> y{};
    };

    /// Odd multiples of a point per lane (normal).
    template <typename Word>
    using points_t = std_array<affine_t<Word>, table_size<point_bits>>;

    /// r = odd multiples of a.
    template <typename Word>
    static constexpr void multiples(points_t<Word>& r,
        const affine_t<Word>& a) NOEXCEPT;

    /// Table internals.
    /// -----------------------------------------------------------------------
    /// Generator tables are computed in chunks, each within the step limits of
    /// a single constant evaluation, sharing one inversion.

    static constexpr size_t chunk_size = 16;

    template <size_t Bits>
    static constexpr size_t chunk_count = table_size<Bits> / chunk_size;

    /// Multiples and prefix products of their z coordinates.
    struct forward_t
    {
        std_array<jacobian_t<uint64_t>, chunk_size> points{};
        std_array<field_t<uint64_t>, chunk_size> products{};
    };

    /// Affine multiples and inverse of the prefix product before the chunk.
    struct backward_t
    {
        std_array<affine_t<uint64_t>, chunk_size> points{};
        field_t<uint64_t> inverse{};
    };

    template <size_t Bits, size_t Chunk>
    static constexpr forward_t forward() NOEXCEPT;

    template <size_t Bits, size_t Chunk>
    static constexpr backward_t backward() NOEXCEPT;

    template <size_t Bits, size_t Chunk = 0>
    static constexpr void tabulate(table_t<Bits>& r, bool mapped) NOEXCEPT;

    template <size_t Bits>
    static constexpr table_t<Bits> tabulate(bool mapped) NOEXCEPT;

    template <size_t Bits, size_t Chunk>
    static constexpr forward_t forwards = forward<Bits, Chunk>();

    template <size_t Bits, size_t Chunk>
    static constexpr backward_t backwards = backward<Bits, Chunk>();

    /// Generator tables.
    /// -----------------------------------------------------------------------

    /// Odd multiples of G.
    template <size_t Bits>
    static constexpr table_t<Bits> generators = tabulate<Bits>(false);

    /// Odd multiples of lambda * G.
    template <size_t Bits>
    static constexpr table_t<Bits> endomorphisms = tabulate<Bits>(true);

    /// Multiplication (weak coordinates).
    /// -----------------------------------------------------------------------

    /// Number of 64 bit lanes in a word.
    template <typename Word>
    static constexpr size_t lanes = capacity<Word, uint64_t>;

    /// Scalar per lane.
    template <typename Word>
    using scalars_t = std_array<scalar_t, lanes<Word>>;

    /// r = g * G + k * a, mask of lanes not computed (exceptional).
    template <typename Word>
    static constexpr Word multiply(jacobian_t<Word>& r,
        const scalars_t<Word>& g, const affine_t<Word>& a,
        const scalars_t<Word>& k) NOEXCEPT;

    /// r = g * G + k * a, all cases.
    static constexpr void multiply_complete(jacobian_t<uint64_t>& r,
        const scalar_t& g, const affine_t<uint64_t>& a,
        const scalar_t& k) NOEXCEPT;

    /// Multiplication internals.
    /// -----------------------------------------------------------------------

    /// Split half magnitude bound (below 2^128, and at most 2^128 made odd).
    static constexpr size_t half_bits = 129;

    /// Number of window digits of a split half.
    template <size_t Bits>
    static constexpr size_t digit_count = ceilinged_divide(half_bits, Bits);

    /// Split half as window digits of its odd magnitude.
    template <size_t Bits>
    struct recoded_t
    {
        digits_t<digit_count<Bits>> digits{};
        bool negative{};
        bool even{};
    };

    template <size_t Bits, typename Word>
    using recodes_t = std_array<recoded_t<Bits>, lanes<Word>>;

    template <size_t Bits>
    static constexpr void recode(recoded_t<Bits>& r,
        const scalar_t& half) NOEXCEPT;

    template <typename Word>
    static constexpr Word pack(
        const std_array<uint64_t, lanes<Word>>& values) NOEXCEPT;

    template <size_t Bits, typename Word>
    static constexpr void digit(Word& index, Word& negative,
        const recodes_t<Bits, Word>& halves, size_t position,
        bool interleaved) NOEXCEPT;

    template <size_t Bits, typename Word>
    static constexpr void lookup(affine_t<Word>& r, const table_t<Bits>& table,
        Word index, Word negative) NOEXCEPT;

    template <typename Word>
    static constexpr void lookup(affine_t<Word>& r, const points_t<Word>& table,
        Word offset, Word negative) NOEXCEPT;

    template <typename Word>
    static constexpr void add_point(jacobian_t<Word>& r,
        const affine_t<Word>& b, Word& faults) NOEXCEPT;

    template <size_t Bits, typename Word>
    static constexpr void correct(jacobian_t<Word>& r, const affine_t<Word>& a,
        const recodes_t<Bits, Word>& halves, Word& faults) NOEXCEPT;

    /// Verification.
    /// -----------------------------------------------------------------------

    /// n as a field element, and p - n.
    static constexpr field_t<uint64_t> order_field
    {
        0x00025e8cd0364141,
        0x000e6af48a03bbfd,
        0x000ffffffebaaedc,
        0x000fffffffffffff,
        0x0000ffffffffffff
    };

    static constexpr scalar_t prime_minus_order
    {
        0x402da1722fc9baee, 0x4551231950b75fc4,
        0x0000000000000001, 0x0000000000000000
    };

    /// r from compressed key, false if invalid (normal).
    static constexpr bool from_bytes(affine_t<uint64_t>& r,
        const ec_compressed& key) NOEXCEPT;

    /// r from uncompressed or hybrid key, false if invalid (normal).
    static constexpr bool from_bytes(affine_t<uint64_t>& r,
        const ec_uncompressed& key) NOEXCEPT;

    /// ECDSA verification of hash by point, s high or low.
    static constexpr bool verify_ecdsa(const affine_t<uint64_t>& point,
        const bytes_t& hash, const bytes_t& r, const bytes_t& s) NOEXCEPT;

    /// BIP340 challenge hash of r, x-only key, and message.
    static hash_digest challenge(const bytes_t& r, const bytes_t& key,
        const data_slice& message) NOEXCEPT;

    /// BIP340 verification of challenge hash by x-only key.
    static constexpr bool verify_schnorr(const bytes_t& key,
        const hash_digest& digest, const bytes_t& r, const bytes_t& s) NOEXCEPT;

    /// Batch verification.
    /// -----------------------------------------------------------------------
    /// Rows verify in lanes of Word, with results of one for each valid row,
    /// and true if all rows are valid. Signatures are r and s, big-endian.

    template <typename Word>
    static bool verify_ecdsa(data_chunk& results,
        std::span<const ec_compressed> keys,
        std::span<const hash_digest> hashes,
        std::span<const ec_signature> signatures) NOEXCEPT;

    template <typename Word>
    static bool verify_schnorr(data_chunk& results,
        std::span<const ec_xonly> keys,
        std::span<const hash_digest> messages,
        std::span<const ec_signature> signatures) NOEXCEPT;

    /// Batch internals.
    /// -----------------------------------------------------------------------

    /// Each value = value^-1 by one inversion, no value zero.
    template <typename Element>
    static void inverse(std::vector<Element>& values) NOEXCEPT;

    template <typename Word>
    static std_array<uint64_t, lanes<Word>> unpack(Word value) NOEXCEPT;

    template <typename Word>
    static void pack(field_t<Word>& r,
        const std_array<field_t<uint64_t>, lanes<Word>>& rows) NOEXCEPT;

    template <typename Word>
    static void unpack(std_array<jacobian_t<uint64_t>, lanes<Word>>& rows,
        const jacobian_t<Word>& a) NOEXCEPT;
};

} // namespace secp256k1
} // namespace system
} // namespace libbitcoin

BC_PUSH_WARNING(NO_ARRAY_INDEXING)

#include <bitcoin/system/impl/crypto/secp256k1/algorithm_field.ipp>
#include <bitcoin/system/impl/crypto/secp256k1/algorithm_scalar.ipp>
#include <bitcoin/system/impl/crypto/secp256k1/algorithm_inversion.ipp>
#include <bitcoin/system/impl/crypto/secp256k1/algorithm_group.ipp>
#include <bitcoin/system/impl/crypto/secp256k1/algorithm_table.ipp>
#include <bitcoin/system/impl/crypto/secp256k1/algorithm_multiply.ipp>
#include <bitcoin/system/impl/crypto/secp256k1/algorithm_verify.ipp>
#include <bitcoin/system/impl/crypto/secp256k1/algorithm_batch.ipp>

BC_POP_WARNING()

#endif
