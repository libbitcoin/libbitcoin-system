#include <bitcoin/network/protocol.hpp>

#include <bitcoin/utility/logger.hpp>

using std::placeholders::_1;
using std::placeholders::_2;

namespace libbitcoin {

protocol::protocol()
  : hosts_filename_("hosts")
{
    hosts_dir_ = std::make_shared<hosts>();
}

void protocol::start(completion_handler handle_complete)
{
}
void protocol::stop(completion_handler handle_complete)
{
    hosts_dir_->save(hosts_filename_,
        std::bind(&protocol::handle_save, this, _1, handle_complete));
}
void protocol::handle_save(const std::error_code& ec,
    completion_handler handle_complete)
{
    if (ec)
    {
        log_error() << "Failed to save hosts '" << hosts_filename_ << "': "
            << ec.message();
        handle_complete(ec);
        return;
    }
    handle_complete(std::error_code());
}

std::vector<message::network_address> seed_nodes{
    message::network_address{0, 0,
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            255, 255, 127, 0, 0, 1},
        8333}
};
void protocol::bootstrap(completion_handler handle_complete)
{
    hosts_dir_->load(hosts_filename_,
        std::bind(&protocol::load_hosts, this, _1, handle_complete));
}
void protocol::load_hosts(const std::error_code& ec,
    completion_handler handle_complete)
{
    if (ec)
    {
        log_error() << "Could not load hosts file: " << ec.message();
        handle_complete(ec);
        return;
    }
    hosts_dir_->fetch_count(
        std::bind(&protocol::if_0_seed, this, _1, _2, handle_complete));
}
void protocol::if_0_seed(const std::error_code& ec, size_t hosts_count,
    completion_handler handle_complete)
{
    if (ec)
    {
        log_error() << "Unable to check hosts empty: " << ec.message();
        handle_complete(ec);
        return;
    }
    if (hosts_count == 0)
        for (const message::network_address& address: seed_nodes)
        {
            atomic_counter_ptr counter = std::make_shared<atomic_counter>();
            hosts_dir_->store(address,
                std::bind(&protocol::handle_seed_store, this,
                    _1, seed_nodes.size(), counter, handle_complete));
        }
    else
        handle_complete(std::error_code());
}
void protocol::handle_seed_store(const std::error_code& ec,
    size_t total_count, atomic_counter_ptr counter,
    completion_handler handle_complete)
{
    if (ec)
    {
        log_error() << "Failed to seed nodes: " << ec.message();
        handle_complete(ec);
        return;
    }
    ++(*counter);
    if (*counter == total_count)
    {
        handle_complete(std::error_code());
    }
}

} // libbitcoin

