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

    unsigned char hash[32];
    
    uint32_t version;
    unsigned char prev_hash[32];
    unsigned char merkle_root[32];
    uint32_t timestamp;
    uint32_t bits;
    unsigned char nonce[4];
    
    typedef std::vector<transaction> transaction_list;
    std::vector<transaction> transactions;
};

} // libbitcoin

#endif

