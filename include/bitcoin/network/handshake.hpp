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
    typedef std::function<void (const std::error_code&)> setter_handler;

    handshake();

    void connect(network_ptr net, const std::string& hostname,
        uint16_t port, network::connect_handler handle_connect);
    void start(channel_ptr node, handshake_handler handle_handshake);

    void set_external_ip(const message::ip_address& ip,
        setter_handler handle_set);
    void set_user_agent(const std::string& user_agent,
        setter_handler handle_set);

private:
    typedef std::atomic<size_t> atomic_counter;
    typedef std::shared_ptr<atomic_counter> atomic_counter_ptr;

    void initiate_handshake(const std::error_code& ec,
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

    void do_set_external_ip(const message::ip_address& ip,
        setter_handler handle_set);
    void do_set_user_agent(const std::string& user_agent,
        setter_handler handle_set);

    thread_core_ptr threaded_;
    strand_ptr strand_;
    message::version template_version_;
};

} // libbitcoin

#endif

