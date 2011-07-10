#ifndef LIBBITCOIN_SHA256_H
#define LIBBITCOIN_SHA256_H

#include <openssl/sha.h>
#include <cstdint>

#include <bitcoin/types.hpp>

namespace libbitcoin {

constexpr size_t sha256_length = SHA256_DIGEST_LENGTH;

hash_digest generate_sha256_hash(const data_chunk& chunk);
uint32_t generate_sha256_checksum(const data_chunk& chunk);

} // libbitcoin

#endif

