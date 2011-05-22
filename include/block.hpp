#ifndef LIBBITCOIN_BLOCK_H
#define LIBBITCOIN_BLOCK_H

#include <stdint.h>
#include <vector>

#include "transaction.h"

namespace libbitcoin {

class block
{
public:
    unsigned char hash[32];
    
    uint32_t version;
    unsigned char prev_hash[32];
    unsigned char merkle_root[32];
    uint32_t timestamp;
    uint32_t bits;
    unsigned char nonce[4];
    
    std::vector<transaction> transactions;
    
    void calculate_hash();
};

}
#endif
