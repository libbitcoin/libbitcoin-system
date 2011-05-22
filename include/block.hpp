#ifndef LIBBITCOIN_BLOCK_H
#define LIBBITCOIN_BLOCK_H

#include <stdint.h>
#include <vector>

#include "transaction.hpp"

namespace libbitcoin {

class block
{
public:
    void calculate_hash();

    unsigned char hash_[32];
    
    uint32_t version_;
    unsigned char prev_hash_[32];
    unsigned char merkle_root_[32];
    uint32_t timestamp_;
    uint32_t bits_;
    unsigned char nonce_[4];
    
    std::vector<transaction> transactions_;
};

} // libbitcoin

#endif

