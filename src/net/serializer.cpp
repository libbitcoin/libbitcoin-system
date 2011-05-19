#include "net/serializer.h"

#include <boost/detail/endian.hpp>
#include <iostream>

#include "net/messages.h"

namespace libbitcoin {
namespace net {

inline void copy_data(std::vector<char> &data, 
        const char* bytes, size_t len)
{
    for (size_t i = 0; i < len; i++)
        data.push_back(bytes[i]);
}

inline void copy_data_reverse(std::vector<char> &data, 
        const char* bytes, size_t len)
{
    for (int i = len - 1; i >= 0; i--) 
        data.push_back(bytes[i]);
}

template<typename T>
void write_data(std::vector<char> &data, T val, size_t len, bool reverse=false)
{
    char* bytes = reinterpret_cast<char*>(&val);
    #ifdef BOOST_LITTLE_ENDIAN
        // do nothing
    #elif BOOST_BIG_ENDIAN
        reverse = true;
    #else
        #error "Endian isn't defined!"
    #endif

    if (reverse)
        copy_data_reverse(data, bytes, len);
    else
        copy_data(data, bytes, len);
}

void serializer::write_byte(uint8_t v)
{
    data.push_back(v);
}

void serializer::write_4_bytes(uint32_t v)
{
    write_data(data, v, 4);
}

void serializer::write_8_bytes(uint64_t v)
{
    write_data(data, v, 8);
}

void serializer::write_net_addr(message::net_addr addr)
{
    write_8_bytes(addr.services);
    for (size_t i = 0; i < 16; i++)
        data.push_back(addr.ip_addr[i]);
    write_data(data, addr.port, 2, true);
}

void serializer::write_command(std::string command)
{
    constexpr size_t comm_len = 12;
    char comm_str[comm_len] = { 0 };
    command.copy(comm_str, comm_len);
    // should use std::copy
    for (size_t i = 0; i < comm_len; i++)
        data.push_back(comm_str[i]);
}

serializer::stream serializer::get_data()
{
    return data;
}

} // net
} // libbitcoin

