/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_EC_SERIALIZE_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_EC_SERIALIZE_HPP

#include <bitcoin/system/crypto/secp256k1.hpp>

#include <secp256k1.h>
#include <bitcoin/system/data/data.hpp>
#include "ec_context.hpp"

namespace libbitcoin {
namespace system {

// secp256k1_ec_pubkey_parse supports compressed (33 bytes, header byte 0x02
// or 0x03), uncompressed (65 bytes, header byte 0x04), or hybrid (65 bytes,
// header byte 0x06 or 0x07) format public keys.
inline bool ec_public_key_parse(const secp256k1_context* context,
    secp256k1_pubkey& out, const data_slice& point) NOEXCEPT
{
    static constexpr auto ec_success = 1;
    return !point.empty() && secp256k1_ec_pubkey_parse(context, &out,
        point.data(), point.size()) == ec_success;
}

template <size_t Size>
inline bool ec_public_key_serialize(const secp256k1_context* context,
    data_array<Size>& out, const secp256k1_pubkey point) NOEXCEPT
{
    constexpr auto flags = (Size == ec_compressed_size ?
        SECP256K1_EC_COMPRESSED : SECP256K1_EC_UNCOMPRESSED);

    auto size = Size;
    secp256k1_ec_pubkey_serialize(context, out.data(), &size, &point, flags);
    return size == Size;
}

} // namespace system
} // namespace libbitcoin

#endif
