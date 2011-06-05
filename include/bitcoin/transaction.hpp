#ifndef LIBBITCOIN_TRANSACTION_H
#define LIBBITCOIN_TRANSACTION_H

#include <string>
#include <vector>
#include <cstdint>

#include <bitcoin/util/sha256.hpp>

namespace libbitcoin {

struct transaction_input
{
    uint8_t hash[sha256::length];
    uint32_t index;
    std::string script;
    uint32_t sequence;
};
typedef std::vector<transaction_input> transaction_input_array;

struct transaction_output
{
    uint64_t value;
    std::string script;
};
typedef std::vector<transaction_output> transaction_output_array;

struct transaction
{
    void calculate_hash();

    uint8_t hash[sha256::length];
    uint32_t version;
    uint32_t locktime;
    
    transaction_input_array inputs;
    transaction_output_array outputs;
};
typedef std::vector<transaction> transaction_array;

} // libbitcoin

#endif

