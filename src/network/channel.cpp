#include <bitcoin/network/channel.hpp>

namespace libbitcoin {

using boost::posix_time::seconds;
using boost::posix_time::minutes;
using boost::posix_time::time_duration;
using boost::asio::buffer;

// Connection timeout time
const time_duration disconnect_timeout = seconds(0) + minutes(90);

channel::channel(socket_ptr socket, thread_core_ptr threaded,
    exporter_ptr saver)
  : killed_(false), socket_(socket), export_(saver), threaded_(threaded)
{
    strand_ = threaded_->create_strand();
    timeout_.reset(new deadline_timer(*threaded_->service()));
}

channel::~channel()
{
    stop();
}

void channel::start()
{
    read_header();
    reset_timeout();
}
void channel::stop()
{
    killed_ = true;
    timeout_->cancel();
    timeout_.reset();
    socket_.reset();
}

void channel::handle_timeout(const boost::system::error_code& ec)
{
    if (ec == boost::asio::error::operation_aborted)
        return;
    log_info(log_domain::network) << "Forcing disconnect due to timeout.";
    // No response for a while so disconnect
    tcp::endpoint remote_endpoint = socket_->remote_endpoint();
    log_debug(log_domain::network) << "Closing channel "
        << remote_endpoint.address().to_string();
    // TODO should be async
    // TODO handlers should be able to be registered for this event
    // this error codes would be delegated to it
    boost::system::error_code ret_ec;
    socket_->shutdown(tcp::socket::shutdown_both, ret_ec);
    socket_->close(ret_ec);
    stop();
}

void channel::reset_timeout()
{
    timeout_->cancel();
    timeout_->expires_from_now(disconnect_timeout);
    timeout_->async_wait(std::bind(
        &channel::handle_timeout, shared_from_this(), _1));
}

bool channel::problems_check(const boost::system::error_code& ec)
{
    if (killed_)
        return true;
    else if (!ec)
        return false;
    stop();
    return true;
}

void channel::read_header()
{
    async_read(*socket_, buffer(inbound_header_),
        strand_->wrap(std::bind(&channel::handle_read_header,
            shared_from_this(), _1, _2)));
}

void channel::read_checksum(const message::header& header_msg)
{
    async_read(*socket_, buffer(inbound_checksum_),
        strand_->wrap(std::bind(&channel::handle_read_checksum, 
            shared_from_this(), header_msg, _1, _2)));
}

void channel::read_payload(const message::header& header_msg)
{
    inbound_payload_.resize(header_msg.payload_length);
    async_read(*socket_, buffer(inbound_payload_, header_msg.payload_length),
        strand_->wrap(std::bind(&channel::handle_read_payload, 
            shared_from_this(), header_msg, _1, _2)));
}

void channel::handle_read_header(const boost::system::error_code& ec,
    size_t bytes_transferred)
{
    if (problems_check(ec))
        return;
    BITCOIN_ASSERT(bytes_transferred == header_chunk_size);
    data_chunk header_stream =
            data_chunk(inbound_header_.begin(), inbound_header_.end());
    BITCOIN_ASSERT(header_stream.size() == header_chunk_size);
    message::header header_msg =
            export_->header_from_network(header_stream);

    if (!export_->verify_header(header_msg))
    {
        log_debug(log_domain::network) << "Bad header received.";
        stop();
        return;
    }

    log_info(log_domain::network) << "r: " << header_msg.command
            << " (" << header_msg.payload_length << " bytes)";
    if (export_->checksum_used(header_msg))
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

void channel::handle_read_checksum(message::header& header_msg,
        const boost::system::error_code& ec, size_t bytes_transferred)
{
    if (problems_check(ec))
        return;
    BITCOIN_ASSERT(bytes_transferred == header_checksum_size);
    data_chunk checksum_stream = data_chunk(
            inbound_checksum_.begin(), inbound_checksum_.end());
    BITCOIN_ASSERT(checksum_stream.size() == header_checksum_size);
    //header_msg.checksum = cast_stream<uint32_t>(checksum_stream);
    header_msg.checksum = export_->checksum_from_network(checksum_stream);
    read_payload(header_msg);
    reset_timeout();
}

void channel::handle_read_payload(const message::header& header_msg,
        const boost::system::error_code& ec, size_t bytes_transferred)
{
    if (problems_check(ec))
        return;
    BITCOIN_ASSERT(bytes_transferred == header_msg.payload_length);
    data_chunk payload_stream = data_chunk(
        inbound_payload_.begin(), inbound_payload_.end());
    BITCOIN_ASSERT(payload_stream.size() == header_msg.payload_length);
    if (!export_->verify_checksum(header_msg, payload_stream))
    {
        log_warning(log_domain::network) << "Bad checksum!";
        stop();
        return;
    }

    if (header_msg.command == "version")
    {
        if (!transport<message::version>(payload_stream,
            std::bind(&exporter::version_from_network, export_, _1),
            version_registry_))
        {
            return;
        }
    }
    else if (header_msg.command == "verack")
    {
        relay(message::verack(), verack_registry_);
    }
    else if (header_msg.command == "addr")
    {
        if (!transport<message::address>(payload_stream,
            std::bind(&exporter::address_from_network, export_, _1),
            address_registry_))
        {
            return;
        }
    }
    else if (header_msg.command == "inv")
    {
        if (!transport<message::inventory>(payload_stream,
            std::bind(&exporter::inventory_from_network, export_, _1),
            inventory_registry_))
        {
            return;
        }
    }
    else if (header_msg.command == "block")
    {
        if (!transport<message::block>(payload_stream,
            std::bind(&exporter::block_from_network, export_, _1),
            block_registry_))
        {
            return;
        }
    }

    read_header();
    reset_timeout();
}

void channel::pre_handle_send(const boost::system::error_code& ec,
    send_handler handle_send)
{
    if (problems_check(ec))
        handle_send(error::channel_stopped);
    else
        handle_send(std::error_code());
}

void channel::subscribe_version(receive_version_handler handle_receive)
{
    generic_subscribe<message::version>(handle_receive, version_registry_);
}
void channel::subscribe_verack(receive_verack_handler handle_receive)
{
    generic_subscribe<message::verack>(handle_receive, verack_registry_);
}
void channel::subscribe_address(receive_address_handler handle_receive)
{
    generic_subscribe<message::address>(handle_receive, address_registry_);
}
void channel::subscribe_inventory(receive_inventory_handler handle_receive)
{
    generic_subscribe<message::inventory>(handle_receive, inventory_registry_);
}
void channel::subscribe_block(receive_block_handler handle_receive)
{
    generic_subscribe<message::block>(handle_receive, block_registry_);
}

} // libbitcoin

