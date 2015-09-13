/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/wallet/key_formats.hpp>

#include <cstdint>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/formats/base58.hpp>
#include <bitcoin/bitcoin/math/checksum.hpp>
#include <bitcoin/bitcoin/math/ec_keys.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/endian.hpp>
#include <bitcoin/bitcoin/wallet/payment_address.hpp>

namespace libbitcoin {
namespace wallet {

std::string secret_to_wif(const ec_secret& secret, bool compressed)
{
    auto version = to_array(payment_address::wif_version);
    data_chunk data;

    if (compressed)
        data = build_chunk({ version, secret, to_array(0x01) }, checksum_size);
    else
        data = build_chunk({version, secret}, checksum_size);

    append_checksum(data);
    return encode_base58(data);
}

ec_secret wif_to_secret(const std::string& wif)
{
    data_chunk decoded;
    if (!decode_base58(decoded, wif))
        return ec_secret();
    // 1 marker, 32 byte secret, optional 1 compressed flag, 4 checksum bytes
    if (decoded.size() != 1 + hash_size + 4 &&
        decoded.size() != 1 + hash_size + 1 + 4)
        return ec_secret();
    if (!verify_checksum(decoded))
        return ec_secret();
    // Check first byte is valid
    if (decoded[0] != payment_address::wif_version)
        return ec_secret();

    // Checks passed. Drop the 0x80 start byte and checksum.
    decoded.erase(decoded.begin());
    decoded.erase(decoded.end() - 4, decoded.end());
    // If length is still 33 and last byte is 0x01, drop it.
    if (decoded.size() == 33 && decoded[32] == (uint8_t)0x01)
        decoded.erase(decoded.begin()+32);
    ec_secret secret;
    BITCOIN_ASSERT(secret.size() == decoded.size());
    std::copy(decoded.begin(), decoded.end(), secret.begin());
    return secret;
}

bool is_wif_compressed(const std::string& wif)
{
    data_chunk decoded;
    if (!decode_base58(decoded, wif))
        return false;
    return decoded.size() == (1 + hash_size + 1 + 4) &&
        decoded[33] == (uint8_t)0x01;
}

bool check_minikey(const std::string& minikey)
{
    // Legacy minikeys are 22 chars long
    if (minikey.size() != 22 && minikey.size() != 30)
        return false;
    return sha256_hash(to_chunk(minikey + "?"))[0] == 0x00;
}

ec_secret minikey_to_secret(const std::string& minikey)
{
    if (!check_minikey(minikey))
        return ec_secret();
    return sha256_hash(to_chunk(minikey));
}

} // namespace wallet
} // namespace libbitcoin
