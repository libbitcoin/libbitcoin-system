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
#ifndef LIBBITCOIN_SYSTEM_X509_VERIFY_HPP
#define LIBBITCOIN_SYSTEM_X509_VERIFY_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/x509/certificate.hpp>

namespace libbitcoin {
namespace system {
namespace x509 {

/// The purpose for which the leaf certificate is verified.
enum class purpose : uint8_t
{
    server,
    client
};

/// Verify the chain (leaf first, then issuers in order) to a configured
/// anchor at the time (unix seconds). An anchor is trusted as an issuer, and
/// a leaf that is itself an anchor is trusted (pinned). There are no roots.
BC_API code verify(const certificates& chain, const certificates& anchors,
    uint64_t time, purpose intent) NOEXCEPT;

} // namespace x509
} // namespace system
} // namespace libbitcoin

#endif
