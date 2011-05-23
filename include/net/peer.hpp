#ifndef LIBBITCOIN_NET_PEER_H
#define LIBBITCOIN_NET_PEER_H

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/cstdint.hpp>
#include <boost/utility.hpp>
#include <boost/asio/streambuf.hpp>
#include <memory>
#include <mutex>
#include <deque>

#include "net/serializer.hpp"
#include "net/messages.hpp"

namespace libbitcoin {
namespace net {

using boost::asio::io_service;
using boost::asio::ip::tcp;
using boost::asio::deadline_timer;
namespace placeholders = boost::asio::placeholders;
namespace posix_time = boost::posix_time;
using std::shared_ptr;

class delegator_default;
class dialect;

class peer : private boost::noncopyable, public std::enable_shared_from_this<peer>
{
public:
    peer(shared_ptr<delegator_default> parent_gateway, 
            shared_ptr<dialect> translator);
    ~peer();

    bool connect(std::shared_ptr<io_service> io_service,
            std::string ip_addr, unsigned short port);

    void send(message::version version);
    void close();

private:
    void handle_read_header(const boost::system::error_code &ec, 
            size_t bytes_transferred);
    void handle_send(const boost::system::error_code &ec);

    // Calls clean_shutdown and removes self from parent
    void destroy_self();
    // Closes socket
    void shutdown();

    shared_ptr<tcp::socket> socket_;
    shared_ptr<delegator_default> parent_gateway_;
    shared_ptr<dialect> translator_;

    boost::asio::streambuf response_;
    shared_ptr<deadline_timer> timeout_;
};

} // net
} // libbitcoin

#endif
