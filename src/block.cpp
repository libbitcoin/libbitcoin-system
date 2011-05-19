#include <openssl/sha.h>
#include <boost/detail/endian.hpp>

#include "block.h"

unsigned char[32] block::hash()
{
    unsigned char[32] digest;
    SHA256_CTX ctx;
    
    //TODO handle errors
    
    if(BOOST_LITTLE_ENDIAN)
    {
        SHA256_Init(&ctx);
        SHA256_Update(&ctx,&version,sizeof(version));
        SHA256_Update(&ctx,&prev_hash,sizeof(prev_hash));
        SHA256_Update(&ctx,&merkle_root,sizeof(merkle_root));
        SHA256_Update(&ctx,&timestamp,sizeof(timestamp));
        SHA256_Update(&ctx,&bits,sizeof(bits));
        SHA256_Update(&ctx,&nonce,sizeof(nonce));
        SHA256_Final(&digest,&ctx);
        
        SHA256_Init(&ctx);
        SHA256_Update(&ctx,&digest,sizeof(digest));
        SHA256_Final(&digest,&ctx);
        
        return digest;
    }
    else if(BOOST_BIG_ENDIAN)
    {
        //TODO handle big endian
    }
    else
    {
        //TODO handle not handling
    }
    

}
