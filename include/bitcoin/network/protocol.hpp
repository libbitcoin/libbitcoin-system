#ifndef LIBBITCOIN_PROTOCOL_H
#define LIBBITCOIN_PROTOCOL_H

#include <memory>
#include <system_error>

#include <bitcoin/types.hpp>
#include <bitcoin/messages.hpp>
#include <bitcoin/utility/threads.hpp>

namespace libbitcoin {

class protocol
  : public std::enable_shared_from_this<protocol>,
    public threaded_service
{
public:
    typedef std::function<void (const std::error_code&)> completion_handler;

    typedef std::function<void (const std::error_code&, size_t)>
        fetch_connection_count_handler;

    protocol();
    void start(completion_handler handle_complete);
    void stop(completion_handler handle_complete);

    void bootstrap(completion_handler handle_complete);
    void run();

    void fetch_connection_count(
        fetch_connection_count_handler handle_fetch);

private:
    struct connection_info
    {
        message::network_address address;
        channel_ptr node;
    };
    typedef std::vector<connection_info> connection_list;

    // start sequence
    void handle_bootstrap(const std::error_code& ec,
        atomic_counter_ptr count_paths, completion_handler handle_complete);
    void handle_start_handshake_service(const std::error_code& ec,
        atomic_counter_ptr count_paths, completion_handler handle_complete);

    // stop sequence
    void handle_save(const std::error_code& ec,
        completion_handler handle_complete);

    // bootstrap sequence
    void load_hosts(const std::error_code& ec,
        completion_handler handle_complete);
    void if_0_seed(const std::error_code& ec, size_t hosts_count,
        completion_handler handle_complete);

    // seed addresses from dns seeds
    class seeds
      : public std::enable_shared_from_this<seeds>
    {
    public:
        void start(protocol* parent, completion_handler handle_complete);
    private:
        void error_case(const std::error_code& ec);

        void connect_dns_seed(const std::string& hostname);
        void request_addresses(const std::error_code& ec,
            channel_ptr dns_seed_node);
        void handle_send_get_address(const std::error_code& ec);

        void save_addresses(const std::error_code& ec,
            const message::address& packet, channel_ptr);
        void handle_store(const std::error_code& ec);

        completion_handler handle_complete_;
        size_t ended_paths_;
        bool finished_;

        // From parent
        hosts_ptr hosts_;
        handshake_ptr handshake_;
        network_ptr network_;
        strand_ptr strand_;
    };
    std::shared_ptr<seeds> load_seeds_;
    friend class seeds;

    // run loop
    // Connect outwards
    void try_connect();
    void attempt_connect(const std::error_code& ec,
        const message::network_address& packet);
    void handle_connect(const std::error_code& ec, channel_ptr node,
        const message::network_address& address);

    // Accept inwards connections
    void handle_listen(const std::error_code& ec, acceptor_ptr accept);
    void handle_accept(const std::error_code& ec, channel_ptr node,
        acceptor_ptr accept);

    // Channel setup
    void setup_new_channel(channel_ptr node);
    void channel_stopped(const std::error_code& ec,
        channel_ptr which_node);

    void subscribe_address(channel_ptr node);
    void receive_address_message(const std::error_code& ec,
        const message::address& addr, channel_ptr node);
    void handle_store_address(const std::error_code& ec);

    // fetch methods
    void do_fetch_connection_count(
        fetch_connection_count_handler handle_fetch);

    const std::string hosts_filename_;
    hosts_ptr hosts_;
    handshake_ptr handshake_;
    network_ptr network_;

    size_t max_outbound_;
    connection_list connections_;
};

} // libbitcoin

#endif

