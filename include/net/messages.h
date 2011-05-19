#ifndef LIBBITCOIN_NET_MESSAGES_H
#define LIBBITCOIN_NET_MESSAGES_H

#include <boost/cstdint.hpp>
#include <string>

namespace libbitcoin {
namespace net {
namespace message {

using namespace boost;

struct net_addr
{
    uint_fast64_t services;
    uint_fast8_t ip_addr[16];
    uint_fast16_t port;
};

struct message
{
    // Empty
};

struct version : public message 
{
    uint_fast32_t version;
    uint_fast64_t services;
    uint_fast64_t timestamp;
    net_addr addr_me;
    net_addr addr_you;
    uint_fast64_t nonce;
    std::string sub_version_num;
    uint_fast32_t start_height;
};

} // message
} // net
} // libbitcoin

#endif

