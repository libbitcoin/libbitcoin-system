#include "net/peer.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <time.h>

#include "net/dialect.hpp"
#include "net/messages.hpp"

#include "shared_const_buffer.hpp"

namespace libbitcoin {
namespace net {

// Header minus checksum is 4 + 12 + 4 = 20 bytes
constexpr size_t header_chunk_size = 20;
// Checksum size is 4 bytes
constexpr size_t header_checksum_size = 4;

peer::peer(shared_ptr<dialect> translator)
 : translator(translator), verack_recv(false), verack_sent(false)
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

        async_read(*socket, response,
                boost::asio::transfer_at_least(header_chunk_size),
                boost::bind(&peer::handle_read_header, this,
                    placeholders::error, placeholders::bytes_transferred));
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

static serializer::stream consume_response(boost::asio::streambuf &response, 
        size_t size)
{
    std::string str;
    std::istream(&response) >> str;
    serializer::stream stream(str.begin(), str.end());
    // Only accept first n bytes
    stream.resize(size);
    // And consume it from the streambuf
    response.consume(size);
    return stream;
}

void peer::handle_read_header(const boost::system::error_code& ec,
        size_t bytes_transferred)
{
    if (ec) {
        // Temporary. Should remove itself from parent container.
        // Too destructive.
        throw peer_exception();
    }
    assert(bytes_transferred >= header_chunk_size);
    assert(response.size() == bytes_transferred);
    serializer::stream header_stream = 
            consume_response(response, header_chunk_size);
    message::header header_msg = translator->header_from_network(header_stream);
    /*
     * Should be implemented outside in core or whatever
    if (header_msg.magic != 0xd9b4bef9)
        throw peer_exception();
    if (header_msg.command == "version") {
        // if (header_msg.length != ...
        // Read payload
    }
    else {
        // Read checksum
    }
    */
    std::cout << header_msg.command << "\n";
    std::cout << "payload is " << header_msg.length << " bytes.\n";
}

void peer::handle_send(const boost::system::error_code& ec)
{
    if (ec) 
        throw peer_exception();
}

void peer::send(message::version version)
{
    serializer::stream msg = translator->to_network(version);
    shared_const_buffer buffer(msg);
    async_write(*socket, buffer,
            boost::bind(&peer::handle_send, this, placeholders::error));
}

} // net
} // libbitcoin

