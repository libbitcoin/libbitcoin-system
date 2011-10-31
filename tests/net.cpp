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
    inv_count += packet.invs.size();
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
    packet.hash_stop.fill(0);
    return packet;
}

void handle_handshake(const std::error_code& ec, channel_handle chandle,
    network_ptr net)
{
    net->subscribe_inv(chandle, std::bind(&receive_inv, net, chandle, _1));
    net->send(chandle, create_getblocks_message(), 
        std::bind(&handle_send_getblock, _1));
}

int main()
{
    network_ptr net(new network_impl);
    handshake_connect(net, "localhost", 8333,
        std::bind(&handle_handshake, _1, _2, net));

    std::unique_lock<std::mutex> lock(mutex);
    condition.wait(lock, []{ return inv_count >= 500; });
    return 0;
}

