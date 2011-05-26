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

#include "bitcoin/net/serializer.hpp"
#include "bitcoin/net/messages.hpp"

namespace libbitcoin {
namespace net {

using boost::asio::io_service;
using boost::asio::ip::tcp;
using boost::asio::deadline_timer;
using std::shared_ptr;

class delegator;
class dialect;
class peer;

typedef shared_ptr<peer> peer_ptr;

class peer : private boost::noncopyable, public std::enable_shared_from_this<peer>
{
public:
    struct init_data
    {
        shared_ptr<delegator> parent_gateway;
        shared_ptr<dialect> translator;
        shared_ptr<io_service> service;
        shared_ptr<tcp::socket> socket;
    };

    peer(const init_data& dat);
    ~peer();

    void send(message::version version);
    void close();

private:
    void read_header();
    void read_checksum(message::header header_msg);
    void read_payload(message::header header_msg);

    void handle_read_header(const boost::system::error_code& ec, 
            size_t bytes_transferred);
    void handle_read_checksum(message::header header_msg,
            const boost::system::error_code& ec, size_t bytes_transferred);
    void handle_read_payload(message::header header_msg,
            const boost::system::error_code& ec, size_t bytes_transferred);

    void handle_send(const boost::system::error_code& ec);

    void handle_timeout(const boost::system::error_code& ec);
    void reset_timeout();

    // Removes self from parent
    void destroy_self();

    shared_ptr<tcp::socket> socket_;
    shared_ptr<delegator> parent_gateway_;
    shared_ptr<dialect> translator_;

    boost::asio::streambuf response_;
    shared_ptr<deadline_timer> timeout_;
};

} // net
} // libbitcoin

#endif
