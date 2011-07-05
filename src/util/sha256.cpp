#include <bitcoin/util/sha256.hpp>

#include <boost/detail/endian.hpp>

#include <bitcoin/util/logger.hpp>

namespace libbitcoin {

sha256::sha256()
{
    SHA256_Init(&ctx_);
}

template<typename T>
void update_ctx(SHA256_CTX &ctx, T val)
{
    SHA256_Update(&ctx, &val, sizeof(T));
}

void sha256::push_byte(uint8_t v)
{
    update_ctx(ctx_, v);
}
void sha256::push_2_bytes(uint16_t v)
{
    update_ctx(ctx_, v);
}
void sha256::push_4_bytes(uint32_t v)
{
    update_ctx(ctx_, v);
}
void sha256::push_8_bytes(uint64_t v)
{
    update_ctx(ctx_, v);
}

void sha256::push_str(const std::string& val)
{
#ifdef BOOST_LITTLE_ENDIAN
    SHA256_Update(&ctx_, val.c_str(), val.length());
#elif BOOST_BIG_ENDIAN
    #error Platform not supported
#else
    #error Platform not supported
#endif
}

void sha256::push_var_uint(uint64_t var_uint)
{
    if (var_uint < 0xfd)
    {
        uint8_t count = var_uint;
        logger(LOG_DEBUG) << var_uint << " " << sizeof(count) << "\n";
        push_byte(count);
    }
    else if (var_uint <= 0xffff)
    {
        const uint8_t prefix = 0xFD;
        uint16_t count = var_uint;
        logger(LOG_DEBUG) << var_uint << " " << sizeof(count) << "\n";
        push_byte(prefix);
        push_2_bytes(count);
    }
    else if (var_uint <= 0xffffffff)
    {
        const uint8_t prefix = 0xFE;
        uint32_t count = var_uint;
        logger(LOG_DEBUG) << var_uint << " " << sizeof(count) << "\n";
        push_byte(prefix);
        push_4_bytes(count);
    }
    else if (var_uint <= 0xffffffffffffffff)
    {
        const uint8_t prefix = 0xFF;
        uint64_t count = var_uint;
        logger(LOG_DEBUG) << var_uint << " " << sizeof(count) << "\n";
        push_byte(prefix);
        push_8_bytes(count);
    }
    else
    {
        //TODO handle
        return;
    }
}

void sha256::push_data(const data_chunk& data)
{
    for (auto it = data.cbegin(); it != data.cend(); ++it)
        push_byte(*it);
}

hash_digest sha256::finalize()
{
    byte hash_carr[length];
    finalize(hash_carr);
    hash_digest hash;
    std::copy(hash_carr, hash_carr + length, hash.begin());
    return hash;
}

void sha256::finalize(byte hash[SHA256_DIGEST_LENGTH])
{
    SHA256_Final(digest_, &ctx_);
    SHA256_Init(&ctx_);
    SHA256_Update(&ctx_, digest_, sizeof(digest_));
    SHA256_Final(hash, &ctx_);
}

uint32_t generate_sha256_checksum(const data_chunk& chunk)
{
    sha256 hasher;
    hasher.push_data(chunk);
    hash_digest hash = hasher.finalize();
    data_chunk begin_bytes(hash.cbegin(), hash.cbegin() + 4);
    return cast_chunk<uint32_t>(begin_bytes);
}

} // libbitcoin

