#include "net/peer.h"

#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <time.h>

#include "net/dialects/original.h"
#include "net/messages.h"

#include "shared_const_buffer.h"

// DEBUG
#include <iostream>

namespace libbitcoin {
namespace net {

peer::peer(shared_ptr<dialect> dialect_translator)
 : dialect_translator(dialect_translator), recv_buff_idx(0)
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
                boost::asio::buffer(recv_buff),
                boost::bind(&peer::handle_recv, this, _1, _2));
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

void peer::handle_recv(const boost::system::error_code& ec,
        size_t bytes_transferred)
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
            boost::asio::buffer(recv_buff),
            boost::bind(&peer::handle_recv, this, _1, _2));
}

void peer::handle_send(const boost::system::error_code& ec)
{
    if (ec) {
        throw peer_exception();
    }
}

void peer::send(message::version version)
{
    serializer::stream msg = dialect_translator->translate(version);
    shared_const_buffer buffer(msg);
    async_write(*socket, buffer,
            boost::bind(&peer::handle_send, this, placeholders::error));
}

} // net
} // libbitcoin

