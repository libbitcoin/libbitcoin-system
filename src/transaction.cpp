#include "transaction.hpp"

#include <boost/detail/endian.hpp>
#include <iostream>
#include <string>
#include <openssl/sha.h>

#include "util/logger.hpp"

namespace libbitcoin {

template<typename T>
void update_sha256(SHA256_CTX& ctx, T& val)
{
    SHA256_Update(&ctx, &val, sizeof(T));
}

template<>
void update_sha256<std::string>(SHA256_CTX& ctx, std::string& val)
{
    SHA256_Update(&ctx, val.c_str(), val.length());
}

static void hash_var_uint(SHA256_CTX &ctx, uint64_t var_uint)
{
    if(var_uint < 0xFD)
    {
        uint8_t count = var_uint;
        
        logger(DLOG_INFO) << var_uint << " " << sizeof(count) << "\n";
        
        update_sha256(ctx, count);
    }
    else if(var_uint <= 0xffff)
    {
        const uint8_t prefix = 0xFD;
        uint16_t count = var_uint;
        
        logger(DLOG_INFO) << var_uint << " " << sizeof(count) << "\n";
        
        update_sha256(ctx, prefix);
        update_sha256(ctx, count);
    }
    else if(var_uint <= 0xffffffff)
    {
        const uint8_t prefix = 0xFE;
        uint32_t count = var_uint;
        
        update_sha256(ctx, prefix);
        update_sha256(ctx, count);
    }
    else if(var_uint <= 0xffffffffffffffff)
    {
        const uint8_t prefix = 0xFF;
        uint64_t count = var_uint;
        
        update_sha256(ctx, prefix);
        update_sha256(ctx, count);
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
    update_sha256(ctx, version);
    
    hash_var_uint(ctx, inputs.size());
    
    for(auto it = inputs.begin(); it != inputs.end(); ++it)
    {
        update_sha256(ctx, it->hash);
        update_sha256(ctx, it->index);
        
        hash_var_uint(ctx, it->script.length());
        
        update_sha256(ctx, it->script);
        update_sha256(ctx, it->sequence);
    }
    
    hash_var_uint(ctx, outputs.size());
    
    for(auto it = outputs.begin(); it != outputs.end(); ++it)
    {
        update_sha256(ctx, it->value);
        hash_var_uint(ctx, it->script.length());
        update_sha256(ctx, it->script);
    }
    update_sha256(ctx, locktime);
    
    SHA256_Final(digest, &ctx);
    
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, digest, sizeof(digest));
    SHA256_Final(hash, &ctx);
#elif BOOST_BIG_ENDIAN
    #error "Platform not supported"
#else
    #error "Platform not supported"
#endif
}

} // libbitcoin

