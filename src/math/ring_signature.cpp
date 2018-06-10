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

#include <map>
#include <boost/optional.hpp>
#include <secp256k1.h>
#include <bitcoin/bitcoin/math/ec_arithmetic.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>
#include <bitcoin/bitcoin/wallet/hd_private.hpp>

namespace libbitcoin {

typedef std::map<ec_compressed, ec_secret> secret_keys_map;

typedef std::vector<size_t> index_list;

// Returns sha256(message || flatten(rings))
hash_digest prepare_digest(data_slice message, const key_rings& rings)
{
    // Calculate size of result
    size_t data_size = message.size();
    for (const auto& ring: rings)
        data_size += ring.size() * ec_compressed_size;
    // Build the actual result data
    data_chunk data;
    data.reserve(data_size);
    extend_data(data, message);
    for (const auto& ring: rings)
        for (const auto& key: ring)
            extend_data(data, key);
    return sha256_hash(data);
}

// Take a list of secret keys and generate a mapping from public key -> secret
secret_keys_map generate_keys_map(const secret_list& secrets)
{
    secret_keys_map keys;
    for (const auto& secret: secrets)
    {
        // Compute public key
        ec_compressed public_key;
        if (!secret_to_public(public_key, secret))
            return secret_keys_map();

        // Insert into map
        keys.insert({ public_key, secret });
    }

    return keys;
}

// Make a list of public keys for which we have the corresponding secret key
// in a single ring of public keys.
const point_list known_keys_in_ring(
    const secret_keys_map& secret_keys, const point_list& ring)
{
    point_list known_ring;
    for (const auto& key: ring)
    {
        // Is this public key in our secret keys map?
        if (secret_keys.find(key) != secret_keys.end())
            known_ring.push_back(key);
    }
    return known_ring;
}

// For all rings, make a list of known public keys corresponding to each ring.
const key_rings partition_keys_into_rings(
    const secret_keys_map& secret_keys, const key_rings& rings)
{
    key_rings known_keys;
    // For each ring in our rings...
    for (const auto& ring: rings)
    {
        // ... find known keys corresponding to our private keys
        const auto known_ring = known_keys_in_ring(secret_keys, ring);
        // And add this list to our rings of known public keys.
        known_keys.push_back(known_ring);
    }
    return known_keys;
}

// Make a list of indexes of where our known key occurs in each ring of
// public keys. That is given a ring of {A, B, C} where we know the
// private key of B, it will return 1 (the index in the ring).
// This function computes this for all rings.
index_list search_key_indexes(
    const key_rings& rings, const key_rings& known_keys_by_ring)
{
    index_list known_key_indexes;
    BITCOIN_ASSERT(known_keys_by_ring.size() == rings.size());
    // Iterate all our rings.
    for (size_t i = 0; i < rings.size(); ++i)
    {
        const auto& ring = rings[i];
        const auto& known = known_keys_by_ring[i];
        // Find known key in this current ring.
        const auto it = std::find(ring.begin(), ring.end(), known.back());
        BITCOIN_ASSERT(it != ring.end());
        // Calculate and store the index
        const auto index = std::distance(ring.begin(), it);
        known_key_indexes.push_back(index);
    }
    return known_key_indexes;
}

index_list generate_known_indexes(
    const key_rings& rings, const secret_keys_map& secret_keys)
{
    // Organize known public keys into corresponding rings
    const auto known_keys_by_ring =
        partition_keys_into_rings(secret_keys, rings);

    // Check we know a secret key in each ring
    auto empty_ring = [](const point_list& ring)
    {
        return ring.empty();
    };
    const bool any_ring_zero_size = std::any_of(
        known_keys_by_ring.begin(), known_keys_by_ring.end(), empty_ring);

    if (any_ring_zero_size)
        return index_list();

    // Compute indexes for known keys inside the rings.
    return search_key_indexes(rings, known_keys_by_ring);
}

template <typename R_Type>
ec_scalar borromean_hash(const hash_digest& M, const R_Type& R,
    size_t i, size_t j)
{
    // e = H(M || R || i || j)
    data_chunk data(hash_size + R.size() + 4 + 4);
    auto serial = make_unsafe_serializer(data.begin());
    serial.write_bytes(R);
    serial.write_hash(M);
    serial.write_4_bytes_big_endian(i);
    serial.write_4_bytes_big_endian(j);
    return sha256_hash(data);
}

ec_point calculate_R(const ec_scalar& s, const ec_scalar& e,
    const ec_point& P)
{
    return s * ec_point::G + e * P;
}

ec_point calculate_last_R_signing(const point_list& ring,
    size_t i, const hash_digest& digest, const ring_signature& signature,
    const size_t known_key_index, const secret_list& salts)
{
    auto R_i_j = salts[i] * ec_point::G;
    BITCOIN_ASSERT(R_i_j);

    // ... Start one above index of known key and loop until the end
    for (size_t j = known_key_index + 1; j < ring.size(); ++j)
    {
        BITCOIN_ASSERT(j < signature.proofs[i].size());
        const ec_scalar s = signature.proofs[i][j];
        if (!s)
            return ec_point();

        // Calculate e and R until the end of this ring.
        const auto e_i_j = borromean_hash(digest, R_i_j.point(), i, j);
        if (!e_i_j)
            return ec_point();

        R_i_j = calculate_R(s, e_i_j, ring[j]);
        if (!R_i_j)
            return ec_point();
    }
    return R_i_j;
}

bool calculate_e0(ring_signature& out, const key_rings& rings,
    const hash_digest& digest, const secret_list& salts,
    const index_list& known_key_indexes)
{
    data_chunk e0_data;
    e0_data.reserve(ec_compressed_size * rings.size() + hash_size);

    BITCOIN_ASSERT(known_key_indexes.size() == rings.size());
    BITCOIN_ASSERT(salts.size() == rings.size());
    BITCOIN_ASSERT(out.proofs.size() == rings.size());
    for (size_t i = 0; i < rings.size(); ++i)
    {
        // Current ring and index of known key
        const auto& ring = rings[i];
        const auto known_key_index = known_key_indexes[i];

        // Calculate the last R value...
        const auto last_R = calculate_last_R_signing(
            ring, i, digest, out, known_key_index, salts);
        if (!last_R)
            return false;

        // Add this ring to e0
        extend_data(e0_data, last_R.point());
    }
    extend_data(e0_data, digest);
    // Hash data to produce e0 value
    out.challenge = sha256_hash(e0_data);
    return true;
}

bool calculate_e_at_known_key_index(ec_scalar& e_i_j,
    const ring_signature& signature, const point_list& ring,
    const hash_digest& digest, const size_t i, const size_t known_key_index)
{
    BITCOIN_ASSERT(signature.proofs[i].size() > known_key_index);
    BITCOIN_ASSERT(ring.size() > known_key_index);
    // Loop until index of known key.
    for (size_t j = 0; j < known_key_index; ++j)
    {
        const ec_scalar s = signature.proofs[i][j];
        if (!s)
            return false;

        // Calculate e and R until we reach our index.
        const auto R = calculate_R(s, e_i_j, ring[j]);
        if (!R)
            return false;

        e_i_j = borromean_hash(digest, R.point(), i, j + 1);
        if (!e_i_j)
            return false;
    }
    return true;
}

bool join_rings_together(ring_signature& out, const key_rings& rings,
    const hash_digest& digest, const secret_list& salts,
    const index_list& known_key_indexes, secret_keys_map& secret_keys)
{
    for (size_t i = 0; i < rings.size(); ++i)
    {
        // Current ring and index of known key
        const auto& ring = rings[i];
        const auto known_key_index = known_key_indexes[i];

        // Calculate starting e value of this current ring.
        auto e_i_j = borromean_hash(digest, out.challenge, i, 0);
        if (!e_i_j)
            return false;

        if (!calculate_e_at_known_key_index(
                e_i_j, out, ring, digest, i, known_key_index))
            return false;

        // Find secret key used for calculation in the next step.
        BITCOIN_ASSERT(known_key_index < ring.size());
        const auto& known_public_key = ring[known_key_index];
        BITCOIN_ASSERT(secret_keys.find(known_public_key) != secret_keys.end());
        const auto& secret = secret_keys[known_public_key];

        // Now close the ring using this calculation:
        const auto& k = salts[i];
        const auto& x = secret;

        const auto s = k - e_i_j * x;
        if (!s)
            return false;

        // Close the ring
        out.proofs[i][known_key_index] = s;
    }
    return true;
}

bool sign(ring_signature& out, const secret_list& secrets,
    const key_rings& rings, const hash_digest& digest,
    const secret_list& salts)
{
    // Create public key -> secret key map
    auto secret_keys = generate_keys_map(secrets);
    if (secret_keys.empty())
        return false;

    // Create index of secret key per ring
    const auto known_key_indexes = generate_known_indexes(rings, secret_keys);
    if (known_key_indexes.size() != rings.size())
        return false;

    // Step 1: calculate e0
    return calculate_e0(out, rings, digest, salts, known_key_indexes) &&

    // Step 2: join up each ring at the index where we know the secret key
        join_rings_together(out, rings, digest, salts, known_key_indexes,
            secret_keys);
}

ec_point calculate_last_R_verify(const point_list& ring,
    ec_scalar e_i_j, size_t i, const hash_digest& digest,
    const ring_signature& signature)
{
    ec_point R_i_j;
    BITCOIN_ASSERT(signature.proofs[i].size() == ring.size());
    for (size_t j = 0; j < ring.size(); ++j)
    {
        // s_i_j
        const ec_scalar s = signature.proofs[i][j];

        if (!s || !e_i_j)
            return ec_point();

        // Calculate R and e values until the end.
        R_i_j = calculate_R(s, e_i_j, ring[j]);
        if (!R_i_j)
            return ec_point();

        // Next e value
        e_i_j = borromean_hash(digest, R_i_j.point(), i, j + 1);
        if (!e_i_j)
            return ec_point();
    }
    return R_i_j;
}

bool verify(const key_rings& rings, const hash_digest& digest,
    const ring_signature& signature)
{
    // As compared with signing, we only have to perform a single step.
    // The ring has already been computed, so now we just need to verify
    // it by calculating e0, and looping all the way until the end R value
    // which we use to recalculate e0.
    // If the values match then we have a valid ring signature.

    data_chunk e0_data;
    e0_data.reserve(ec_compressed_size * rings.size() + hash_size);

    BITCOIN_ASSERT(signature.proofs.size() == rings.size());
    // Loop through rings
    for (size_t i = 0; i < rings.size(); ++i)
    {
        const auto& ring = rings[i];

        // Calculate first e value for this ring.
        const auto e_i_0 = borromean_hash(digest, signature.challenge, i, 0);
        if (!e_i_0)
            return false;

        // Calculate the last R value...
        const auto last_R = calculate_last_R_verify(
            ring, e_i_0, i, digest, signature);
        if (!last_R)
            return false;

        // Add this ring to e0
        extend_data(e0_data, last_R.point());
    }
    extend_data(e0_data, digest);
    // Hash data to produce e0 value
    const auto e0_hash = sha256_hash(e0_data);

    // Verification step.
    return e0_hash == signature.challenge;
}

} // namespace libbitcoin

