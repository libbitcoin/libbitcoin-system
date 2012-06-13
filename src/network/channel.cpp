#include <bitcoin/network/channel.hpp>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;

using boost::posix_time::seconds;
using boost::posix_time::minutes;
using boost::posix_time::time_duration;
using boost::asio::buffer;

// Connection timeout time
const time_duration initial_timeout = seconds(0) + minutes(1);
const time_duration disconnect_timeout = seconds(0) + minutes(90);

const time_duration heartbeat_time = seconds(0) + minutes(30);

channel_stream_loader::~channel_stream_loader()
{
    for (channel_loader_module_base* module: modules_)
        delete module;
}

void channel_stream_loader::add(channel_loader_module_base* module)
{
    modules_.push_back(module);
}

void channel_stream_loader::load_lookup(const std::string& symbol,
    const data_chunk& stream) const
{
    for (channel_loader_module_base* module: modules_)
        if (module->lookup_symbol() == symbol)
            module->attempt_load(stream);
}

channel_proxy::channel_proxy(async_service& service, socket_ptr socket)
  : strand_(service.get_service()), stopped_(false), socket_(socket), 
    timeout_(service.get_service()), heartbeat_(service.get_service())
{
#define CHANNEL_TRANSPORT_MECHANISM(MESSAGE_TYPE) \
    MESSAGE_TYPE##_subscriber_ = \
        std::make_shared<MESSAGE_TYPE##_subscriber_type>(service); \
    loader_.add(new channel_loader_module<message::MESSAGE_TYPE>( \
        std::bind(&MESSAGE_TYPE##_subscriber_type::relay, \
            MESSAGE_TYPE##_subscriber_, _1, _2)));

    CHANNEL_TRANSPORT_MECHANISM(version);
    CHANNEL_TRANSPORT_MECHANISM(verack);
    CHANNEL_TRANSPORT_MECHANISM(address);
    CHANNEL_TRANSPORT_MECHANISM(get_address);
    CHANNEL_TRANSPORT_MECHANISM(inventory);
    CHANNEL_TRANSPORT_MECHANISM(get_data);
    CHANNEL_TRANSPORT_MECHANISM(get_blocks);
    CHANNEL_TRANSPORT_MECHANISM(transaction);
    CHANNEL_TRANSPORT_MECHANISM(block);

#undef CHANNEL_TRANSPORT_MECHANISM

    raw_subscriber_ = 
        std::make_shared<raw_subscriber_type>(service);
    stop_subscriber_ =
        std::make_shared<stop_subscriber_type>(service);
}

channel_proxy::~channel_proxy()
{
    stop();
}

void channel_proxy::start()
{
    read_header();
    set_timeout(initial_timeout);
    set_heartbeat(heartbeat_time);
}

void channel_proxy::stop()
{
    if (stopped_)
        return;
    stop_impl();
    stop_subscriber_->relay(error::service_stopped);
}
void channel_proxy::stop_impl()
{
    // We need this because the timeout timer shares this code with stop()
    // But sends a different error_code
    stopped_ = true;
    // Ignore the error_code. We don't really care at this point.
    boost::system::error_code ret_ec;
    timeout_.cancel(ret_ec);
    heartbeat_.cancel(ret_ec);
    socket_->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ret_ec);
    socket_->close(ret_ec);
    clear_subscriptions();
}

void channel_proxy::clear_subscriptions()
{
    version_subscriber_->relay(error::service_stopped, 
        message::version());
    verack_subscriber_->relay(error::service_stopped, 
        message::verack());
    address_subscriber_->relay(error::service_stopped, 
        message::address());
    get_address_subscriber_->relay(error::service_stopped,
        message::get_address());
    inventory_subscriber_->relay(error::service_stopped, 
        message::inventory());
    get_data_subscriber_->relay(error::service_stopped, 
        message::get_data());
    get_blocks_subscriber_->relay(error::service_stopped, 
        message::get_blocks());
    transaction_subscriber_->relay(error::service_stopped, 
        message::transaction());
    block_subscriber_->relay(error::service_stopped, 
        message::block());
    raw_subscriber_->relay(error::service_stopped,
        message::header(), data_chunk());
}

bool channel_proxy::stopped() const
{
    return stopped_;
}

bool timer_errors(const boost::system::error_code& ec, bool stopped)
{
    if (ec == boost::asio::error::operation_aborted)
        return true;
    else if (ec)
    {
        log_error(log_domain::network) << ec.message();
        return true;
    }
    else if (stopped)
        return true;
    return false;
}

void channel_proxy::handle_timeout(const boost::system::error_code& ec)
{
    if (timer_errors(ec, stopped_))
        return;
    log_info(log_domain::network) << "Forcing disconnect due to timeout.";
    // No response for a while so disconnect
    boost::system::error_code ret_ec;
    tcp::endpoint remote_endpoint = socket_->remote_endpoint(ret_ec);
    if (!ec)
        log_debug(log_domain::network) << "Closing channel "
            << remote_endpoint.address().to_string();
    ret_ec = boost::system::error_code();
    // Force the socket closed
    // Should we do something with these error_codes?
    socket_->shutdown(tcp::socket::shutdown_both, ret_ec);
    socket_->close(ret_ec);
    stop_impl();
    stop_subscriber_->relay(error::channel_timeout);
}

void handle_ping(const std::error_code&)
{
    // if there's a problem sending then this channel will be stopped
}
void channel_proxy::handle_heartbeat(const boost::system::error_code& ec)
{
    if (timer_errors(ec, stopped_))
        return;
    send(message::ping(), handle_ping);
}

void channel_proxy::set_timeout(
    const boost::posix_time::time_duration timeout)
{
    timeout_.cancel();
    timeout_.expires_from_now(timeout);
    timeout_.async_wait(std::bind(
        &channel_proxy::handle_timeout, shared_from_this(), _1));
}
void channel_proxy::set_heartbeat(
    const boost::posix_time::time_duration timeout)
{
    heartbeat_.cancel();
    heartbeat_.expires_from_now(timeout);
    heartbeat_.async_wait(std::bind(
        &channel_proxy::handle_heartbeat, shared_from_this(), _1));
}
void channel_proxy::reset_timers()
{
    set_timeout(disconnect_timeout);
    set_heartbeat(heartbeat_time);
}

bool channel_proxy::problems_check(const boost::system::error_code& ec)
{
    if (stopped_)
        return true;
    else if (!ec)
        return false;
    stop();
    return true;
}

void channel_proxy::read_header()
{
    async_read(*socket_, buffer(inbound_header_),
        strand_.wrap(std::bind(&channel_proxy::handle_read_header,
            shared_from_this(), _1, _2)));
}

void channel_proxy::read_checksum(const message::header& header_msg)
{
    async_read(*socket_, buffer(inbound_checksum_),
        strand_.wrap(std::bind(&channel_proxy::handle_read_checksum, 
            shared_from_this(), _1, _2, header_msg)));
}

void channel_proxy::read_payload(const message::header& header_msg)
{
    inbound_payload_.resize(header_msg.payload_length);
    async_read(*socket_, buffer(inbound_payload_, header_msg.payload_length),
        strand_.wrap(std::bind(&channel_proxy::handle_read_payload, 
            shared_from_this(), _1, _2, header_msg)));
}

bool verify_header(const message::header& header_msg)
{
    if (header_msg.magic != magic_value)
        return false;
    if (header_msg.command == "version")
    {
        if (header_msg.payload_length < 85)
            return false;
    }
    else if (header_msg.command == "verack"
        || header_msg.command == "getaddr"
        || header_msg.command == "ping")
    {
        if (header_msg.payload_length != 0)
            return false;
    }
    else if (header_msg.command == "inv"
        || header_msg.command == "addr"
        || header_msg.command == "getdata"
        || header_msg.command == "getblocks"
        || header_msg.command == "getheaders"
        || header_msg.command == "tx"
        || header_msg.command == "block"
        || header_msg.command == "headers"
        || header_msg.command == "alert")
    {
        // Should check if sizes make sense
        // i.e for addr should be multiple of 30x + 1 byte
        // Also then add ASSERTS to handlers above.
    }
    // Ignore unknown headers
    return true;
}

void channel_proxy::handle_read_header(const boost::system::error_code& ec,
    size_t bytes_transferred)
{
    if (problems_check(ec))
        return;
    BITCOIN_ASSERT(bytes_transferred == header_chunk_size);
    data_chunk header_stream =
            data_chunk(inbound_header_.begin(), inbound_header_.end());
    BITCOIN_ASSERT(header_stream.size() == header_chunk_size);
    message::header header_msg;
    satoshi_load(header_stream.begin(), header_stream.end(), header_msg);

    if (!verify_header(header_msg))
    {
        log_debug(log_domain::network) << "Bad header received.";
        stop();
        return;
    }

    log_info(log_domain::network) << "r: " << header_msg.command
            << " (" << header_msg.payload_length << " bytes)";
    read_checksum(header_msg);
    reset_timers();
}

void channel_proxy::handle_read_checksum(const boost::system::error_code& ec,
    size_t bytes_transferred, message::header& header_msg)
{
    if (problems_check(ec))
        return;
    BITCOIN_ASSERT(bytes_transferred == header_checksum_size);
    data_chunk checksum_stream = data_chunk(
            inbound_checksum_.begin(), inbound_checksum_.end());
    BITCOIN_ASSERT(checksum_stream.size() == header_checksum_size);
    header_msg.checksum = cast_chunk<uint32_t>(checksum_stream);
    read_payload(header_msg);
    reset_timers();
}

void channel_proxy::handle_read_payload(const boost::system::error_code& ec,
    size_t bytes_transferred, const message::header& header_msg)
{
    if (problems_check(ec))
        return;
    BITCOIN_ASSERT(bytes_transferred == header_msg.payload_length);
    data_chunk payload_stream = data_chunk(
        inbound_payload_.begin(), inbound_payload_.end());
    BITCOIN_ASSERT(payload_stream.size() == header_msg.payload_length);
    if (header_msg.checksum != generate_sha256_checksum(payload_stream))
    {
        log_warning(log_domain::network) << "Bad checksum!";
        raw_subscriber_->relay(error::bad_stream,
            message::header(), data_chunk());
        stop();
        return;
    }
    raw_subscriber_->relay(std::error_code(),
        header_msg, inbound_payload_);

    // This must happen before calling subscribe notification handlers
    // In case user tries to stop() this channel.
    read_header();
    reset_timers();

    loader_.load_lookup(header_msg.command, payload_stream);
}

void channel_proxy::call_handle_send(const boost::system::error_code& ec,
    send_handler handle_send)
{
    if (problems_check(ec))
        handle_send(error::service_stopped);
    else
        handle_send(std::error_code());
}

void channel_proxy::subscribe_version(receive_version_handler handle_receive)
{
    generic_subscribe<message::version>(
        handle_receive, version_subscriber_);
}
void channel_proxy::subscribe_verack(receive_verack_handler handle_receive)
{
    generic_subscribe<message::verack>(
        handle_receive, verack_subscriber_);
}
void channel_proxy::subscribe_address(receive_address_handler handle_receive)
{
    generic_subscribe<message::address>(
        handle_receive, address_subscriber_);
}
void channel_proxy::subscribe_inventory(
    receive_inventory_handler handle_receive)
{
    generic_subscribe<message::inventory>(
        handle_receive, inventory_subscriber_);
}
void channel_proxy::subscribe_get_data(
    receive_get_data_handler handle_receive)
{
    generic_subscribe<message::get_data>(
        handle_receive, get_data_subscriber_);
}
void channel_proxy::subscribe_get_blocks(
    receive_get_blocks_handler handle_receive)
{
    generic_subscribe<message::get_blocks>(
        handle_receive, get_blocks_subscriber_);
}
void channel_proxy::subscribe_transaction(
    receive_transaction_handler handle_receive)
{
    generic_subscribe<message::transaction>(
        handle_receive, transaction_subscriber_);
}
void channel_proxy::subscribe_block(receive_block_handler handle_receive)
{
    generic_subscribe<message::block>(
        handle_receive, block_subscriber_);
}
void channel_proxy::subscribe_get_address(
    receive_get_address_handler handle_receive)
{
    generic_subscribe<message::get_address>(
        handle_receive, get_address_subscriber_);
}
void channel_proxy::subscribe_raw(receive_raw_handler handle_receive)
{
    if (stopped_)
        handle_receive(error::service_stopped,
            message::header(), data_chunk());
    else
        raw_subscriber_->subscribe(handle_receive);
}

void channel_proxy::subscribe_stop(stop_handler handle_stop)
{
    if (stopped_)
        handle_stop(error::service_stopped);
    else
        stop_subscriber_->subscribe(handle_stop);
}

void channel_proxy::send_raw(const message::header& packet_header,
    const data_chunk& payload, send_handler handle_send)
{
    if (stopped_)
        handle_send(error::service_stopped);
    else
        strand_.post(std::bind(&channel_proxy::do_send_raw,
            shared_from_this(), packet_header, payload, handle_send));
}
void channel_proxy::do_send_raw(const message::header& packet_header,
    const data_chunk& payload, send_handler handle_send)
{
    data_chunk raw_header(satoshi_raw_size(packet_header));
    satoshi_save(packet_header, raw_header.begin());
    // Construct completed packet with header + payload
    data_chunk whole_message = raw_header;
    extend_data(whole_message, payload);
    do_send_common(whole_message, handle_send);
}
void channel_proxy::do_send_common(const data_chunk& whole_message,
    send_handler handle_send)
{
    shared_const_buffer buffer(whole_message);
    async_write(*socket_, buffer,
        std::bind(&channel_proxy::call_handle_send, shared_from_this(),
            std::placeholders::_1, handle_send));
}

// channel

channel::channel(async_service& service, socket_ptr socket)
{
    channel_proxy_ptr proxy =
        std::make_shared<channel_proxy>(service, socket);
    proxy->start();
    weak_proxy_ = proxy;
}
channel::~channel()
{
    stop();
}

void channel::stop()
{
    channel_proxy_ptr proxy = weak_proxy_.lock();
    // Slowly shutdown
    if (proxy)
        proxy->stop();
}
bool channel::stopped() const
{
    channel_proxy_ptr proxy = weak_proxy_.lock();
    // Slowly shutdown
    if (!proxy)
        return true;
    else
        return proxy->stopped();
}

void channel::send_raw(const message::header& packet_header,
    const data_chunk& payload, channel_proxy::send_handler handle_send)
{
    channel_proxy_ptr proxy = weak_proxy_.lock();
    if (!proxy)
        handle_send(error::service_stopped);
    else
        proxy->send_raw(packet_header, payload, handle_send);
}

void channel::subscribe_version(
    channel_proxy::receive_version_handler handle_receive)
{
    channel_proxy_ptr proxy = weak_proxy_.lock();
    if (!proxy)
        handle_receive(error::service_stopped, message::version());
    else
        proxy->subscribe_version(handle_receive);
}
void channel::subscribe_verack(
    channel_proxy::receive_verack_handler handle_receive)
{
    channel_proxy_ptr proxy = weak_proxy_.lock();
    if (!proxy)
        handle_receive(error::service_stopped, message::verack());
    else
        proxy->subscribe_verack(handle_receive);
}
void channel::subscribe_address(
    channel_proxy::receive_address_handler handle_receive)
{
    channel_proxy_ptr proxy = weak_proxy_.lock();
    if (!proxy)
        handle_receive(error::service_stopped, message::address());
    else
        proxy->subscribe_address(handle_receive);
}
void channel::subscribe_get_address(
    channel_proxy::receive_get_address_handler handle_receive)
{
    channel_proxy_ptr proxy = weak_proxy_.lock();
    if (!proxy)
        handle_receive(error::service_stopped, message::get_address());
    else
        proxy->subscribe_get_address(handle_receive);
}
void channel::subscribe_inventory(
    channel_proxy::receive_inventory_handler handle_receive)
{
    channel_proxy_ptr proxy = weak_proxy_.lock();
    if (!proxy)
        handle_receive(error::service_stopped, message::inventory());
    else
        proxy->subscribe_inventory(handle_receive);
}
void channel::subscribe_get_data(
    channel_proxy::receive_get_data_handler handle_receive)
{
    channel_proxy_ptr proxy = weak_proxy_.lock();
    if (!proxy)
        handle_receive(error::service_stopped, message::get_data());
    else
        proxy->subscribe_get_data(handle_receive);
}
void channel::subscribe_get_blocks(
    channel_proxy::receive_get_blocks_handler handle_receive)
{
    channel_proxy_ptr proxy = weak_proxy_.lock();
    if (!proxy)
        handle_receive(error::service_stopped, message::get_blocks());
    else
        proxy->subscribe_get_blocks(handle_receive);
}
void channel::subscribe_transaction(
    channel_proxy::receive_transaction_handler handle_receive)
{
    channel_proxy_ptr proxy = weak_proxy_.lock();
    if (!proxy)
        handle_receive(error::service_stopped, message::transaction());
    else
        proxy->subscribe_transaction(handle_receive);
}
void channel::subscribe_block(
    channel_proxy::receive_block_handler handle_receive)
{
    channel_proxy_ptr proxy = weak_proxy_.lock();
    if (!proxy)
        handle_receive(error::service_stopped, message::block());
    else
        proxy->subscribe_block(handle_receive);
}
void channel::subscribe_raw(
    channel_proxy::receive_raw_handler handle_receive)
{
    channel_proxy_ptr proxy = weak_proxy_.lock();
    if (!proxy)
        handle_receive(error::service_stopped,
            message::header(), data_chunk());
    else
        proxy->subscribe_raw(handle_receive);
}

void channel::subscribe_stop(
    channel_proxy::stop_handler handle_stop)
{
    channel_proxy_ptr proxy = weak_proxy_.lock();
    if (!proxy)
        handle_stop(error::service_stopped);
    else
        proxy->subscribe_stop(handle_stop);
}

} // namespace libbitcoin

