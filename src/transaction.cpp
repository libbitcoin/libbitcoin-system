#include <boost/detail/endian.hpp>
#include <string>
#include <openssl/sha.h>

#include "transaction.h"

namespace libbitcoin {

static void hash_var_uint(SHA256_CTX &ctx,uint64_t var_uint)
{
    if(var_uint < 0xFD)
    {
        uint8_t count = var_uint;
        SHA256_Update(&ctx,&count,sizeof(count));
    }
    else if(var_uint <= 0xffff)
    {
        const unsigned char prefix = 0xFD;
        uint16_t count = var_uint;
        
        SHA256_Update(&ctx,&prefix,sizeof(prefix));
        SHA256_Update(&ctx,&count,sizeof(count));
    }
    else if(var_uint <= 0xffffffff)
    {
        const unsigned char prefix = 0xFE;
        uint32_t count = var_uint;
        
        SHA256_Update(&ctx,&prefix,sizeof(prefix));
        SHA256_Update(&ctx,&count,sizeof(count));
    }
    else if(var_uint <= 0xffffffffffffffff)
    {
        const unsigned char prefix = 0xFF;
        uint64_t count = var_uint;
        
        SHA256_Update(&ctx,&prefix,sizeof(prefix));
        SHA256_Update(&ctx,&count,sizeof(count));
    }
    else
    {
        //TODO handle
    }
}

void transaction::calculate_hash()
{
    SHA256_CTX ctx;
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256_Init(&ctx);
    
#ifdef BOOST_LITTLE_ENDIAN
    SHA256_Update(&ctx,&this->version,sizeof(this->version));
    
    hash_var_uint(ctx,this->inputs.size());
    
    for(std::vector<transaction_input>::iterator it=this->inputs.begin();
        it != this->inputs.end();++it)
    {
        SHA256_Update(&ctx,&it->hash,sizeof(it->hash));
        SHA256_Update(&ctx,&it->index,sizeof(it->index));
        SHA256_Update(&ctx,it->script.c_str(),it->script.length());
        SHA256_Update(&ctx,&it->sequence,sizeof(it->sequence));
    }
    
    hash_var_uint(ctx,this->outputs.size());
    
    for(std::vector<transaction_output>::iterator it=this->outputs.begin();
        it != this->outputs.end();++it)
    {
        SHA256_Update(&ctx,&it->value,sizeof(it->value));
        SHA256_Update(&ctx,it->script.c_str(),it->script.length());
    }
    SHA256_Update(&ctx,&this->locktime,sizeof(this->locktime));
    
    SHA256_Final(digest,&ctx);
    
    SHA256_Init(&ctx);
    SHA256_Update(&ctx,digest,sizeof(digest));
    SHA256_Final(this->hash,&ctx);
#elif BOOST_BIG_ENDIAN
#error Platform not supported
#else
#error Platform not supported
#endif
}

} // libbitcoin
