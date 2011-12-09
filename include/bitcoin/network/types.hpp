#ifndef LIBBITCOIN_NET_TYPES_H
#define LIBBITCOIN_NET_TYPES_H

#include <boost/asio.hpp>
#include <memory>

#include <bitcoin/types.hpp>

namespace libbitcoin {

class network;
class channel;

using boost::asio::ip::tcp;
using boost::asio::io_service;

typedef std::shared_ptr<tcp::socket> socket_ptr;
typedef std::shared_ptr<tcp::acceptor> acceptor_ptr;

typedef std::shared_ptr<network> network_ptr;
typedef std::shared_ptr<channel> channel_ptr;
typedef std::vector<channel_ptr> channel_list;

} // libbitcoin

#endif

