#include <bitcoin/format.hpp>

#include <boost/algorithm/string.hpp>
#include <bitcoin/primitives.hpp>
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

template <typename Point>
std::ostream& concat_point(std::ostream& stream, const Point& point)
{
    stream << point.hash << ":" << point.index;
    return stream;
}
std::ostream& operator<<(std::ostream& stream, const output_point& point)
{
    return concat_point(stream, point);
}

data_chunk decode_hex(std::string hex_str)
{
    // Trim the fat.
    boost::algorithm::trim(hex_str);
    data_chunk result(hex_str.size() / 2);
    for (size_t i = 0; i + 1 < hex_str.size(); i += 2)
    {
        BITCOIN_ASSERT(hex_str.size() - i >= 2);
        auto byte_begin = hex_str.begin() + i;
        auto byte_end = hex_str.begin() + i + 2;
        int val = stoi(std::string(byte_begin, byte_end), 0, 16);
        BITCOIN_ASSERT(val <= 0xff);
        result[i / 2] = val;
    }
    return result;
}

} // namespace libbitcoin

