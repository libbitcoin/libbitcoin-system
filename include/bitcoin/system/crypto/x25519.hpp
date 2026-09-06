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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_X25519_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_X25519_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// X25519 Diffie-Hellman function over Curve25519 (rfc7748).
/// Scalars are clamped and points are u-coordinates, both little-endian.
class BC_API x25519
{
public:
    /// Scalars and points are always 256 bits.
    static constexpr size_t key_size = 32;
    typedef data_array<key_size> key;

    /// Compute scalar * point (false if the result is the zero point).
    static bool multiply(key& out, const key& scalar, const key& point) NOEXCEPT;

    /// Compute scalar * base point (u = 9).
    static bool multiply(key& out, const key& scalar) NOEXCEPT;

    /// Generate a keypair from maybe_random (not for wallet keys).
    static void generate(key& private_key, key& public_key,
        bool sanitize=true) NOEXCEPT;

protected:
    /// Field elements are sixteen radix 2^16 limbs (unreduced) over int64.
    static constexpr size_t limbs = 16;
    typedef std_array<int64_t, limbs> field;

    static constexpr void carry(field& out) NOEXCEPT;
    static constexpr void square(field& out, const field& a) NOEXCEPT;
    static constexpr void invert(field& out, const field& a) NOEXCEPT;
    static constexpr void select(field& p, field& q, int64_t bit) NOEXCEPT;
    static constexpr void add(field& out, const field& a, const field& b) NOEXCEPT;
    static constexpr void subtract(field& out, const field& a, const field& b) NOEXCEPT;
    static constexpr void multiply(field& out, const field& a, const field& b) NOEXCEPT;

    static void pack(key& out, const field& in) NOEXCEPT;
    static void unpack(field& out, const key& in) NOEXCEPT;
};

} // namespace system
} // namespace libbitcoin

#endif
