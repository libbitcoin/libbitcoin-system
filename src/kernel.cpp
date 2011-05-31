#include "bitcoin/kernel.hpp"

namespace libbitcoin {

void kernel::register_network(shared_ptr<net::network> net_comp)
{
    network_component_ = net_comp;
}

shared_ptr<net::network> kernel::network()
{
    return network_component_;
}

} // libbitcoin

