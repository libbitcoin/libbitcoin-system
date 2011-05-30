#include "channel.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>
#include <iterator>
#include <ctime>

#include "bitcoin/util/logger.hpp"
#include "bitcoin/util/assert.hpp"
#include "bitcoin/net/delegator.hpp"
#include "bitcoin/net/messages.hpp"

#include "shared_const_buffer.hpp"
#include "dialect.hpp"

namespace placeholders = boost::asio::placeholders;
using boost::posix_time::seconds;
using boost::posix_time::minutes;
using boost::posix_time::time_duration;
using boost::asio::buffer;

namespace libbitcoin {
namespace net {

// Connection timeout time
const time_duration disconnect_timeout = seconds(30) + minutes(1);

channel_handle channel::chan_id_counter = 0;

channel::channel(const init_data& dat)
 : socket_(dat.socket), parent_gateway_(dat.parent_gateway), 
        translator_(dat.translator) 
{
    // Unique IDs are assigned to channels by incrementing a shared counter
    // among instances.
    channel_id_ = chan_id_counter;
    chan_id_counter++;

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
    if (problems_check(ec))
        return;
    logger(LOG_INFO) << "Forcing disconnect due to timeout.";
    // No response for a while so disconnect
    destroy_self();
}

void channel::reset_timeout()
{
    timeout_->cancel();
    timeout_->expires_from_now(disconnect_timeout);
    timeout_->async_wait(
            boost::bind(&channel::handle_timeout, this, placeholders::error));
}

void channel::destroy_self()
{
    parent_gateway_->disconnect(channel_id_);
}

bool channel::problems_check(const boost::system::error_code& ec)
{
    if (ec == boost::asio::error::operation_aborted) 
    {
        // Do nothing
        return true;
    }
    else if (ec) 
    {
        destroy_self();
        return true;
    }
    return false;
}

void channel::read_header()
{
    auto callback = boost::bind(&channel::handle_read_header, this,
            placeholders::error, placeholders::bytes_transferred);
    async_read(*socket_, buffer(inbound_header_), callback);
}

void channel::read_checksum(message::header header_msg)
{
    auto callback = boost::bind(&channel::handle_read_checksum, this, 
            header_msg, placeholders::error, placeholders::bytes_transferred);
    async_read(*socket_, buffer(inbound_checksum_), callback);
}

void channel::read_payload(message::header header_msg)
{
    auto callback = boost::bind(&channel::handle_read_payload, this, 
            header_msg, placeholders::error, placeholders::bytes_transferred);
    inbound_payload_.resize(header_msg.payload_length);
    async_read(*socket_, buffer(inbound_payload_, header_msg.payload_length), 
            callback);
}

void channel::handle_read_header(const boost::system::error_code& ec,
        size_t bytes_transferred)
{
    if (problems_check(ec))
        return;
    BITCOIN_ASSERT(bytes_transferred == header_chunk_size);
    serializer::stream header_stream = 
            serializer::stream(inbound_header_.begin(), inbound_header_.end()); 
    BITCOIN_ASSERT(header_stream.size() == header_chunk_size);
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

    logger(LOG_DEBUG) << header_msg.command;
    logger(LOG_DEBUG) << "payload is " << header_msg.payload_length 
            << " bytes.";
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
    reset_timeout();
}

void channel::handle_read_checksum(message::header header_msg,
        const boost::system::error_code& ec, size_t bytes_transferred)
{
    if (problems_check(ec))
        return;
    BITCOIN_ASSERT(bytes_transferred == header_checksum_size);
    serializer::stream checksum_stream = serializer::stream(
            inbound_checksum_.begin(), inbound_checksum_.end());
    BITCOIN_ASSERT(checksum_stream.size() == header_checksum_size);
    read_payload(header_msg);
    reset_timeout();
}

void channel::handle_read_payload(message::header header_msg,
        const boost::system::error_code& ec, size_t bytes_transferred)
{
    if (problems_check(ec))
        return;
    BITCOIN_ASSERT(bytes_transferred == header_msg.payload_length);
    serializer::stream payload_stream = serializer::stream(
            inbound_payload_.begin(), inbound_payload_.end());
    BITCOIN_ASSERT(payload_stream.size() == header_msg.payload_length);
    if (header_msg.command == "version")
    {
        message::version payload = 
                translator_->version_from_network(payload_stream); 
        logger(LOG_DEBUG) << "nonce is " << payload.nonce;
        logger(LOG_DEBUG) << "last block is " << payload.start_height;
    }
    read_header();
    reset_timeout();
}

void channel::handle_send(const boost::system::error_code& ec)
{
    if (problems_check(ec))
        return;
}

void channel::send(message::version version)
{
    serializer::stream msg = translator_->to_network(version);
    shared_const_buffer buffer(msg);
    async_write(*socket_, buffer,
            boost::bind(&channel::handle_send, this, placeholders::error));
}

channel_handle channel::get_id() const
{
    return channel_id_;
}

} // net
} // libbitcoin

