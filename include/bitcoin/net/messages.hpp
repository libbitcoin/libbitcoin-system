#ifndef LIBBITCOIN_NET_MESSAGES_H
#define LIBBITCOIN_NET_MESSAGES_H

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace libbitcoin {
namespace net {
namespace message {

typedef std::array<uint8_t, 32> hash_digest;
typedef std::array<uint8_t, 16> ip_address;

struct net_addr
{
    uint32_t timestamp;
    uint64_t services;
    ip_address ip_addr;
    uint16_t port;
};

enum class inv_type
{
    error,
    transaction,
    block,
    none
};

struct inv_vect
{
    inv_type type;
    hash_digest hash;
};

typedef std::vector<inv_vect> inv_list;

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
    net_addr addr_me;
    net_addr addr_you;
    uint64_t nonce;
    std::string sub_version_num;
    uint32_t start_height;
};

struct verack
{
};

struct getaddr
{
};

struct getblocks
{
    // 10 sequential hashes, then exponential samples until reaching genesis
    std::vector<hash_digest> locator_start_hashes;
    hash_digest hash_stop;
};

struct addr
{
    std::vector<net_addr> addr_list;
};

struct getdata
{
    inv_list invs;
};

struct inv
{
    inv_list invs;
};

} // message
} // net
} // libbitcoin

#endif

