#include <bitcoin/utility/elliptic_curve_key.hpp>

#include <algorithm>
#include <iostream>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>

#include <bitcoin/utility/assert.hpp>

namespace libbitcoin {

elliptic_curve_key::elliptic_curve_key()
  : key_(nullptr)
{
}
elliptic_curve_key::~elliptic_curve_key()
{
    if (key_ != nullptr)
        EC_KEY_free(key_);
}

elliptic_curve_key::elliptic_curve_key(const elliptic_curve_key& other)
{
    key_ = EC_KEY_dup(other.key_);
}
elliptic_curve_key& elliptic_curve_key::operator=(
    const elliptic_curve_key& other)
{
    EC_KEY_copy(key_, other.key_);
    return *this;
}

bool elliptic_curve_key::set_public_key(const data_chunk& pubkey)
{
    if (!initialize())
        return false;
    const unsigned char* pubkey_bytes = pubkey.data();
    if (!o2i_ECPublicKey(&key_, &pubkey_bytes, pubkey.size()))
        return false;
    return true;
}

data_chunk elliptic_curve_key::public_key() const
{
    // same as get_private_key
    int length = i2o_ECPublicKey(key_, NULL);
    if (!length)
        return private_data();
    data_chunk pubkey(length, 0);
    uint8_t* pubkey_begin = pubkey.data();
    if (i2o_ECPublicKey(key_, &pubkey_begin) != length)
        return data_chunk();
    return pubkey;
}

bool elliptic_curve_key::verify(hash_digest hash, const data_chunk& signature)
{
    BITCOIN_ASSERT(key_ != nullptr);
    // SSL likes a reversed hash
    std::reverse(hash.begin(), hash.end());
    // -1 = error, 0 = bad sig, 1 = good
    if (ECDSA_verify(0, hash.data(), hash.size(), 
            signature.data(), signature.size(), key_) == 1)
        return true;
    return false;
}

bool elliptic_curve_key::new_key_pair()
{
    if (!initialize())
        return false;
    if (!EC_KEY_generate_key(key_))
        return false;
    return true;
}

bool elliptic_curve_key::set_private_key(const private_data& privkey)
{
    if (!initialize())
        return false;
    const uint8_t* privkey_begin = privkey.data();
    if (!d2i_ECPrivateKey(&key_, &privkey_begin, privkey.size()))
        return false;
    return true;
}

private_data elliptic_curve_key::private_key() const
{
    int length = i2d_ECPrivateKey(key_, NULL);
    if (!length)
        return private_data();
    private_data privkey(length, 0);
    uint8_t* privkey_begin = privkey.data();
    if (i2d_ECPrivateKey(key_, &privkey_begin) != length)
        return private_data();
    return privkey;
}

data_chunk elliptic_curve_key::sign(hash_digest hash) const
{
    BITCOIN_ASSERT(key_ != nullptr);
    // SSL likes a reversed hash
    std::reverse(hash.begin(), hash.end());
    data_chunk signature(ECDSA_size(key_));
    unsigned int signature_length = signature.size();
    if (!ECDSA_sign(0, hash.data(), hash.size(), 
            signature.data(), &signature_length, key_))
        return data_chunk();
    signature.resize(signature_length);
    return signature;
}

// Generate a private key from just the secret parameter
bool EC_KEY_regenerate_key(EC_KEY *eckey, BIGNUM *priv_key)
{
    bool success = false;
    BN_CTX *ctx = nullptr;
    EC_POINT *pub_key = nullptr;

    if (!eckey)
        return 0;

    const EC_GROUP *group = EC_KEY_get0_group(eckey);

    ctx = BN_CTX_new();
    if (!ctx)
        goto error;

    pub_key = EC_POINT_new(group);
    if (!pub_key)
        goto error;

    if (!EC_POINT_mul(group, pub_key, priv_key, nullptr, nullptr, ctx))
        goto error;

    EC_KEY_set_private_key(eckey, priv_key);
    EC_KEY_set_public_key(eckey, pub_key);

    success = true;

error:
    if (pub_key)
        EC_POINT_free(pub_key);
    if (ctx)
        BN_CTX_free(ctx);

    return success;
}

bool elliptic_curve_key::set_secret(const secret_parameter& secret)
{
    key_ = EC_KEY_new_by_curve_name(NID_secp256k1);
    if (!key_)
        return false;
    BIGNUM *bignum = BN_bin2bn(secret.data(), secret.size(), BN_new());
    if (!bignum)
        return false;
    if (!EC_KEY_regenerate_key(key_, bignum))
    {
        BN_clear_free(bignum);
        return false;
    }
    BN_clear_free(bignum);
    return true;
}

secret_parameter elliptic_curve_key::secret() const
{
    const BIGNUM *bignum = EC_KEY_get0_private_key(key_);
    int num_bytes = BN_num_bytes(bignum);
    if (!bignum)
        return secret_parameter();
    secret_parameter secret;
    int copied_bytes = BN_bn2bin(bignum, &secret[32 - num_bytes]);
    if (copied_bytes != num_bytes)
        return secret_parameter();
    return secret;
}

bool elliptic_curve_key::initialize()
{
    // Already initialized
    if (key_ != nullptr)
        return true;
    key_ = EC_KEY_new_by_curve_name(NID_secp256k1);
    return key_ != nullptr;
}

} // namespace libbitcoin

