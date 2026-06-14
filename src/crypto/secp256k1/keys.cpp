/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/crypto/secp256k1.hpp>

#include <secp256k1.h>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include "ec_context.hpp"
#include "serialize.hpp"

namespace libbitcoin {
namespace system {

// local
// ----------------------------------------------------------------------------

static constexpr auto ec_success = 1;

template <size_t Size>
static bool secret_to_public(const secp256k1_context* context,
    data_array<Size>& out, const ec_secret& secret) NOEXCEPT
{
    secp256k1_pubkey pubkey;
    return secp256k1_ec_pubkey_create(context, &pubkey, secret.data()) ==
        ec_success && ec_public_key_serialize(context, out, pubkey);
}

// Convert keys
// ----------------------------------------------------------------------------

bool compress(ec_compressed& out, const ec_uncompressed& point) NOEXCEPT
{
    secp256k1_pubkey pubkey;
    const auto context = ec_context_verify::context();
    return ec_public_key_parse(context, pubkey, point) &&
        ec_public_key_serialize(context, out, pubkey);
}

bool decompress(ec_uncompressed& out, const ec_compressed& point) NOEXCEPT
{
    secp256k1_pubkey pubkey;
    const auto context = ec_context_verify::context();
    return ec_public_key_parse(context, pubkey, point) &&
        ec_public_key_serialize(context, out, pubkey);
}

bool secret_to_public(ec_compressed& out, const ec_secret& secret) NOEXCEPT
{
    const auto context = ec_context_sign::context();
    return secret_to_public(context, out, secret);
}

bool secret_to_public(ec_uncompressed& out, const ec_secret& secret) NOEXCEPT
{
    const auto context = ec_context_sign::context();
    return secret_to_public(context, out, secret);
}

// Verify keys
// ----------------------------------------------------------------------------

bool verify_secret(const ec_secret& secret) NOEXCEPT
{
    const auto context = ec_context_verify::context();
    return secp256k1_ec_seckey_verify(context, secret.data()) == ec_success;
}

bool verify_point(const data_slice& point) NOEXCEPT
{
    secp256k1_pubkey pubkey;
    const auto context = ec_context_verify::context();
    return ec_public_key_parse(context, pubkey, point);
}

// Detect public keys
// ----------------------------------------------------------------------------

bool is_even_key(const ec_compressed& point) NOEXCEPT
{
    return point.front() == ec_even_sign;
}

bool is_compressed_key(const data_slice& point) NOEXCEPT
{
    const auto size = point.size();
    if (size != ec_compressed_size)
        return false;

    const auto first = point.front();
    return first == ec_even_sign || first == ec_odd_sign;
}

bool is_uncompressed_key(const data_slice& point) NOEXCEPT
{
    const auto size = point.size();
    if (size != ec_uncompressed_size)
        return false;

    const auto first = point.front();
    return first == ec_uncompressed_sign;
}

bool is_hybrid_key(const data_slice& point) NOEXCEPT
{
    const auto size = point.size();
    if (size != ec_uncompressed_size)
        return false;

    const auto first = point.front();
    return first == ec_hybrid_even_sign || first == ec_hybrid_odd_sign;
}

bool is_public_key(const data_slice& point) NOEXCEPT
{
    return is_compressed_key(point) || is_uncompressed_key(point);
}

bool is_endorsement(const endorsement& endorsement) NOEXCEPT
{
    const auto size = endorsement.size();
    return size >= min_endorsement_size && size <= max_endorsement_size;
}

} // namespace system
} // namespace libbitcoin
