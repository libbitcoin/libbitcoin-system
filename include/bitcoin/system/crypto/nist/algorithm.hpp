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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_NIST_ALGORITHM_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_NIST_ALGORITHM_HPP

#include <span>
#include <bitcoin/system/crypto/maybe_random.hpp>
#include <bitcoin/system/crypto/nist/nist.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/intrinsics/intrinsics.hpp>
#include <bitcoin/system/math/math.hpp>

// Based on:
// SEC 1 [Elliptic Curve Cryptography] (2.3, 4.1).
// secg.org/sec1-v2.pdf
// RFC 6979 [Deterministic Usage of DSA and ECDSA].
// datatracker.ietf.org/doc/html/rfc6979
// [Renes, Costello, Batina] Complete addition formulas for prime order
// elliptic curves (algorithms 4 and 6, a = -3).
// eprint.iacr.org/2015/1060

namespace libbitcoin {
namespace system {
namespace nist {

/// ECDSA over a short Weierstrass curve with a = -3. Secret key operations
/// (key derivation, signing) are constant time.
template <typename Curve, if_same<typename Curve::T, nist::curve_t> = true>
class algorithm
{
public:
    /// Types.
    /// -----------------------------------------------------------------------

    using C = Curve;
    static constexpr auto size = C::size;
    using secret_t     = data_array<size>;
    using point_t      = data_array<add1(two * size)>;
    using compressed_t = data_array<add1(size)>;
    using signature_t  = data_array<two * size>;

    /// Keys.
    /// -----------------------------------------------------------------------

    /// A secret in [1, n - 1] from maybe_random.
    static secret_t generate() NOEXCEPT;

    /// The uncompressed (sec1) public key of secret, false if not in range.
    static bool public_key(point_t& out, const secret_t& secret) NOEXCEPT;

    /// False if the uncompressed (sec1) point is not on the curve.
    static bool is_valid(const point_t& point) NOEXCEPT;

    /// Convert between uncompressed and compressed (sec1) points.
    static bool compress(compressed_t& out, const point_t& point) NOEXCEPT;
    static bool decompress(point_t& out, const compressed_t& point) NOEXCEPT;

    /// ECDSA.
    /// -----------------------------------------------------------------------

    /// Sign the digest as r || s with the rfc6979 nonce.
    static bool sign(signature_t& out, const secret_t& secret,
        const_byte_span digest) NOEXCEPT;

    /// Verify the r || s signature of the digest by the uncompressed point.
    static bool verify(const signature_t& signature, const point_t& point,
        const_byte_span digest) NOEXCEPT;

    /// Encode r || s as a der ECDSA-Sig-Value.
    static data_chunk encode(const signature_t& signature) NOEXCEPT;

    /// Decode a strictly der ECDSA-Sig-Value to r || s.
    static bool decode(signature_t& out, const_byte_span der) NOEXCEPT;

protected:
    /// Types.
    /// -----------------------------------------------------------------------

    /// Little-endian 64 bit limbs.
    static constexpr auto words = size / sizeof(uint64_t);
    using limbs_t = std_array<uint64_t, words>;
    using bytes_t = data_array<size>;

    /// A modulus with its Montgomery constants (R = 2^(64 * words)).
    struct modulus_t
    {
        limbs_t value{};
        limbs_t one{};
        limbs_t squared{};
        limbs_t inverse_exponent{};
        limbs_t root_exponent{};
        uint64_t inverse{};
    };

    /// Homogeneous projective point (x / z, y / z), Montgomery coordinates,
    /// (0 : 1 : 0) is infinity.
    struct projective_t
    {
        limbs_t x{};
        limbs_t y{};
        limbs_t z{};
    };

    /// Multiples 0 through 15 of a point.
    static constexpr auto window = 4_size;
    static constexpr auto entries = power2(window);
    static constexpr auto windows = C::strength / window;
    using table_t = std_array<projective_t, entries>;

    /// Limbs.
    /// -----------------------------------------------------------------------

    static constexpr limbs_t to_limbs(const bytes_t& bytes) NOEXCEPT;
    static constexpr bytes_t to_bytes(const limbs_t& limbs) NOEXCEPT;

    INLINE static constexpr uint64_t add_carry(uint64_t& out, uint64_t a,
        uint64_t b, uint64_t carry) NOEXCEPT;
    INLINE static constexpr uint64_t subtract_borrow(uint64_t& out,
        uint64_t a, uint64_t b, uint64_t borrow) NOEXCEPT;
    INLINE static constexpr uint64_t multiply_add(uint64_t t, uint64_t a,
        uint64_t b, uint64_t& carry) NOEXCEPT;

