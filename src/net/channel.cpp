#include "bitcoin/net/channel.hpp"

#include <boost/bind.hpp>
#include <time.h>

#include "bitcoin/util/logger.hpp"
#include "bitcoin/net/delegator.hpp"
#include "bitcoin/net/dialect.hpp"
#include "bitcoin/net/messages.hpp"

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

channel::channel(const init_data& dat)
 : socket_(dat.socket), parent_gateway_(dat.parent_gateway), 
        translator_(dat.translator) 
{
    timeout_.reset(new deadline_timer(*dat.service));
    read_header();
    reset_timeout();
}

channel::~channel() 
{
    tcp::endpoint remote_endpoint = socket_->remote_endpoint();
    logger(LOG_DEBUG) << "Closing channel " 
            << remote_endpoint.address().to_string();
    boost::system::error_code ec;
    socket_->shutdown(tcp::socket::shutdown_both, ec);
    socket_->close(ec);
    timeout_->cancel();
}

void channel::handle_timeout(const boost::system::error_code& ec)
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
        logger(LOG_INFO) << "Forcing disconnect due to timeout.";
        // No response for a while so disconnect
        destroy_self();
    }
}

void channel::reset_timeout()
{
    timeout_->cancel();
    timeout_->expires_from_now(posix_time::seconds(disconnect_timeout));
    timeout_->async_wait(
            boost::bind(&channel::handle_timeout, this, placeholders::error));
}

void channel::destroy_self()
{
    parent_gateway_->disconnect(shared_from_this());
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

void channel::read_header()
{
    async_read(*socket_, response_,
            boost::asio::transfer_at_least(header_chunk_size),
            boost::bind(&channel::handle_read_header, this,
                placeholders::error, placeholders::bytes_transferred));
}

void channel::read_checksum(message::header header_msg)
{
    async_read(*socket_, response_,
            boost::asio::transfer_at_least(4),
            boost::bind(&channel::handle_read_checksum, this, header_msg, 
                placeholders::error, placeholders::bytes_transferred));
}

void channel::read_payload(message::header header_msg)
{
    async_read(*socket_, response_,
            boost::asio::transfer_at_least(header_msg.payload_length),
            boost::bind(&channel::handle_read_payload, this, header_msg, 
                placeholders::error, placeholders::bytes_transferred));
}

void channel::handle_read_header(const boost::system::error_code& ec,
        size_t bytes_transferred)
{
    if (ec) 
    {
        if (ec != boost::asio::error::operation_aborted) 
            destroy_self();
        return;
    }
    BOOST_ASSERT(bytes_transferred >= header_chunk_size);
    BOOST_ASSERT(response_.size() == bytes_transferred);
    serializer::stream header_stream = 
            consume_response(response_, header_chunk_size);
    message::header header_msg = 
            translator_->header_from_network(header_stream);
    /*
     * Should be implemented outside in core or whatever
    if (header_msg.command == "version") {
    }
    else {
        // Read checksum
    }
    */
    // Temporarily we ignore checks
    if (false)
    {
        /*
        if (header_msg.magic != 0xd9b4bef9)
            throw channel_exception();
            // if (header_msg.length != ...
            // Read payload
        */
        destroy_self();
        return;
    }

    if (header_msg.command == "version" || header_msg.command == "verack")
    {
        // Read payload
        read_payload(header_msg);
    }
    else
    {
        // Read checksum
        read_checksum(header_msg);
    }
    logger(LOG_DEBUG) << header_msg.command;
    logger(LOG_DEBUG) << "payload is " << header_msg.payload_length 
            << " bytes.";
    reset_timeout();
}

void channel::handle_read_checksum(message::header header_msg,
        const boost::system::error_code& ec, size_t bytes_transferred)
{
    if (ec) 
    {
        if (ec != boost::asio::error::operation_aborted) 
            destroy_self();
        return;
    }
}

void channel::handle_read_payload(message::header header_msg,
        const boost::system::error_code& ec, size_t bytes_transferred)
{
    if (ec) 
    {
        if (ec != boost::asio::error::operation_aborted) 
            destroy_self();
        return;
    }
    BOOST_ASSERT(bytes_transferred >= header_msg.payload_length);
    BOOST_ASSERT(response_.size() == bytes_transferred);
    serializer::stream payload_stream = 
            consume_response(response_, header_msg.payload_length);
    message::version payload = 
            translator_->version_from_network(payload_stream); 
}

void channel::handle_send(const boost::system::error_code& ec)
{
    if (ec) 
    {
        if (ec != boost::asio::error::operation_aborted) 
            destroy_self();
        return;
    }
}

void channel::send(message::version version)
{
    serializer::stream msg = translator_->to_network(version);
    shared_const_buffer buffer(msg);
    async_write(*socket_, buffer,
            boost::bind(&channel::handle_send, this, placeholders::error));
}

} // net
} // libbitcoin

