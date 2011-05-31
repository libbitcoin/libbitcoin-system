#ifndef LIBBITCOIN_KERNEL_H
#define LIBBITCOIN_KERNEL_H

#include <boost/utility.hpp>
#include <memory>

namespace libbitcoin {

using std::shared_ptr;

namespace net {
    class network;
} // net

class kernel : boost::noncopyable, public std::enable_shared_from_this<kernel>
{
public:
    void register_network(shared_ptr<net::network> net_comp);
    shared_ptr<net::network> network();
private: 
    shared_ptr<net::network> network_component_;
};

typedef shared_ptr<kernel> kernel_ptr;

} // libbitcoin

#endif

