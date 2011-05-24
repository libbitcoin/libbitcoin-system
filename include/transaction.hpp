#ifndef LIBBITCOIN_TRANSACTION_H
#define LIBBITCOIN_TRANSACTION_H

#include <string>
#include <vector>
#include <stdint.h>

namespace libbitcoin {

struct transaction_input
{
    unsigned char hash[32];
    uint32_t index;
    std::string script;
    uint32_t sequence;
};

struct transaction_output
{
    uint64_t value;
    std::string script;
};

class transaction
{
public:
    void calculate_hash();

    unsigned char hash[32];
    uint32_t version;
    uint32_t locktime;
    
    std::vector<transaction_input> inputs;
    std::vector<transaction_output> outputs;
};

} // libbitcoin

#endif

