/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/math/ring_signature.hpp>

#include <algorithm>
#include <map>
#include <numeric>
#include <vector>
#include <secp256k1.h>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>
#include <bitcoin/bitcoin/wallet/hd_private.hpp>

namespace libbitcoin {

typedef std::vector<uint32_t> index_list;
typedef std::map<ec_compressed, ec_secret> secret_keys_map;

inline bool is_null(const ec_secret& scalar)
{
    return scalar == null_hash;
}

static ec_secret borromean_hash(const hash_digest& M, data_slice R, uint32_t i,
    uint32_t j)
{
    // e = H(M || R || i || j)
    data_chunk data(hash_size + R.size() + sizeof(uint32_t) + sizeof(uint32_t));
    auto serial = make_unsafe_serializer(data.begin());
    serial.write_bytes(R);
    serial.write_hash(M);
    serial.write_4_bytes_big_endian(i);
    serial.write_4_bytes_big_endian(j);
    return sha256_hash(data);
}

// Take a list of secret keys and generate a mapping from public key -> secret.
static bool generate_keys_map(secret_keys_map& out, const secret_list& secrets)
{
    for (const auto& secret: secrets)
    {
        ec_compressed public_key;
        if (!secret_to_public(public_key, secret))
            return false;

        out.insert({ public_key, secret });
    }

    return true;
}

// Make a list of public keys for which we have the corresponding secret key
// in a single ring of public keys.
static const point_list known_keys_in_ring(const secret_keys_map& secret_keys,
    const point_list& ring)
{
    point_list known_ring;

    for (const auto& key: ring)
        if (secret_keys.find(key) != secret_keys.end())
            known_ring.push_back(key);

    return known_ring;
}

// For all rings, make a list of known public keys corresponding to each ring.
static key_rings partition_keys_into_rings(const secret_keys_map& secret_keys,
    const key_rings& rings)
{
    key_rings known_keys;
    for (const auto& ring: rings)
        known_keys.push_back(known_keys_in_ring(secret_keys, ring));

    return known_keys;
}

// Make a list of indexes of where our known key occurs in each ring of
// public keys. That is given a ring of {A, B, C} where we know the
// private key of B, it will return 1 (the index in the ring).
// This function computes this for all rings.
static bool create_key_indexes(index_list& out, const key_rings& rings,
    const key_rings& known_keys_by_ring)
{
    BITCOIN_ASSERT(known_keys_by_ring.size() == rings.size());

    for (uint32_t i = 0; i < rings.size(); ++i)
    {
        const auto& ring = rings[i];
        const auto& known = known_keys_by_ring[i];
        const auto it = std::find(ring.begin(), ring.end(), known.back());

        if (it == ring.end())
            return false;

        // The cast is safe because indexes cannot exceed max_uint32.
        const auto index = std::distance(ring.begin(), it);
        out.push_back(static_cast<uint32_t>(index));
    }

    return true;
}

static bool generate_known_indexes(index_list& out, const key_rings& rings,
    const secret_keys_map& secret_keys)
{
    auto known_keys_by_ring = partition_keys_into_rings(secret_keys, rings);
    BITCOIN_ASSERT(known_keys_by_ring.size() == rings.size());

    const auto empty = [](const point_list& ring)
    {
        return ring.empty();
    };

    // Check there is a known secret key in each ring.
    const auto has_empty = std::any_of(known_keys_by_ring.begin(),
        known_keys_by_ring.end(), empty);

    // Compute indexes for known keys inside the rings.
    return !has_empty && create_key_indexes(out, rings, known_keys_by_ring);
}

static bool calculate_R(ec_compressed& out, const ec_secret& s,
    const ec_secret& e, const ec_compressed& ring_key)
{
    auto eP = ring_key;

    ec_compressed sG;
    if (!secret_to_public(sG, s))
        return false;

    // R = s G + e P
    return ec_multiply(eP, e) && ec_sum(out, { sG, eP });
}

static bool calculate_s(ec_secret& out, const ec_secret& k, const ec_secret& e,
    const ec_secret& secret)

{
    out = e;

    // s = k - e * x
    return ec_multiply(out, secret) && ec_negate(out) && ec_add(out, k);
}

static bool calculate_last_R_signing(ec_compressed& R_i_j,
    const point_list& ring, uint32_t i, const hash_digest& digest,
    const ring_signature& signature, const uint32_t known_key_index,
    const secret_list& salts)
{
    if (!secret_to_public(R_i_j, salts[i]))
        return false;

    // Start one above index of known key and loop until the end.
    for (uint32_t j = known_key_index + 1; j < ring.size(); ++j)
    {
        BITCOIN_ASSERT(j < signature.proofs[i].size());
        const auto& s = signature.proofs[i][j];

        // Calculate e and R until the end of this ring.
        const auto e_i_j = borromean_hash(digest, R_i_j, i, j);
        if (is_null(e_i_j))
            return false;

        if (!calculate_R(R_i_j, s, e_i_j, ring[j]))
            return false;
    }
    return true;
}

static bool calculate_e0(ring_signature& out, const key_rings& rings,
    const hash_digest& digest, const secret_list& salts,
    const index_list& known_key_indexes)
{
    data_chunk e0_data;
    e0_data.reserve(ec_compressed_size * rings.size() + hash_size);
    BITCOIN_ASSERT(known_key_indexes.size() == rings.size());
    BITCOIN_ASSERT(out.proofs.size() == rings.size());
    BITCOIN_ASSERT(salts.size() == rings.size());

    for (uint32_t i = 0; i < rings.size(); ++i)
    {
        const auto& ring = rings[i];
        const auto known_key_index = known_key_indexes[i];

        ec_compressed last_R;
        if (!calculate_last_R_signing(last_R, ring, i, digest, out,
            known_key_index, salts))
            return false;

        extend_data(e0_data, last_R);
    }

    extend_data(e0_data, digest);
    out.challenge = sha256_hash(e0_data);
    return true;
}

static bool calculate_e_at_known_key_index(ec_secret& e_i_j,
    const ring_signature& signature, const point_list& ring,
    const hash_digest& digest, const uint32_t i,
    const uint32_t known_key_index)
{
    BITCOIN_ASSERT(signature.proofs[i].size() > known_key_index);
    BITCOIN_ASSERT(ring.size() > known_key_index);

    // Calculate e and R until the index is reached.
    for (uint32_t j = 0; j < known_key_index; ++j)
    {
        ec_compressed R_i_j;
        if (!calculate_R(R_i_j, signature.proofs[i][j], e_i_j, ring[j]))
            return false;

        e_i_j = borromean_hash(digest, R_i_j, i, j + 1);
        if (is_null(e_i_j))
            return false;
    }

    return true;
}

static bool join_rings(ring_signature& out, const key_rings& rings,
    const hash_digest& digest, const secret_list& salts,
    const index_list& known_key_indexes, secret_keys_map& secret_keys)
{
    for (uint32_t i = 0; i < rings.size(); ++i)
    {
        const auto& ring = rings[i];
        const auto known_key_index = known_key_indexes[i];

        // Calculate starting e value of this current ring.
        auto e_i_j = borromean_hash(digest, out.challenge, i, 0);
        if (is_null(e_i_j))
            return false;

        if (!calculate_e_at_known_key_index(e_i_j, out, ring, digest, i,
            known_key_index))
            return false;

        // Find secret key used for calculation in the next step.
        BITCOIN_ASSERT(known_key_index < ring.size());
        const auto& known_public_key = ring[known_key_index];

        BITCOIN_ASSERT(secret_keys.find(known_public_key) != secret_keys.end());
        const auto& secret = secret_keys[known_public_key];

        // Close the ring using this calculation: s = k - e x.
        auto& s = out.proofs[i][known_key_index];
        if (!calculate_s(s, salts[i], e_i_j, secret) || is_null(s))
            return false;
    }

    return true;
}

static bool calculate_last_R_verify(ec_compressed& R_i_j,
    const point_list& ring, ec_secret e_i_j, uint32_t i,
    const hash_digest& digest, const ring_signature& signature)
{
    BITCOIN_ASSERT(signature.proofs[i].size() == ring.size());

    for (uint32_t j = 0; j < ring.size(); ++j)
    {
        const auto& s = signature.proofs[i][j];

        if (is_null(s) || is_null(e_i_j))
            return false;

        if (!calculate_R(R_i_j, s, e_i_j, ring[j]))
            return false;

        e_i_j = borromean_hash(digest, R_i_j, i, j + 1);
    }

    return true;
}

// API
//-----------------------------------------------------------------------------

hash_digest digest(data_slice message, const key_rings& rings)
{
    const auto sum = [](size_t size, const point_list& ring)
    {
        return size + ring.size() * ec_compressed_size;
    };

    const auto size = std::accumulate(rings.begin(), rings.end(),
        message.size(), sum);

    data_chunk data;
    data.reserve(size);
    extend_data(data, message);
    for (const auto& ring: rings)
        for (const auto& key: ring)
            extend_data(data, key);

    return sha256_hash(data);
}

bool sign(ring_signature& out, const secret_list& secrets,
    const key_rings& rings, const hash_digest& digest,
    const secret_list& salts)
{
    // Guard against overflow.
    if (rings.size() >= max_uint32)
        return false;

    secret_keys_map secret_keys;
    if (!generate_keys_map(secret_keys, secrets))
        return false;

    index_list known_key_indexes;
    if (!generate_known_indexes(known_key_indexes, rings, secret_keys))
        return false;

    if (!calculate_e0(out, rings, digest, salts, known_key_indexes))
        return false;

    // Join each ring at the index where the secret key is known.
    return join_rings(out, rings, digest, salts, known_key_indexes,
        secret_keys);
}

// As compared with signing, we only have to perform a single step.
// The ring has already been computed, so now we just need to verify
// it by calculating e0, and looping all the way until the end R value
// which we use to recalculate e0.
// If the values match then we have a valid ring signature.
bool verify(const key_rings& rings, const hash_digest& digest,
    const ring_signature& signature)
{
    // Guard against overflow.
    if (rings.size() >= max_uint32)
        return false;

    data_chunk e0_data;
    e0_data.reserve(ec_compressed_size * rings.size() + hash_size);
    BITCOIN_ASSERT(signature.proofs.size() == rings.size());

    for (uint32_t i = 0; i < rings.size(); ++i)
    {
        // Calculate first e value for this ring.
        const auto e_i_0 = borromean_hash(digest, signature.challenge, i, 0);

        // Calculate the last R value.
        ec_compressed last_R;
        if (!calculate_last_R_verify(last_R, rings[i], e_i_0, i, digest,
            signature))
            return false;

        // Add this ring to e0.
        extend_data(e0_data, last_R);
    }

    extend_data(e0_data, digest);

    // Hash data to produce e0 value.
    const auto e0_hash = sha256_hash(e0_data);

    // Verification step.
    return e0_hash == signature.challenge;
}

} // namespace libbitcoin
