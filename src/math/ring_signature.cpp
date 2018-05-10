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
#include <boost/range/combine.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>
#include <bitcoin/bitcoin/wallet/hd_private.hpp>

namespace libbitcoin {

typedef std::map<ec_compressed, ec_secret> secret_keys_map;

const secret_keys_map generate_keys_map(const secret_list& secrets)
{
    secret_keys_map keys;
    for (const auto& secret: secrets)
    {
        ec_compressed public_key;
        bool rc = secret_to_public(public_key, secret);
        BITCOIN_ASSERT(rc);
        keys.insert({public_key, secret});
    }
    return keys;
}

const point_list known_keys_in_ring(
    const secret_keys_map& secret_keys,
    const point_list& ring)
{
    point_list known_ring;
    for (const auto& key: ring)
    {
        if (secret_keys.find(key) != secret_keys.end())
            known_ring.push_back(key);
    }
    return known_ring;
}

const key_rings partition_keys_into_rings(
    const secret_keys_map& secret_keys,
    const key_rings& rings)
{
    key_rings known_keys;
    for (const auto& ring: rings)
    {
        const auto known_ring = known_keys_in_ring(secret_keys, ring);
        known_keys.push_back(known_ring);
    }
    return known_keys;
}

bool all_rings_nonzero_size(const key_rings& known_keys)
{
    for (const auto& ring: known_keys)
        if (ring.empty())
            return false;
    return true;
}

typedef std::vector<size_t> index_list;

index_list search_key_indexes(
    const key_rings& rings,
    const key_rings& known_keys_by_ring)
{
    index_list known_key_indexes;
    for (size_t i = 0; i < rings.size(); ++i)
    {
        BITCOIN_ASSERT(i < known_keys_by_ring.size());
        const auto& ring = rings[i];
        const auto& known = known_keys_by_ring[i];
        const auto it = std::find(ring.begin(), ring.end(), known.back());
        BITCOIN_ASSERT(it != ring.end());
        size_t index = it - ring.begin();
        known_key_indexes.push_back(index);
    }
    return known_key_indexes;
}

data_chunk concatenate(const data_slice message, const key_rings& rings)
{
    data_chunk result(message.begin(), message.end());
    for (const auto& ring: rings)
        for (const auto& key: ring)
            extend_data(result, key);
    return result;
}

void populate_k_and_s_values(
    secret_list& k, s_values_type& s,
    const key_rings& rings, const data_slice seed)
{
    auto random_scalar = [&seed]()
    {
        static const wallet::hd_private generate_secret(to_chunk(seed));
        static uint32_t i = 0;
        return generate_secret.derive_private(i++).secret();
    };

    for (const auto& ring: rings)
    {
        secret_list s_ring;
        for (const auto& key: ring)
            s_ring.push_back(random_scalar());
        s.push_back(s_ring);

        k.push_back(random_scalar());
    }
}

template <typename R_Type>
ec_secret borromean_hash(const hash_digest& M, const R_Type& R,
    size_t i, size_t j)
{
    // e = H(M || R || i || j)
    data_chunk data(hash_size + R.size() + 4 + 4);
    auto serial = make_unsafe_serializer(data.begin());
    serial.write_hash(M);
    serial.write_bytes(R);
    serial.write_4_bytes_little_endian(i);
    serial.write_4_bytes_little_endian(j);
    return sha256_hash(data);
}

ec_compressed calculate_R(const ec_secret& s, const ec_secret& e,
    const ec_compressed& ring_key)
{
    // R = s G + e P
    //   = (s + ex) G
    //   = k G
    ec_compressed eP = ring_key;
    // eP = e * P
    bool rc = ec_multiply(eP, e);
    BITCOIN_ASSERT(rc);
    // sG = s * G
    ec_compressed sG;
    rc = secret_to_public(sG, s);
    BITCOIN_ASSERT(rc);
    // result = s G + e P
    ec_compressed result;
    rc = ec_sum(result, {sG, eP});
    BITCOIN_ASSERT(rc);
    return result;
}

ec_secret calculate_s(const ec_secret& k, const ec_secret& e,
    const ec_secret& secret)

{
    // s = k - e x
    ec_secret result = e;
    // result = e * x
    bool rc = ec_multiply(result, secret);
    BITCOIN_ASSERT(rc);
    // result = - e x
    rc = ec_negate(result);
    BITCOIN_ASSERT(rc);
    // result = k - e x
    ec_add(result, k);
    return result;
}

bool sign(
    ring_signature& out,
    const secret_list& secrets,
    const key_rings& rings,
    const data_slice message,
    const data_slice seed)
{
    auto secret_keys = generate_keys_map(secrets);
    const auto known_keys_by_ring =
        partition_keys_into_rings(secret_keys, rings);
    // Check we know a secret key in each ring
    if (!all_rings_nonzero_size(known_keys_by_ring))
        return false;

    const index_list known_key_indexes =
        search_key_indexes(rings, known_keys_by_ring);
    BITCOIN_ASSERT(known_key_indexes.size() == rings.size());

    const data_chunk message_data = concatenate(message, rings);
    const hash_digest M = sha256_hash(message_data);

    secret_list k;
    populate_k_and_s_values(k, out.s, rings, seed);

    data_chunk e0_data;

    for (size_t i = 0; i < rings.size(); ++i)
    {
        BITCOIN_ASSERT(i < known_key_indexes.size());
        BITCOIN_ASSERT(i < k.size());
        BITCOIN_ASSERT(i < out.s.size());

        const auto& ring = rings[i];
        const size_t known_key_index = known_key_indexes[i];

        ec_compressed R_i_j;
        bool rc = secret_to_public(R_i_j, k[i]);

        for (size_t j = known_key_index + 1; j < ring.size(); ++j)
        {
            BITCOIN_ASSERT(j < out.s[i].size());
            const auto& s = out.s[i][j];

            const auto e_i_j = borromean_hash(M, R_i_j, i, j);
            R_i_j = calculate_R(s, e_i_j, ring[j]);
        }
        extend_data(e0_data, R_i_j);
    }
    extend_data(e0_data, M);
    out.e = sha256_hash(e0_data);

    for (size_t i = 0; i < rings.size(); ++i)
    {
        BITCOIN_ASSERT(i < known_key_indexes.size());
        BITCOIN_ASSERT(i < k.size());
        BITCOIN_ASSERT(i < out.s.size());

        const auto& ring = rings[i];
        const size_t known_key_index = known_key_indexes[i];

        auto e_i_j = borromean_hash(M, out.e, i, 0);

        for (size_t j = 0; j < known_key_index; ++j)
        {
            BITCOIN_ASSERT(j < out.s[i].size());
            const auto& s = out.s[i][j];

            BITCOIN_ASSERT(j < ring.size());
            const auto R_i_j = calculate_R(s, e_i_j, ring[j]);
            e_i_j = borromean_hash(M, R_i_j, i, j + 1);
        }

        BITCOIN_ASSERT(known_key_index < ring.size());
        const auto& known_public_key = ring[known_key_index];
        BITCOIN_ASSERT(secret_keys.find(known_public_key) != secret_keys.end());
        const auto& secret = secret_keys[known_public_key];
        out.s[i][known_key_index] = calculate_s(k[i], e_i_j, secret);
    }

    return true;
}

bool verify(
    const key_rings& rings,
    const data_slice message,
    const ring_signature& signature)
{
    const data_chunk message_data = concatenate(message, rings);
    const hash_digest M = sha256_hash(message_data);

    data_chunk e0_data;

    for (size_t i = 0; i < rings.size(); ++i)
    {
        BITCOIN_ASSERT(i < signature.s.size());

        const auto& ring = rings[i];

        auto e_i_j = borromean_hash(M, signature.e, i, 0);

        ec_compressed R_i_j;
        for (size_t j = 0; j < ring.size(); ++j)
        {
            BITCOIN_ASSERT(j < signature.s[i].size());
            const auto& s = signature.s[i][j];

            BITCOIN_ASSERT(j < ring.size());
            R_i_j = calculate_R(s, e_i_j, ring[j]);
            e_i_j = borromean_hash(M, R_i_j, i, j + 1);
        }
        extend_data(e0_data, R_i_j);
    }
    extend_data(e0_data, M);
    auto e0_hash = sha256_hash(e0_data);

    return e0_hash == signature.e;
}

} // namespace libbitcoin

