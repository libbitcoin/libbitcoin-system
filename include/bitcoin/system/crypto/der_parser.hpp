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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_EXTERNAL_DER_PARSER_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_EXTERNAL_DER_PARSER_HPP

#include <bitcoin/system/crypto/secp256k1.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

/// Based on example code in bip66, but without signature hash byte.
/// https://github.com/bitcoin/bips/blob/master/bip-0066.mediawiki
bool is_valid_bip66_signature_encoding(
    const bc::system::data_slice& sig) NOEXCEPT;

/// Parse a signature in lax DER, as required for signatures before bip66.
/// https://github.com/bitcoin/bips/blob/master/bip-0066.mediawiki#motivation
/// Based on https://github.com/bitcoin-core/secp256k1/blob/master/contrib/lax_der_parsing.c
bool ecdsa_signature_parse_der_lax(bc::system::ec_signature& sig,
    const uint8_t* input, size_t inputlen) NOEXCEPT;

#endif // LIBBITCOIN_SYSTEM_CRYPTO_EXTERNAL_DER_PARSER_HPP
