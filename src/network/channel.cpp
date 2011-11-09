#include "channel.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <functional>
#include <iterator>
#include <ctime>

#include <bitcoin/util/logger.hpp>
#include <bitcoin/util/assert.hpp>
#include <bitcoin/network/network.hpp>
#include <bitcoin/messages.hpp>

namespace libbitcoin {

using boost::posix_time::seconds;
using boost::posix_time::minutes;
using boost::posix_time::time_duration;
using boost::asio::buffer;

// Connection timeout time
const time_duration disconnect_timeout = seconds(0) + minutes(90);

channel_pimpl::channel_pimpl(network_ptr parent_gateway,
    dialect_ptr translator, service_ptr service, socket_ptr socket)
 : socket_(socket), network_(parent_gateway),
        translator_(translator)
{
    // Unique IDs are assigned to channels by incrementing a shared counter
    // among instances.
    channel_id_ = rand();

    timeout_.reset(new deadline_timer(*service));
    read_header();
    reset_timeout();
}

channel_pimpl::~channel_pimpl()
{
    tcp::endpoint remote_endpoint = socket_->remote_endpoint();
    log_debug() << "Closing channel "
            << remote_endpoint.address().to_string();
    boost::system::error_code ec;
    socket_->shutdown(tcp::socket::shutdown_both, ec);
    socket_->close(ec);
    timeout_->cancel();
}

void channel_pimpl::handle_timeout(const boost::system::error_code& ec)
{
    if (problems_check(ec))
        return;
    log_info() << "Forcing disconnect due to timeout.";
    // No response for a while so disconnect
    destroy_self();
}

void channel_pimpl::reset_timeout()
{
    timeout_->cancel();
    timeout_->expires_from_now(disconnect_timeout);
    timeout_->async_wait(std::bind(
            &channel_pimpl::handle_timeout, this, _1));
}

void channel_pimpl::destroy_self()
{
    network_->disconnect(channel_id_);
}

bool channel_pimpl::problems_check(const boost::system::error_code& ec)
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

void channel_pimpl::read_header()
{
    auto callback = std::bind(
            &channel_pimpl::handle_read_header, this, _1, _2);
    async_read(*socket_, buffer(inbound_header_), callback);
}

void channel_pimpl::read_checksum(const message::header& header_msg)
{
    auto callback = std::bind(&channel_pimpl::handle_read_checksum, 
            this, header_msg, _1, _2);
    async_read(*socket_, buffer(inbound_checksum_), callback);
}

void channel_pimpl::read_payload(const message::header& header_msg)
{
    auto callback = std::bind(&channel_pimpl::handle_read_payload, 
            this, header_msg, _1, _2);
    inbound_payload_.resize(header_msg.payload_length);
    async_read(*socket_, buffer(inbound_payload_, header_msg.payload_length),
            callback);
}

void channel_pimpl::handle_read_header(const boost::system::error_code& ec,
        size_t bytes_transferred)
{
    if (problems_check(ec))
        return;
    BITCOIN_ASSERT(bytes_transferred == header_chunk_size);
    data_chunk header_stream =
            data_chunk(inbound_header_.begin(), inbound_header_.end());
    BITCOIN_ASSERT(header_stream.size() == header_chunk_size);
    message::header header_msg =
            translator_->header_from_network(header_stream);

    if (!translator_->verify_header(header_msg))
    {
        log_debug() << "Bad header received.";
        destroy_self();
        return;
    }

    log_info(log_domain::network) << "r: " << header_msg.command
            << " (" << header_msg.payload_length << " bytes)";
    if (translator_->checksum_used(header_msg))
    {
        // Read checksum
        read_checksum(header_msg);
    }
    else
    {
        // Read payload
        read_payload(header_msg);
    }
    reset_timeout();
}

void channel_pimpl::handle_read_checksum(message::header& header_msg,
        const boost::system::error_code& ec, size_t bytes_transferred)
{
    if (problems_check(ec))
        return;
    BITCOIN_ASSERT(bytes_transferred == header_checksum_size);
    data_chunk checksum_stream = data_chunk(
            inbound_checksum_.begin(), inbound_checksum_.end());
    BITCOIN_ASSERT(checksum_stream.size() == header_checksum_size);
    //header_msg.checksum = cast_stream<uint32_t>(checksum_stream);
    header_msg.checksum = translator_->checksum_from_network(checksum_stream);
    read_payload(header_msg);
    reset_timeout();
}

void channel_pimpl::handle_read_payload(const message::header& header_msg,
        const boost::system::error_code& ec, size_t bytes_transferred)
{
    if (problems_check(ec))
        return;
    BITCOIN_ASSERT(bytes_transferred == header_msg.payload_length);
    data_chunk payload_stream = data_chunk(
        inbound_payload_.begin(), inbound_payload_.end());
    BITCOIN_ASSERT(payload_stream.size() == header_msg.payload_length);
    if (!translator_->verify_checksum(header_msg, payload_stream))
    {
        log_warning() << "Bad checksum!";
        destroy_self();
        return;
    }
    if (header_msg.command == "version")
    {
        if (!transport_payload<message::version>(payload_stream,
            std::bind(&dialect::version_from_network, translator_, _1)))
            return;
    }
    else if (header_msg.command == "verack")
        network_->relay(channel_id_, message::verack());
    else if (header_msg.command == "addr")
    {
        if (!transport_payload<message::addr>(payload_stream,
            std::bind(&dialect::addr_from_network, translator_, _1)))
            return;
    }
    else if (header_msg.command == "inv")
    {
        if (!transport_payload<message::inv>(payload_stream,
            std::bind(&dialect::inv_from_network, translator_, _1)))
            return;
    }
    else if (header_msg.command == "block")
    {
        if (!transport_payload<message::block>(payload_stream,
            std::bind(&dialect::block_from_network, translator_, _1)))
            return;
    }
    read_header();
    reset_timeout();
}

void channel_pimpl::pre_handle_send(const boost::system::error_code& ec,
    network::send_handler handle_send)
{
    if (problems_check(ec))
        handle_send(error::network_channel_not_found);
    else
        handle_send(std::error_code());
}

channel_handle channel_pimpl::get_id() const
{
    return channel_id_;
}

} // libbitcoin

