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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SHAMIR_ALGORITHM_CIPHER_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SHAMIR_ALGORITHM_CIPHER_IPP

// Cipher
// ============================================================================
// The master secret is encrypted before it is shared, so the shares are
// useless without the passphrase, and any passphrase decrypts to some secret.
// The round function is pbkdf2, at an iteration count scaled by the exponent,
// which is carried by each share. An extendable backup salts the rounds
// without the identifier, so the same shares extend to another share set.

namespace libbitcoin {
namespace system {
namespace shamir {

// public
// ----------------------------------------------------------------------------

TEMPLATE
typename CLASS::secret_t CLASS::
encrypt(const secret_t& secret, const std::string& passphrase,
    uint8_t exponent, uint16_t identifier, bool extendable) NOEXCEPT
{
    return feistel(secret, passphrase, exponent, identifier, extendable,
        false);
}

TEMPLATE
typename CLASS::secret_t CLASS::
decrypt(const secret_t& secret, const std::string& passphrase,
    uint8_t exponent, uint16_t identifier, bool extendable) NOEXCEPT
{
    return feistel(secret, passphrase, exponent, identifier, extendable, true);
}

// protected
// ----------------------------------------------------------------------------

TEMPLATE
data_chunk CLASS::
salt_prefix(uint16_t identifier, bool extendable) NOEXCEPT
{
    if (extendable)
        return {};

    const auto id = to_big_endian(identifier);
    return splice(rs1024::customize(extendable), id);
}

TEMPLATE
typename CLASS::half_t CLASS::
round_function(uint8_t round, const std::string& passphrase, uint8_t exponent,
    const data_chunk& prefix, const half_t& right) NOEXCEPT
{
    const auto iterations = shift_left<size_t>(base_iterations / rounds,
        exponent);
    const auto password = splice(to_array(round), passphrase);
    const auto salt = splice(prefix, right);
    return pbkd<sha256>::key<half_bytes>(password, salt, iterations);
}

TEMPLATE
typename CLASS::secret_t CLASS::
feistel(const secret_t& secret, const std::string& passphrase,
    uint8_t exponent, uint16_t identifier, bool extendable,
    bool reverse) NOEXCEPT
{
    auto parts = system::split(secret);
    auto& left = parts.first;
    auto& right = parts.second;
    const auto prefix = salt_prefix(identifier, extendable);

    for (auto round = zero; round < rounds; ++round)
    {
        const auto reversed = sub1(rounds) - round;
        const auto index = reverse ? reversed : round;
        const auto key = round_function(narrow_cast<uint8_t>(index),
            passphrase, exponent, prefix, right);
        const auto mixed = xor_data<half_bytes>(left, key);
        left = right;
        right = mixed;
    }

    return splice(right, left);
}

} // namespace shamir
} // namespace system
} // namespace libbitcoin

#endif
