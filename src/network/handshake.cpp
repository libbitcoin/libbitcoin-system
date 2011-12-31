#include <bitcoin/network/network.hpp>

#include <atomic>
#include <functional>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;

typedef std::atomic<size_t> atomic_counter;
typedef std::shared_ptr<atomic_counter> atomic_counter_ptr;

const size_t clearance_count = 3;

message::version create_version_message()
{
    message::version version;
    // this is test data.
    version.version = 31900;
    version.services = 1;
    version.timestamp = time(NULL);
    version.address_me.services = version.services;
    version.address_me.ip = 
        message::ip_address{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                            0x00, 0x00, 0xff, 0xff, 0x0a, 0x00, 0x00, 0x01};
    version.address_me.port = 8333;
    version.address_you.services = version.services;
    version.address_you.ip = 
        message::ip_address{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                            0x00, 0x00, 0xff, 0xff, 0x0a, 0x00, 0x00, 0x01};
    version.address_you.port = 8333;
    version.nonce = rand();
    version.user_agent = "/libbitcoin:0.1a/";
    version.start_height = 0;
    return version;
}

void handle_message_sent(const std::error_code& ec,
    atomic_counter_ptr counter, handshake_handler completion_callback)
{
    if (ec)
        completion_callback(ec);
    else if (++(*counter) == clearance_count)
        completion_callback(std::error_code());
}

void receive_version(const std::error_code& ec, const message::version&,
    channel_ptr node, atomic_counter_ptr counter,
    handshake_handler completion_callback)
{
    if (ec)
        completion_callback(ec);
    else
        node->send(message::verack(), std::bind(
            &handle_message_sent, _1, counter, completion_callback));
}

void receive_verack(const std::error_code& ec, const message::verack&, 
    atomic_counter_ptr counter, handshake_handler completion_callback)
{
    if (ec)
        completion_callback(ec);
    else if (++(*counter) == clearance_count)
        completion_callback(std::error_code());
}

void handshake(channel_ptr node, handshake_handler handle_handshake)
{
    atomic_counter_ptr counter(new atomic_counter(0));
    node->send(create_version_message(),
        std::bind(&handle_message_sent, _1, counter, handle_handshake));

    node->subscribe_version(std::bind(
        &receive_version, _1, _2, node, counter, handle_handshake));
    node->subscribe_verack(std::bind(
        &receive_verack, _1, _2, counter, handle_handshake));
}

void start_handshake(const std::error_code& ec, channel_ptr node,
    network::connect_handler handle_connect)
{
    if (ec)
        handle_connect(ec, node);
    else
        handshake(node, std::bind(handle_connect, _1, node));
}

void handshake_connect(network_ptr net, const std::string& hostname,
    uint16_t port, network::connect_handler handle_connect)
{
    net->connect(hostname, port, 
        std::bind(&start_handshake, _1, _2, handle_connect));
}

} // libbitcoin

