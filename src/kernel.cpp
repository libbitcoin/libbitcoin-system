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

bool kernel::recv_message(net::channel_handle chandle,
        net::message::version message)
{
    return true;
}

} // libbitcoin

