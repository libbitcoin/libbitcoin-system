#ifndef LIBBITCOIN_MESSAGES_H
#define LIBBITCOIN_MESSAGES_H

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include <bitcoin/types.hpp>
#include <bitcoin/script.hpp>

namespace libbitcoin {
namespace message {

typedef std::vector<hash_digest> block_locator;

typedef std::array<uint8_t, 16> ip_address;

struct network_address
{
    uint32_t timestamp;
    uint64_t services;
    ip_address ip;
    uint16_t port;
};

enum class inventory_type
{
    error,
    transaction,
    block,
    none
};

struct inventory_vector
{
    inventory_type type;
    hash_digest hash;
};

typedef std::vector<inventory_vector> inventory_list;

struct header
{
    uint32_t magic;
    std::string command;
    uint32_t payload_length;
    // Ignored by version and verack commands
    uint32_t checksum;
};

struct version
{
    uint32_t version;
    uint64_t services;
    uint64_t timestamp;
    network_address address_me;
    network_address address_you;
    uint64_t nonce;
    std::string user_agent;
    uint32_t start_height;
};

struct verack
{
};

struct get_address
{
};


struct get_blocks
{
    // 10 sequential hashes, then exponential samples until reaching genesis
    block_locator locator_start_hashes;
    hash_digest hash_stop;
};

struct output_point
{
    hash_digest hash;
    uint32_t index;
};

// Not in the protocol but still useful
typedef output_point input_point;

struct transaction_input
{
    output_point previous_output;
    script input_script;
    uint32_t sequence;
};

struct transaction_output
{
    uint64_t value;
    script output_script;
};

typedef std::vector<transaction_input> transaction_input_list;
typedef std::vector<transaction_output> transaction_output_list;

struct transaction
{
    uint32_t version;
    uint32_t locktime;
    transaction_input_list inputs;
    transaction_output_list outputs;
};
typedef std::vector<transaction> transaction_list;

struct block
{
    uint32_t version;
    hash_digest previous_block_hash;
    hash_digest merkle;
    uint32_t timestamp;
    uint32_t bits;
    uint32_t nonce;
    transaction_list transactions;
};

struct address
{
    std::vector<network_address> addresses;
};

struct get_data
{
    inventory_list inventories;
};

struct inventory
{
    inventory_list inventories;
};

} // message
} // libbitcoin

#endif

