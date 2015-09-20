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
#ifndef LIBBITCOIN_WIF_KEYS_HPP
#define LIBBITCOIN_WIF_KEYS_HPP

#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/ec_keys.hpp>

namespace libbitcoin {
namespace wallet {

static BC_CONSTEXPR uint8_t wif_mainnet = 0x80;

/**
 * Convert a secret parameter to the wallet import format.
 * The compressed flag indicates whether a payment address derived from the wif
 * secret should use the compressed derived public key.
 */
BC_API std::string encode_wif(const ec_secret& secret,
    uint8_t version=wif_mainnet, bool compressed=true);

/**
 * Convert wallet import format (wif) key to its parts.
 */
BC_API bool decode_wif(ec_secret& out_secret, uint8_t& out_version,
    bool& out_compressed, const std::string& wif);

/**
 * Returns true if the wif key is both valid and represents a compressed
 * payment address.
 */
BC_API bool is_wif_compressed(const std::string& wif);

} // namespace wallet
} // namespace libbitcoin

#endif
