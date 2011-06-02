#ifndef LIBBITCOIN_KERNEL_H
#define LIBBITCOIN_KERNEL_H

#include <boost/utility.hpp>
#include <memory>

#include "bitcoin/net/messages.hpp"
#include "bitcoin/net/types.hpp"

namespace libbitcoin {

using std::shared_ptr;

class kernel : boost::noncopyable, public std::enable_shared_from_this<kernel>
{
public:
    void register_network(shared_ptr<net::network> net_comp);
    shared_ptr<net::network> get_network();
    // Callbacks from network component
    void send_failed(net::channel_handle chandle, 
            net::message::version message);
    void send_failed(net::channel_handle chandle, 
            net::message::verack message);

    bool recv_message(net::channel_handle chandle,
            net::message::version message);
    bool recv_message(net::channel_handle chandle,
            net::message::verack message);
private: 
    shared_ptr<net::network> network_component_;
};

typedef shared_ptr<kernel> kernel_ptr;

} // libbitcoin

#endif

