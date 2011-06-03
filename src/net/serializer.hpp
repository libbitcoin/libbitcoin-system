#ifndef LIBBITCOIN_NET_SERIALIZER_H
#define LIBBITCOIN_NET_SERIALIZER_H

#include <boost/asio/streambuf.hpp>
#include <string>
#include <vector>

#include "bitcoin/net/messages.hpp"

namespace libbitcoin {
namespace net {

class serializer
{
public:
    typedef std::vector<char> stream;

    void write_byte(uint8_t v);
    void write_4_bytes(uint32_t v);
    void write_8_bytes(uint64_t v);
    void write_net_addr(message::net_addr addr);
    void write_command(std::string command);

    stream get_data();
private:
    stream data_;
};

class deserializer
{
public:
    deserializer(const serializer::stream& stream);

    uint8_t read_byte();
    uint16_t read_2_bytes();
    uint32_t read_4_bytes();
    uint64_t read_8_bytes();
    uint64_t read_var_uint();
    message::net_addr read_net_addr();
    message::hash_digest read_hash();
    std::string read_fixed_len_str(size_t len);
private:
    const serializer::stream& stream_;
    size_t pointer_;
};

} // net
} // libbitcoin

#endif
