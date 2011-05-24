#ifndef LIBBITCOIN_BLOCK_H
#define LIBBITCOIN_BLOCK_H

#include <cstdint>
#include <vector>

#include "transaction.hpp"
#include "util/sha256.hpp"

namespace libbitcoin {

class block
{
public:
    void calculate_hash();

    uint8_t hash[sha256::length];
    
    uint32_t version;
    uint8_t prev_hash[sha256::length];
    uint8_t merkle_root[sha256::length];
    uint32_t timestamp;
    uint32_t bits;
    uint8_t nonce[4];
    
    transaction_array transactions;
};

} // libbitcoin

#endif

