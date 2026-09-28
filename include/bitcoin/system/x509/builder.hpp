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
#ifndef LIBBITCOIN_SYSTEM_X509_BUILDER_HPP
#define LIBBITCOIN_SYSTEM_X509_BUILDER_HPP

#include <string>
#include <vector>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/x509/private_key.hpp>

namespace libbitcoin {
namespace system {
namespace x509 {

/// Certificate subject, serial and validity (unix seconds).
struct subject
{
    std::string common_name{};
    std::vector<std::string> dns_names{};
    std::vector<data_chunk> ip_addresses{};
    data_array<16> serial{};
    uint64_t not_before{};
    uint64_t not_after{};
};

/// Build a self-signed v3 ecdsa-with-SHA256 certificate for the key, with
/// critical basic constraints (not an authority) and key usage (digital
/// signature), server and client authentication, and alternative names.
/// False if the key is invalid or the validity is empty.
BC_API bool build_self_signed(data_chunk& out, const secret& key,
    const subject& subject) NOEXCEPT;

/// Encode the certificate as a "CERTIFICATE" block.
BC_API std::string encode_certificate(const_byte_span der) NOEXCEPT;

} // namespace x509
} // namespace system
} // namespace libbitcoin

#endif
