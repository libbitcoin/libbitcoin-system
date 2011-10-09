#include <bitcoin/types.hpp>

#include <bitcoin/util/assert.hpp>

namespace libbitcoin {

void extend_data(data_chunk& chunk, const data_chunk& other)
{
    chunk.insert(chunk.end(), other.cbegin(), other.cend());
}

data_chunk bytes_from_pretty(std::string byte_stream)
{
    std::stringstream ss;
    ss << std::hex << byte_stream;
    data_chunk stack;
    while (ss.good())
    {
        int val;
        ss >> val;
        stack.push_back(val);
    }
    return stack;
}

hash_digest hash_from_pretty(std::string byte_stream)
{
    std::stringstream ss;
    ss << std::hex << byte_stream;
    hash_digest hash;
    for (size_t i = 0; i < hash.size(); ++i)
    {
        int val;
        ss >> val;
        BITCOIN_ASSERT(val <= 0xff);
        hash[i] = val;
    }
    return hash;
}

} // libbitcoin

