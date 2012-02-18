#ifndef LIBBITCOIN_PROTOCOL_H
#define LIBBITCOIN_PROTOCOL_H

#include <memory>

#include <bitcoin/network/hosts.hpp>

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

    const std::string hosts_filename_;
    hosts_ptr hosts_dir_;
};

} // libbitcoin

#endif

