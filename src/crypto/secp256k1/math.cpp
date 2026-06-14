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

#include <algorithm>
#include <secp256k1.h>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/math/math.hpp>
#include "ec_context.hpp"
#include "serialize.hpp"

namespace libbitcoin {
namespace system {

// local
// ----------------------------------------------------------------------------

static constexpr auto ec_success = 1;

// Create an array of secp256k1_pubkey pointers for secp256k1 call.
static std::vector<const secp256k1_pubkey*> to_secp256k1_pubkey_pointers(
    const std::vector<secp256k1_pubkey>& keys) NOEXCEPT
{
    std::vector<const secp256k1_pubkey*> pointers(keys.size());
    std::transform(keys.begin(), keys.end(), pointers.begin(),
        [](const secp256k1_pubkey& point) NOEXCEPT
        {
            return &point;
        });

    return pointers;
}

static bool ec_public_keys_parse(const secp256k1_context* context,
    std::vector<secp256k1_pubkey>& out, const ec_compresseds& points) NOEXCEPT
{
    out.resize(points.size());
    auto key = out.begin();

    for (const auto& point: points)
        if (!ec_public_key_parse(context, *key++, point))
            return false;

    return true;
}

template <size_t Size>
static bool ec_add(const secp256k1_context* context, data_array<Size>& in_out,
    const ec_secret& secret) NOEXCEPT
{
    secp256k1_pubkey pubkey;
    return ec_public_key_parse(context, pubkey, in_out) &&
        secp256k1_ec_pubkey_tweak_add(context, &pubkey, secret.data()) ==
        ec_success && ec_public_key_serialize(context, in_out, pubkey);
}

template <size_t Size>
static bool ec_multiply(const secp256k1_context* context,
    data_array<Size>& in_out, const ec_secret& secret) NOEXCEPT
{
    secp256k1_pubkey pubkey;
    return ec_public_key_parse(context, pubkey, in_out) &&
        secp256k1_ec_pubkey_tweak_mul(context, &pubkey, secret.data()) ==
        ec_success && ec_public_key_serialize(context, in_out, pubkey);
}

template <size_t Size>
static bool ec_negate(const secp256k1_context* context,
    data_array<Size>& in_out) NOEXCEPT
{
    secp256k1_pubkey pubkey;
    return ec_public_key_parse(context, pubkey, in_out) &&
        secp256k1_ec_pubkey_negate(context, &pubkey) == ec_success &&
        ec_public_key_serialize(context, in_out, pubkey);
}

// Add EC values
// ----------------------------------------------------------------------------

bool ec_add(ec_compressed& point, const ec_secret& scalar) NOEXCEPT
{
    const auto context = ec_context_verify::context();
    return ec_add(context, point, scalar);
}

bool ec_add(ec_uncompressed& point, const ec_secret& scalar) NOEXCEPT
{
    const auto context = ec_context_verify::context();
    return ec_add(context, point, scalar);
}

// secrets are normal
bool ec_add(ec_secret& left, const ec_secret& right) NOEXCEPT
{
    const auto context = ec_context_verify::context();
    return secp256k1_ec_seckey_tweak_add(context, left.data(), right.data())
        == ec_success;
}

bool ec_add(ec_compressed& left, const ec_compressed& right) NOEXCEPT
{
    return ec_sum(left, { left, right });
}

bool ec_add(ec_compressed& left, const ec_uncompressed& right) NOEXCEPT
{
    ec_compressed out;
    return compress(out, right) && ec_add(left, out);
}

bool ec_sum(ec_compressed& out, const ec_compresseds& points) NOEXCEPT
{
    if (points.empty())
        return false;

    const auto context = ec_context_verify::context();

    std::vector<secp256k1_pubkey> keys;
    if (!ec_public_keys_parse(context, keys, points))
        return false;

    secp256k1_pubkey pubkey;
    return secp256k1_ec_pubkey_combine(context, &pubkey,
        to_secp256k1_pubkey_pointers(keys).data(), points.size()) ==
            ec_success && ec_public_key_serialize(context, out, pubkey);
}

// Multiply EC values
// ----------------------------------------------------------------------------

bool ec_multiply(ec_compressed& point, const ec_secret& scalar) NOEXCEPT
{
    const auto context = ec_context_verify::context();
    return ec_multiply(context, point, scalar);
}

bool ec_multiply(ec_uncompressed& point, const ec_secret& scalar) NOEXCEPT
{
    const auto context = ec_context_verify::context();
    return ec_multiply(context, point, scalar);
}

bool ec_multiply(ec_secret& left, const ec_secret& right) NOEXCEPT
{
    const auto context = ec_context_verify::context();
    return secp256k1_ec_seckey_tweak_mul(context, left.data(), right.data()) ==
        ec_success;
}

// Negate EC values
// ----------------------------------------------------------------------------

bool ec_negate(ec_secret& scalar) NOEXCEPT
{
    const auto context = ec_context_verify::context();
    return secp256k1_ec_seckey_negate(context, scalar.data()) == ec_success;
}

bool ec_negate(ec_compressed& point) NOEXCEPT
{
    const auto context = ec_context_verify::context();
    return ec_negate(context, point);
}

bool ec_negate(ec_uncompressed& point) NOEXCEPT
{
    const auto context = ec_context_verify::context();
    return ec_negate(context, point);
}

} // namespace system
} // namespace libbitcoin
