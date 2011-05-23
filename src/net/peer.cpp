#include "net/peer.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <time.h>

#include "net/delegator.hpp"
#include "net/dialect.hpp"
#include "net/messages.hpp"

#include "shared_const_buffer.hpp"

namespace placeholders = boost::asio::placeholders;
namespace posix_time = boost::posix_time;

namespace libbitcoin {
namespace net {

// Header minus checksum is 4 + 12 + 4 = 20 bytes
constexpr size_t header_chunk_size = 20;
// Checksum size is 4 bytes
constexpr size_t header_checksum_size = 4;
// Connection timeout time in seconds
constexpr size_t disconnect_timeout = 30;

peer::peer(shared_ptr<delegator_default> parent_gateway, 
        shared_ptr<dialect> translator)
 : parent_gateway_(parent_gateway), translator_(translator)
{
}

peer::~peer() 
{
    shutdown();
}

bool peer::connect(shared_ptr<io_service> service,
        std::string ip_addr, unsigned short port)
{
    timeout_.reset(new deadline_timer(*service));
    socket_.reset(new tcp::socket(*service));
    try 
    {
        tcp::resolver resolver(*service);
        tcp::resolver::query query(ip_addr, boost::lexical_cast<std::string>(port));

        tcp::endpoint endpoint = *resolver.resolve(query);
        socket_->connect(endpoint);

        async_read(*socket_, response_,
                boost::asio::transfer_at_least(header_chunk_size),
                boost::bind(&peer::handle_read_header, this,
                    placeholders::error, placeholders::bytes_transferred));
        reset_timeout();
    }
    catch (std::exception &ex) 
    {
        return false;
    }
    return true;
}

void peer::handle_timeout(const boost::system::error_code& ec)
{
    if (ec == boost::asio::error::operation_aborted) 
    {
        // Do nothing
    }
    else if (ec) 
    {
        destroy_self();
    }
    else 
    {
        // No response for a while so disconnect
        destroy_self();
    }
}

void peer::reset_timeout()
{
    timeout_->cancel();
    timeout_->expires_from_now(posix_time::seconds(disconnect_timeout));
    timeout_->async_wait(
            boost::bind(&peer::handle_timeout, this, placeholders::error));
}

void peer::shutdown()
{
    boost::system::error_code ec;
    socket_->shutdown(tcp::socket::shutdown_both, ec);
    socket_->close(ec);
}
void peer::destroy_self()
{
    parent_gateway_->disconnect(shared_from_this());
    shutdown();
}

static serializer::stream consume_response(boost::asio::streambuf& response, 
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
    if (ec) 
    {
        destroy_self();
        return;
    }
    BOOST_ASSERT(bytes_transferred >= header_chunk_size);
    BOOST_ASSERT(response_.size() == bytes_transferred);
    serializer::stream header_stream = 
            consume_response(response_, header_chunk_size);
    message::header header_msg = translator_->header_from_network(header_stream);
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
    reset_timeout();
}

void peer::handle_send(const boost::system::error_code& ec)
{
    if (ec) 
    {
        destroy_self();
        return;
    }
}

void peer::send(message::version version)
{
    serializer::stream msg = translator_->to_network(version);
    shared_const_buffer buffer(msg);
    async_write(*socket_, buffer,
            boost::bind(&peer::handle_send, this, placeholders::error));
}

} // net
} // libbitcoin

