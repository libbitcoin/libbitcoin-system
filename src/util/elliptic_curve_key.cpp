#include <bitcoin/util/elliptic_curve_key.hpp>

#include <algorithm>
#include <iostream>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>

namespace libbitcoin {

elliptic_curve_key::elliptic_curve_key()
  : key_(nullptr)
{
}

bool elliptic_curve_key::set_public_key(data_chunk pubkey)
{
    if (!initialize())
        return false;
    const unsigned char* pubkey_bytes = &pubkey[0];
    if (!o2i_ECPublicKey(&key_, &pubkey_bytes, pubkey.size()))
        return false;
    return true;
}

bool elliptic_curve_key::verify(hash_digest hash, data_chunk signature)
{
    // SSL likes a reversed hash
    std::reverse(hash.begin(), hash.end());
    // -1 = error, 0 = bad sig, 1 = good
    if (ECDSA_verify(0, hash.data(), hash.size(), 
            &signature[0], signature.size(), key_) == 1)
        return true;
    return false;
}

bool elliptic_curve_key::initialize()
{
    key_ = EC_KEY_new_by_curve_name(NID_secp256k1);
    return key_ != nullptr;
}

} // libbitcoin

