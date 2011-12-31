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

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;

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
        const message::inventory&)> receive_inventory_handler;
    typedef std::function<void (const std::error_code&,
        const message::block&)> receive_block_handler;

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
    // inv
    // getdata
    // getblocks
    // getheaders
    // tx
    // block
    // headers
    // getaddr
    // checkorder [deprecated]
    // submitorder [deprecated]
    // reply [deprecated]
    // ping
    // alert

    void subscribe_version(receive_version_handler handle_receive);
    void subscribe_verack(receive_verack_handler handle_receive);
    void subscribe_address(receive_address_handler handle_receive);
    void subscribe_inventory(receive_inventory_handler handle_receive);
    void subscribe_block(receive_block_handler handle_receive);

    template<typename Message>
    void send(const Message& packet, send_handler handle_send)
    {
        if (killed_)
            handle_send(error::channel_stopped);
        else
            strand_->post(std::bind(&channel::do_send<Message>,
                shared_from_this(), packet, handle_send));
    }

private:
    typedef std::stack<receive_version_handler> version_registry_stack;
    typedef std::stack<receive_verack_handler> verack_registry_stack;
    typedef std::stack<receive_address_handler> address_registry_stack;
    typedef std::stack<receive_inventory_handler> inventory_registry_stack;
    typedef std::stack<receive_block_handler> block_registry_stack;

    template<typename Message, typename Callback, typename Registry>
    void generic_subscribe(Callback handle_message, Registry& registry)
    {
        // Subscribing must be immediate. We cannot switch thread contexts
        if (killed_)
            handle_message(error::channel_stopped, Message());
        else
            do_generic_subscribe(handle_message, registry);
    }
    template<typename Callback, typename Registry>
    void do_generic_subscribe(Callback handle_message, Registry& registry)
    {
        std::unique_lock<std::mutex> lock(registries_mutex_);
        registry.push(handle_message);
        lock.unlock();
    }

    template<typename Message>
    void do_send(const Message& packet, send_handler handle_send)
    {
        data_chunk payload = export_->to_network(packet),
            header = export_->create_header(packet, payload);
        // Construct completed packet with header + payload
        data_chunk whole_message = header;
        extend_data(whole_message, payload);
        shared_const_buffer buffer(whole_message);
        async_write(*socket_, buffer, std::bind(
            &channel::pre_handle_send, this, _1, handle_send));
    }

    template<typename Message, typename Registry>
    void relay(const Message& packet, Registry& registry)
    {
        std::unique_lock<std::mutex> lock(registries_mutex_);
        Registry notify_copy = registry;
        registry = Registry();
        lock.unlock();

        while (!notify_copy.empty())
        {
            notify_copy.top()(std::error_code(), packet);
            notify_copy.pop();
        }
        BITCOIN_ASSERT(notify_copy.empty());
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

    template<typename Message, typename Registry>
    bool transport(const data_chunk& payload_stream,
        std::function<Message (const data_chunk&)> read_message,
        Registry& registry)
    {
        Message packet;
        try
        {
            packet = read_message(payload_stream);
        }
        catch (end_of_stream)
        {
            log_warning(log_domain::network) << "Premature end of stream";
            stop();
            return false;
        }
        relay(packet, registry);
        return true;
    }

    // Calls the send handler after a successful send, translating
    // the boost error_code to std::error_code
    void pre_handle_send(const boost::system::error_code& ec,
        send_handler handle_send);

    void handle_timeout(const boost::system::error_code& ec);
    void reset_timeout();

    bool problems_check(const boost::system::error_code& ec);

    std::atomic<bool> killed_;

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
    deadline_timer_ptr timeout_;

    std::mutex registries_mutex_;
    version_registry_stack version_registry_;
    verack_registry_stack verack_registry_;
    address_registry_stack address_registry_;
    inventory_registry_stack inventory_registry_;
    block_registry_stack block_registry_;
};

} // libbitcoin

#endif

