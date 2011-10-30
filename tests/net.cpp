#include <bitcoin/network/network.hpp>
#include <bitcoin/util/logger.hpp>

#include <atomic>
#include <functional>
#include <iostream>

using std::placeholders::_1;
using std::placeholders::_2;
using namespace libbitcoin;

void error_exit(const std::string& message, int status=1)
{
    log_error() << "net: " << message;
    exit(status);
}

std::mutex mutex;
std::condition_variable condition;
size_t inv_count = 0;

void receive_inv(network_ptr net, channel_handle chandle,
    const message::inv& packet)
{
    log_info() << "Received:";
    for (const message::inv_vect& ivv: packet.invs)
    {
        if (ivv.type != message::inv_type::block)
            log_info() << "  --";
        else
            log_info() << "  " << pretty_hex(ivv.hash);
    }
    net->subscribe_inv(chandle, std::bind(&receive_inv, net, chandle, _1));
    std::unique_lock<std::mutex> lock(mutex);
    ++inv_count;
    condition.notify_one();
}

void handle_send_getblock(const std::error_code& ec)
{
    if (ec)
        error_exit(ec.message());
}

message::getblocks create_getblocks_message()
{
    message::getblocks packet;
    hash_digest genesis{0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0xd6, 0x68, 
                        0x9c, 0x08, 0x5a, 0xe1, 0x65, 0x83, 0x1e, 0x93, 
                        0x4f, 0xf7, 0x63, 0xae, 0x46, 0xa2, 0xa6, 0xc1, 
                        0x72, 0xb3, 0xf1, 0xb6, 0x0a, 0x8c, 0xe2, 0x6f};
    packet.locator_start_hashes.push_back(genesis);
    packet.hash_stop = hash_digest{0, 0, 0, 0, 0, 0, 0, 0, 
                                   0, 0, 0, 0, 0, 0, 0, 0, 
                                   0, 0, 0, 0, 0, 0, 0, 0, 
                                   0, 0, 0, 0, 0, 0, 0, 0};
    return packet;
}

typedef std::atomic<size_t> atomic_counter;
typedef std::shared_ptr<atomic_counter> atomic_counter_ptr;
typedef std::function<void ()> callback_handler;

const size_t clearance_count = 2;

void send_getblocks(network_ptr net, channel_handle chandle)
{
    net->send(chandle, create_getblocks_message(), 
        std::bind(&handle_send_getblock, _1));
}

void handle_verack_sent(const std::error_code& ec,
    atomic_counter_ptr counter, callback_handler callback)
{
    if (ec)
        error_exit(ec.message());
    if (++(*counter) == clearance_count)
        callback();
}

void receive_version(channel_handle chandle, const message::version&,
    network_ptr net, atomic_counter_ptr counter, callback_handler callback)
{
    net->send(chandle, message::verack(),
        std::bind(&handle_verack_sent, _1, counter, callback));
}

void receive_verack(const message::verack&, 
    atomic_counter_ptr counter, callback_handler callback)
{
    if (++(*counter) == clearance_count)
        callback();
}

void handle_connect(const std::error_code& ec, channel_handle chandle,
    network_ptr net)
{
    if (ec)
        error_exit(ec.message());
    net->subscribe_inv(chandle, std::bind(&receive_inv, net, chandle, _1));

    atomic_counter_ptr counter(new atomic_counter(0));
    callback_handler callback = std::bind(&send_getblocks, net, chandle);
    net->subscribe_version(chandle,
        std::bind(&receive_version, chandle, _1, net, counter, callback));
    net->subscribe_verack(chandle,
        std::bind(&receive_verack, _1, counter, callback));
}

int main()
{
    network_ptr net(new network_impl);
    net->connect("localhost", 8333, std::bind(&handle_connect, _1, _2, net));

    std::unique_lock<std::mutex> lock(mutex);
    condition.wait(lock, []{ return inv_count >= 500; });
    return 0;
}

