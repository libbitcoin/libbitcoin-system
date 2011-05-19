#ifndef LIBBITCOIN_NET_PEER_H
#define LIBBITCOIN_NET_PEER_H

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/cstdint.hpp>
#include <boost/utility.hpp>
#include <memory>

#include "net/messages.h"

namespace libbitcoin {
namespace net {

using boost::asio::io_service;
using boost::asio::ip::tcp;
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
    void recv_handler(shared_ptr<tcp::socket> socket,
            const boost::system::error_code &ec, size_t bytes_transferred);

    void send(message::version version);

private:
    shared_ptr<tcp::socket> socket;
    boost::array<boost::uint8_t, 4096> recv_buff;
    // Current message being constructed.
    // Flushed once message is completed.
    std::vector<char> recv_message;
    shared_ptr<dialect> dialect_translator;
    size_t recv_buff_idx;
};

} // net
} // libbitcoin

#endif
