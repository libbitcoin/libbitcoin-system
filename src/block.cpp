#include <openssl/sha.h>
#include <boost/detail/endian.hpp>

#include "block.hpp"

namespace libbitcoin {

void block::calculate_hash()
{
    unsigned char digest[32];
    SHA256_CTX ctx;
    
    //TODO handle errors
    
#ifdef BOOST_LITTLE_ENDIAN
    SHA256_Init(&ctx);
    SHA256_Update(&ctx,&this->version,sizeof(version));
    SHA256_Update(&ctx,&this->prev_hash,sizeof(prev_hash));
    SHA256_Update(&ctx,&this->merkle_root,sizeof(merkle_root));
    SHA256_Update(&ctx,&this->timestamp,sizeof(timestamp));
    SHA256_Update(&ctx,&this->bits,sizeof(bits));
    SHA256_Update(&ctx,&this->nonce,sizeof(nonce));
    SHA256_Final(digest,&ctx);
    
    SHA256_Init(&ctx);
    SHA256_Update(&ctx,&digest,sizeof(digest));
    SHA256_Final(this->hash,&ctx);
#elif BOOST_BIG_ENDIAN
#error Platform not supported
#else
#error Platform not supported
#endif
}

} // libbitcoin
