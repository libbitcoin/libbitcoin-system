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
#include <bitcoin/system/error/x509_error_t.hpp>

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/macros.hpp>

namespace libbitcoin {
namespace system {
namespace error {

DEFINE_ERROR_T_MESSAGE_MAP(x509_error)
{
    { x509_success, "x509 success" },
    { chain_empty, "certificate chain empty" },
    { chain_untrusted, "certificate chain not anchored" },
    { certificate_not_yet_valid, "certificate not yet valid" },
    { certificate_expired, "certificate expired" },
    { issuer_mismatch, "certificate issuer does not match" },
    { certificate_signature, "certificate signature invalid" },
    { issuer_not_authority, "certificate issuer not an authority" },
    { path_length_exceeded, "certificate path length exceeded" },
    { key_usage_invalid, "certificate key usage invalid" },
    { extended_key_usage_invalid, "certificate extended key usage invalid" }
};

DEFINE_ERROR_T_CATEGORY(x509_error, "x509", "x509 code")

} // namespace error
} // namespace system
} // namespace libbitcoin
