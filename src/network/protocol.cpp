#include <bitcoin/network/protocol.hpp>

#include <bitcoin/network/hosts.hpp>
#include <bitcoin/network/handshake.hpp>
#include <bitcoin/utility/logger.hpp>

using std::placeholders::_1;
using std::placeholders::_2;

namespace libbitcoin {

static std::string pretty(const message::ip_address& ip)
{
    std::ostringstream oss;
    oss << (int)ip[12] << '.' << (int)ip[13] << '.'
        << (int)ip[14] << '.' << (int)ip[15];
    return oss.str();
}

protocol::protocol()
  : hosts_filename_("hosts"), max_outbound_(8)
{
    hosts_ = std::make_shared<hosts>();
    handshake_ = std::make_shared<handshake>();
    network_ = std::make_shared<network>();
}

void protocol::start(completion_handler handle_complete)
{
    atomic_counter_ptr count_paths = std::make_shared<atomic_counter>(0);
    bootstrap(strand()->wrap(
        std::bind(&protocol::handle_bootstrap, shared_from_this(),
            _1, count_paths, handle_complete)));
    handshake_->start(strand()->wrap(
        std::bind(&protocol::handle_start_handshake_service,
            shared_from_this(), _1, count_paths, handle_complete)));
}
void protocol::handle_bootstrap(const std::error_code& ec,
    atomic_counter_ptr count_paths, completion_handler handle_complete)
{
    if (ec)
    {
        log_error(log_domain::protocol)
            << "Failed to bootstrap: " << ec.message();
        handle_complete(ec);
        return;
    }
    ++(*count_paths);
    if (*count_paths == 2)
    {
        handle_complete(std::error_code());
        run();
    }
}
void protocol::handle_start_handshake_service(const std::error_code& ec,
    atomic_counter_ptr count_paths, completion_handler handle_complete)
{
    if (ec)
    {
        log_error(log_domain::protocol)
            << "Failed to start handshake service: " << ec.message();
        handle_complete(ec);
        return;
    }
    ++(*count_paths);
    if (*count_paths == 2)
    {
        handle_complete(std::error_code());
        run();
    }
}

void protocol::stop(completion_handler handle_complete)
{
    hosts_->save(hosts_filename_,
        strand()->wrap(std::bind(&protocol::handle_save, shared_from_this(),
            _1, handle_complete)));
}
void protocol::handle_save(const std::error_code& ec,
    completion_handler handle_complete)
{
    if (ec)
    {
        log_error(log_domain::protocol) << "Failed to save hosts '"
            << hosts_filename_ << "': " << ec.message();
        handle_complete(ec);
        return;
    }
    handle_complete(std::error_code());
}

void protocol::bootstrap(completion_handler handle_complete)
{
    hosts_->load(hosts_filename_,
        strand()->wrap(std::bind(&protocol::load_hosts, shared_from_this(),
            _1, handle_complete)));
}
void protocol::load_hosts(const std::error_code& ec,
    completion_handler handle_complete)
{
    if (ec)
    {
        log_error(log_domain::protocol)
            << "Could not load hosts file: " << ec.message();
        handle_complete(ec);
        return;
    }
    hosts_->fetch_count(
        strand()->wrap(std::bind(&protocol::if_0_seed, shared_from_this(),
            _1, _2, handle_complete)));
}

const std::vector<std::string> dns_seeds
{
    "bitseed.xf2.org",
    "dnsseed.bluematt.me",
    "seed.bitcoin.sipa.be",
    "dnsseed.bitcoin.dashjr.org"
};

void protocol::if_0_seed(const std::error_code& ec, size_t hosts_count,
    completion_handler handle_complete)
{
    if (ec)
    {
        log_error(log_domain::protocol) 
            << "Unable to check hosts empty: " << ec.message();
        handle_complete(ec);
        return;
    }
    if (hosts_count == 0)
    {
        seed_endpoint_states seed_states =
            std::make_shared<std::vector<seed_point>>();
        for (const std::string& hostname: dns_seeds)
            connect_dns_seed(hostname, seed_states, handle_complete);
    }
    else
        handle_complete(std::error_code());
}

void protocol::check_seed_states_final(seed_endpoint_states states,
    completion_handler handle_complete)
{
    if (states->size() != dns_seeds.size())
        return;
    for (const seed_point& point: *states)
        if (!point.error_code)
        {
            handle_complete(point.error_code);
            return;
        }
    handle_complete((*states)[0].error_code);
}

void protocol::connect_dns_seed(const std::string& hostname,
    seed_endpoint_states seed_states, completion_handler handle_complete)
{
    handshake_->connect(network_, hostname, 8333,
        strand()->wrap(std::bind(&protocol::request_addresses,
            shared_from_this(), _1, _2, seed_states, handle_complete)));
}
void protocol::request_addresses(const std::error_code& ec,
    channel_ptr dns_seed_node, seed_endpoint_states seed_states,
    completion_handler handle_complete)
{
    if (ec)
    {
        log_error(log_domain::protocol) 
            << "Failed to connect to seed node: " << ec.message();
        seed_states->push_back({seed_state::connect, ec});
        check_seed_states_final(seed_states, handle_complete);
    }
    else
    {
        dns_seed_node->send(message::get_address(),
            strand()->wrap(std::bind(&protocol::handle_send_get_address,
                shared_from_this(), _1, dns_seed_node,
                    seed_states, handle_complete)));
        dns_seed_node->subscribe_address(
            strand()->wrap(std::bind(&protocol::save_seeded_addresses,
                shared_from_this(), _1, _2, dns_seed_node)));
    }

}
void protocol::handle_send_get_address(const std::error_code& ec,
    channel_ptr dns_seed_node, seed_endpoint_states seed_states,
    completion_handler handle_complete)
{
    if (ec)
    {
        log_error(log_domain::protocol)
            << "Sending get_address message failed: " << ec.message();
        seed_states->push_back({seed_state::send_request, ec});
        check_seed_states_final(seed_states, handle_complete);
    }
    else
    {
        seed_states->push_back({seed_state::send_request, ec});
        if (seed_states->size() == dns_seeds.size())
            handle_complete(std::error_code());
    }
}

void protocol::save_seeded_addresses(const std::error_code& ec,
    const message::address& packet, channel_ptr dns_seed_node)
{
    if (ec)
    {
        log_error(log_domain::protocol)
            << "Problem receiving addresses from seed nodes: "
            << ec.message();
    }
    else
    {
        log_info() << "Storing seeded addresses.";
        for (const message::network_address& net_address: packet.addresses)
            hosts_->store(net_address,
                strand()->wrap(std::bind(&protocol::handle_seed_store,
                    shared_from_this(), _1)));
    }
}
void protocol::handle_seed_store(const std::error_code& ec)
{
    if (ec)
        log_error(log_domain::protocol) 
            << "Failed to store addresses from seed nodes: "
            << ec.message();
}

void protocol::run()
{
    strand()->dispatch(std::bind(&protocol::try_connect, shared_from_this()));
    network_->listen(8333,
        strand()->wrap(std::bind(&protocol::handle_listen,
            shared_from_this(), _1, _2)));
}
void protocol::try_connect()
{
    if (connections_.size() >= max_outbound_)
        return;
    for (size_t i = connections_.size(); i < max_outbound_; ++i)
        hosts_->fetch_address(
            strand()->wrap(std::bind(&protocol::attempt_connect,
                shared_from_this(), _1, _2)));
}
void protocol::attempt_connect(const std::error_code& ec,
    const message::network_address& address)
{
    if (ec)
    {
        log_error(log_domain::protocol)
            << "Problem fetching random address: " << ec.message();
        return;
    }
    // Are we already connected to shared_from_this() address?
    for (const connection_info& connection: connections_)
    {
        if (connection.address.ip == address.ip &&
            connection.address.port == address.port)
        {
            log_info(log_domain::protocol)
                << "Already connected to " << pretty_hex(address.ip);
            // Retry another connection
            strand()->post(
                std::bind(&protocol::try_connect, shared_from_this()));
            return;
        }
    }
    log_info(log_domain::protocol) << "Trying "
        << pretty(address.ip) << ":" << address.port;
    handshake_->connect(network_, pretty(address.ip), address.port,
        strand()->wrap(std::bind(&protocol::handle_connect,
            shared_from_this(), _1, _2, address)));
}
void protocol::handle_connect(const std::error_code& ec, channel_ptr node,
    const message::network_address& address)
{
    if (ec)
    {
        log_info(log_domain::protocol) << "Unable to connect to "
            << pretty(address.ip) << ":" << address.port
            << " - " << ec.message();
        strand()->post(std::bind(&protocol::try_connect, shared_from_this()));
    }
    else
    {
        connections_.push_back({address, node});
        log_info(log_domain::protocol) << "Connected to "
            << pretty(address.ip) << ":" << address.port
            << " (" << connections_.size() << " connections)";
        setup_new_channel(node);
    }
}

void protocol::handle_listen(const std::error_code& ec, acceptor_ptr accept)
{
    if (ec)
    {
        log_error(log_domain::protocol)
            << "Error while listening: " << ec.message();
    }
    else
    {
        accept->accept(
            strand()->wrap(std::bind(&protocol::handle_accept,
                shared_from_this(), _1, _2, accept)));
    }
}
void protocol::handle_accept(const std::error_code& ec, channel_ptr node,
    acceptor_ptr accept)
{
    if (ec)
    {
        log_error(log_domain::protocol)
            << "Problem accepting connection: " << ec.message();
    }
    else
    {
        setup_new_channel(node);
    }
}

void handle_send(const std::error_code& ec)
{
    if (ec)
        log_error(log_domain::protocol)
            << "Sending error: " << ec.message();
}
void protocol::setup_new_channel(channel_ptr node)
{
    // Remove channel from list of connections
    node->subscribe_stop(
        strand()->wrap(std::bind(&protocol::channel_stopped,
            shared_from_this(), _1, node)));
    subscribe_address(node);
    node->send(message::get_address(), handle_send);
}
void protocol::channel_stopped(const std::error_code& ec,
    channel_ptr which_node)
{
    auto it = connections_.begin();
    for (; it != connections_.end(); ++it)
        if (it->node == which_node)
            break;
    BITCOIN_ASSERT(it != connections_.end());
    connections_.erase(it);
    // Recreate connections if need be
    try_connect();
}

void protocol::subscribe_address(channel_ptr node)
{
    node->subscribe_address(
        strand()->wrap(std::bind(&protocol::receive_address_message,
            shared_from_this(), _1, _2, node)));
}
void protocol::receive_address_message(const std::error_code& ec,
    const message::address& packet, channel_ptr node)
{
    if (ec)
    {
        log_error(log_domain::protocol)
            << "Problem receiving addresses: " << ec.message();
    }
    else
    {
        log_info() << "Storing addresses.";
        for (const message::network_address& net_address: packet.addresses)
            hosts_->store(net_address,
                strand()->wrap(std::bind(&protocol::handle_store_address,
                    shared_from_this(), _1)));
    }
}
void protocol::handle_store_address(const std::error_code& ec)
{
    if (ec)
        log_error(log_domain::protocol) 
            << "Failed to store address: " << ec.message();
}

void protocol::fetch_connection_count(
    fetch_connection_count_handler handle_fetch)
{
    strand()->post(
        std::bind(&protocol::do_fetch_connection_count,
            shared_from_this(), handle_fetch));
}
void protocol::do_fetch_connection_count(
    fetch_connection_count_handler handle_fetch)
{
    handle_fetch(std::error_code(), connections_.size());
}

} // libbitcoin

