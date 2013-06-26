#include <bitcoin/utility/serializer.hpp>

#include <algorithm>
#include <string>

#include <bitcoin/utility/assert.hpp>

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

void serializer::write_network_address(network_address_type addr)
{
    write_8_bytes(addr.services);
    extend_data(data_, addr.ip);
    extend_data(data_, uncast_type(addr.port, true));
}

void serializer::write_hash(const hash_digest& hash)
{
    data_.insert(data_.end(), hash.rbegin(), hash.rend());
}

void serializer::write_short_hash(const short_hash& hash)
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

} // namespace libbitcoin

