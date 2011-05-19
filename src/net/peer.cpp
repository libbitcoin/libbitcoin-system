#include "net/peer.h"

#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <time.h>

#include "net/messages.h"

// DEBUG
#include <iostream>

namespace libbitcoin {
namespace net {

using boost::asio::buffer;

peer::peer(shared_ptr<dialect> dialect_translator)
 : recv_buff_idx(0), dialect_translator(dialect_translator)
{
}

peer::~peer() 
{
    boost::system::error_code ec;
    socket->shutdown(tcp::socket::shutdown_both, ec);
    socket->close(ec);
}

bool peer::connect(shared_ptr<io_service> service,
        std::string ip_addr, unsigned short port)
{
    socket.reset(new tcp::socket(*service));
    try {
        tcp::resolver resolver(*service);
        tcp::resolver::query query(ip_addr, boost::lexical_cast<std::string>(port));

        tcp::endpoint endpoint = *resolver.resolve(query);
        socket->connect(endpoint);

        socket->async_read_some(
                buffer(recv_buff),
                boost::bind(&peer::recv_handler, this, socket, _1, _2));
    }
    catch (std::exception &ex) {
        return false;
    }
    return true;
}

const char* peer_exception::what() const throw()
{
    return "Network error in peer.";
}

void peer::recv_handler(shared_ptr<tcp::socket> socket,
        const boost::system::error_code &ec, size_t bytes_transferred)
{
    if (ec) {
        // Temporary. Should remove itself from parent container.
        // Too destructive.
        throw peer_exception();
    }
    recv_buff_idx += bytes_transferred;
    for (size_t i = 0; i < recv_buff_idx; i++)
        std::cout << recv_buff[i];
    std::cout << recv_buff_idx << "\n";
    socket->async_read_some(
            buffer(recv_buff),
            boost::bind(&peer::recv_handler, this, socket, _1, _2));
}

void peer::send(message::version version)
{
}

} // net
} // libbitcoin

