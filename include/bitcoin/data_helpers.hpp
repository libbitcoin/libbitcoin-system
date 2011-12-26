#ifndef LIBBITCOIN_DATA_HELPERS_H
#define LIBBITCOIN_DATA_HELPERS_H

namespace libbitcoin {

template<typename D, typename T>
void extend_data(D& chunk, const T& other)
{
    chunk.insert(std::end(chunk), std::begin(other), std::end(other));
}

template<typename T>
T cast_chunk(data_chunk chunk, bool reverse=false)
{
    #ifdef BOOST_LITTLE_ENDIAN
        // do nothing
    #elif BOOST_BIG_ENDIAN
        reverse = !reverse;
    #else
        #error "Endian isn't defined!"
    #endif

    if (reverse)
        std::reverse(begin(chunk), end(chunk));

    T val = 0;
    for (size_t i = 0; i < sizeof(T) && i < chunk.size(); ++i)
        val += static_cast<T>(chunk[i]) << (i*8);
    return val;
}

template<typename T>
data_chunk uncast_type(T val, bool reverse=false)
{
    #ifdef BOOST_LITTLE_ENDIAN
        // do nothing
    #elif BOOST_BIG_ENDIAN
        reverse = !reverse;
    #else
        #error "Endian isn't defined!"
    #endif

    data_chunk chunk;
    for (size_t i = 0; i < sizeof(T); ++i)
        chunk.push_back(reinterpret_cast<byte*>(&val)[i]);

    if (reverse)
        std::reverse(begin(chunk), end(chunk));
    return chunk;
}

template<typename T>
std::string pretty_hex(T data)
{
    std::stringstream ss;
    ss << std::hex;
    for (int val: data)
        ss << std::setw(2) << std::setfill('0') << val << ' ';
    // Remove end ' '
    std::string ret = ss.str();
    if (ret.size() == 0)
        return "";
    ret.resize(ret.size() - 1);
    return ret;
}

data_chunk bytes_from_pretty(std::string byte_stream);
hash_digest hash_from_pretty(std::string byte_stream);

}

#endif