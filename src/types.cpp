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
        BITCOIN_ASSERT(val <= 0xff);
        stack.push_back(val);
    }
    return stack;
}

hash_digest hash_from_pretty(std::string byte_stream)
{
    data_chunk raw_bytes = bytes_from_pretty(byte_stream);
    BITCOIN_ASSERT(raw_bytes.size() == 32);
    hash_digest hash;
    std::copy(raw_bytes.begin(), raw_bytes.end(), hash.begin());
    return hash;
}

} // libbitcoin

