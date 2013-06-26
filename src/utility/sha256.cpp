#include <bitcoin/utility/sha256.hpp>
#include <bitcoin/format.hpp>

#include <boost/detail/endian.hpp>

namespace libbitcoin {

hash_digest generate_sha256_hash(const data_chunk& chunk)
{
    SHA256_CTX ctx;
    hash_digest digest;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, &chunk[0], chunk.size());
    SHA256_Final(digest.data(), &ctx);
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, digest.data(), sha256_digest_size);
    SHA256_Final(digest.data(), &ctx);
    // SSL gives us the hash backwards
    std::reverse(digest.begin(), digest.end());
    return digest;
}

uint32_t generate_sha256_checksum(const data_chunk& chunk)
{
    hash_digest hash = generate_sha256_hash(chunk);
    data_chunk begin_bytes(hash.rbegin(), hash.rbegin() + 4);
    return cast_chunk<uint32_t>(begin_bytes);
}

} // namespace libbitcoin

