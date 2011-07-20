#ifndef LIBBITCOIN_SERIALIZER_H
#define LIBBITCOIN_SERIALIZER_H

#include <boost/asio/streambuf.hpp>
#include <string>
#include <vector>

#include <bitcoin/net/messages.hpp>
#include <bitcoin/types.hpp>

namespace libbitcoin {

class serializer
{
public:
    void write_byte(uint8_t v);
    void write_2_bytes(uint16_t v);
    void write_4_bytes(uint32_t v);
    void write_8_bytes(uint64_t v);
    void write_var_uint(uint64_t v);
    void write_data(const data_chunk& other_data);
    void write_net_addr(net::message::net_addr addr);
    void write_hash(hash_digest hash);
    void write_command(std::string command);

    data_chunk get_data() const;
private:
    data_chunk data_;
};

class deserializer
{
public:
    deserializer(const data_chunk& stream);

    uint8_t read_byte();
    uint16_t read_2_bytes();
    uint32_t read_4_bytes();
    uint64_t read_8_bytes();
    uint64_t read_var_uint();
    data_chunk read_raw_bytes(uint64_t n_bytes);
    net::message::net_addr read_net_addr();
    hash_digest read_hash();
    std::string read_fixed_len_str(size_t len);
private:
    const data_chunk& stream_;
    size_t pointer_;
};

} // libbitcoin

#endif

