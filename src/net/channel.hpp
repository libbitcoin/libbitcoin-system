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

#include "bitcoin/net/network.hpp"
#include "bitcoin/net/messages.hpp"

#include "serializer.hpp"

namespace libbitcoin {
namespace net {

using boost::asio::io_service;
using boost::asio::ip::tcp;
using boost::asio::deadline_timer;
using std::shared_ptr;

class network;
class dialect;

class channel_pimpl : boost::noncopyable
{
public:
    struct init_data
    {
        shared_ptr<network> parent_gateway;
        shared_ptr<dialect> translator;
        shared_ptr<io_service> service;
        shared_ptr<tcp::socket> socket;
    };

    channel_pimpl(const init_data& dat);
    ~channel_pimpl();

    void send(message::version version);
    void send(message::verack verack);
    void send(message::getaddr getaddr);
    channel_handle get_id() const;

private:
    static channel_handle chan_id_counter;
    channel_handle channel_id_;

    void read_header();
    void read_checksum(message::header header_msg);
    void read_payload(message::header header_msg);

    void handle_read_header(const boost::system::error_code& ec, 
            size_t bytes_transferred);
    void handle_read_checksum(message::header header_msg,
            const boost::system::error_code& ec, size_t bytes_transferred);
    void handle_read_payload(message::header header_msg,
            const boost::system::error_code& ec, size_t bytes_transferred);

    void handle_send(const boost::system::error_code& ec);

    void handle_timeout(const boost::system::error_code& ec);
    void reset_timeout();

    // Removes self from parent
    void destroy_self();
    bool problems_check(const boost::system::error_code& ec);

    shared_ptr<tcp::socket> socket_;
    network_ptr parent_gateway_;
    dialect_ptr translator_;

    // Header minus checksum is 4 + 12 + 4 = 20 bytes
    static const size_t header_chunk_size = 20;
    // Checksum size is 4 bytes
    static const size_t header_checksum_size = 4;

    boost::array<uint8_t, header_chunk_size> inbound_header_;
    boost::array<uint8_t, header_checksum_size> inbound_checksum_;
    std::vector<uint8_t> inbound_payload_;
    shared_ptr<deadline_timer> timeout_;
};

} // net
} // libbitcoin

#endif

