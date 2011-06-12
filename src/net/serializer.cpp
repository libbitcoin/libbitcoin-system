#include "serializer.hpp"

#include <boost/detail/endian.hpp>
#include <algorithm>
#include <iostream>
#include <string>

#include <bitcoin/net/messages.hpp>
#include <bitcoin/util/assert.hpp>
#include <bitcoin/types.hpp>

namespace libbitcoin {
namespace net {

inline void copy_data(std::vector<byte> &data, 
        const byte* raw_bytes, size_t len)
{
    for (size_t i = 0; i < len; i++)
        data.push_back(raw_bytes[i]);
}

inline void copy_data_reverse(std::vector<byte> &data, 
        const byte* raw_bytes, size_t len)
{
    for (int i = len - 1; i >= 0; i--) 
        data.push_back(raw_bytes[i]);
}

template<typename T>
void write_data(std::vector<byte> &data, T val, size_t len, bool reverse=false)
{
    byte* raw_bytes = reinterpret_cast<byte*>(&val);
    #ifdef BOOST_LITTLE_ENDIAN
        // do nothing
    #elif BOOST_BIG_ENDIAN
        reverse = true;
    #else
        #error "Endian isn't defined!"
    #endif

    if (reverse)
        copy_data_reverse(data, raw_bytes, len);
    else
        copy_data(data, raw_bytes, len);
}

void serializer::write_byte(uint8_t v)
{
    data_.push_back(v);
}

void serializer::write_4_bytes(uint32_t v)
{
    write_data(data_, v, 4);
}

void serializer::write_8_bytes(uint64_t v)
{
    write_data(data_, v, 8);
}

void serializer::write_net_addr(message::net_addr addr)
{
    write_8_bytes(addr.services);
    for (size_t i = 0; i < 16; i++)
        data_.push_back(addr.ip_addr[i]);
    write_data(data_, addr.port, 2, true);
}

void serializer::write_command(std::string command)
{
    constexpr size_t comm_len = 12;
    char comm_str[comm_len] = { 0 };
    command.copy(comm_str, comm_len);
    // should use std::copy
    for (size_t i = 0; i < comm_len; i++)
        data_.push_back(comm_str[i]);
}

data_chunk serializer::get_data()
{
    return data_;
}

template<typename T>
T consume_object(const data_chunk& stream, size_t& pointer)
{
    T* val = reinterpret_cast<T*>(&stream[pointer]);
    pointer += sizeof(T);
    return *val;
}

template<typename T>
T read_data(const data_chunk& data, size_t& pointer, bool reverse=false)
{
    #ifdef BOOST_LITTLE_ENDIAN
        // do nothing
    #elif BOOST_BIG_ENDIAN
        reverse = true;
    #else
        #error "Endian isn't defined!"
    #endif

    T val;
    if (reverse) 
    {
        const char* real_bytes = reinterpret_cast<const char*>(&data[pointer]);
        std::string reverse_bytes(real_bytes, sizeof(T));
        std::reverse(reverse_bytes.begin(), reverse_bytes.end());
        val = *reinterpret_cast<const T*>(reverse_bytes.c_str());
    }
    else 
    {
        val = *reinterpret_cast<const T*>(&data[pointer]);
    }
    pointer += sizeof(T);
    return val;
}

deserializer::deserializer(const data_chunk& stream)
 : stream_(stream), pointer_(0)
{
}

uint8_t deserializer::read_byte()
{
    return read_data<uint8_t>(stream_, pointer_);
}

uint16_t deserializer::read_2_bytes()
{
    return read_data<uint16_t>(stream_, pointer_);
}

uint32_t deserializer::read_4_bytes()
{
    return read_data<uint32_t>(stream_, pointer_);
}

uint64_t deserializer::read_8_bytes()
{
    return read_data<uint64_t>(stream_, pointer_);
}

uint64_t deserializer::read_var_uint()
{
    uint8_t length = read_byte();
    uint64_t value = 0;
    if (length < 0xfd)
        value = length;
    else if (length == 0xfd)
        value += read_2_bytes();
    else if (length == 0xfe)
        value += read_4_bytes();
    else if (length == 0xff)
        value += read_8_bytes();
    return value;
}

template<unsigned int N>
void read_bytes(const data_chunk& stream, size_t& pointer,
        std::array<uint8_t, N>& byte_array, bool reverse=false)
{
    #ifdef BOOST_LITTLE_ENDIAN
        // do nothing
    #elif BOOST_BIG_ENDIAN
        reverse = true;
    #else
        #error "Endian isn't defined!"
    #endif

    if (reverse)
    {
        std::reverse_copy(
                stream.begin() + pointer,
                stream.begin() + pointer + byte_array.size(),
                byte_array.begin());
    }
    else
    {
        std::copy(
                stream.begin() + pointer,
                stream.begin() + pointer + byte_array.size(),
                byte_array.begin());
    }
    pointer += byte_array.size();
}

message::net_addr deserializer::read_net_addr()
{
    message::net_addr addr;
    addr.services = read_8_bytes();
    // Read IP address
    read_bytes(stream_, pointer_, addr.ip_addr);
    addr.port = read_data<uint16_t>(stream_, pointer_, true);
    return addr;
}

message::hash_digest deserializer::read_hash()
{
    message::hash_digest hash;
    read_bytes(stream_, pointer_, hash, true);
    return hash;
}

std::string deserializer::read_fixed_len_str(size_t len)
{
    BITCOIN_ASSERT(pointer_ + len <= stream_.size());
    std::string ret(
            stream_.begin() + pointer_, 
            stream_.begin() + pointer_ + len);
    pointer_ += len;
    // Removes trailing 0s... Needed for string comparisons
    return ret.c_str();
}

} // net
} // libbitcoin

