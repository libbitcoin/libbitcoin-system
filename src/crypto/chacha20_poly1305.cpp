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
#include <bitcoin/system/crypto/chacha20_poly1305.hpp>

#include <algorithm>
#include <bitcoin/system/crypto/chacha20.hpp>
#include <bitcoin/system/crypto/poly1305.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/math/math.hpp>

// based on:
// datatracker.ietf.org/doc/html/rfc8439
// [Nir, Langley]

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

constexpr data_array<16> zero_pad{};

// Bytes required to pad size to a multiple of the poly1305 block (16).
static constexpr size_t padding(size_t size) NOEXCEPT
{
    return (zero_pad.size() - (size % zero_pad.size())) % zero_pad.size();
}

// chacha20_poly1305
// ----------------------------------------------------------------------------

chacha20_poly1305::chacha20_poly1305(const chacha20::secret& key) NOEXCEPT
  : cipher_(key)
{
}

void chacha20_poly1305::set_key(const chacha20::secret& key) NOEXCEPT
{
    cipher_.set_key(key);
}

// private
void chacha20_poly1305::authenticate(poly1305::tag& out,
    std::span<const uint8_t> aad, std::span<const uint8_t> cipher) NOEXCEPT
{
    // rfc8439
    // First, a Poly1305 one-time key is generated from the 256-bit key and
    // nonce: the ChaCha20 block function with block counter zero, taking the
    // first 256 bits of the 512-bit state [cipher_ must be at block zero].
    poly1305::secret key{};
    cipher_.stream(key);

    // rfc8439
    // The Poly1305 message is the aad zero-padded to a multiple of sixteen,
    // the ciphertext likewise padded, the aad length, and then the ciphertext
    // length, both in octets as 64-bit little-endian words.
    poly1305 mac{ key };
    mac.write(aad);
    mac.write({ zero_pad.data(), padding(aad.size()) });
    mac.write(cipher);
    mac.write({ zero_pad.data(), padding(cipher.size()) });
    mac.write(to_little_endian<uint64_t>(aad.size()));
    mac.write(to_little_endian<uint64_t>(cipher.size()));
    mac.flush(out);
    key = {};
}

void chacha20_poly1305::encrypt(std::span<const uint8_t> plain,
    std::span<const uint8_t> aad, uint32_t nonce32, uint64_t nonce64,
    std::span<uint8_t> cipher) NOEXCEPT
{
    BC_ASSERT(cipher.size() == plain.size() + expansion);
    const auto text = cipher.first(plain.size());

    // rfc8439
    // Encryption uses the ChaCha20 block counter starting at one.
    cipher_.seek(nonce32, nonce64, one);
    cipher_.crypt(plain, text);

    // Tag key is drawn from block counter zero.
    cipher_.seek(nonce32, nonce64, zero);
    authenticate(unsafe_array_cast<uint8_t, expansion>(
        cipher.last(expansion).data()), aad, text);
}

bool chacha20_poly1305::decrypt(std::span<uint8_t> plain,
    std::span<const uint8_t> aad, uint32_t nonce32, uint64_t nonce64,
    std::span<const uint8_t> cipher) NOEXCEPT
{
    BC_ASSERT(cipher.size() == plain.size() + expansion);
    const auto text = cipher.first(plain.size());

    // Verify tag (key drawn from block counter zero) in constant time.
    poly1305::tag expected{};
    cipher_.seek(nonce32, nonce64, zero);
    authenticate(expected, aad, text);

    uint8_t difference{};
    const auto actual = cipher.last(expansion);
    for (size_t byte{}; byte < expansion; ++byte)
        difference |= expected[byte] ^ actual[byte];

    // rfc8439
    // Decryption uses the ChaCha20 block counter starting at one.
    const auto authenticated = is_zero(difference);
    if (authenticated)
    {
        cipher_.seek(nonce32, nonce64, one);
        cipher_.crypt(text, plain);
    }
    else
    {
        std::fill(plain.begin(), plain.end(), 0x00_u8);
    }

    return authenticated;
}

void chacha20_poly1305::stream(uint32_t nonce32, uint64_t nonce64,
    std::span<uint8_t> out) NOEXCEPT
{
    // rfc8439
    // The first output block is consumed by the poly1305 key generation.
    cipher_.seek(nonce32, nonce64, one);
    cipher_.stream(out);
}

// fschacha20_poly1305
// ----------------------------------------------------------------------------

fschacha20_poly1305::fschacha20_poly1305(const chacha20::secret& key,
    uint32_t interval) NOEXCEPT
  : aead_(key), interval_(interval)
{
}

// private
void fschacha20_poly1305::next() NOEXCEPT
{
    // bip324
    // The key is rotated after every rekey_interval messages, to the
    // keystream of nonce {0xffffffff, rekey counter}.
    if (++packets_ == interval_)
    {
        chacha20::secret key{};
        aead_.stream(0xffffffff_u32, rekeys_, key);
        aead_.set_key(key);
        key = {};
        packets_ = 0;
        ++rekeys_;
    }
}

void fschacha20_poly1305::encrypt(std::span<const uint8_t> plain,
    std::span<const uint8_t> aad, std::span<uint8_t> cipher) NOEXCEPT
{
    // bip324
    // The nonce is {message counter, rekey counter}.
    aead_.encrypt(plain, aad, packets_, rekeys_, cipher);
    next();
}

bool fschacha20_poly1305::decrypt(std::span<uint8_t> plain,
    std::span<const uint8_t> aad, std::span<const uint8_t> cipher) NOEXCEPT
{
    const auto valid = aead_.decrypt(plain, aad, packets_, rekeys_, cipher);
    next();
    return valid;
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
