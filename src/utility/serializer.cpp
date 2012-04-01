#include <bitcoin/utility/serializer.hpp>

#include <algorithm>
#include <string>

#include <bitcoin/messages.hpp>
#include <bitcoin/utility/assert.hpp>
#include <bitcoin/format.hpp>
#include <bitcoin/types.hpp>

namespace libbitcoin {

void serializer::write_byte(uint8_t v)
{
    data_.push_back(v);
}

void serializer::write_2_bytes(uint16_t v)
{
    extend_data(data_, uncast_type(v));
}

void serializer::write_4_bytes(uint32_t v)
{
    extend_data(data_, uncast_type(v));
}

void serializer::write_8_bytes(uint64_t v)
{
    extend_data(data_, uncast_type(v));
}

void serializer::write_variable_uint(uint64_t v)
{
    if (v < 0xfd)
    {
        write_byte(v);
    }
    else if (v <= 0xffff)
    {
        write_byte(0xfd);
        write_2_bytes(v);
    }
    else if (v <= 0xffffffff)
    {
        write_byte(0xfe);
        write_4_bytes(v);
    }
    else
    {
        write_byte(0xff);
        write_8_bytes(v);
    }
}

void serializer::write_data(const data_chunk& other_data)
{
    extend_data(data_, other_data);
}

void serializer::write_network_address(message::network_address addr)
{
    write_8_bytes(addr.services);
    extend_data(data_, addr.ip);
    extend_data(data_, uncast_type(addr.port, true));
}

void serializer::write_hash(hash_digest hash)
{
    data_.insert(data_.end(), hash.rbegin(), hash.rend());
}

void serializer::write_fixed_string(const std::string& command,
    size_t string_size)
{
    BITCOIN_ASSERT(command.size() <= string_size);
    data_chunk raw_string(string_size);
    std::copy(command.begin(), command.end(), raw_string.begin());
    extend_data(data_, raw_string);
}

void serializer::write_string(const std::string& str)
{
    write_variable_uint(str.size());
    write_fixed_string(str, str.size());
}

data_chunk serializer::data() const
{
    return data_;
}

template<typename ConstIterator>
void check_distance(ConstIterator begin, ConstIterator end, size_t distance)
{
    if (static_cast<size_t>(std::distance(begin, end)) < distance)
        throw end_of_stream();
}

template<typename T, typename Iterator>
T read_data_impl(Iterator& begin, Iterator end, bool reverse=false)
{
    check_distance(begin, end, sizeof(T));
    data_chunk chunk(begin, begin + sizeof(T));
    T val = cast_chunk<T>(chunk, reverse);
    begin += sizeof(T);
    return val;
}

deserializer::deserializer(const data_chunk& stream)
  : begin_(stream.cbegin()), end_(stream.cend())
{
}

uint8_t deserializer::read_byte()
{
    check_distance(begin_, end_, 1);
    return *(begin_++);
}

uint16_t deserializer::read_2_bytes()
{
    return read_data_impl<uint16_t>(begin_, end_);
}

uint32_t deserializer::read_4_bytes()
{
    return read_data_impl<uint32_t>(begin_, end_);
}

uint64_t deserializer::read_8_bytes()
{
    return read_data_impl<uint64_t>(begin_, end_);
}

uint64_t deserializer::read_variable_uint()
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

template<unsigned int N, typename Iterator>
void read_bytes(Iterator& begin, const Iterator& end,
    std::array<uint8_t, N>& byte_array, bool reverse=false)
{
    check_distance(begin, end, byte_array.size());
    #ifdef BOOST_LITTLE_ENDIAN
        // do nothing
    #elif BOOST_BIG_ENDIAN
        reverse = !reverse;
    #else
        #error "Endian isn't defined!"
    #endif

    if (reverse)
        std::reverse_copy(
            begin, begin + byte_array.size(), byte_array.begin());
    else
        std::copy(begin, begin + byte_array.size(), byte_array.begin());
    begin += byte_array.size();
}

data_chunk deserializer::read_data(uint64_t n_bytes)
{
    check_distance(begin_, end_, n_bytes);
    data_chunk raw_bytes;
    for (uint64_t i = 0; i < n_bytes; ++i)
        raw_bytes.push_back(read_byte());
    return raw_bytes;
}

message::network_address deserializer::read_network_address()
{
    message::network_address addr;
    addr.services = read_8_bytes();
    // Read IP address
    read_bytes<16>(begin_, end_, addr.ip);
    addr.port = read_data_impl<uint16_t>(begin_, end_, true);
    return addr;
}

hash_digest deserializer::read_hash()
{
    hash_digest hash;
    read_bytes<32>(begin_, end_, hash, true);
    return hash;
}

std::string deserializer::read_fixed_string(size_t len)
{
    data_chunk string_bytes = read_data(len);
    std::string result(string_bytes.begin(), string_bytes.end());
    // Removes trailing 0s... Needed for string comparisons
    return result.c_str();
}

std::string deserializer::read_string()
{
    uint64_t string_size = read_variable_uint();
    return read_fixed_string(string_size);
}

} // namespace libbitcoin

