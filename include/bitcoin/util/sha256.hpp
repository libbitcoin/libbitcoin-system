#ifndef LIBBITCOIN_SHA256_H
#define LIBBITCOIN_SHA256_H

#include <openssl/sha.h>
#include <string>
#include <cstdint>

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

    SHA256_CTX& ctx() { return ctx_; }

    void finalize(unsigned char hash[length]);
private:
    SHA256_CTX ctx_;
    unsigned char digest_[length];
};

} // libbitcoin

#endif

