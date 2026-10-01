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
#include "ec_context.hpp"

#include <bitcoin/system/crypto/secp256k1.hpp>
#include <bitcoin/system/define.hpp>

#if defined(HAVE_ULTRAFAST)
    #include <ufsecp/ufsecp_version.h>
#endif

#define EC_TEXT(value) #value
#define EC_STRING(value) EC_TEXT(value)

namespace libbitcoin {
namespace system {

// Context.
// ----------------------------------------------------------------------------

ec_context::ec_context(int flags) NOEXCEPT
  : context_(secp256k1_context_create(flags))
{
    BC_ASSERT(!is_null(context_));
}

ec_context::~ec_context() NOEXCEPT
{
    if (!is_null(context_))
        secp256k1_context_destroy(context_);
}

const secp256k1_context* ec_context::get() const NOEXCEPT
{
    return context_;
}

// Singletons.
// ----------------------------------------------------------------------------

const secp256k1_context* ec_context_sign::context() NOEXCEPT
{
    static const ec_context instance{ SECP256K1_CONTEXT_SIGN };
    return instance.get();
}

const secp256k1_context* ec_context_verify::context() NOEXCEPT
{
    static const ec_context instance{ SECP256K1_CONTEXT_VERIFY };
    return instance.get();
}

// Dependency identifier.
// ----------------------------------------------------------------------------

std::string secp256k1_library() NOEXCEPT
{
#if defined(HAVE_ULTRAFAST)
    return std::string{ "ultrafast " } + ufsecp_version_string();
#elif defined(HAVE_SECP256K1) && defined(SECP256K1_VERSION)
    return "libsecp256k1 " EC_STRING(SECP256K1_VERSION);
#elif defined(HAVE_SECP256K1)
    return "libsecp256k1";
#else
    return "internal";
#endif
}

} // namespace system
} // namespace libbitcoin
