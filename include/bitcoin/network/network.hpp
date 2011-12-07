#ifndef LIBBITCOIN_NET_NETWORK_H
#define LIBBITCOIN_NET_NETWORK_H

#include <boost/asio.hpp>
#include <boost/utility.hpp>
#include <memory>
#include <thread>

#include <bitcoin/messages.hpp>
#include <bitcoin/network/types.hpp>
#include <bitcoin/error.hpp>
#include <bitcoin/util/threaded_service.hpp>

namespace libbitcoin {

using boost::asio::ip::tcp;
using boost::asio::io_service;

class network
 : public threaded_service, public std::enable_shared_from_this<network>
{
public:
    typedef std::function<void (
        const std::error_code&, channel_handle chandle)> connect_handler;

    typedef std::function<void (const std::error_code&)> send_handler;

    typedef std::function<void (const message::version&)> 
        receive_version_handler;
    typedef std::function<void (const message::verack&)>
        receive_verack_handler;
    typedef std::function<void (const message::addr&)>
        receive_addr_handler;
    typedef std::function<void (const message::inventory&)>
        receive_inventory_handler;
    typedef std::function<void (const message::block&)>
        receive_block_handler;

    network();
    ~network();

    network(const network&) = delete;
    void operator=(const network&) = delete;

    bool start_accept();
    void connect(const std::string& hostname, unsigned short port, 
        connect_handler handle_connect);
    void listen(connect_handler handle_connect);
    size_t connection_count() const;
    void disconnect(channel_handle chandle);  

    void set_ip_address(std::string ip_addr);
    message::ip_address get_ip_address() const;

    // List of bitcoin messages
    // version
    // verack
    // addr
    // inv
    // getdata
    // getblocks
    // getheaders
    // tx
    // block
    // headers
    // getaddr
    // checkorder [deprecated]
    // submitorder [deprecated]
    // reply [deprecated]
    // ping
    // alert

    void subscribe_version(channel_handle chandle, 
        receive_version_handler handle_receive);
    void subscribe_verack(channel_handle chandle,
        receive_verack_handler handle_receive);
    void subscribe_addr(channel_handle chandle,
        receive_addr_handler handle_receive);
    void subscribe_inventory(channel_handle chandle,
        receive_inventory_handler handle_receive);
    void subscribe_block(channel_handle chandle,
        receive_block_handler handle_receive);

    void send(channel_handle chandle, const message::version& packet, 
        send_handler handle_send);
    void send(channel_handle chandle, const message::verack& packet, 
        send_handler handle_send);
    void send(channel_handle chandle, const message::getaddr& packet, 
        send_handler handle_send);
    void send(channel_handle chandle, const message::getdata& packet, 
        send_handler handle_send);
    void send(channel_handle chandle, const message::getblocks& packet, 
        send_handler handle_send);
    void send(channel_handle chandle, const message::transaction& packet,
        send_handler handle_send);

    // Non public
    void relay(channel_handle, const message::version& packet);
    void relay(channel_handle, const message::verack& packet);
    void relay(channel_handle, const message::addr& packet);
    void relay(channel_handle, const message::inventory& packet);
    void relay(channel_handle, const message::block& packet);

private:
    typedef shared_ptr<tcp::socket> socket_ptr;
    typedef shared_ptr<tcp::acceptor> acceptor_ptr;

    // G++ really needs template aliases already!
    typedef std::multimap<channel_handle, receive_version_handler>
        version_registry_map;
    typedef std::multimap<channel_handle, receive_verack_handler>
        verack_registry_map;
    typedef std::multimap<channel_handle, receive_addr_handler>
        addr_registry_map;
    typedef std::multimap<channel_handle, receive_inventory_handler>
        inventory_registry_map;
    typedef std::multimap<channel_handle, receive_block_handler>
        block_registry_map;

    void handle_connect(const boost::system::error_code& ec, 
        socket_ptr socket, std::string ip_addr, 
        connect_handler handle_connect);
    void handle_accept(socket_ptr socket);

    template<typename Callback, typename Registry>
    void generic_subscribe(channel_handle chandle,
        Callback handle_message, Registry& registry)
    {
        std::unique_lock<std::mutex> lock(registries_mutex_);
        registry.insert(std::make_pair(chandle, handle_message));
        lock.unlock();
    }

    template<typename Message, typename Registry>
    void generic_relay(channel_handle chandle, 
        const Message& packet, Registry& registry)
    {
        std::unique_lock<std::mutex> lock(registries_mutex_);
        auto range = registry.equal_range(chandle);
        std::vector<typename Registry::mapped_type> notify_methods;
        for (auto it = range.first; it != range.second; ++it)
            notify_methods.push_back(it->second);
        registry.erase(range.first, range.second);
        lock.unlock();
        for (auto notify: notify_methods)
            notify(packet);
    }
    
    channel_handle create_channel(socket_ptr socket);

    acceptor_ptr acceptor_;
    std::vector<connect_handler> listeners_;

    dialect_ptr default_dialect_;
    channel_list channels_;

    message::ip_address our_ip_address_;

    std::mutex registries_mutex_;
    version_registry_map version_registry_;
    verack_registry_map verack_registry_;
    addr_registry_map addr_registry_;
    inventory_registry_map inventory_registry_;
    block_registry_map block_registry_;
};

void handshake(network_ptr net, channel_handle chandle, 
    network::send_handler handle_handshake);

void handshake_connect(network_ptr net, const std::string& hostname,
    unsigned short port, network::connect_handler handle_connect);

} // libbitcoin

#endif

