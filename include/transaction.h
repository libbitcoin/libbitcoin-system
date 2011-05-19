#ifndef LIBBITCOIN_TRANSACTION_H
#define LIBBITCOIN_TRANSACTION_H

#include <string>
#include <vector>
#include <stdint.h>

#include "SHA256.h"

namespace libbitcoin {

class transaction_input
{
    SHA256 hash;
    uint_fast32_t index;
    std::string script;
    uint_fast32_t sequence;
};

struct transaction_output
{
    uint_fast64_t value;
    std::string script;
};

class transaction
{
    SHA256 hash;
    uint_fast32_t version;
    uint_fast32_t locktime;
    
    std::vector<transaction_input> inputs;
    std::vector<transaction_output> outputs;
    
    SHA256 hash();
};
}
#endif

