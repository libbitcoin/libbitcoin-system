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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_CHACHA20_POLY1305_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_CHACHA20_POLY1305_HPP

#include <span>
#include <bitcoin/system/crypto/chacha20.hpp>
#include <bitcoin/system/crypto/poly1305.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// ChaCha20-Poly1305 authenticated encryption with associated data (rfc8439).
/// The nonce is expressed as a 32-bit and a 64-bit little-endian segment
/// (bip324 convention), and MUST not be repeated for the same key.
class BC_API chacha20_poly1305 final
{
public:
    /// Ciphertext expansion (the appended tag).
    static constexpr size_t expansion = poly1305::tag_size;

    chacha20_poly1305(const chacha20::secret& key) NOEXCEPT;

    /// Rekey.
    void set_key(const chacha20::secret& key) NOEXCEPT;

    /// Encrypt plain with aad into cipher (cipher = plain size + expansion).
    void encrypt(std::span<const uint8_t> plain, std::span<const uint8_t> aad,
        uint32_t nonce32, uint64_t nonce64, std::span<uint8_t> cipher) NOEXCEPT;

    /// Decrypt cipher with aad into plain (cipher = plain size + expansion).
    /// False if the tag does not authenticate, in which case plain is cleared.
    bool decrypt(std::span<uint8_t> plain, std::span<const uint8_t> aad,
        uint32_t nonce32, uint64_t nonce64,
        std::span<const uint8_t> cipher) NOEXCEPT;

    /// Write keystream into out (as encrypt of zeros, without the tag).
    void stream(uint32_t nonce32, uint64_t nonce64,
        std::span<uint8_t> out) NOEXCEPT;

private:
    void authenticate(poly1305::tag& out, std::span<const uint8_t> aad,
        std::span<const uint8_t> cipher) NOEXCEPT;

    chacha20 cipher_;
};

/// Forward-secure ChaCha20-Poly1305 (bip324).
/// The nonce is automatically incremented for each message, and the key is
/// rotated after every interval of messages.
class BC_API fschacha20_poly1305 final
{
public:
    DELETE_COPY_MOVE(fschacha20_poly1305);

    /// Ciphertext expansion (the appended tag).
    static constexpr size_t expansion = chacha20_poly1305::expansion;

    fschacha20_poly1305(const chacha20::secret& key,
        uint32_t interval) NOEXCEPT;

    /// Encrypt plain with aad into cipher (cipher = plain size + expansion).
    void encrypt(std::span<const uint8_t> plain, std::span<const uint8_t> aad,
        std::span<uint8_t> cipher) NOEXCEPT;

    /// Decrypt cipher with aad into plain (cipher = plain size + expansion).
    /// False if the tag does not authenticate, in which case plain is cleared.
    bool decrypt(std::span<uint8_t> plain, std::span<const uint8_t> aad,
        std::span<const uint8_t> cipher) NOEXCEPT;

private:
    void next() NOEXCEPT;

    chacha20_poly1305 aead_;
    const uint32_t interval_;
    uint32_t packets_{};
    uint64_t rekeys_{};
};

} // namespace system
} // namespace libbitcoin

#endif
