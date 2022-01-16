/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_INITIALIZER_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_INITIALIZER_HPP

#include <secp256k1.h>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Base class for secp256k1 singleton context management.
class BC_API secp256k1_initializer
{
public:
    /// Free the context if successfully initialized.
    virtual ~secp256k1_initializer() noexcept;

protected:
    secp256k1_initializer(int flags) noexcept;

    secp256k1_context* context_;
};

/// A signing context singleton initializer.
class BC_API secp256k1_signing
  : public secp256k1_initializer
{
public:
    static secp256k1_context* context() noexcept;

protected:
    secp256k1_signing() noexcept;
};

/// A verification context singleton initializer.
class BC_API secp256k1_verification
  : public secp256k1_initializer
{
public:
    static secp256k1_context* context() noexcept;

protected:
    secp256k1_verification() noexcept;
};

} // namespace system
} // namespace libbitcoin

#endif