    INLINE static constexpr void select(limbs_t& out, uint64_t mask,
        const limbs_t& a, const limbs_t& b) NOEXCEPT;
    INLINE static constexpr uint64_t zero_mask(const limbs_t& a) NOEXCEPT;
    static constexpr bool is_zero(const limbs_t& a) NOEXCEPT;
    static constexpr bool is_less(const limbs_t& a, const limbs_t& b) NOEXCEPT;
    static constexpr uint64_t sum(limbs_t& out, const limbs_t& a,
        const limbs_t& b) NOEXCEPT;
    static constexpr uint64_t difference(limbs_t& out, const limbs_t& a,
        const limbs_t& b) NOEXCEPT;

    /// Modular arithmetic (operands less than the modulus).
    /// -----------------------------------------------------------------------

    static consteval modulus_t make_modulus(const limbs_t& value) NOEXCEPT;
    static constexpr const modulus_t& field() NOEXCEPT;
    static constexpr const modulus_t& scalar() NOEXCEPT;

    static constexpr limbs_t add(const limbs_t& a, const limbs_t& b,
        const modulus_t& m) NOEXCEPT;
    static constexpr limbs_t subtract(const limbs_t& a, const limbs_t& b,
        const modulus_t& m) NOEXCEPT;
    static constexpr limbs_t multiply(const limbs_t& a, const limbs_t& b,
        const modulus_t& m) NOEXCEPT;
    static constexpr limbs_t power(const limbs_t& a, const limbs_t& exponent,
        const modulus_t& m) NOEXCEPT;
    static constexpr limbs_t inverse(const limbs_t& a,
        const modulus_t& m) NOEXCEPT;
    static constexpr limbs_t to_montgomery(const limbs_t& a,
        const modulus_t& m) NOEXCEPT;
    static constexpr limbs_t from_montgomery(const limbs_t& a,
        const modulus_t& m) NOEXCEPT;

    /// Constants.
    /// -----------------------------------------------------------------------

    static constexpr modulus_t field_modulus =
        make_modulus(to_limbs(C::prime));
    static constexpr modulus_t scalar_modulus =
        make_modulus(to_limbs(C::order));
    static constexpr limbs_t curve_b = to_montgomery(to_limbs(C::b),
        field_modulus);

    /// Points.
    /// -----------------------------------------------------------------------

    static constexpr projective_t infinity() NOEXCEPT;
    static constexpr projective_t generator() NOEXCEPT;
    static constexpr limbs_t curve(const limbs_t& x) NOEXCEPT;
    static constexpr projective_t add(const projective_t& p,
        const projective_t& q) NOEXCEPT;
    static constexpr projective_t twice(const projective_t& p) NOEXCEPT;
    static constexpr bool to_affine(limbs_t& x, limbs_t& y,
        const projective_t& p) NOEXCEPT;

    static constexpr bool parse(projective_t& out,
        const point_t& point) NOEXCEPT;
    static constexpr point_t serialize(const limbs_t& x,
        const limbs_t& y) NOEXCEPT;

    /// Multiplication.
    /// -----------------------------------------------------------------------

    static constexpr table_t make_table(const projective_t& p) NOEXCEPT;
    static constexpr table_t generator_table() NOEXCEPT;
    static constexpr size_t digit(const limbs_t& scalar,
        size_t index) NOEXCEPT;
    static constexpr projective_t lookup(const table_t& table,
        size_t digit) NOEXCEPT;
    static constexpr projective_t multiply(const table_t& table,
        const limbs_t& scalar) NOEXCEPT;
    static constexpr projective_t multiply(const limbs_t& g,
        const table_t& generators, const limbs_t& q,
        const table_t& table) NOEXCEPT;

    /// ECDSA.
    /// -----------------------------------------------------------------------

    static constexpr limbs_t to_scalar(const_byte_span digest) NOEXCEPT;
    static constexpr bool is_scalar(const limbs_t& value) NOEXCEPT;
    static constexpr limbs_t reduce(const limbs_t& value) NOEXCEPT;

    /// DER.
    /// -----------------------------------------------------------------------

    static void encode_integer(data_chunk& out, const_byte_span value) NOEXCEPT;
    static bool decode_integer(bytes_t& out, const_byte_span& der) NOEXCEPT;
};

} // namespace nist
} // namespace system
} // namespace libbitcoin

#define TEMPLATE template <typename Curve, \
    if_same<typename Curve::T, nist::curve_t> If>
#define CLASS algorithm<Curve, If>

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_USE_OF_SPAN)

#include <bitcoin/system/impl/crypto/nist/algorithm_der.ipp>
#include <bitcoin/system/impl/crypto/nist/algorithm_ecdsa.ipp>
#include <bitcoin/system/impl/crypto/nist/algorithm_keys.ipp>
#include <bitcoin/system/impl/crypto/nist/algorithm_limbs.ipp>
#include <bitcoin/system/impl/crypto/nist/algorithm_modular.ipp>
#include <bitcoin/system/impl/crypto/nist/algorithm_multiply.ipp>
#include <bitcoin/system/impl/crypto/nist/algorithm_points.ipp>

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

#undef CLASS
#undef TEMPLATE

#endif
