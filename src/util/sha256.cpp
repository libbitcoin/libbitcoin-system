#include <bitcoin/util/sha256.hpp>

#include <boost/detail/endian.hpp>

#include <bitcoin/util/logger.hpp>

namespace libbitcoin {

hash_digest generate_sha256_hash(const data_chunk& chunk)
{
    SHA256_CTX ctx;
    hash_digest digest;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, &chunk[0], chunk.size());
    SHA256_Final(digest.data(), &ctx);
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, digest.data(), sha256_length);
    SHA256_Final(digest.data(), &ctx);
    return digest;
}

uint32_t generate_sha256_checksum(const data_chunk& chunk)
{
    hash_digest hash = generate_sha256_hash(chunk);
    data_chunk begin_bytes(hash.cbegin(), hash.cbegin() + 4);
    return cast_chunk<uint32_t>(begin_bytes);
}

} // libbitcoin

