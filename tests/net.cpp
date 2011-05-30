#include "bitcoin/net/messages.hpp"
#include "bitcoin/net/delegator.hpp"
#include "bitcoin/net/dialect.hpp"

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
    libbitcoin::net::delegator_ptr net(new libbitcoin::net::default_delegator(libbitcoin::net::network_flags::none));
    libbitcoin::net::channel_handle channel = net->connect("localhost");
    channel = net->connect("localhost");
    channel = net->connect("localhost");
    channel = net->connect("localhost");
    if (false) {
        std::cerr << "noes\n";
        return -1;
    }
    libbitcoin::net::message::version version = create_version_message();
    //channel->send(version);
    net->send(channel, version);
    //net->disconnect(channel);
    std::cin.get();
    return 0;
}

int run_accept()
{
    libbitcoin::net::delegator_ptr net(new libbitcoin::net::default_delegator(libbitcoin::net::network_flags::accept_incoming));
    std::cin.get();
    return 0;
}

int main()
{
    return run_connect();
    //return run_accept();
}

