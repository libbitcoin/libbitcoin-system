#ifndef LIBBITCOIN_KERNEL_H
#define LIBBITCOIN_KERNEL_H

#include <boost/utility.hpp>
#include <memory>

#include <bitcoin/net/messages.hpp>
#include <bitcoin/net/types.hpp>
#include <bitcoin/storage/types.hpp>

namespace libbitcoin {

using std::shared_ptr;

class kernel : boost::noncopyable, public std::enable_shared_from_this<kernel>
{
public:
    void register_network(net::network_ptr net_comp);
    net::network_ptr get_network();
    // Callbacks from network component
    void send_failed(net::channel_handle chandle, 
            net::message::version message);
    void send_failed(net::channel_handle chandle, 
            net::message::verack message);
    void send_failed(net::channel_handle chandle, 
            net::message::getaddr message);
    void send_failed(net::channel_handle chandle, 
            net::message::inv message);

    bool recv_message(net::channel_handle chandle,
            net::message::version message);
    bool recv_message(net::channel_handle chandle,
            net::message::verack message);
    bool recv_message(net::channel_handle chandle,
            net::message::addr message);
    bool recv_message(net::channel_handle chandle,
            net::message::inv message);

    void register_storage(storage::storage_ptr stor_comp);
    storage::storage_ptr get_storage();
private: 
    net::network_ptr network_component_;
    storage::storage_ptr storage_component_;
};

typedef shared_ptr<kernel> kernel_ptr;

} // libbitcoin

#endif

