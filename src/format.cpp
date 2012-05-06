#include <bitcoin/format.hpp>

#include <bitcoin/utility/assert.hpp>

namespace libbitcoin {

std::ostream& operator<<(std::ostream& stream, const data_chunk& data)
{
    stream << pretty_hex(data);
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const hash_digest& hash)
{
    stream << pretty_hex(hash);
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const short_hash& hash)
{
    stream << pretty_hex(hash);
    return stream;
}

data_chunk bytes_from_pretty(std::string byte_stream)
{
    data_chunk stack;
    for (auto it = byte_stream.begin(); it != byte_stream.end(); it += 2)
    {
        if (it != byte_stream.begin() && *it == ' ')
            ++it;
        BITCOIN_ASSERT(it + 1 != byte_stream.end());
        if (it + 1 == byte_stream.end())
            return data_chunk();
        std::istringstream ss(std::string(it, it + 2));
        int val;
        ss >> std::hex >> val;
        BITCOIN_ASSERT(val <= 0xff);
        stack.push_back(val);
    }
    return stack;
}

} // namespace libbitcoin

