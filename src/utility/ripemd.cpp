#include <bitcoin/utility/ripemd.hpp>

#include <boost/detail/endian.hpp>
#include <openssl/sha.h>

#include <bitcoin/utility/logger.hpp>

namespace libbitcoin {

short_hash generate_ripemd_hash(const data_chunk& chunk)
{
    hash_digest sha_hash;
    SHA256_CTX sha_ctx;
    SHA256_Init(&sha_ctx);
    SHA256_Update(&sha_ctx, &chunk[0], chunk.size());
    SHA256_Final(sha_hash.data(), &sha_ctx);

    short_hash ripemd_hash;
    RIPEMD160_CTX ripemd_ctx;
    RIPEMD160_Init(&ripemd_ctx);
    RIPEMD160_Update(&ripemd_ctx, sha_hash.data(), SHA256_DIGEST_LENGTH);
    RIPEMD160_Final(ripemd_hash.data(), &ripemd_ctx);

    return ripemd_hash;
}

} // namespace libbitcoin

