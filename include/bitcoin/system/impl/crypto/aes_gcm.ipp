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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_AES_GCM_IPP
#define LIBBITCOIN_SYSTEM_CRYPTO_AES_GCM_IPP

// based on:
// nvlpubs.nist.gov/nistpubs/Legacy/SP/nistspecialpublication800-38d.pdf

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_USE_OF_SPAN)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

TEMPLATE
CLASS::
aes_gcm(const secret& key) NOEXCEPT
{
    set_key(key);
}

TEMPLATE
CLASS::
~aes_gcm() NOEXCEPT
{
    wipe(schedule_);
    wipe(key_);
}

TEMPLATE
void CLASS::
set_key(const secret& key) NOEXCEPT
{
    // The hash subkey is the cipher of the zero block.
    schedule_ = Aes::expand(key);
    key_ = {};
    Aes::encrypt(key_, schedule_);
}

// private
// The 96-bit nonce is followed by the 32-bit big-endian block counter.
TEMPLATE
typename CLASS::block CLASS::
counter(const nonce& iv, uint32_t value) NOEXCEPT
{
    block out{};
    std::copy(iv.cbegin(), iv.cend(), out.begin());
    to_big<nonce_size>(out, value);
    return out;
}

// private
TEMPLATE
void CLASS::
authenticate(block& out, const_byte_span aad, const_byte_span cipher,
    const nonce& iv) const NOEXCEPT
{
    // The hash is of the aad and ciphertext, each zero padded, followed by
    // their lengths in bits as 64-bit big-endian words.
    block lengths{};
    to_big<0>(lengths, to_bits<uint64_t>(aad.size()));
    to_big<8>(lengths, to_bits<uint64_t>(cipher.size()));

    aes::ghash<> hash{ key_ };
    hash.write(aad);
    hash.write(cipher);
    hash.write(lengths);
    out = hash.flush();

    // The tag is the hash masked by the cipher of the initial counter.
    auto mask = counter(iv, 1_u32);
    Aes::encrypt(mask, schedule_);
    for (size_t byte{}; byte < expansion; ++byte)
        out[byte] = bit_xor(out[byte], mask[byte]);
}

TEMPLATE
void CLASS::
encrypt(const_byte_span plain, const_byte_span aad, const nonce& iv,
    byte_span cipher) NOEXCEPT
{
    encrypt(plain, {}, aad, iv, cipher);
}

TEMPLATE
void CLASS::
encrypt(const_byte_span plain1, const_byte_span plain2,
    const_byte_span aad, const nonce& iv, byte_span cipher) NOEXCEPT
{
    BC_ASSERT(cipher.size() == plain1.size() + plain2.size() + expansion);
    const auto text = cipher.first(plain1.size() + plain2.size());

    // Segments are concatenated in place so the keystream is continuous.
    const auto second = std::next(text.begin(), plain1.size());
    std::copy(plain1.begin(), plain1.end(), text.begin());
    std::copy(plain2.begin(), plain2.end(), second);

    // Encryption uses the block counter starting at two.
    auto next = counter(iv, 2_u32);
    Aes::ctr(text, text, next, schedule_);

    authenticate(unsafe_array_cast<uint8_t, expansion>(
        cipher.last(expansion).data()), aad, text, iv);
}

TEMPLATE
bool CLASS::
decrypt(byte_span plain, const_byte_span aad, const nonce& iv,
    const_byte_span cipher) NOEXCEPT
{
    BC_ASSERT(cipher.size() == plain.size() + expansion);
    const auto text = cipher.first(plain.size());

    // Verify tag in constant time.
    block expected{};
    authenticate(expected, aad, text, iv);

    const auto tag = cipher.last(expansion);
    const auto authenticated = constant_time_equal(expected, tag);

    // Decryption uses the block counter starting at two.
    if (authenticated)
    {
        auto next = counter(iv, 2_u32);
        Aes::ctr(plain, text, next, schedule_);
    }
    else
    {
        std::fill(plain.begin(), plain.end(), 0x00_u8);
    }

    return authenticated;
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin

#endif
