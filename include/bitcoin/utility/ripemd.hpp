#ifndef LIBBITCOIN_RIPEMD_HPP
#define LIBBITCOIN_RIPEMD_HPP

#include <openssl/ripemd.h>
#include <cstdint>

#include <bitcoin/types.hpp>

namespace libbitcoin {

constexpr size_t ripemd_digest_size = RIPEMD160_DIGEST_LENGTH;
static_assert(ripemd_digest_size == 20, "RIPEMD160_DIGEST_LENGTH not defined correctly!");

short_hash generate_ripemd_hash(const data_chunk& chunk);

} // namespace libbitcoin

#endif

