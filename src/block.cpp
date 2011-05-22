#include <boost/detail/endian.hpp>
#include <openssl/sha.h>

#include "block.hpp"

namespace libbitcoin {

void block::calculate_hash()
{
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256_CTX ctx;
    
    //TODO handle errors
    
#ifdef BOOST_LITTLE_ENDIAN
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, &version_, sizeof(version_));
    SHA256_Update(&ctx, &prev_hash_, sizeof(prev_hash_));
    SHA256_Update(&ctx, &merkle_root_, sizeof(merkle_root_));
    SHA256_Update(&ctx, &timestamp_, sizeof(timestamp_));
    SHA256_Update(&ctx, &bits_, sizeof(bits_));
    SHA256_Update(&ctx, &nonce_, sizeof(nonce_));
    SHA256_Final(digest, &ctx);
    
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, &digest, sizeof(digest));
    SHA256_Final(hash_, &ctx);
#elif BOOST_BIG_ENDIAN
    #error "Platform not supported"
#else
    #error "Platform not supported"
#endif
}

} // libbitcoin

