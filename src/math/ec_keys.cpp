/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/math/ec_keys.hpp>

#include <algorithm>
#include <mutex>
#include <secp256k1.h>
#include <secp256k1_recovery.h>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/math/secp256k1_initializer.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/endian.hpp>

namespace libbitcoin {

BC_CONSTFUNC int to_flags(bool compressed)
{
    return compressed ? SECP256K1_EC_COMPRESSED : SECP256K1_EC_UNCOMPRESSED;
}

ec_point secret_to_public_key(const ec_secret& secret, bool compressed)
{
    secp256k1_pubkey key;
    const auto context = signing.context();

    if (secp256k1_ec_pubkey_create(context, &key, secret.data()) != 1)
        return ec_point();

    const auto flags = to_flags(compressed);
    const auto size = compressed ? ec_compressed_size : ec_uncompressed_size;
    auto out_size = size;
    ec_point out(size);

    secp256k1_ec_pubkey_serialize(context, out.data(), &out_size, &key, flags);
    return out_size == size ? out : ec_point();
}

bool verify_public_key(const ec_point& public_key)
{
    secp256k1_pubkey key;
    const auto context = verification.context();
    return (secp256k1_ec_pubkey_parse(context, &key, public_key.data(),
        public_key.size()) == 1);
}

bool verify_public_key_fast(const ec_point& public_key)
{
    if (public_key.size() == ec_compressed_size)
        return public_key[0] == 0x02 || public_key[0] == 0x03;

    if (public_key.size() == ec_uncompressed_size)
        return public_key[0] == 0x04;

    return false;
}

bool verify_private_key(const ec_secret& private_key)
{
    const auto context = verification.context();
    return secp256k1_ec_seckey_verify(context, private_key.data()) == 1;
}

endorsement sign(const ec_secret& secret, const hash_digest& hash)
{
    secp256k1_ecdsa_signature signature;
    const auto context = signing.context();

    if (secp256k1_ecdsa_sign(context, &signature, hash.data(), secret.data(),
        secp256k1_nonce_function_rfc6979, nullptr) != 1)
        return endorsement();

    auto size = max_endorsement_size;
    endorsement out(size);
    if (secp256k1_ecdsa_signature_serialize_der(context, out.data(), &size,
        &signature) != 1)
        return endorsement();

    out.resize(size);
    return out;
}

compact_signature sign_compact(const ec_secret& secret,
    const hash_digest& hash)
{
    secp256k1_ecdsa_recoverable_signature signature;
    const auto context = signing.context();

    if (secp256k1_ecdsa_sign_recoverable(context, &signature, hash.data(),
        secret.data(), secp256k1_nonce_function_rfc6979, nullptr) != 1)
        return compact_signature();

    compact_signature out;
    secp256k1_ecdsa_recoverable_signature_serialize_compact(context,
        out.signature.data(), &out.recid, &signature);
    return out;
}

// secp256k1_ecdsa_verify rejects non-normalized signatures, so we normalize.
bool verify_signature(const ec_point& public_key, const hash_digest& hash,
    const endorsement& signature)
{
    secp256k1_ecdsa_signature original;
    const auto context = verification.context();

    if (secp256k1_ecdsa_signature_parse_der(context, &original, signature.data(),
        signature.size()) != 1)
        return false;

    secp256k1_pubkey key;
    secp256k1_ecdsa_signature normal;
    secp256k1_ecdsa_signature_normalize(context, &normal, &original);
    return
        secp256k1_ec_pubkey_parse(context, &key, public_key.data(),
            public_key.size()) == 1 &&
        secp256k1_ecdsa_verify(context, &normal, hash.data(), &key) == 1;
}

ec_point recover_compact(const compact_signature& signature,
    const hash_digest& hash, bool compressed)
{
    secp256k1_pubkey key;
    secp256k1_ecdsa_recoverable_signature sign;
    const auto context = verification.context();

    if (secp256k1_ecdsa_recoverable_signature_parse_compact(context, &sign,
        signature.signature.data(), signature.recid) != 1)
        return ec_point();

    if (secp256k1_ecdsa_recover(context, &key, &sign, hash.data()) != 1)
        return ec_point();

    const auto flags = to_flags(compressed);
    const auto size = compressed ? ec_compressed_size : ec_uncompressed_size;
    auto out_size = size;
    ec_point out(size);
    secp256k1_ec_pubkey_serialize(context, out.data(), &out_size, &key, flags);
    return out_size == size ? out : ec_point();
}

bool ec_add(ec_secret& a, const ec_secret& b)
{
    const auto context = verification.context();
    return secp256k1_ec_privkey_tweak_add(context, a.data(), b.data()) == 1;
}

bool ec_add(ec_point& a, const ec_secret& b)
{
    secp256k1_pubkey point;
    const auto context = verification.context();

    if (secp256k1_ec_pubkey_parse(context, &point, a.data(), a.size()) != 1 ||
        secp256k1_ec_pubkey_tweak_add(context, &point, b.data()) != 1)
        return false;

    const auto compressed = a.size() == ec_compressed_size;
    const auto size = compressed ? ec_compressed_size : ec_uncompressed_size;

    if (size != a.size())
        return false;

    auto out_size = size;
    const auto flags = to_flags(compressed);
    secp256k1_ec_pubkey_serialize(context, a.data(), &out_size, &point, flags);
    return out_size == size;
}

bool ec_multiply(ec_secret& a, const ec_secret& b)
{
    const auto context = verification.context();
    return secp256k1_ec_privkey_tweak_mul(context, a.data(), b.data()) == 1;
}

bool ec_multiply(ec_point& a, const ec_secret& b)
{
    secp256k1_pubkey point;
    const auto context = verification.context();

    if (secp256k1_ec_pubkey_parse(context, &point, a.data(), a.size()) != 1 ||
        secp256k1_ec_pubkey_tweak_mul(context, &point, b.data()) != 1)
        return false;

    const auto compressed = a.size() == ec_compressed_size;
    const auto size = compressed ? ec_compressed_size : ec_uncompressed_size;

    if (size != a.size())
        return false;

    auto out_size = size;
    const auto flags = to_flags(compressed);
    secp256k1_ec_pubkey_serialize(context, a.data(), &out_size, &point, flags);
    return out_size == size;
}

///////////////////////////////////////////////////////////////////////////////
// DEPRECATED (now redundant with secp256k1 implementation)
///////////////////////////////////////////////////////////////////////////////
ec_secret create_nonce(ec_secret secret, hash_digest hash, uint32_t index)
{
    hash_digest K
    {{
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    }};
    hash_digest V
    {{
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
    }};

    K = hmac_sha256_hash(build_data({V, to_byte(0x00), secret, hash}), K);
    V = hmac_sha256_hash(V, K);
    K = hmac_sha256_hash(build_data({V, to_byte(0x01), secret, hash}), K);
    V = hmac_sha256_hash(V, K);

    while (true)
    {
        V = hmac_sha256_hash(V, K);

        if (0 == index)
            return V;
        --index;

        K = hmac_sha256_hash(build_data({V, to_byte(0x00)}), K);
        V = hmac_sha256_hash(V, K);
    }
}

///////////////////////////////////////////////////////////////////////////////
// DEPRECATED (deterministic signatures are safer)
///////////////////////////////////////////////////////////////////////////////
endorsement sign(ec_secret secret, hash_digest hash, ec_secret /* nonce */)
{
    // THE CALLER'S NONCE IS IGNORED.
    return sign(secret, hash);
}

///////////////////////////////////////////////////////////////////////////////
// DEPRECATED (deterministic signatures are safer)
///////////////////////////////////////////////////////////////////////////////
compact_signature sign_compact(ec_secret secret, hash_digest hash,
    ec_secret /* nonce */)
{
    // THE CALLER'S NONCE IS IGNORED.
    return sign_compact(secret, hash);
}

} // namespace libbitcoin

