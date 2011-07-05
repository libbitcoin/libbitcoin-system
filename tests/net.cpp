#include <bitcoin/net/messages.hpp>
#include <bitcoin/net/network.hpp>
#include <bitcoin/storage/storage.hpp>
#include <bitcoin/kernel.hpp>

#include <memory>
#include <iostream>
#include <iomanip>

libbitcoin::net::message::version create_version_message()
{
    libbitcoin::net::message::version version;
    // this is test data. should be in the core eventually
    version.version = 31900;
    version.services = 1;
    version.timestamp = time(NULL);
    version.addr_me.services = version.services;
    version.addr_me.ip_addr = decltype(version.addr_me.ip_addr){0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0};
    version.addr_me.ip_addr[0] = 0;
    version.addr_me.port = 8333;
    version.addr_you.services = version.services;
    version.addr_you.ip_addr = decltype(version.addr_you.ip_addr){0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff, 10, 0, 0, 1};
    version.addr_you.port = 8333;
    version.nonce = 0xdeadbeef;
    version.sub_version_num = "";
    version.start_height = 0;
    return version;
}

libbitcoin::net::message::getblocks create_getblocks_message()
{
    libbitcoin::net::message::getblocks getblocks;
    libbitcoin::hash_digest genesis_hash{0, 0, 0, 0, 0, 0x19, 0xd6, 0x68, 0x9c, 0x8, 0x5a, 0xe1, 0x65, 0x83, 0x1e, 0x93, 0x4f, 0xf7, 0x63, 0xae, 0x46, 0xa2, 0xa6, 0xc1, 0x72, 0xb3, 0xf1, 0xb6, 0xa, 0x8c, 0xe2, 0x6f};
    getblocks.locator_start_hashes.push_back(genesis_hash);
    getblocks.hash_stop = libbitcoin::hash_digest{0, 0, 0, 0, 0x82, 0xb5, 0x1, 0x55, 0x89, 0xa3, 0xfd, 0xf2, 0xd4, 0xba, 0xff, 0x40, 0x3e, 0x6f, 0xb, 0xe0, 0x35, 0xa5, 0xd9, 0x74, 0x2c, 0x1c, 0xae, 0x62, 0x95, 0x46, 0x44, 0x49};
    return getblocks;
}

int run_connect()
{
    libbitcoin::kernel_ptr kernel(new libbitcoin::kernel);
    libbitcoin::net::network_ptr net(new libbitcoin::net::network_impl(kernel));
    bool ec = false;
    libbitcoin::net::channel_handle channel = net->connect(ec, "localhost");
    if (ec) {
        std::cerr << "noes\n";
        return -1;
    }
    libbitcoin::net::message::version version = create_version_message();
    net->send(channel, version);
    sleep(3);
    net->disconnect(channel);
    std::cin.get();
    return 0;
}

int run_accept()
{
    libbitcoin::kernel_ptr kernel(new libbitcoin::kernel);
    libbitcoin::net::network_ptr net(new libbitcoin::net::network_impl(kernel));
    net->start_accept();
    std::cin.get();
    return 0;
}

int run_kernel()
{
    libbitcoin::kernel_ptr kernel(new libbitcoin::kernel);
    libbitcoin::net::network_ptr net(
            new libbitcoin::net::network_impl(kernel));
    kernel->register_network(net);
    libbitcoin::storage::storage_ptr storage(
            new libbitcoin::storage::memory_storage(kernel));
    kernel->register_storage(storage);
    bool ec = false;
    libbitcoin::net::channel_handle channel = net->connect(ec, "localhost");
    if (ec) {
        std::cerr << "noes\n";
        return -1;
    }
    libbitcoin::net::message::version version = create_version_message();
    net->send(channel, version);

    std::cin.get();
    //net->send(channel, libbitcoin::net::message::getaddr());

    libbitcoin::net::message::getblocks getblocks = create_getblocks_message();
    net->send(channel, getblocks);
    std::cin.get();
    return 0;
}

int main()
{
    return run_kernel();
    //return run_connect();
    //return run_accept();
}

