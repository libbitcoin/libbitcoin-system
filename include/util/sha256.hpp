#ifndef LIBBITCOIN_SHA256_H
#define LIBBITCOIN_SHA256_H

#include <openssl/sha.h>
#include <string>
#include <stdint.h>

namespace libbitcoin {

class sha256
{
public:
    enum { length = SHA256_DIGEST_LENGTH };

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

    void final(unsigned char hash[SHA256_DIGEST_LENGTH]);
private:
    SHA256_CTX ctx_;
    unsigned char digest_[SHA256_DIGEST_LENGTH];
};

} // libbitcoin

#endif

