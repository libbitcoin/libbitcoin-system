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

class exporter;
class blockchain;
class kernel;
class network;
class chrono_clock;

typedef shared_ptr<exporter> exporter_ptr;
typedef shared_ptr<blockchain> blockchain_ptr;
typedef shared_ptr<kernel> kernel_ptr;
typedef shared_ptr<network> network_ptr;
typedef shared_ptr<chrono_clock> chrono_clock_ptr;

typedef shared_ptr<io_service> service_ptr;
typedef shared_ptr<io_service::work> work_ptr;
typedef shared_ptr<io_service::strand> strand_ptr;
typedef shared_ptr<deadline_timer> deadline_timer_ptr;

typedef std::array<uint8_t, 32> hash_digest;
typedef std::array<uint8_t, 20> short_hash;

typedef unsigned char byte;
typedef std::vector<byte> data_chunk;

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
struct hash<hash_digest>
  : public std_hash_wrapper<hash_digest>
{
};

template <>
struct hash<short_hash>
  : public std_hash_wrapper<short_hash>
{
};

} // std

#endif

