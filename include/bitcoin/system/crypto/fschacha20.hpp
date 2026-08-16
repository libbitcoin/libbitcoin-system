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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_FSCHACHA20_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_FSCHACHA20_HPP

#include <bitcoin/system/crypto/chacha20.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Forward-secure ChaCha20 (bip324), rekeys every interval of crypt calls.
/// Each call encrypts or decrypts one chunk of a fixed-length chunk stream.
class BC_API fschacha20 final
{
public:
    DELETE_COPY_MOVE(fschacha20);

    fschacha20(const chacha20::secret& key, uint32_t interval) NOEXCEPT;

    /// XOR in with keystream into out (in/out may be the same).
    void crypt(const_byte_span in, byte_span out) NOEXCEPT;

private:
    chacha20 cipher_;
    const uint32_t interval_;
    uint32_t chunks_{};
    uint64_t rekeys_{};
};

} // namespace system
} // namespace libbitcoin

#endif
