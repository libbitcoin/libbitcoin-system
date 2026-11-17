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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_CURVE25519_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_CURVE25519_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

/// The field of Curve25519 and edwards25519, p = 2^255 - 19.
class BC_API curve25519
{
protected:
    /// Field elements are sixteen radix 2^16 limbs (unreduced) over int64.
    static constexpr size_t limbs = 16;
    typedef std_array<int64_t, limbs> field;

    /// Field elements serialize to 256 bits, little-endian.
    static constexpr size_t element_size = 32;
    typedef data_array<element_size> element;

    /// Limbs are radix 2^16, and may be negative between carries.
    static constexpr auto radix_bits = 16_size;
    static constexpr auto radix = bit_right<int64_t>(radix_bits);

    /// The prime p in radix 2^16 limbs (low, middle, high).
    static constexpr int64_t prime_low = 0xffed;
    static constexpr int64_t prime_middle = 0xffff;
    static constexpr int64_t prime_high = 0x7fff;

    /// The fold of a limb carried beyond 2^256, 38 = 2 * 19 (mod p).
    static constexpr int64_t fold = 38;

    static constexpr void carry(field& out) NOEXCEPT;
    static constexpr void square(field& out, const field& a) NOEXCEPT;
    static constexpr void invert(field& out, const field& a) NOEXCEPT;
    static constexpr void select(field& p, field& q, int64_t bit) NOEXCEPT;
    static constexpr void add(field& out, const field& a,
        const field& b) NOEXCEPT;
    static constexpr void subtract(field& out, const field& a,
        const field& b) NOEXCEPT;
    static constexpr void multiply(field& out, const field& a,
        const field& b) NOEXCEPT;

    static inline void pack(element& out, const field& in) NOEXCEPT;
    static inline void unpack(field& out, const element& in) NOEXCEPT;
};

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/crypto/curve25519.ipp>

#endif
