#ifndef LIBBITCOIN_KERNEL_H
#define LIBBITCOIN_KERNEL_H

#include <boost/asio.hpp>
#include <boost/utility.hpp>
#include <memory>
#include <thread>

#include <bitcoin/net/messages.hpp>
#include <bitcoin/types.hpp>

namespace libbitcoin {

using boost::asio::deadline_timer;
using std::shared_ptr;

class kernel : boost::noncopyable, public std::enable_shared_from_this<kernel>
{
public:
    kernel();

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
    void send_failed(net::channel_handle chandle,
            net::message::getdata message);
    void send_failed(net::channel_handle chandle,
            net::message::getblocks message);

    bool recv_message(net::channel_handle chandle,
            net::message::version message);
    bool recv_message(net::channel_handle chandle,
            net::message::verack message);
    bool recv_message(net::channel_handle chandle,
            net::message::addr message);
    bool recv_message(net::channel_handle chandle,
            net::message::inv message);
    bool recv_message(net::channel_handle chandle,
            net::message::block message);

    void handle_connect(net::channel_handle chandle);

    void register_storage(storage::storage_ptr stor_comp);
    storage::storage_ptr get_storage();

private:
    void reset_inventory_poll();
    void request_inventories(const boost::system::error_code& ec);
    void accept_inventories(net::message::inv_list invs, bool ec);
    void send_to_random(net::channel_handle chandle,
            net::message::getdata request_message);

    net::network_ptr network_component_;
    storage::storage_ptr storage_component_;

    service_ptr service_;
    std::thread runner_;
    work_ptr work_;

    shared_ptr<deadline_timer> poll_invs_timeout_;
};

typedef shared_ptr<kernel> kernel_ptr;

} // libbitcoin

#endif

