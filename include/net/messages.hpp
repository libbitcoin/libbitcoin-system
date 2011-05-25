#ifndef LIBBITCOIN_NET_MESSAGES_H
#define LIBBITCOIN_NET_MESSAGES_H

#include <cstdint>
#include <string>

namespace libbitcoin {
namespace net {
namespace message {

struct net_addr
{
    uint64_t services;
    uint8_t ip_addr[16];
    uint16_t port;
};

struct header
{
    uint32_t magic;
    std::string command;
    uint32_t payload_length;
    // Ignored by version and verack commands
    uint32_t checksum;
};

struct message
{
    // Empty
};

struct version : public message 
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

} // message
} // net
} // libbitcoin

#endif

