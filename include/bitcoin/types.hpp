#ifndef LIBBITCOIN_TYPES_H
#define LIBBITCOIN_TYPES_H

#include <boost/detail/endian.hpp>
#include <boost/asio.hpp>
#include <array>
#include <memory>
#include <sstream>
#include <iomanip>
#include <vector>

#include <bitcoin/net/types.hpp>

namespace libbitcoin {

using std::shared_ptr;
using boost::asio::io_service;

class dialect;
typedef shared_ptr<dialect> dialect_ptr;

class storage;
typedef shared_ptr<storage> storage_ptr;

typedef shared_ptr<io_service> service_ptr;
typedef shared_ptr<io_service::work> work_ptr;
typedef shared_ptr<io_service::strand> strand_ptr;

typedef std::array<uint8_t, 32> hash_digest;
typedef std::array<uint8_t, 20> short_hash;

typedef unsigned char byte;
typedef std::vector<byte> data_chunk;

typedef std::vector<hash_digest> block_locator;

void extend_data(data_chunk& chunk, const data_chunk& other);

template<typename T>
T cast_chunk(data_chunk chunk, bool reverse=false)
{
    #ifdef BOOST_LITTLE_ENDIAN
        reverse = !reverse;
    #elif BOOST_BIG_ENDIAN
        // do nothing
    #else
        #error "Endian isn't defined!"
    #endif

    if (reverse)
        std::reverse(chunk.begin(), chunk.end());

    T val = 0;
    for (size_t i = 0; i < sizeof(T); ++i)
        val += static_cast<T>(chunk[i]) << (i*8);
    return val;
}

template<typename T>
data_chunk uncast_type(T val)
{
    data_chunk chunk;
    for (size_t i = 0; i < sizeof(T); ++i)
        chunk.push_back(reinterpret_cast<byte*>(&val)[i]);
    return chunk;
}

template<typename T>
std::string hexlify(T data)
{
    std::stringstream ss;
    ss << std::hex;
    for (int val: data)
        ss << std::setw(2) << std::setfill('0') << val << ' ';
    // Remove end ' '
    std::string ret = ss.str();
    ret.resize(ret.size() - 1);
    return ret;
}

} // libbitcoin

#endif


