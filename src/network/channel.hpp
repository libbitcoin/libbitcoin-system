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
#include <bitcoin/util/serializer.hpp>

namespace libbitcoin {

class network;
class dialect;

class channel_pimpl
  : private boost::noncopyable
{
public:
    channel_pimpl(network_ptr parent_gateway, dialect_ptr translator,
        service_ptr service, socket_ptr socket);
    ~channel_pimpl();

    void send(const message::version& packet,
        network::send_handler handle_send);
    void send(const message::verack& packet,
        network::send_handler handle_send);
    void send(const message::getaddr& packet,
        network::send_handler handle_send);
    void send(const message::getdata& packet,
        network::send_handler handle_send);
    void send(const message::getblocks& packet,
        network::send_handler handle_send);

    channel_handle get_id() const;

private:
    channel_handle channel_id_;

    void read_header();
    void read_checksum(const message::header& header_msg);
    void read_payload(const message::header& header_msg);

    void handle_read_header(const boost::system::error_code& ec,
        size_t bytes_transferred);
    void handle_read_checksum(message::header& header_msg,
        const boost::system::error_code& ec, size_t bytes_transferred);
    void handle_read_payload(const message::header& header_msg,
        const boost::system::error_code& ec, size_t bytes_transferred);

    template<typename P>
    bool transport_payload(P payload, bool ret_errc)
    {
        if (ret_errc)
        {
            destroy_self();
            return false;
        }
        network_->relay(channel_id_, payload);
        return true;
    }

    void pre_handle_send(const boost::system::error_code& ec,
        network::send_handler handle_send);

    void handle_timeout(const boost::system::error_code& ec);
    void reset_timeout();

    // Removes self from parent
    void destroy_self();
    bool problems_check(const boost::system::error_code& ec);

    // Construct a version message
    message::version create_version_message();

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

