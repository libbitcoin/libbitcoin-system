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
#include <bitcoin/system/crypto/fschacha20_poly1305.hpp>

#include <bitcoin/system/crypto/chacha20.hpp>
#include <bitcoin/system/crypto/chacha20_poly1305.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

// based on github.com/bitcoin/bips/blob/master/bip-0324.mediawiki

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_USE_OF_SPAN)

fschacha20_poly1305::fschacha20_poly1305(const chacha20::secret& key,
    uint32_t interval) NOEXCEPT
  : interval_(interval), aead_(key)
{
}

// private
void fschacha20_poly1305::next() NOEXCEPT
{
    // The key is rotated after every rekey_interval messages, to the
    // keystream of nonce { 0xffffffff, rekey counter }.
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

void fschacha20_poly1305::encrypt(const_byte_span plain, const_byte_span aad,
    byte_span cipher) NOEXCEPT
{
    encrypt(plain, {}, aad, cipher);
}

void fschacha20_poly1305::encrypt(const_byte_span plain1,
    const_byte_span plain2, const_byte_span aad, byte_span cipher) NOEXCEPT
{
    // The nonce is { message counter, rekey counter }.
    aead_.encrypt(plain1, plain2, aad, packets_, rekeys_, cipher);
    next();
}

bool fschacha20_poly1305::decrypt(byte_span plain, const_byte_span aad,
    const_byte_span cipher) NOEXCEPT
{
    const auto valid = aead_.decrypt(plain, aad, packets_, rekeys_, cipher);
    next();
    return valid;
}

BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
