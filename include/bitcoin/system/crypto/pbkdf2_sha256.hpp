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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_PBKDF2_SHA256_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_PBKDF2_SHA256_HPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace pbkdf2 {

/// buffer_size <= [32 * (2^32 - 1)].
void sha256(const uint8_t* passphrase, size_t passphrase_size,
    const uint8_t* salt, size_t salt_size, uint64_t interations,
    uint8_t* buffer, size_t buffer_size) NOEXCEPT;

} // namespace sha256
} // namespace system
} // namespace libbitcoin

#endif
