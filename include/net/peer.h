#ifndef LIBBITCOIN_NET_PEER_H
#define LIBBITCOIN_NET_PEER_H

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/cstdint.hpp>
#include <boost/utility.hpp>
#include <memory>
#include <deque>

#include "net/serializer.h"
#include "net/messages.h"

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

class peer : boost::noncopyable
{
public:
    peer(shared_ptr<dialect> dialect_translator);
    ~peer();
    bool connect(std::shared_ptr<io_service> io_service,
            std::string ip_addr, unsigned short port);

    void send(message::version version);
    void close();

private:
    void handle_recv(const boost::system::error_code &ec, size_t bytes_transferred);
    void handle_send(const boost::system::error_code &ec);

    shared_ptr<tcp::socket> socket;
    shared_ptr<dialect> dialect_translator;

    boost::array<boost::uint8_t, 4096> recv_buff;
    size_t recv_buff_idx;
    // Current message being constructed.
    // Flushed once message is completed.
    std::vector<char> recv_msg;
};

} // net
} // libbitcoin

#endif
