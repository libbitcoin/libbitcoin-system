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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_RSA_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_RSA_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/uintx/uintx.hpp>

namespace libbitcoin {
namespace system {

/// RSA signature verification (rfc8017), moduli of up to 4096 bits.
/// Public key operations only, which are not constant time.
class BC_API rsa
{
public:
    /// The largest supported modulus.
    static constexpr size_t maximum_bits = 4096;
    static constexpr size_t maximum_size = bytes<maximum_bits>;

    /// A public key, modulus and exponent as big-endian magnitudes.
    struct public_key
    {
        data_chunk modulus{};
        data_chunk exponent{};
    };

    /// Decode a der RSAPublicKey (pkcs1), false if invalid or too large.
    static bool decode(public_key& out, const_byte_span der) NOEXCEPT;

    /// True if a pkcs1 v1.5 signature of payload, which is a DigestInfo and
    /// digest (rfc8017 9.2), or a bare digest as signed by Tor.
    static bool verify_pkcs1(const public_key& key, const_byte_span signature,
        const_byte_span payload) NOEXCEPT;

    /// True if a pss signature of digest, with mgf1 and a salt of the digest's
    /// hash (sha256, sha384 or sha512 by digest size).
    static bool verify_pss(const public_key& key, const_byte_span signature,
        const_byte_span digest) NOEXCEPT;

protected:
    using element = uintx_t<maximum_bits>;
    using product = uintx_t<two * maximum_bits>;

    static bool is_valid(const public_key& key) NOEXCEPT;
    static element multiply(const element& left, const element& right,
        const element& modulus) NOEXCEPT;
    static element power(const element& base, const element& exponent,
        const element& modulus) NOEXCEPT;
    static bool open(data_chunk& out, const public_key& key,
        const_byte_span signature) NOEXCEPT;
};

} // namespace system
} // namespace libbitcoin

#endif
