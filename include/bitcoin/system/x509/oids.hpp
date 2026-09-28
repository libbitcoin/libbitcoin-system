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
#ifndef LIBBITCOIN_SYSTEM_X509_OIDS_HPP
#define LIBBITCOIN_SYSTEM_X509_OIDS_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace x509 {
namespace oid {

/// Object identifier contents (without tag and length).

/// Keys and curves (rfc5480).
constexpr data_array<7> ec_public_key{ 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02, 0x01 };
constexpr data_array<8> secp256r1{ 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07 };
constexpr data_array<5> secp384r1{ 0x2b, 0x81, 0x04, 0x00, 0x22 };

/// Signatures (rfc5758).
constexpr data_array<8> ecdsa_sha256{ 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x02 };
constexpr data_array<8> ecdsa_sha384{ 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x04, 0x03, 0x03 };

/// Attributes and extensions (rfc5280).
constexpr data_array<3> common_name{ 0x55, 0x04, 0x03 };
constexpr data_array<3> key_usage{ 0x55, 0x1d, 0x0f };
constexpr data_array<3> subject_alternative_name{ 0x55, 0x1d, 0x11 };
constexpr data_array<3> basic_constraints{ 0x55, 0x1d, 0x13 };
constexpr data_array<3> extended_key_usage{ 0x55, 0x1d, 0x25 };
constexpr data_array<4> any_extended_key_usage{ 0x55, 0x1d, 0x25, 0x00 };
constexpr data_array<8> server_authentication{ 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x03, 0x01 };
constexpr data_array<8> client_authentication{ 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07, 0x03, 0x02 };

/// Password based encryption (rfc8018, rfc3565).
constexpr data_array<9> pbes2{ 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x05, 0x0d };
constexpr data_array<9> pbkdf2{ 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x05, 0x0c };
constexpr data_array<8> hmac_sha256{ 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x02, 0x09 };
constexpr data_array<9> aes128_cbc{ 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x01, 0x02 };
constexpr data_array<9> aes256_cbc{ 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x01, 0x2a };

} // namespace oid
} // namespace x509
} // namespace system
} // namespace libbitcoin

#endif
