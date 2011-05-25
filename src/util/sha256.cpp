#include "util/sha256.hpp"

#include <boost/detail/endian.hpp>

#include "util/logger.hpp"

namespace libbitcoin {

sha256::sha256()
{
    SHA256_Init(&ctx_);
}

void sha256::str(const std::string& val)
{
#ifdef BOOST_LITTLE_ENDIAN
    SHA256_Update(&ctx_, val.c_str(), val.length());
#elif BOOST_BIG_ENDIAN
    #error Platform not supported
#else
    #error Platform not supported
#endif
}

void sha256::var_uint(uint64_t var_uint)
{
    if(var_uint < 0xFD)
    {
        uint8_t count = var_uint;
        logger(DLOG_INFO) << var_uint << " " << sizeof(count) << "\n";
        update(count);
    }
    else if(var_uint <= 0xffff)
    {
        const uint8_t prefix = 0xFD;
        uint16_t count = var_uint;
        logger(DLOG_INFO) << var_uint << " " << sizeof(count) << "\n";
        update(prefix);
        update(count);
    }
    else if(var_uint <= 0xffffffff)
    {
        const uint8_t prefix = 0xFE;
        uint32_t count = var_uint;
        logger(DLOG_INFO) << var_uint << " " << sizeof(count) << "\n";
        update(prefix);
        update(count);
    }
    else if(var_uint <= 0xffffffffffffffff)
    {
        const uint8_t prefix = 0xFF;
        uint64_t count = var_uint;
        logger(DLOG_INFO) << var_uint << " " << sizeof(count) << "\n";
        update(prefix);
        update(count);
    }
    else
    {
        //TODO handle
    }
}

void sha256::final(unsigned char hash[SHA256_DIGEST_LENGTH])
{
    SHA256_Final(digest_, &ctx_);
    SHA256_Init(&ctx_);
    SHA256_Update(&ctx_, digest_, sizeof(digest_));
    SHA256_Final(hash, &ctx_);
}

} // libbitcoin

