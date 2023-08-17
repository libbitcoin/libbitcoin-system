/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_EC_CONTEXT_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_EC_CONTEXT_HPP

#include <secp256k1.h>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Base class for secp256k1 singleton context management.
class BC_API ec_context
{
public:
    /// Deletes.
    ec_context(ec_context&&) = delete;
    ec_context(const ec_context&) = delete;
    ec_context& operator=(ec_context&&) = delete;
    ec_context& operator=(const ec_context&) = delete;

    /// Free the context if successfully initialized.
    virtual ~ec_context() NOEXCEPT;

protected:
    ec_context(int flags) NOEXCEPT;

    // This unpublished header hides this external symbol.
    secp256k1_context* context_;
};

/// A signing context singleton initializer.
class BC_API ec_context_sign
  : public ec_context
{
public:
    static const secp256k1_context* context() NOEXCEPT;

protected:
    ec_context_sign() NOEXCEPT;
};

/// A verification context singleton initializer.
class BC_API ec_context_verify
  : public ec_context
{
public:
    static const secp256k1_context* context() NOEXCEPT;

protected:
    ec_context_verify() NOEXCEPT;
};

} // namespace system
} // namespace libbitcoin

#endif
