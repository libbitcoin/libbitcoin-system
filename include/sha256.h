#ifndef LIBBITCOIN_SHA256_H
#define LIBBITCOIN_SHA256_H

#include <boost/asio/buffer.hpp>
#include <openssl/sha.h>

namespace libbitcoin {

class SHA256
{
private:
    bool finalized;
    SHA256_CTX sha256_context;
public:
    SHA256();
    void update(const void *data, unsigned long len);
    void update(std::string data);
    void update(boost::asio::const_buffer data,unsigned long len);
    
    std::string final();
};

} // libbitcoin
#endif
