#include <bitcoin/data_helpers.hpp>

#include <bitcoin/utility/assert.hpp>

namespace libbitcoin {

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

hash_digest hash_from_pretty(std::string byte_stream)
{
    data_chunk raw_bytes = bytes_from_pretty(byte_stream);
    BITCOIN_ASSERT(raw_bytes.size() == 32);
    hash_digest hash;
    std::copy(raw_bytes.begin(), raw_bytes.end(), hash.begin());
    return hash;
}

} // libbitcoin

