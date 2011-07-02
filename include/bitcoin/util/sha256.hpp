#ifndef LIBBITCOIN_SHA256_H
#define LIBBITCOIN_SHA256_H

#include <openssl/sha.h>
#include <string>
#include <cstdint>

#include <bitcoin/types.hpp>

namespace libbitcoin {

class sha256
{
public:
    static const size_t length = SHA256_DIGEST_LENGTH;

    sha256();
    
    template<typename T>
    sha256& operator<<(T& val)
    {
        SHA256_Update(&ctx_, &val, sizeof(T));
        return *this;
    }

    void push_var_uint(uint64_t var_uint);
    void push_str(const std::string& val);
    void push_data(const data_chunk& data);

    SHA256_CTX& ctx() { return ctx_; }

    data_chunk finalize();
    // Deprecated
    void finalize(byte hash[length]);
private:
    SHA256_CTX ctx_;
    byte digest_[length];
};

uint32_t generate_sha256_checksum(const data_chunk& chunk);

} // libbitcoin

#endif

