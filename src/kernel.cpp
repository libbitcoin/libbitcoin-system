#include "bitcoin/kernel.hpp"

#include "bitcoin/net/network.hpp"

namespace libbitcoin {

void kernel::register_network(shared_ptr<net::network> net_comp)
{
    network_component_ = net_comp;
}

shared_ptr<net::network> kernel::get_network()
{
    return network_component_;
}

void kernel::send_failed(net::channel_handle chandle, 
        net::message::version message)
{
}

void kernel::send_failed(net::channel_handle chandle, 
        net::message::verack message)
{
}

bool kernel::recv_message(net::channel_handle chandle,
        net::message::version message)
{
    network_component_->send(chandle, net::message::verack());
    return true;
}

bool kernel::recv_message(net::channel_handle chandle, 
        net::message::verack message)
{
    return true;
}

} // libbitcoin

