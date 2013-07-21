#include <bitcoin/format.hpp>

#include <boost/algorithm/string.hpp>
#include <bitcoin/utility/assert.hpp>

namespace libbitcoin {

std::ostream& operator<<(std::ostream& stream, const data_chunk& data)
{
    stream << encode_hex(data);
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const hash_digest& hash)
{
    stream << encode_hex(hash);
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const short_hash& hash)
{
    stream << encode_hex(hash);
    return stream;
}

data_chunk decode_hex(std::string hex_str)
{
    // Trim the fat.
    boost::algorithm::trim(hex_str);
    data_chunk result(hex_str.size() / 2);
    for (size_t i = 0; i < hex_str.size() - 1; i += 2)
    {
        BITCOIN_ASSERT(hex_str.size() - i >= 2);
        int val;
        auto byte_begin = hex_str.begin() + i;
        auto byte_end = hex_str.begin() + i + 2;
        std::istringstream ss(std::string(byte_begin, byte_end));
        ss >> std::hex >> val;
        BITCOIN_ASSERT(val <= 0xff);
        result[i / 2] = val;
    }
    return result;
}

} // namespace libbitcoin

