#ifndef LIBBITCOIN_TYPES_HPP
#define LIBBITCOIN_TYPES_HPP

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

DECLARE_PTR_TYPE(acceptor);
DECLARE_PTR_TYPE(channel);

#undef DECLARE_PTR_TYPE

typedef std::atomic<size_t> atomic_counter;
typedef std::shared_ptr<atomic_counter> atomic_counter_ptr;

typedef std::shared_ptr<tcp::socket> socket_ptr;

constexpr size_t hash_digest_size = 32;
constexpr size_t short_hash_size = 20;
typedef std::array<uint8_t, hash_digest_size> hash_digest;
typedef std::array<uint8_t, short_hash_size> short_hash;

typedef std::vector<uint8_t> data_chunk;

// A list of indices. Used for creating block_locator objects or
// Storing list of unconfirmed input indexes in tx pool.
typedef std::vector<size_t> index_list;

// List of hashes. Useful primitive.
typedef std::vector<hash_digest> hash_digest_list;
typedef std::vector<short_hash> short_hash_list;

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

