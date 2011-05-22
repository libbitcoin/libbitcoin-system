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
namespace placeholders = boost::asio::placeholders;
using std::shared_ptr;

class dialect;

class peer_exception
{
public:
    const char* what() const throw();
};

class peer : public boost::noncopyable 
{
public:
    peer(shared_ptr<dialect> translator);
    ~peer();

    bool connect(std::shared_ptr<io_service> io_service,
            std::string ip_addr, unsigned short port);

    void send(message::version version);
    void close();

private:
    void handle_read_header(const boost::system::error_code &ec, 
            size_t bytes_transferred);
    void handle_send(const boost::system::error_code &ec);

    shared_ptr<tcp::socket> socket;
    shared_ptr<dialect> translator;

    boost::asio::streambuf response;
    std::mutex ver_mutex;
    bool verack_recv, verack_sent;
};

} // net
} // libbitcoin

#endif
