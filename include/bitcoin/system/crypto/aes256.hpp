/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_AES256_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_AES256_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace aes256 {
    
/// This is an implementation of AES256.
/// NIST selected three members of the Rijndael family, each with a block
/// size of 128 bits, but three different key lengths: 128, 192 and 256 bits.

constexpr size_t block_size = bytes<128>;
typedef data_array<block_size> block;

constexpr size_t secret_size = bytes<256>;
typedef data_array<secret_size> secret;

/// Perform aes256 encryption/decryption on a data block.
void encrypt(block& bytes, const secret& key) NOEXCEPT;
void decrypt(block& bytes, const secret& key) NOEXCEPT;

} // namespace aes256
} // namespace system
} // namespace libbitcoin

#endif
