#ifndef LIBBITCOIN_DATA_HELPERS_H
#define LIBBITCOIN_DATA_HELPERS_H

#include <boost/range/iterator_range.hpp>
#include <boost/range/sub_range.hpp>
#include <boost/detail/endian.hpp>

#include <bitcoin/types.hpp>
#include <bitcoin/utility/assert.hpp>

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

    T value = 0;
    for (size_t i = 0; i < sizeof(T) && i < chunk.size(); ++i)
        value += static_cast<T>(chunk[i]) << (i * 8);
    return value;
}

template<typename T>
data_chunk uncast_type(T value, bool reverse=false)
{
    // TODO Future versions of boost will have boost::native_to_little(value);
    #ifdef BOOST_LITTLE_ENDIAN
        // do nothing
    #elif BOOST_BIG_ENDIAN
        reverse = !reverse;
    #else
        #error "Endian isn't defined!"
    #endif

    data_chunk chunk(sizeof(T));
    byte* value_begin = reinterpret_cast<byte*>(&value);
    std::copy(value_begin, value_begin + sizeof(T), chunk.begin());

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
        ss << std::setw(2) << std::setfill('0') << val;
    return ss.str();
}

data_chunk bytes_from_pretty(std::string byte_stream);

template <typename HashType>
HashType hash_from_pretty(std::string byte_stream)
{
    data_chunk raw_bytes = bytes_from_pretty(byte_stream);
    HashType result;
    BITCOIN_ASSERT(raw_bytes.size() == result.size());
    std::copy(raw_bytes.begin(), raw_bytes.end(), result.begin());
    return result;
}

// Python like range
//   for (char c: range(str, 0, 2))
template <typename RangeType>
boost::sub_range<RangeType> range(RangeType iterable,
    size_t start_offset=0, size_t end_offset=0)
{
    return boost::sub_range<RangeType>(
        iterable.begin() + start_offset, iterable.end() - end_offset);
}

} // namespace libbitcoin

#endif

