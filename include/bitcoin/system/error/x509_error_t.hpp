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
#ifndef LIBBITCOIN_SYSTEM_ERROR_X509_ERROR_T_HPP
#define LIBBITCOIN_SYSTEM_ERROR_X509_ERROR_T_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/macros.hpp>

namespace libbitcoin {
namespace system {
namespace error {

enum x509_error_t : uint8_t
{
    x509_success = 0,
    chain_empty,
    chain_untrusted,
    certificate_not_yet_valid,
    certificate_expired,
    issuer_mismatch,
    certificate_signature,
    issuer_not_authority,
    path_length_exceeded,
    key_usage_invalid,
    extended_key_usage_invalid
};

DECLARE_ERROR_T_CODE_CATEGORY(x509_error);

} // namespace error
} // namespace system
} // namespace libbitcoin

DECLARE_STD_ERROR_REGISTRATION(bc::system::error::x509_error)

#endif
