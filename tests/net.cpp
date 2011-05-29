#include "bitcoin/net/messages.hpp"
#include "bitcoin/net/connection_manager.hpp"
#include "bitcoin/net/dialect.hpp"
#include "bitcoin/net/channel.hpp"

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

int run_connect()
{
    libbitcoin::net::connection_manager_ptr net(new libbitcoin::net::default_connection_manager(libbitcoin::net::connection_flags::none));
    libbitcoin::net::channel_ptr channel = net->connect("localhost");
    if (!channel) {
        std::cerr << "noes\n";
        return -1;
    }
    libbitcoin::net::message::version version = create_version_message();
    channel->send(version);
    channel.reset();
    std::cin.get();
    return 0;
}

int run_accept()
{
    libbitcoin::net::connection_manager_ptr net(new libbitcoin::net::default_connection_manager(libbitcoin::net::connection_flags::accept_incoming));
    std::cin.get();
    return 0;
}

int main()
{
    return run_connect();
    //return run_accept();
}

