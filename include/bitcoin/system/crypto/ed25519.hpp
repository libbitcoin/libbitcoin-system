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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_ED25519_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_ED25519_HPP

#include <bitcoin/system/crypto/curve25519.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/radix/radix.hpp>

namespace libbitcoin {
namespace system {

/// Ed25519 signatures (rfc8032) and key blinding (rend-spec-v3 A.2).
/// Secret key operations constant time, values 256 bit little-endian.
class BC_API ed25519
  : public curve25519
{
public:
    /// Seeds, points (public keys), scalars and blinding factors.
    static constexpr size_t key_size = 32;
    typedef data_array<key_size> key;

    /// Expanded secret, scalar followed by nonce prefix.
    static constexpr size_t expanded_size = two * key_size;
    typedef data_array<expanded_size> expanded;

    /// Signature, point (R) followed by scalar (S).
    static constexpr size_t signature_size = two * key_size;
    typedef data_array<signature_size> signature;

    /// Expanded secret of seed (clamped scalar, prefix).
    static expanded expand(const key& seed) NOEXCEPT;

    /// Point (public key) of expanded secret.
    static key public_key(const expanded& secret) NOEXCEPT;

    /// Sign message by expanded secret, given its point.
    static signature sign(const expanded& secret, const key& point,
        const_byte_span message) NOEXCEPT;

    /// True if value is valid signature of message for point.
    static bool verify(const signature& value, const key& point,
        const_byte_span message) NOEXCEPT;

    /// Blind point by factor (clamped), false if invalid point.
    static bool blind(key& out, const key& point, const key& factor) NOEXCEPT;

    /// Blind expanded secret by factor (clamped), deriving prefix.
    static expanded blind(const expanded& secret, const key& factor) NOEXCEPT;

protected:
    /// Extended coordinates (X : Y : Z : T), x = X/Z, y = Y/Z, xy = T/Z.
    static constexpr size_t coordinates = 4;
    typedef std_array<field, coordinates> extended;

    /// Scalar products, 64 radix 2^8 digits (unreduced) over int64.
    static constexpr size_t digits = 64;
    typedef std_array<int64_t, digits> product;

    /// Curve constant d = -121665 / 121666 (mod p).
    static constexpr field curve_d
    {
        0x78a3, 0x1359, 0x4dca, 0x75eb, 0xd8ab, 0x4141, 0x0a4d, 0x0070,
        0xe898, 0x7779, 0x4079, 0x8cc7, 0xfe73, 0x2b6f, 0x6cee, 0x5203
    };

    /// Curve constant doubled, 2 * d (mod p).
    static constexpr field curve_d2
    {
        0xf159, 0x26b2, 0x9b94, 0xebd6, 0xb156, 0x8283, 0x149a, 0x00e0,
        0xd130, 0xeef3, 0x80f2, 0x198e, 0xfce7, 0x56df, 0xd9dc, 0x2406
    };

    /// Square root of -1 (mod p), 2^((p - 1) / 4).
    static constexpr field root
    {
        0xa0b0, 0x4a0e, 0x1b27, 0xc4ee, 0xe478, 0xad2f, 0x1806, 0x2f43,
        0xd7a7, 0x3dfb, 0x0099, 0x2b4d, 0xdf0b, 0x4fc1, 0x2480, 0x2b83
    };

    /// Base point B, y = 4 / 5 (mod p), x even.
    static constexpr field base_x
    {
        0xd51a, 0x8f25, 0x2d60, 0xc956, 0xa7b2, 0x9525, 0xc760, 0x692c,
        0xdc5c, 0xfdd6, 0xe231, 0xc0a4, 0x53fe, 0xcd6e, 0x36d3, 0x2169
    };
    static constexpr field base_y
    {
        0x6658, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666,
        0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666, 0x6666
    };

    /// Group order l = 2^252 + 27742317777372353535851937790883648493.
    static constexpr key order = base16_array(
        "edd3f55c1a631258d69cf7a2def9de1400000000000000000000000000000010");

    using curve25519::add;
    using curve25519::multiply;
    using curve25519::select;

    /// Field.
    /// -----------------------------------------------------------------------

    static bool parity(const field& a) NOEXCEPT;
    static bool same(const field& a, const field& b) NOEXCEPT;
    static constexpr void root_power(field& out, const field& a) NOEXCEPT;

    /// Points.
    /// -----------------------------------------------------------------------

    static constexpr extended identity() NOEXCEPT;
    static constexpr extended generator() NOEXCEPT;
    static constexpr void opposite(extended& p) NOEXCEPT;
    static constexpr void add(extended& p, const extended& q) NOEXCEPT;
    static constexpr void select(extended& p, extended& q,
        int64_t bit) NOEXCEPT;
    static constexpr extended multiply(const extended& point,
        const key& scalar) NOEXCEPT;
    static key encode(const extended& p) NOEXCEPT;
    static bool decode(extended& out, const key& point) NOEXCEPT;

    /// Scalars.
    /// -----------------------------------------------------------------------

    static constexpr key clamp(const key& scalar) NOEXCEPT;
    static constexpr bool is_canonical(const key& scalar) NOEXCEPT;
    static constexpr key reduce(product& x) NOEXCEPT;
    static constexpr key reduce(const key& scalar) NOEXCEPT;
    static constexpr key reduce(const expanded& wide) NOEXCEPT;
    static constexpr key multiply_add(const key& a, const key& b,
        const key& c) NOEXCEPT;
};

} // namespace system
} // namespace libbitcoin

#endif
