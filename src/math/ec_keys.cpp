/*
 * Copyright (c) 2011-2014 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/endian.hpp>

namespace libbitcoin {

// This class holds no static state but will only initialize its state once for
// the given mutex. This can be assigned to a static or otherwise. It lazily
// inits the context once and destroys the context on destruct as necessary.
class curve
{
private:
    static void set_context(secp256k1_context_t** context, int flags)
    {
        *context = secp256k1_context_create(flags);
    }

public:
    curve(int flags)
        : flags_(flags), context_(nullptr)
    {
    }

    ~curve()
    {
        if (context_ != nullptr)
            secp256k1_context_destroy(context_);
    }

    secp256k1_context_t* context()
    {
        std::call_once(mutex_, set_context, &context_, flags_);
        return context_;
    }

private:
    int flags_;
    std::once_flag mutex_;
    secp256k1_context_t* context_;
};


// We do not share contexts because they may or may not both be required at 
// some point and they can't be independently destroyed.
static curve signing(SECP256K1_CONTEXT_SIGN);
static curve verification(SECP256K1_CONTEXT_VERIFY);

ec_point secret_to_public_key(const ec_secret& secret,
    bool compressed)
{
    const auto signing_context = signing.context();
    size_t public_key_size = ec_uncompressed_size;
    if (compressed)
        public_key_size = ec_compressed_size;

    ec_point out(public_key_size);
    int out_size;

    if (secp256k1_ec_pubkey_create(signing_context, out.data(), &out_size,
        secret.data(), compressed) == 1)
    {
        BITCOIN_ASSERT_MSG(public_key_size == static_cast<size_t>(out_size),
            "secp256k1_ec_pubkey_create returned invalid size");
        return out;
    }

    return ec_point();
}

bool verify_public_key(const ec_point& public_key)
{
    const auto verification_context = verification.context();
    return secp256k1_ec_pubkey_verify(verification_context, public_key.data(),
        static_cast<uint32_t>(public_key.size())) == 1;
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
    const auto verification_context = verification.context();
    return secp256k1_ec_seckey_verify(verification_context, private_key.data())
        == 1;
}

endorsement sign(ec_secret secret, hash_digest hash)
{
    const auto signing_context = signing.context();
    int out_size = max_endorsement_size;
    endorsement signature(out_size);

    if (secp256k1_ecdsa_sign(signing_context, hash.data(), signature.data(),
        &out_size, secret.data(), secp256k1_nonce_function_rfc6979, nullptr)
        != 1)
    {
        BITCOIN_ASSERT_MSG(false, "secp256k1_ecdsa_sign failed");
        out_size = 0;
    };

    signature.resize(out_size);
    return signature;
}

compact_signature sign_compact(ec_secret secret, hash_digest hash)
{
    const auto signing_context = signing.context();
    compact_signature out;

    if (secp256k1_ecdsa_sign_compact(signing_context, hash.data(),
        out.signature.data(), secret.data(), secp256k1_nonce_function_rfc6979,
        nullptr, &out.recid) != 1)
    {
        BITCOIN_ASSERT_MSG(false, "secp256k1_ecdsa_sign_compact failed");
        return compact_signature();
    }

    return out;
}

bool verify_signature(const ec_point& public_key, hash_digest hash,
    const endorsement& signature)
{
    auto signing_context = verification.context();
    auto result = secp256k1_ecdsa_verify(signing_context, hash.data(),
        signature.data(), 
        static_cast<uint32_t>(signature.size()), public_key.data(),
        static_cast<uint32_t>(public_key.size()));

    BITCOIN_ASSERT_MSG(result >= 0, "secp256k1_ecdsa_verify failed");

    return result == 1;
}

ec_point recover_compact(compact_signature signature, hash_digest hash,
    bool compressed)
{
    const auto verification_context = verification.context();
    size_t public_key_size = ec_uncompressed_size;
    if (compressed)
        public_key_size = ec_compressed_size;

    ec_point out(public_key_size);
    int out_size;

    if (secp256k1_ecdsa_recover_compact(verification_context, hash.data(),
        signature.signature.data(), out.data(), &out_size, compressed,
        signature.recid) == 1)
    {
        BITCOIN_ASSERT_MSG(public_key_size == static_cast<size_t>(out_size),
            "secp256k1_ecdsa_recover_compact returned invalid size");
        return out;
    }

    return ec_point();
}

bool ec_add(ec_point& a, const ec_secret& b)
{
    const auto verification_context = verification.context();
    return secp256k1_ec_pubkey_tweak_add(verification_context, a.data(),
        static_cast<uint32_t>(a.size()), b.data()) == 1;
}

bool ec_add(ec_secret& a, const ec_secret& b)
{
    const auto verification_context = verification.context();
    return secp256k1_ec_privkey_tweak_add(verification_context, a.data(),
        b.data()) == 1;
}

bool ec_multiply(ec_point& a, const ec_secret& b)
{
    const auto verification_context = verification.context();
    return secp256k1_ec_pubkey_tweak_mul(verification_context, a.data(),
        static_cast<uint32_t>(a.size()), b.data()) == 1;
}

bool ec_multiply(ec_secret& a, const ec_secret& b)
{
    const auto verification_context = verification.context();
    return secp256k1_ec_privkey_tweak_mul(verification_context, a.data(),
        b.data()) == 1;
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

