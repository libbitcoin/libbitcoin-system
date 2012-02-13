#ifndef LIBBITCOIN_NET_CHANNEL_H
#define LIBBITCOIN_NET_CHANNEL_H

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/utility.hpp>
#include <boost/asio/streambuf.hpp>
#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <stack>

#include <bitcoin/network/network.hpp>
#include <bitcoin/network/shared_const_buffer.hpp>
#include <bitcoin/messages.hpp>
#include <bitcoin/exporter.hpp>
#include <bitcoin/data_helpers.hpp>
#include <bitcoin/utility/assert.hpp>
#include <bitcoin/utility/logger.hpp>
#include <bitcoin/utility/serializer.hpp>
#include <bitcoin/utility/subscriber.hpp>

namespace libbitcoin {

class channel
  : public std::enable_shared_from_this<channel>
{
public:
    typedef std::function<void (const std::error_code&)> send_handler;

    typedef std::function<void (const std::error_code&, 
        const message::version&)> receive_version_handler;

    typedef std::function<void (const std::error_code&,
        const message::verack&)> receive_verack_handler;

    typedef std::function<void (const std::error_code&,
        const message::address&)> receive_address_handler;

    typedef std::function<void (const std::error_code&,
        const message::get_address&)> receive_get_address_handler;

    typedef std::function<void (const std::error_code&,
        const message::inventory&)> receive_inventory_handler;

    typedef std::function<void (const std::error_code&,
        const message::get_data&)> receive_get_data_handler;

    typedef std::function<void (const std::error_code&,
        const message::get_blocks&)> receive_get_blocks_handler;

    typedef std::function<void (const std::error_code&,
        const message::transaction&)> receive_transaction_handler;

    typedef std::function<void (const std::error_code&,
        const message::block&)> receive_block_handler;

    typedef std::function<void (const std::error_code&,
        const message::header&, const data_chunk&)> receive_raw_handler;

    typedef std::function<void (const std::error_code&)> stop_handler;

    channel(socket_ptr socket, thread_core_ptr threaded, exporter_ptr saver);
    ~channel();

    channel(const channel&) = delete;
    void operator=(const channel&) = delete;

    void start();
    void stop();

    // List of bitcoin messages
    // version
    // verack
    // addr
    // getaddr
    // inv
    // getdata
    // getblocks
    // tx
    // block
    // getheaders   [unused]
    // headers      [unused]
    // checkorder   [deprecated]
    // submitorder  [deprecated]
    // reply        [deprecated]
    // ping         [internal]
    // alert        [not supported]

    template <typename Message>
    void send(const Message& packet, send_handler handle_send)
    {
        if (stopped_)
            handle_send(error::channel_stopped);
        else
            strand_->post(std::bind(&channel::do_send<Message>,
                shared_from_this(), packet, handle_send));
    }
    void send_raw(const message::header& packet_header,
        const data_chunk& payload, send_handler handle_send);

    void subscribe_version(receive_version_handler handle_receive);
    void subscribe_verack(receive_verack_handler handle_receive);
    void subscribe_address(receive_address_handler handle_receive);
    void subscribe_get_address(receive_get_address_handler handle_receive);
    void subscribe_inventory(receive_inventory_handler handle_receive);
    void subscribe_get_data(receive_get_data_handler handle_receive);
    void subscribe_get_blocks(receive_get_blocks_handler handle_receive);
    void subscribe_transaction(receive_transaction_handler handle_receive);
    void subscribe_block(receive_block_handler handle_receive);
    void subscribe_raw(receive_raw_handler handle_receive);

    void subscribe_stop(stop_handler handle_stop);

private:
    typedef subscriber<const std::error_code&, const message::version&>
        version_subscriber_type;
    typedef subscriber<const std::error_code&, const message::verack&>
        verack_subscriber_type;
    typedef subscriber<const std::error_code&, const message::address&>
        address_subscriber_type;
    typedef subscriber<const std::error_code&, const message::get_address&>
        get_address_subscriber_type;
    typedef subscriber<const std::error_code&, const message::inventory&>
        inventory_subscriber_type;
    typedef subscriber<const std::error_code&, const message::get_data&>
        get_data_subscriber_type;
    typedef subscriber<const std::error_code&, const message::get_blocks&>
        get_blocks_subscriber_type;
    typedef subscriber<const std::error_code&, const message::transaction&>
        transaction_subscriber_type;
    typedef subscriber<const std::error_code&, const message::block&>
        block_subscriber_type;
    typedef subscriber<const std::error_code&,
        const message::header&, const data_chunk&> raw_subscriber_type;

    typedef subscriber<const std::error_code&> stop_subscriber_type;

    template <typename Message>
    void do_send(const Message& packet, send_handler handle_send)
    {
        shared_const_buffer buffer(
            create_raw_message(export_, packet));
        async_write(*socket_, buffer,
            std::bind(&channel::call_handle_send, shared_from_this(),
                std::placeholders::_1, handle_send));
    }
    void do_send_raw(const message::header& packet_header,
        const data_chunk& payload, send_handler handle_send);
    void do_send_common(const data_chunk& whole_message,
        send_handler handle_send);

    template <typename Message, typename Callback, typename SubscriberPtr>
    void generic_subscribe(Callback handle_message,
        SubscriberPtr message_subscribe)
    {
        // Subscribing must be immediate. We cannot switch thread contexts
        if (stopped_)
            handle_message(error::channel_stopped, Message());
        else
            message_subscribe->subscribe(handle_message);
    }

    void read_header();
    void read_checksum(const message::header& header_msg);
    void read_payload(const message::header& header_msg);

    void handle_read_header(const boost::system::error_code& ec,
        size_t bytes_transferred);
    void handle_read_checksum(message::header& header_msg,
        const boost::system::error_code& ec, size_t bytes_transferred);
    void handle_read_payload(const message::header& header_msg,
        const boost::system::error_code& ec, size_t bytes_transferred);

    template<typename Message, typename ExportFunc, typename SubscriberPtr>
    bool transport(const data_chunk& payload_stream,
        ExportFunc export_func, SubscriberPtr message_subscriber)
    {
        Message packet;
        try
        {
            auto convert =
                std::bind(export_func, export_, std::placeholders::_1);
            packet = convert(payload_stream);
        }
        catch (end_of_stream)
        {
            log_warning(log_domain::network) << "Premature end of stream";
            message_subscriber->relay(error::bad_stream, Message());
            stop();
            return false;
        }
        message_subscriber->relay(std::error_code(), packet);
        return true;
    }

    // Calls the send handler after a successful send, translating
    // the boost error_code to std::error_code
    void call_handle_send(const boost::system::error_code& ec,
        send_handler handle_send);

    void handle_timeout(const boost::system::error_code& ec);
    void handle_heartbeat(const boost::system::error_code& ec);
    
    void set_timeout(const boost::posix_time::time_duration timeout);
    void set_heartbeat(const boost::posix_time::time_duration timeout);
    void reset_timers();

    bool problems_check(const boost::system::error_code& ec);
    void stop_impl();

    std::atomic<bool> stopped_;

    socket_ptr socket_;
    network_ptr network_;
    exporter_ptr export_;

    // Header minus checksum is 4 + 12 + 4 = 20 bytes
    static constexpr size_t header_chunk_size = 20;
    // Checksum size is 4 bytes
    static constexpr size_t header_checksum_size = 4;

    boost::array<uint8_t, header_chunk_size> inbound_header_;
    boost::array<uint8_t, header_checksum_size> inbound_checksum_;
    std::vector<uint8_t> inbound_payload_;

    // We keep the service alive for lifetime rules
    thread_core_ptr threaded_;
    strand_ptr strand_;
    deadline_timer_ptr timeout_, heartbeat_;

    version_subscriber_type::ptr version_subscriber_;
    verack_subscriber_type::ptr verack_subscriber_;
    address_subscriber_type::ptr address_subscriber_;
    get_address_subscriber_type::ptr get_address_subscriber_;
    inventory_subscriber_type::ptr inventory_subscriber_;
    get_data_subscriber_type::ptr get_data_subscriber_;
    get_blocks_subscriber_type::ptr get_blocks_subscriber_;
    transaction_subscriber_type::ptr transaction_subscriber_;
    block_subscriber_type::ptr block_subscriber_;
    raw_subscriber_type::ptr raw_subscriber_;

    stop_subscriber_type::ptr stop_subscriber_;
};

} // libbitcoin

#endif

