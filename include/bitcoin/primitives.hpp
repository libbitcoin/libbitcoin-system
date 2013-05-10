#ifndef LIBBITCOIN_PRIMITIVES_HPP
#define LIBBITCOIN_PRIMITIVES_HPP

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include <bitcoin/types.hpp>
#include <bitcoin/script.hpp>

namespace libbitcoin {

typedef std::vector<hash_digest> block_locator_type;

typedef std::array<uint8_t, 16> ip_address_type;

struct network_address_type
{
    uint32_t timestamp;
    uint64_t services;
    ip_address_type ip;
    uint16_t port;
};

enum class inventory_type_id
{
    error,
    transaction,
    block,
    none
};

struct inventory_vector_type
{
    inventory_type_id type;
    hash_digest hash;
};

typedef std::vector<inventory_vector_type> inventory_list;

struct header_type
{
    uint32_t magic;
    std::string command;
    uint32_t payload_length;
    // Ignored by version and verack commands
    uint32_t checksum;
};

struct version_type
{
    uint32_t version;
    uint64_t services;
    uint64_t timestamp;
    network_address_type address_me;
    network_address_type address_you;
    uint64_t nonce;
    std::string user_agent;
    uint32_t start_depth;
};

struct verack_type
{
};

struct get_address_type
{
};


struct get_blocks_type
{
    // 10 sequential hashes, then exponential samples until reaching genesis
    block_locator_type start_hashes;
    hash_digest hash_stop;
};

struct output_point
{
    hash_digest hash;
    uint32_t index;
};

typedef output_point input_point;
typedef std::vector<input_point> input_point_list;
typedef std::vector<output_point> output_point_list;

struct transaction_input_type
{
    output_point previous_output;
    script input_script;
    uint32_t sequence;
};

struct transaction_output_type
{
    uint64_t value;
    script output_script;
};

typedef std::vector<transaction_input_type> transaction_input_list;
typedef std::vector<transaction_output_type> transaction_output_list;

struct transaction_type
{
    uint32_t version;
    uint32_t locktime;
    transaction_input_list inputs;
    transaction_output_list outputs;
};
typedef std::vector<transaction_type> transaction_list;

struct block_type
{
    uint32_t version;
    hash_digest previous_block_hash;
    hash_digest merkle;
    uint32_t timestamp;
    uint32_t bits;
    uint32_t nonce;
    transaction_list transactions;
};

typedef std::vector<network_address_type> network_address_list;

struct address_type
{
    network_address_list addresses;
};

struct get_data_type
{
    inventory_list inventories;
};

struct inventory_type
{
    inventory_list inventories;
};

struct ping_type
{
    uint64_t nonce;
};
struct pong_type
{
    uint64_t nonce;
};

} // namespace libbitcoin

#endif

