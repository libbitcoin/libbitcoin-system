#ifndef LIBBITCOIN_SHA256_HPP
#define LIBBITCOIN_SHA256_HPP

#include <openssl/sha.h>
#include <cstdint>

#include <bitcoin/types.hpp>

namespace libbitcoin {

constexpr size_t sha256_digest_size = SHA256_DIGEST_LENGTH;
static_assert(sha256_digest_size == 32, "SHA256_DIGEST_LENGTH not defined correctly!");

hash_digest generate_sha256_hash(const data_chunk& data);
uint32_t generate_sha256_checksum(const data_chunk& data);

} // namespace libbitcoin

#endif

