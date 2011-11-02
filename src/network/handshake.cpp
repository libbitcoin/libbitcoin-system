#include <bitcoin/network/network.hpp>

#include <atomic>
#include <functional>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;

typedef std::atomic<size_t> atomic_counter;
typedef std::shared_ptr<atomic_counter> atomic_counter_ptr;

const size_t clearance_count = 3;

message::version create_version_message(
    message::ip_address network_ip, channel_handle chandle)
{
    message::version version;
    // this is test data.
    version.version = 31900;
    version.services = 1;
    version.timestamp = time(NULL);
    version.addr_me.services = version.services;
    version.addr_me.ip_addr = network_ip;
    version.addr_me.port = 8333;
    version.addr_you.services = version.services;
    version.addr_you.ip_addr = 
        message::ip_address{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                            0x00, 0x00, 0xff, 0xff, 0x0a, 0x00, 0x00, 0x01};
    version.addr_you.port = 8333;
    version.nonce = chandle;
    version.sub_version_num = "";
    version.start_height = 0;
    return version;
}

void handle_message_sent(const std::error_code& ec,
    atomic_counter_ptr counter, network::send_handler callback)
{
    if (ec)
        callback(ec);
    else if (++(*counter) == clearance_count)
        callback(std::error_code());
}

void receive_version(channel_handle chandle, const message::version&,
    network_ptr net, atomic_counter_ptr counter, 
    network::send_handler callback)
{
    net->send(chandle, message::verack(),
        std::bind(&handle_message_sent, _1, counter, callback));
}

void receive_verack(const message::verack&, 
    atomic_counter_ptr counter, network::send_handler callback)
{
    if (++(*counter) == clearance_count)
        callback(std::error_code());
}

void handshake(network_ptr net, channel_handle chandle, 
    network::send_handler callback)
{
    atomic_counter_ptr counter(new atomic_counter(0));
    net->send(chandle, create_version_message(net->get_ip_address(), chandle),
        std::bind(&handle_message_sent, _1, counter, callback));

    net->subscribe_version(chandle, 
        std::bind(&receive_version, chandle, _1, net, counter, callback));
    net->subscribe_verack(chandle, 
        std::bind(&receive_verack, _1, counter, callback));
}

void start_handshake(const std::error_code& ec, channel_handle chandle,
    network_ptr net, network::connect_handler handle_connect)
{
    if (ec)
        handle_connect(ec, chandle);
    else
        handshake(net, chandle, std::bind(handle_connect, _1, chandle));
}

void handshake_connect(network_ptr net, const std::string& hostname,
    unsigned short port, network::connect_handler handle_connect)
{
    net->connect(hostname, port, 
        std::bind(&start_handshake, _1, _2, net, handle_connect));
}

} // libbitcoin

