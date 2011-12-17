#ifndef LIBBITCOIN_TYPES_H
#define LIBBITCOIN_TYPES_H

#include <boost/detail/endian.hpp>
#include <boost/asio.hpp>
#include <array>
#include <memory>
#include <sstream>
#include <iomanip>
#include <vector>

#include <bitcoin/network/types.hpp>

namespace libbitcoin {

using std::shared_ptr;
using boost::asio::io_service;
using boost::asio::ip::tcp;
using boost::asio::deadline_timer;

class dialect;
class blockchain;
class kernel;
class network;
class clock;

typedef shared_ptr<dialect> dialect_ptr;
typedef shared_ptr<blockchain> blockchain_ptr;
typedef shared_ptr<kernel> kernel_ptr;
typedef shared_ptr<network> network_ptr;
typedef shared_ptr<clock> clock_ptr;

typedef shared_ptr<io_service> service_ptr;
typedef shared_ptr<io_service::work> work_ptr;
typedef shared_ptr<io_service::strand> strand_ptr;
typedef shared_ptr<deadline_timer> deadline_timer_ptr;

typedef std::array<uint8_t, 32> hash_digest;
typedef std::array<uint8_t, 20> short_hash;

typedef unsigned char byte;
typedef std::vector<byte> data_chunk;

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

// Make hash_digest and short_hash hashable for std::*map variants
template <typename HashType>
struct std_hash_wrapper
{
    size_t operator()(const HashType& h) const
    {
        std::hash<std::string> functor;
        return functor(std::string(std::begin(h), std::end(h)));
    }
};

} // libbitcoin

// Extend std namespace with our hash wrappers
namespace std
{
    using libbitcoin::std_hash_wrapper;
    using libbitcoin::hash_digest;
    using libbitcoin::short_hash;

    template <>
    struct hash<hash_digest> : public std_hash_wrapper<hash_digest>
    {
    };

    template <>
    struct hash<short_hash> : public std_hash_wrapper<short_hash>
    {
    };
}

#endif

