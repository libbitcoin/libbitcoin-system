#ifndef LIBBITCOIN_SERIALIZER_HPP
#define LIBBITCOIN_SERIALIZER_HPP

#include <boost/asio/streambuf.hpp>
#include <string>
#include <vector>

#include <bitcoin/format.hpp>
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

template <typename Iterator>
class deserializer
{
public:
    deserializer(const Iterator begin, const Iterator end)
      : begin_(begin), end_(end) {}

    uint8_t read_byte()
    {
        check_distance(begin_, end_, 1);
        return *(begin_++);
    }
    uint16_t read_2_bytes()
    {
        return read_data_impl<uint16_t>(begin_, end_);
    }
    uint32_t read_4_bytes()
    {
        return read_data_impl<uint32_t>(begin_, end_);
    }
    uint64_t read_8_bytes()
    {
        return read_data_impl<uint64_t>(begin_, end_);
    }

    uint64_t read_variable_uint()
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

    data_chunk read_data(uint64_t n_bytes)
    {
        check_distance(begin_, end_, n_bytes);
        data_chunk raw_bytes;
        for (uint64_t i = 0; i < n_bytes; ++i)
            raw_bytes.push_back(read_byte());
        return raw_bytes;
    }

    network_address_type read_network_address()
    {
        network_address_type addr;
        addr.services = read_8_bytes();
        // Read IP address
        read_bytes<16>(begin_, end_, addr.ip);
        addr.port = read_data_impl<uint16_t>(begin_, end_, true);
        return addr;
    }

    hash_digest read_hash()
    {
        hash_digest hash;
        read_bytes<32>(begin_, end_, hash, true);
        return hash;
    }

    short_hash read_short_hash()
    {
        short_hash hash;
        read_bytes<20>(begin_, end_, hash, true);
        return hash;
    }

    std::string read_fixed_string(size_t len)
    {
        data_chunk string_bytes = read_data(len);
        std::string result(string_bytes.begin(), string_bytes.end());
        // Removes trailing 0s... Needed for string comparisons
        return result.c_str();
    }

    std::string read_string()
    {
        uint64_t string_size = read_variable_uint();
        return read_fixed_string(string_size);
    }

private:
    // Try to advance iterator 'distance' incremenets forwards.
    // Throw if we prematurely reach the end.
    static void check_distance(Iterator begin, Iterator end, size_t distance)
    {
        for (size_t i = 0; i < distance; ++i)
        {
            // Is this a valid byte?
            if (begin == end)
                throw end_of_stream();
            // If so move to next value.
            ++begin;
        }
    }

    template<typename T>
    static T read_data_impl(Iterator& begin, Iterator end, bool reverse=false)
    {
        check_distance(begin, end, sizeof(T));
        data_chunk chunk(begin, begin + sizeof(T));
        T val = cast_chunk<T>(chunk, reverse);
        begin += sizeof(T);
        return val;
    }

    template<unsigned int N>
    static void read_bytes(Iterator& begin, const Iterator& end,
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

    Iterator begin_, end_;
};

template <typename Iterator>
deserializer<Iterator> make_deserializer(
    const Iterator begin, const Iterator end)
{
    return deserializer<Iterator>(begin, end);
}

} // namespace libbitcoin

#endif

