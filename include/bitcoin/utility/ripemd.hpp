#ifndef LIBBITCOIN_RIPEMD_H
#define LIBBITCOIN_RIPEMD_H

#include <openssl/ripemd.h>
#include <cstdint>

#include <bitcoin/types.hpp>

namespace libbitcoin {

constexpr size_t ripemd_length = RIPEMD160_DIGEST_LENGTH;

short_hash generate_ripemd_hash(const data_chunk& chunk);

} // libbitcoin

#endif

