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
#include <bitcoin/system/crypto/xsalsa20_poly1305.hpp>

#include <bitcoin/system/crypto/poly1305.hpp>
#include <bitcoin/system/crypto/salsa20.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

// based on:
// nacl.cr.yp.to/secretbox.html (crypto_secretbox_xsalsa20poly1305)
// [Bernstein]

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_USE_OF_SPAN)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

xsalsa20_poly1305::xsalsa20_poly1305(const salsa20::secret& key) NOEXCEPT
  : key_(key)
{
}

void xsalsa20_poly1305::set_key(const salsa20::secret& key) NOEXCEPT
{
    key_ = key;
}

void xsalsa20_poly1305::encrypt(const_byte_span plain, const nonce& nonce,
    byte_span cipher) NOEXCEPT
{
    BC_ASSERT(cipher.size() == plain.size() + expansion);
    salsa20 stream{ key_, nonce };

    // The first 32 bytes of keystream are the one-time authentication key,
    // and the remainder of the keystream encrypts the message.
    poly1305::secret key{};
    stream.stream(key);
    const auto text = cipher.subspan(expansion);
    stream.crypt(plain, text);

    // The tag over the ciphertext is prepended.
    poly1305 mac{ key };
    mac.write(text);
    mac.flush(unsafe_array_cast<uint8_t, expansion>(cipher.data()));
    key = {};
}

bool xsalsa20_poly1305::decrypt(byte_span plain, const nonce& nonce,
    const_byte_span cipher) NOEXCEPT
{
    BC_ASSERT(cipher.size() == plain.size() + expansion);
    salsa20 stream{ key_, nonce };

    poly1305::secret key{};
    stream.stream(key);
    const auto text = cipher.subspan(expansion);

    // Verify the tag in constant time.
    poly1305::tag expected{};
    poly1305 mac{ key };
    mac.write(text);
    mac.flush(expected);
    key = {};

    uint8_t difference{};
    const auto actual = cipher.first(expansion);
    for (size_t byte{}; byte < expansion; ++byte)
        difference |= bit_xor(expected[byte], actual[byte]);

    const auto authenticated = is_zero(difference);
    if (authenticated)
        stream.crypt(text, plain);
    else
        std::fill(plain.begin(), plain.end(), 0x00_u8);

    return authenticated;
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
