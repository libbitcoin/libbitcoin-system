#include <bitcoin/deterministic_wallet.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <openssl/ec.h>
#include <openssl/obj_mac.h>
#include <openssl/sha.h>

#include <bitcoin/constants.hpp>
#include <bitcoin/format.hpp>
#include <bitcoin/utility/assert.hpp>
#include <bitcoin/utility/sha256.hpp>

namespace libbitcoin {

template <typename ssl_type>
struct ssl_wrapper
{
    ssl_wrapper(ssl_type* obj)
      : obj(obj) {}
    virtual ~ssl_wrapper()
    {
    }
    operator ssl_type*()
    {
        return obj;
    }

    ssl_type* obj;
};

struct ssl_bignum
  : public ssl_wrapper<BIGNUM>
{
    ssl_bignum()
      : ssl_wrapper(BN_new()) {}
    ~ssl_bignum()
    {
        BN_free(obj);
    }
};

#define SSL_TYPE(name, ssl_type, free_func) \
    struct name \
      : public ssl_wrapper<ssl_type> \
    { \
        name(ssl_type* obj) \
          : ssl_wrapper(obj) {} \
        ~name() \
        { \
            free_func(obj); \
        } \
    };

SSL_TYPE(ec_group, EC_GROUP, EC_GROUP_free)
SSL_TYPE(ec_point, EC_POINT, EC_POINT_free)
SSL_TYPE(bn_ctx, BN_CTX, BN_CTX_free)

const std::string bignum_hex(BIGNUM* bn)
{
    char* repr = BN_bn2hex(bn);
    std::string result = repr;
    OPENSSL_free(repr);
    boost::algorithm::to_lower(result);
    return result;
}

const data_chunk bignum_data(BIGNUM* bn)
{
    data_chunk result(BN_num_bytes(bn));
    BN_bn2bin(bn, result.data());
    return result;
}

void deterministic_wallet::new_seed()
{
    constexpr size_t bits_needed = 8 * seed_size / 2;
    ssl_bignum rand_value;
    BN_rand(rand_value, bits_needed, 0, 0);
    seed_ = bignum_hex(rand_value);
    BITCOIN_ASSERT(seed_.size() == seed_size);
}

secret_parameter stretch_seed(const std::string& seed)
{
    BITCOIN_ASSERT(seed.size() == deterministic_wallet::seed_size);
    secret_parameter stretched;
    std::copy(seed.begin(), seed.end(), stretched.begin());
    secret_parameter oldseed = stretched;
    for (size_t i = 0; i < 100000; ++i)
    {
        SHA256_CTX ctx;
        SHA256_Init(&ctx);
        SHA256_Update(&ctx, stretched.data(), stretched.size());
        SHA256_Update(&ctx, oldseed.data(), oldseed.size());
        SHA256_Final(stretched.data(), &ctx);
    }
    return stretched;
}

data_chunk pubkey_from_secret(const secret_parameter& secret)
{
    elliptic_curve_key privkey;
    if (!privkey.set_secret(secret))
        return data_chunk();
    return privkey.public_key();
}

bool deterministic_wallet::set_seed(const std::string& seed)
{
    if (seed.size() != seed_size)
        return false;
    seed_ = seed;
    stretched_seed_ = stretch_seed(seed);
    master_public_key_ = pubkey_from_secret(stretched_seed_);
    // Snip the beginning 04 byte for compat reasons.
    master_public_key_.erase(master_public_key_.begin());
    if (master_public_key_.empty())
        return false;
    return true;
}
const std::string& deterministic_wallet::seed() const
{
    return seed_;
}

bool deterministic_wallet::set_master_public_key(const data_chunk& mpk)
{
    master_public_key_ = mpk;
    return true;
}
const data_chunk& deterministic_wallet::master_public_key() const
{
    return master_public_key_;
}

data_chunk deterministic_wallet::generate_public_key(size_t n, bool for_change)
{
    hash_digest sequence = get_sequence(n, for_change);

    ssl_bignum x, y, z;
    BN_bin2bn(sequence.data(), sequence.size(), z);
    BN_bin2bn(master_public_key_.data(), 32, x);
    BN_bin2bn(master_public_key_.data() + 32, 32, y);

    // Create a point.
    ec_group group(EC_GROUP_new_by_curve_name(NID_secp256k1));
    ec_point mpk(EC_POINT_new(group));
    bn_ctx ctx(BN_CTX_new());
    EC_POINT_set_affine_coordinates_GFp(group, mpk, x, y, ctx);
    ec_point result(EC_POINT_new(group));

    // result pubkey_point = mpk_pubkey_point + z*curve.generator
    ssl_bignum one;
    BN_one(one);
    EC_POINT_mul(group, result, z, mpk, one, ctx);

    // Create the actual public key.
    EC_POINT_get_affine_coordinates_GFp(group, result, x, y, ctx);
    // 04 + x + y
    data_chunk raw_pubkey{0x04};
    extend_data(raw_pubkey, bignum_data(x));
    extend_data(raw_pubkey, bignum_data(y));
    return raw_pubkey;
}

secret_parameter deterministic_wallet::generate_secret(
    size_t n, bool for_change)
{
    if (seed_.empty())
        return null_hash;

    ssl_bignum z;
    hash_digest sequence = get_sequence(n, for_change);
    BN_bin2bn(sequence.data(), sequence.size(), z);

    ec_group group(EC_GROUP_new_by_curve_name(NID_secp256k1));
    ssl_bignum order;
    bn_ctx ctx(BN_CTX_new());
    EC_GROUP_get_order(group, order, ctx);

    // secexp = (stretched_seed + z) % order
    ssl_bignum secexp;
    BN_bin2bn(stretched_seed_.data(), stretched_seed_.size(), secexp);
    BN_add(secexp, secexp, z);
    BN_mod(secexp, secexp, order, ctx);

    secret_parameter secret;
    BITCOIN_ASSERT(BN_num_bytes(secexp) == secret.size());
    BN_bn2bin(secexp, secret.data());
    return secret;
}

hash_digest deterministic_wallet::get_sequence(size_t n, bool for_change)
{
    data_chunk chunk;
    extend_data(chunk, boost::lexical_cast<std::string>(n));
    chunk.push_back(':');
    chunk.push_back(for_change ? '1' : '0');
    chunk.push_back(':');
    extend_data(chunk, master_public_key_);
    hash_digest result = generate_sha256_hash(chunk);
    std::reverse(result.begin(), result.end());
    return result;
}

} // namespace libbitcoin

