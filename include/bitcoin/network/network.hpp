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
  : private boost::noncopyable
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
    typedef std::function<void (const message::inv&)>
        receive_inv_handler;
    typedef std::function<void (const message::block&)>
        receive_block_handler;

    virtual bool start_accept() = 0;
    virtual void connect(std::string hostname, unsigned short port,
        connect_handler handle_connect) = 0;
    virtual void begin_handshake(channel_handle chandle,
        send_handler handle_send) = 0;
    virtual void listen(connect_handler handle_connect) = 0;
    virtual size_t connection_count() const = 0;
    virtual void disconnect(channel_handle handle) = 0;

    virtual void set_ip_address(std::string ip_addr) = 0;
    virtual message::ip_address get_ip_address() const = 0;

    virtual void subscribe_version(channel_handle chandle, 
        receive_version_handler handle_receive) = 0;
    virtual void subscribe_verack(channel_handle chandle,
        receive_verack_handler handle_receive) = 0;
    virtual void subscribe_addr(channel_handle chandle,
        receive_addr_handler handle_receive) = 0;
    virtual void subscribe_inv(channel_handle chandle,
        receive_inv_handler handle_receive) = 0;
    virtual void subscribe_block(channel_handle chandle,
        receive_block_handler handle_receive) = 0;

    virtual void send(channel_handle chandle,
        const message::version& packet, send_handler handle_send) = 0;
    virtual void send(channel_handle chandle,
        const message::verack& packet, send_handler handle_send) = 0;
    virtual void send(channel_handle chandle,
        const message::getaddr& packet, send_handler handle_send) = 0;
    virtual void send(channel_handle chandle,
        const message::getdata& packet, send_handler handle_send) = 0;
    virtual void send(channel_handle chandle,
        const message::getblocks& packet, send_handler handle_send) = 0;

    // Non public
    virtual void relay(channel_handle, const message::version& packet) = 0;
    virtual void relay(channel_handle, const message::verack& packet) = 0;
    virtual void relay(channel_handle, const message::addr& packet) = 0;
    virtual void relay(channel_handle, const message::inv& packet) = 0;
    virtual void relay(channel_handle, const message::block& packet) = 0;
};

class network_impl
  : public network, 
    public threaded_service,
    public std::enable_shared_from_this<network_impl>
{
public:
    network_impl();
    ~network_impl();
    bool start_accept();
    void connect(std::string hostname, unsigned short port, 
        connect_handler handle_connect);
    void begin_handshake(channel_handle chandle,
        send_handler handle_send);
    void listen(connect_handler handle_connect);
    size_t connection_count() const;
    void disconnect(channel_handle chandle);  

    void set_ip_address(std::string ip_addr);
    message::ip_address get_ip_address() const;

    void subscribe_version(channel_handle chandle, 
        receive_version_handler handle_receive);
    void subscribe_verack(channel_handle chandle,
        receive_verack_handler handle_receive);
    void subscribe_addr(channel_handle chandle,
        receive_addr_handler handle_receive);
    void subscribe_inv(channel_handle chandle,
        receive_inv_handler handle_receive);
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

    // Non public
    void relay(channel_handle, const message::version& packet);
    void relay(channel_handle, const message::verack& packet);
    void relay(channel_handle, const message::addr& packet);
    void relay(channel_handle, const message::inv& packet);
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
    typedef std::multimap<channel_handle, receive_inv_handler>
        inv_registry_map;
    typedef std::multimap<channel_handle, receive_block_handler>
        block_registry_map;

    void handle_connect(const boost::system::error_code& ec, 
        socket_ptr socket, std::string ip_addr, 
        connect_handler handle_connect);
    void handle_accept(socket_ptr socket);
    
    channel_handle create_channel(socket_ptr socket);

    acceptor_ptr acceptor_;
    std::vector<connect_handler> listeners_;

    dialect_ptr default_dialect_;
    channel_list channels_;

    message::ip_address our_ip_address_;

    version_registry_map version_registry_;
    verack_registry_map verack_registry_;
    addr_registry_map addr_registry_;
    inv_registry_map inv_registry_;
    block_registry_map block_registry_;
};

} // libbitcoin

#endif

