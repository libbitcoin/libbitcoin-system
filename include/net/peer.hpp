#ifndef LIBBITCOIN_NET_PEER_H
#define LIBBITCOIN_NET_PEER_H

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/utility.hpp>
#include <boost/asio/streambuf.hpp>
#include <cstdint>
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
using std::shared_ptr;

class delegator_default;
class dialect;
class peer;

typedef shared_ptr<peer> peer_ptr;

class peer : private boost::noncopyable, public std::enable_shared_from_this<peer>
{
public:
    peer(shared_ptr<delegator_default> parent_gateway, 
            shared_ptr<dialect> translator);
    ~peer();

    bool connect(shared_ptr<io_service> io_service,
            std::string ip_addr, unsigned short port);

    void send(message::version version);
    void close();

private:
    void handle_read_header(const boost::system::error_code& ec, 
            size_t bytes_transferred);
    void handle_send(const boost::system::error_code& ec);

    void handle_timeout(const boost::system::error_code& ec);
    void reset_timeout();

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
