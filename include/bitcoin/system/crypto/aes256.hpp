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

/// Advanced Encryption Standard (AES) 256.
class BC_API aes256 final
{
public:
    /// AES block is always 128 bits.
    typedef data_array<bytes<128>> block;

    /// AES-256 secret is always 256 bits.
    typedef data_array<bytes<256>> secret;

    /// nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197-upd1.pdf
    static void encrypt_ecb(block& bytes, const secret& key) NOEXCEPT;
    static void decrypt_ecb(block& bytes, const secret& key) NOEXCEPT;

private:
    struct context
    {
        secret key;
        secret enckey;
        secret deckey;
    };

    static void initialize(context& context, const secret& key) NOEXCEPT;
    static void encrypt_ecb(context& context, block& bytes) NOEXCEPT;
    static void decrypt_ecb(context& context, block& bytes) NOEXCEPT;
};

} // namespace system
} // namespace libbitcoin

#endif
