/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_HMAC_SHA256_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_HMAC_SHA256_HPP

#include <bitcoin/system/crypto/sha256.hpp>
#include <bitcoin/system/crypto/sha256_context.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace hmac_sha256 {

struct hmac_sha256_context
{
    sha256::context in;
    sha256::context out;
};

void hash(const uint8_t* data, size_t size, const uint8_t* key,
    size_t key_size, uint8_t* digest) NOEXCEPT;

void initialize(hmac_sha256_context& context, const uint8_t* key,
    size_t size) NOEXCEPT;

void update(hmac_sha256_context& context, const uint8_t* data,
    size_t size) NOEXCEPT;

void finalize(hmac_sha256_context& context, uint8_t* digest) NOEXCEPT;

} // namespace hmac_sha256
} // namespace system
} // namespace libbitcoin

#endif
