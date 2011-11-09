#ifndef LIBBITCOIN_NET_CHANNEL_H
#define LIBBITCOIN_NET_CHANNEL_H

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/utility.hpp>
#include <boost/asio/streambuf.hpp>
#include <cstdint>
#include <memory>
#include <mutex>
#include <deque>

#include <bitcoin/network/network.hpp>
#include <bitcoin/messages.hpp>
#include <bitcoin/dialect.hpp>
#include <bitcoin/util/serializer.hpp>

#include "shared_const_buffer.hpp"

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;

class network;
class dialect;

class channel_pimpl
 : private boost::noncopyable
{
public:
    channel_pimpl(network_ptr parent_gateway, dialect_ptr translator,
        service_ptr service, socket_ptr socket);
    ~channel_pimpl();

    template<typename Message>
    void send(const Message& packet, network::send_handler handle_send)
    {
        data_chunk payload = translator_->to_network(packet),
            header = translator_->create_header(packet, payload);
        // Construct completed packet with header + payload
        data_chunk whole_message = header;
        extend_data(whole_message, payload);
        shared_const_buffer buffer(whole_message);
        async_write(*socket_, buffer, std::bind(
            &channel_pimpl::pre_handle_send, this, _1, handle_send));
    }

    channel_handle get_id() const;

private:
    void read_header();
    void read_checksum(const message::header& header_msg);
    void read_payload(const message::header& header_msg);

    void handle_read_header(const boost::system::error_code& ec,
        size_t bytes_transferred);
    void handle_read_checksum(message::header& header_msg,
        const boost::system::error_code& ec, size_t bytes_transferred);
    void handle_read_payload(const message::header& header_msg,
        const boost::system::error_code& ec, size_t bytes_transferred);

    template<typename Message>
    bool transport_payload(const data_chunk& payload_stream,
        std::function<Message (const data_chunk&)> read_message)
    {
        Message packet;
        try
        {
            packet = read_message(payload_stream);
        }
        catch (end_of_stream)
        {
            destroy_self();
            return false;
        }
        network_->relay(channel_id_, packet);
        return true;
    }

    void pre_handle_send(const boost::system::error_code& ec,
        network::send_handler handle_send);

    void handle_timeout(const boost::system::error_code& ec);
    void reset_timeout();

    // Removes self from parent
    void destroy_self();
    bool problems_check(const boost::system::error_code& ec);

    channel_handle channel_id_;

    socket_ptr socket_;
    network_ptr network_;
    dialect_ptr translator_;

    // Header minus checksum is 4 + 12 + 4 = 20 bytes
    static constexpr size_t header_chunk_size = 20;
    // Checksum size is 4 bytes
    static constexpr size_t header_checksum_size = 4;

    boost::array<uint8_t, header_chunk_size> inbound_header_;
    boost::array<uint8_t, header_checksum_size> inbound_checksum_;
    std::vector<uint8_t> inbound_payload_;
    deadline_timer_ptr timeout_;
};

} // libbitcoin

#endif

