#ifndef LIBBITCOIN_SERIALIZER_H
#define LIBBITCOIN_SERIALIZER_H

#include <boost/asio/streambuf.hpp>
#include <string>
#include <vector>

#include <bitcoin/messages.hpp>
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
    void write_network_address(message::network_address addr);
    void write_hash(hash_digest hash);
    void write_command(std::string command);

    data_chunk get_data() const;
private:
    data_chunk data_;
};

class end_of_stream
 : std::exception {};

class deserializer
{
public:
    deserializer(const data_chunk& stream);

    uint8_t read_byte();
    uint16_t read_2_bytes();
    uint32_t read_4_bytes();
    uint64_t read_8_bytes();
    uint64_t read_var_uint();
    data_chunk read_data(uint64_t n_bytes);
    message::network_address read_network_address();
    hash_digest read_hash();
    std::string read_fixed_len_str(size_t len);
private:
    typedef data_chunk::const_iterator const_iterator;

    const_iterator begin_, end_;
};

} // libbitcoin

#endif

