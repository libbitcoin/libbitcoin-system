#ifndef LIBBITCOIN_NETWORK_HANDSHAKE_H
#define LIBBITCOIN_NETWORK_HANDSHAKE_H

#include <atomic>

#include <bitcoin/messages.hpp>
#include <bitcoin/network/network.hpp>

namespace libbitcoin {

class handshake
  : public std::enable_shared_from_this<handshake>
{
public:
    typedef std::function<void (const std::error_code&)> handshake_handler;

    typedef std::function<void (
        const std::error_code&, const message::ip_address&)>
            discover_ip_handler;

    typedef std::function<void (
        const std::error_code&, const message::network_address&)>
            fetch_network_address_handler;

    typedef std::function<void (const std::error_code&)> setter_handler;

    handshake();

    void connect(network_ptr net, const std::string& hostname,
        uint16_t port, network::connect_handler handle_connect);
    void start(channel_ptr node, handshake_handler handle_handshake);

    void discover_external_ip(discover_ip_handler handler_discover);
    void fetch_network_address(fetch_network_address_handler handle_fetch);
    void set_port(uint16_t port, setter_handler handle_set);
    void set_user_agent(const std::string& user_agent,
        setter_handler handle_set);

private:
    typedef std::atomic<size_t> atomic_counter;
    typedef std::shared_ptr<atomic_counter> atomic_counter_ptr;

    void handle_discover_ip(const std::error_code& ec,
        network_ptr net, const std::string& hostname,
        uint16_t port, network::connect_handler handle_connect);
    void handle_connect(const std::error_code& ec,
        channel_ptr node, network::connect_handler handle_connect);

    void handle_message_sent(const std::error_code& ec,
        atomic_counter_ptr counter,
        handshake::handshake_handler completion_callback);

    void receive_version(const std::error_code& ec,
        const message::version&, channel_ptr node, atomic_counter_ptr counter,
        handshake::handshake_handler completion_callback);

    void receive_verack(const std::error_code& ec,
        const message::verack&, atomic_counter_ptr counter,
        handshake::handshake_handler completion_callback);

    bool lookup_external(const std::string& website,
        message::ip_address& ip);
    message::ip_address localhost_ip();
    void do_discover_external_ip(discover_ip_handler handler_discover);
    void do_fetch_network_address(fetch_network_address_handler handle_fetch);
    void do_set_port(uint16_t port, setter_handler handle_set);
    void do_set_user_agent(const std::string& user_agent,
        setter_handler handle_set);

    thread_core_ptr threaded_;
    strand_ptr strand_;
    message::version template_version_;
};

} // libbitcoin

#endif

