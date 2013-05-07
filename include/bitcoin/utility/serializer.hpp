#ifndef LIBBITCOIN_SERIALIZER_HPP
#define LIBBITCOIN_SERIALIZER_HPP

#include <boost/asio/streambuf.hpp>
#include <string>
#include <vector>

#include <bitcoin/primitives.hpp>
#include <bitcoin/types.hpp>

namespace libbitcoin {

class serializer
{
public:
    void write_byte(uint8_t v);
    void write_2_bytes(uint16_t v);
    void write_4_bytes(uint32_t v);
    void write_8_bytes(uint64_t v);
    void write_variable_uint(uint64_t v);
    void write_data(const data_chunk& other_data);
    void write_network_address(network_address_type addr);
    void write_hash(const hash_digest& hash);
    void write_short_hash(const short_hash& hash);
    void write_fixed_string(const std::string& command, size_t string_size);
    void write_string(const std::string& str);

    data_chunk data() const;
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
    uint64_t read_variable_uint();
    data_chunk read_data(uint64_t n_bytes);
    network_address_type read_network_address();
    hash_digest read_hash();
    short_hash read_short_hash();
    std::string read_fixed_string(size_t len);
    std::string read_string();
private:
    typedef data_chunk::const_iterator const_iterator;

    const_iterator begin_, end_;
};

} // namespace libbitcoin

#endif

