#ifndef LIBBITCOIN_NET_SERIALIZER_H
#define LIBBITCOIN_NET_SERIALIZER_H

#include <string>
#include <vector>

#include "net/messages.h"

namespace libbitcoin {
namespace net {

class serializer
{
public:
    void write_byte(uint8_t v);
    void write_4_bytes(uint32_t v);
    void write_8_bytes(uint64_t v);
    void write_net_addr(message::net_addr addr);
    void write_command(std::string command);
    std::string str();
private:
    std::vector<char> data;
};

class deserializer
{
public:
};

} // net
} // libbitcoin

#endif
