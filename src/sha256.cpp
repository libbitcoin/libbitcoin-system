#include <boost/asio/buffer.hpp>
#include <openssl/sha.h>

#include "sha256.h"

//TODO handle openssl errors

namespace libbitcoin {

SHA256::SHA256()
{
    SHA256_Init(&this->sha256_context);
    this->finalized = false;
}

void SHA256::update(const void *data, unsigned long len)
{
    if(finalized)
    {
        //TODO handle this
    }
    
    SHA256_Update(&this->sha256_context, data, len);
}

void SHA256::update(std::string data)
{
    if(finalized)
    {
        //TODO handle this
    }
    
    SHA256_Update(&this->sha256_context, data.c_str(), data.length());
}

void SHA256::update(boost::asio::const_buffer data,unsigned long len)
{
    if(finalized)
    {
        //TODO handle this
    }
    
    SHA256_Update(&this->sha256_context, boost::asio::buffer_cast<const void *>(data),len);
}

std::string SHA256::final()
{
    if(finalized)
    {
        //TODO handle this
    }
    
    unsigned char digest[SHA256_DIGEST_LENGTH];
    
    SHA256_Final(digest,&this->sha256_context);
    this->finalized = true;
    
    return std::string(reinterpret_cast<char *>(&digest),sizeof(digest));
}

} // libbitcoin
