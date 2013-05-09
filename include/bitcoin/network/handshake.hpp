#ifndef LIBBITCOIN_NETWORK_HANDSHAKE_HPP
#define LIBBITCOIN_NETWORK_HANDSHAKE_HPP

#include <atomic>

#include <bitcoin/primitives.hpp>
#include <bitcoin/network/network.hpp>
#include <bitcoin/threadpool.hpp>

namespace libbitcoin {

class handshake
{
public:
    typedef std::function<void (const std::error_code&)> start_handler;

    typedef std::function<void (const std::error_code&)> handshake_handler;

    typedef std::function<void (
        const std::error_code&, const ip_address_type&)>
            discover_ip_handler;

    typedef std::function<void (
        const std::error_code&, const network_address_type&)>
            fetch_network_address_handler;

    typedef std::function<void (const std::error_code&)> setter_handler;

    handshake(threadpool& pool);

    handshake(const handshake&) = delete;
    void operator=(const handshake&) = delete;

    void start(start_handler handle_start);

    void ready(channel_ptr node, handshake_handler handle_handshake);

    void discover_external_ip(discover_ip_handler handle_discover);
    void fetch_network_address(fetch_network_address_handler handle_fetch);
    void set_port(uint16_t port, setter_handler handle_set);
    void set_user_agent(const std::string& user_agent,
        setter_handler handle_set);
    void set_start_depth(uint32_t depth, setter_handler handle_set);

private:
    void handle_connect(const std::error_code& ec,
        channel_ptr node, network::connect_handler handle_connect);

    void handle_message_sent(const std::error_code& ec,
        atomic_counter_ptr counter,
        handshake::handshake_handler completion_callback);

    void receive_version(const std::error_code& ec,
        const version_type&, channel_ptr node, atomic_counter_ptr counter,
        handshake::handshake_handler completion_callback);

    void receive_verack(const std::error_code& ec,
        const verack_type&, atomic_counter_ptr counter,
        handshake::handshake_handler completion_callback);

    bool lookup_external(const std::string& website,
        ip_address_type& ip);
    ip_address_type localhost_ip();
    void do_discover_external_ip(discover_ip_handler handler_discover);
    void do_fetch_network_address(fetch_network_address_handler handle_fetch);
    void do_set_port(uint16_t port, setter_handler handle_set);
    void do_set_user_agent(const std::string& user_agent,
        setter_handler handle_set);
    void do_set_start_depth(uint32_t depth, setter_handler handle_set);

    io_service::strand strand_;
    version_type template_version_;
};

void connect(handshake& shake, network& net,
    const std::string& hostname, uint16_t port,
    network::connect_handler handle_connect);

} // namespace libbitcoin

#endif

