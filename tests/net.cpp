#include <bitcoin/messages.hpp>
#include <bitcoin/network/network.hpp>
#include <bitcoin/storage/postgresql_storage.hpp>
#include <bitcoin/kernel.hpp>
#include <bitcoin/constants.hpp>

#include <memory>
#include <iostream>
#include <iomanip>

using libbitcoin::network_ptr;
using libbitcoin::kernel_ptr;
using std::placeholders::_1;
using std::placeholders::_2;

libbitcoin::message::getblocks create_getblocks_message()
{
    libbitcoin::message::getblocks getblocks;
    libbitcoin::hash_digest genesis_hash{0, 0, 0, 0, 0, 0x19, 0xd6, 0x68, 0x9c, 0x8, 0x5a, 0xe1, 0x65, 0x83, 0x1e, 0x93, 0x4f, 0xf7, 0x63, 0xae, 0x46, 0xa2, 0xa6, 0xc1, 0x72, 0xb3, 0xf1, 0xb6, 0xa, 0x8c, 0xe2, 0x6f};
    getblocks.locator_start_hashes.push_back(genesis_hash);
    //getblocks.hash_stop = libbitcoin::hash_digest{0, 0, 0, 0, 0x82, 0xb5, 0x1, 0x55, 0x89, 0xa3, 0xfd, 0xf2, 0xd4, 0xba, 0xff, 0x40, 0x3e, 0x6f, 0xb, 0xe0, 0x35, 0xa5, 0xd9, 0x74, 0x2c, 0x1c, 0xae, 0x62, 0x95, 0x46, 0x44, 0x49};
    getblocks.hash_stop = libbitcoin::hash_digest{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    return getblocks;
}

void handle_connect(std::error_code ec, kernel_ptr kernel, 
    libbitcoin::channel_handle channel)
{
    network_ptr net = kernel->get_network();
    if (ec) {
        std::cerr << "noes\n";
        return;
    }
    sleep(3);
    net->disconnect(channel);
}

int run_connect()
{
    libbitcoin::kernel_ptr kernel(new libbitcoin::kernel);
    libbitcoin::network_ptr net(new libbitcoin::network_impl(kernel));
    net->connect("localhost", 8333, std::bind(&handle_connect, _1, kernel, _2));
    std::cin.get();
    return 0;
}

int run_accept()
{
    libbitcoin::kernel_ptr kernel(new libbitcoin::kernel);
    libbitcoin::network_ptr net(new libbitcoin::network_impl(kernel));
    net->start_accept();
    std::cin.get();
    return 0;
}

void handle_connect_kernel(kernel_ptr kernel, std::error_code ec, 
    libbitcoin::channel_handle channel)
{
    network_ptr net = kernel->get_network();
    if (ec) {
        std::cerr << "noes\n";
        return;
    }
    std::cout << "handle connect\n";

    //net->send(channel, libbitcoin::message::getaddr());
}

int run_kernel(kernel_ptr kernel)
{
    libbitcoin::network_ptr net(
            new libbitcoin::network_impl(kernel));
    kernel->register_network(net);
    //libbitcoin::storage_ptr storage(
    //        new libbitcoin::memory_storage());
    libbitcoin::storage_ptr storage(
            new libbitcoin::postgresql_storage("bitcoin", "genjix", ""));
    kernel->register_storage(storage);
    net->connect("localhost", 8333, std::bind(&handle_connect_kernel, kernel, _1, _2));
    std::cin.get();

    libbitcoin::message::getblocks getblocks = create_getblocks_message();
    net->send(0, getblocks);
    std::cin.get();
    return 0;
}

int main()
{
    std::cout << "max money = " << libbitcoin::max_money() << "\n";
    libbitcoin::kernel_ptr kernel(new libbitcoin::kernel);
    return run_kernel(kernel);
    //return run_connect();
    //return run_accept();
}

