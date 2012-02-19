#ifndef LIBBITCOIN_PROTOCOL_H
#define LIBBITCOIN_PROTOCOL_H

#include <memory>
#include <system_error>

#include <bitcoin/types.hpp>
#include <bitcoin/messages.hpp>
#include <bitcoin/utility/threads.hpp>

namespace libbitcoin {

class protocol
  : public std::enable_shared_from_this<protocol>
{
public:
    typedef std::function<void (const std::error_code&)> completion_handler;

    protocol();
    void start(completion_handler handle_complete);
    void stop(completion_handler handle_complete);

    void bootstrap(completion_handler handle_complete);
    void run();

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
    void handle_seed_store(const std::error_code& ec,
        size_t total_count, atomic_counter_ptr counter,
        completion_handler handle_complete);

    // run loop

    const std::string hosts_filename_;
    const size_t max_outbound_;
    hosts_ptr hosts_;
    handshake_ptr handshake_;

    thread_core_ptr threaded_;
    strand_ptr strand_;
    connection_list connections_;
};

} // libbitcoin

#endif

