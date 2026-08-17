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
#include <bitcoin/system/crypto/fschacha20.hpp>

#include <bitcoin/system/crypto/chacha20.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

// based on github.com/bitcoin/bips/blob/master/bip-0324.mediawiki

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_USE_OF_SPAN)

fschacha20::fschacha20(const chacha20::secret& key, uint32_t interval) NOEXCEPT
  : cipher_(key), interval_(interval)
{
}

void fschacha20::crypt(const_byte_span in, byte_span out) NOEXCEPT
{
    cipher_.crypt(in, out);

    // The key is rotated after every rekey_interval chunks, to the next 32
    // keystream bytes, and the nonce set to { 0, rekey counter }.
    if (++chunks_ == interval_)
    {
        chacha20::secret key{};
        cipher_.stream(key);
        cipher_.set_key(key);
        cipher_.seek(0, ++rekeys_, 0);
        key = {};
        chunks_ = 0;
    }
}

BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
