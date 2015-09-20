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
#include <bitcoin/bitcoin/wallet/wif_keys.hpp>

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

static constexpr uint8_t wif_uncompressed_size = 1 + hash_size + checksum_size;
static constexpr uint8_t wif_compressed_size = wif_uncompressed_size + 1;
static constexpr uint8_t wif_compressed_sentinel = 0x01;

static bool is_compressed(const data_chunk& decoded)
{
    return decoded.size() == wif_compressed_size &&
        decoded[wif_compressed_size - checksum_size - 1] == wif_compressed_sentinel;
}

bool is_wif_compressed(const std::string& wif)
{
    data_chunk decoded;
    return decode_base58(decoded, wif) && verify_checksum(decoded) &&
        is_compressed(decoded);
}

std::string encode_wif(const ec_secret& secret, uint8_t version, bool compressed)
{
    const auto version_byte = to_array(version);
    const auto compressed_byte = to_array(wif_compressed_sentinel);
    auto data = compressed ?
        build_chunk({ version_byte, secret, compressed_byte }, checksum_size) :
        build_chunk({ version_byte, secret }, checksum_size);

    append_checksum(data);
    return encode_base58(data);
}

bool decode_wif(ec_secret& out_secret, uint8_t& out_version, bool& out_compressed,
    const std::string& wif)
{
    data_chunk decoded;
    if (!decode_base58(decoded, wif) || !verify_checksum(decoded))
        return false;

    if (decoded.size() != wif_compressed_size &&
        decoded.size() != wif_uncompressed_size)
        return false;

    out_version = decoded.front();
    out_compressed = is_compressed(decoded);
    const auto decoded_secret_end = decoded.begin() + 1 + ec_secret_size;
    std::copy(decoded.begin() + 1, decoded_secret_end, out_secret.begin());
    return true;
}

} // namespace wallet
} // namespace libbitcoin
