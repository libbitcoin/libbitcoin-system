#ifndef LIBBITCOIN_TYPES_H
#define LIBBITCOIN_TYPES_H

#include <boost/asio.hpp>
#include <array>
#include <memory>
#include <sstream>
#include <iomanip>
#include <vector>

namespace libbitcoin {

using boost::asio::ip::tcp;

#define DECLARE_PTR_TYPE(nametype) \
    class nametype; \
    typedef std::shared_ptr<nametype> nametype##_ptr

DECLARE_PTR_TYPE(async_service);
DECLARE_PTR_TYPE(exporter);
DECLARE_PTR_TYPE(blockchain);
DECLARE_PTR_TYPE(chrono_clock);
DECLARE_PTR_TYPE(transaction_pool);
DECLARE_PTR_TYPE(acceptor);
DECLARE_PTR_TYPE(network);
DECLARE_PTR_TYPE(channel);
DECLARE_PTR_TYPE(handshake);
DECLARE_PTR_TYPE(hosts);
DECLARE_PTR_TYPE(protocol);
DECLARE_PTR_TYPE(poller);
DECLARE_PTR_TYPE(session);

#undef DECLARE_PTR_TYPE

typedef std::atomic<size_t> atomic_counter;
typedef std::shared_ptr<atomic_counter> atomic_counter_ptr;

typedef std::shared_ptr<tcp::socket> socket_ptr;

typedef std::array<uint8_t, 32> hash_digest;
typedef std::array<uint8_t, 20> short_hash;

typedef uint8_t byte;
typedef std::vector<byte> data_chunk;

// A list of indices. Used for creating block_locator objects or
// Storing list of unconfirmed input indexes in tx pool.
typedef std::vector<size_t> index_list;

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

struct node_address
{
    std::string hostname;
    uint16_t port;
};

} // namespace libbitcoin

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
} // namespace std

#endif

