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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_EC_CONTEXT_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SECP256K1_EC_CONTEXT_HPP

#include <bitcoin/system/define.hpp>
#if defined(HAVE_SECP256K1)
    #include <secp256k1.h>
    #include <secp256k1_ellswift.h>
    #include <secp256k1_recovery.h>
    #include <secp256k1_schnorrsig.h>
#else
    #include "interface.hpp"
#endif

namespace libbitcoin {
namespace system {

/// A secp256k1 context, destroyed with the instance.
class BC_API ec_context
{
public:
    DELETE_COPY_MOVE(ec_context);

    ec_context(int flags) NOEXCEPT;
    ~ec_context() NOEXCEPT;

    /// The context.
    const secp256k1_context* get() const NOEXCEPT;

private:
    // This unpublished header hides this external symbol.
    secp256k1_context* context_;
};

/// The signing context singleton.
struct BC_API ec_context_sign
{
    static const secp256k1_context* context() NOEXCEPT;
};

/// The verification context singleton.
struct BC_API ec_context_verify
{
    static const secp256k1_context* context() NOEXCEPT;
};

} // namespace system
} // namespace libbitcoin

#endif
