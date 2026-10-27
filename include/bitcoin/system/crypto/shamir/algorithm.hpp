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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SHAMIR_ALGORITHM_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SHAMIR_ALGORITHM_HPP

#include <bitcoin/system/crypto/shamir/rs1024.hpp>
#include <bitcoin/system/crypto/shamir/shamir.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>

// Based on:
// SLIP-0039 [Shamir's Secret-Sharing for Mnemonic Codes].
// github.com/satoshilabs/slips/blob/master/slip-0039.md

namespace libbitcoin {
namespace system {
namespace shamir {

/// Secret sharing over GF(256), with the slip39 digest share, share word
/// checksum and master secret encryption. Field multiplication is constant
/// time (no tables).
template <typename Shamir,
    if_same<typename Shamir::T, shamir::shamirk_t> = true>
class algorithm
{
public:
    /// Types.
    /// -----------------------------------------------------------------------

    using K = Shamir;
    static constexpr auto secret_bytes = K::size;
    using secret_t = data_array<secret_bytes>;

    /// A share value at its field coordinate.
    struct share_t
    {
        uint8_t index{};
        secret_t value{};
    };

    using shares_t = std_vector<share_t>;

    /// Sharing.
    /// -----------------------------------------------------------------------

    /// Random bytes required to split a secret at the given threshold.
    static constexpr size_t entropy_size(size_t threshold) NOEXCEPT;

    /// Split secret into count shares, of which threshold are required.
    /// False if the sharing is invalid or entropy is not entropy_size.
    static bool split(shares_t& out, const secret_t& secret, size_t threshold,
        size_t count, const data_slice& entropy) NOEXCEPT;

    /// Recover the secret from threshold shares, verifying the digest share.
    /// False if the shares are insufficient, repeat a coordinate, or the
    /// digest does not match.
    static bool recover(secret_t& out, const shares_t& shares,
        size_t threshold) NOEXCEPT;

    /// Encryption.
    /// -----------------------------------------------------------------------

    /// Encrypt the master secret (four round feistel network).
    static secret_t encrypt(const secret_t& secret,
        const std::string& passphrase, uint8_t exponent, uint16_t identifier,
        bool extendable) NOEXCEPT;

    /// Decrypt the encrypted master secret.
    static secret_t decrypt(const secret_t& secret,
        const std::string& passphrase, uint8_t exponent, uint16_t identifier,
        bool extendable) NOEXCEPT;

protected:
    /// Field.
    /// -----------------------------------------------------------------------

    /// GF(256) is reduced by x^8 + x^4 + x^3 + x + 1 (rijndael).
    static constexpr auto polynomial = 0x1b_u8;

    /// Any nonzero element raised to this power is its inverse.
    static constexpr auto inverse_power = 254_u8;

    static constexpr uint8_t multiply(uint8_t left, uint8_t right) NOEXCEPT;
    static constexpr uint8_t invert(uint8_t value) NOEXCEPT;

    /// The byte-wise lagrange interpolation of the shares at a coordinate.
    static secret_t interpolate(uint8_t index,
        const shares_t& shares) NOEXCEPT;

    /// Sharing.
    /// -----------------------------------------------------------------------

    /// The secret and its digest are shared at these reserved coordinates,
    /// so share coordinates are below the digest coordinate.
    static constexpr auto secret_index = 255_u8;
    static constexpr auto digest_index = 254_u8;

    /// The digest share holds a truncated code over its own random bytes.
    static constexpr auto digest_bytes = 4_size;
    static constexpr auto random_bytes = secret_bytes - digest_bytes;
    using digest_t = data_array<digest_bytes>;
    using random_t = data_array<random_bytes>;

    static digest_t digest(const data_slice& random,
        const secret_t& secret) NOEXCEPT;
    static bool is_valid_sharing(size_t threshold, size_t count) NOEXCEPT;
    static bool is_distinct(const shares_t& shares) NOEXCEPT;

    /// Encryption.
    /// -----------------------------------------------------------------------

    static constexpr auto rounds = 4_size;
    static constexpr auto base_iterations = 10000_size;
    static constexpr auto half_bytes = to_half(secret_bytes);
    using half_t = data_array<half_bytes>;

    /// An extendable backup is salted without its identifier.
    static data_chunk salt_prefix(uint16_t identifier,
        bool extendable) NOEXCEPT;
    static half_t round_function(uint8_t round, const std::string& passphrase,
        uint8_t exponent, const data_chunk& prefix,
        const half_t& right) NOEXCEPT;
    static secret_t feistel(const secret_t& secret,
        const std::string& passphrase, uint8_t exponent, uint16_t identifier,
        bool extendable, bool reverse) NOEXCEPT;
};

} // namespace shamir
} // namespace system
} // namespace libbitcoin

#define TEMPLATE template <typename Shamir, \
    if_same<typename Shamir::T, shamir::shamirk_t> If>
#define CLASS algorithm<Shamir, If>

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

#include <bitcoin/system/impl/crypto/shamir/algorithm_cipher.ipp>
#include <bitcoin/system/impl/crypto/shamir/algorithm_field.ipp>
#include <bitcoin/system/impl/crypto/shamir/algorithm_shares.ipp>

BC_POP_WARNING()
BC_POP_WARNING()

#undef CLASS
#undef TEMPLATE

#endif
