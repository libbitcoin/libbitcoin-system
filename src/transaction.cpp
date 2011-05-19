#include <openssl/sha.h>
#include <boost/detail/endian.hpp>

#include "transaction.h"

namespace libbitcoin {

static void hash_var_uint(SHA256_CTX *ctx,uint64_t var_uint)
{
    if(var_uint < 0xFD)
    {
        uint8_t count = var_uint;
        SHA256_Update(ctx,&count,sizeof(count));
    }
    else if(var_uint <= 0xffff)
    {
        const unsigned char prefix = 0xFD;
        uint16_t count = var_uint;
        
        SHA256_Update(ctx,&prefix,sizeof(prefix));
        SHA256_Update(ctx,&count,sizeof(count));
    }
    else if(var_uint <= 0xffffffff)
    {
        const unsigned char prefix = 0xFE;
        uint32_t count = var_uint;
        
        SHA256_Update(ctx,&prefix,sizeof(prefix));
        SHA256_Update(ctx,&count,sizeof(count));
    }
    else if(var_uint <= 0xffffffffffffffff)
    {
        const unsigned char prefix = 0xFF;
        uint64_t count = var_uint;
        
        SHA256_Update(ctx,&prefix,sizeof(prefix));
        SHA256_Update(ctx,&count,sizeof(count));
    }
    else
    {
        //TODO handle
    }
}

void transaction::calculate_hash()
{
    unsigned char digest[32];
    SHA256_CTX ctx;
    
    //TODO handle errors
    
#ifdef BOOST_LITTLE_ENDIAN
    SHA256_Init(&ctx);
    SHA256_Update(&ctx,&this->version,sizeof(version));
    hash_var_uint(&ctx,&this->inputs.size());
    SHA256_Update(&ctx,&this->version,sizeof(version));
    SHA256_Update(&ctx,&this->,sizeof(prev_hash));
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
