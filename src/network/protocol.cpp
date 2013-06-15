#include <bitcoin/network/protocol.hpp>

#include <bitcoin/network/hosts.hpp>
#include <bitcoin/network/handshake.hpp>
#include <bitcoin/utility/logger.hpp>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;

static std::string pretty(const ip_address_type& ip)
{
    std::ostringstream oss;
    oss << (int)ip[12] << '.' << (int)ip[13] << '.'
        << (int)ip[14] << '.' << (int)ip[15];
    return oss.str();
}

protocol::protocol(threadpool& pool, hosts& hsts,
    handshake& shake, network& net)
  : strand_(pool.service()), hosts_filename_("hosts"),
    hosts_(hsts), handshake_(shake), network_(net), max_outbound_(8)
{
    channel_subscribe_ = std::make_shared<channel_subscriber_type>(pool);
}

void protocol::start(completion_handler handle_complete)
{
    atomic_counter_ptr count_paths = std::make_shared<atomic_counter>(0);
    bootstrap(strand_.wrap(
        std::bind(&protocol::handle_bootstrap,
            this, _1, count_paths, handle_complete)));
    handshake_.start(strand_.wrap(
        std::bind(&protocol::handle_start_handshake_service,
            this, _1, count_paths, handle_complete)));
}
void protocol::handle_bootstrap(const std::error_code& ec,
    atomic_counter_ptr count_paths, completion_handler handle_complete)
{
    if (ec)
    {
        log_error(LOG_PROTOCOL) << "Failed to bootstrap: " << ec.message();
        handle_complete(ec);
        return;
    }
    if (++(*count_paths) == 2)
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
        log_error(LOG_PROTOCOL)
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
    hosts_.save(hosts_filename_,
        strand_.wrap(std::bind(&protocol::handle_save,
            this, _1, handle_complete)));
}
void protocol::handle_save(const std::error_code& ec,
    completion_handler handle_complete)
{
    if (ec)
    {
        log_error(LOG_PROTOCOL) << "Failed to save hosts '"
            << hosts_filename_ << "': " << ec.message();
        handle_complete(ec);
        return;
    }
    channel_subscribe_->relay(error::service_stopped, nullptr);
    handle_complete(std::error_code());
}

void protocol::bootstrap(completion_handler handle_complete)
{
    hosts_.load(hosts_filename_,
        strand_.wrap(std::bind(&protocol::load_hosts,
            this, _1, handle_complete)));
}
void protocol::load_hosts(const std::error_code& ec,
    completion_handler handle_complete)
{
    if (ec)
    {
        log_error(LOG_PROTOCOL)
            << "Could not load hosts file: " << ec.message();
        handle_complete(ec);
        return;
    }
    hosts_.fetch_count(
        strand_.wrap(std::bind(&protocol::if_0_seed,
            this, _1, _2, handle_complete)));
}

void protocol::if_0_seed(const std::error_code& ec, size_t hosts_count,
    completion_handler handle_complete)
{
    if (ec)
    {
        log_error(LOG_PROTOCOL)
            << "Unable to check hosts empty: " << ec.message();
        handle_complete(ec);
        return;
    }
    if (hosts_count == 0)
    {
        load_seeds_ = std::make_shared<seeds>(this);
        load_seeds_->start(handle_complete);
    }
    else
        handle_complete(std::error_code());
}

const std::vector<std::string> dns_seeds
{
    "bitseed.xf2.org",
    "dnsseed.bluematt.me",
    "seed.bitcoin.sipa.be",
    "dnsseed.bitcoin.dashjr.org",
    "archivum.info",
    "progressbar.sk",
    "faucet.bitcoin.st",
    "bitcoin.securepayment.cc"
};

protocol::seeds::seeds(protocol* parent)
  : strand_(parent->strand_), hosts_(parent->hosts_),
    handshake_(parent->handshake_), network_(parent->network_)
{
}
void protocol::seeds::start(completion_handler handle_complete)
{
    handle_complete_ = handle_complete;
    ended_paths_ = 0;
    finished_ = false;
    for (const std::string& hostname: dns_seeds)
        connect_dns_seed(hostname);
}

void protocol::seeds::error_case(const std::error_code& ec)
{
    if (finished_)
        return;
    ++ended_paths_;
    if (ended_paths_ == dns_seeds.size())
    {
        finished_ = true;
        handle_complete_(ec);
    }
}

void protocol::seeds::connect_dns_seed(const std::string& hostname)
{
    connect(handshake_, network_, hostname, 8333,
        strand_.wrap(std::bind(&protocol::seeds::request_addresses,
            shared_from_this(), _1, _2)));
}
void protocol::seeds::request_addresses(
    const std::error_code& ec, channel_ptr dns_seed_node)
{
    if (ec)
    {
        log_error(LOG_PROTOCOL)
            << "Failed to connect to seed node: " << ec.message();
        error_case(ec);
    }
    else
    {
        dns_seed_node->send(get_address_type(),
            strand_.wrap(std::bind(&protocol::seeds::handle_send_get_address,
                shared_from_this(), _1)));
        dns_seed_node->subscribe_address(
            strand_.wrap(std::bind(&protocol::seeds::save_addresses,
                shared_from_this(), _1, _2, dns_seed_node)));
    }

}
void protocol::seeds::handle_send_get_address(const std::error_code& ec)
{
    if (ec)
    {
        log_error(LOG_PROTOCOL)
            << "Sending get_address message failed: " << ec.message();
        error_case(ec);
    }
}

void protocol::seeds::save_addresses(const std::error_code& ec,
    const address_type& packet, channel_ptr)
{
    if (ec)
    {
        log_error(LOG_PROTOCOL)
            << "Problem receiving addresses from seed nodes: "
            << ec.message();
        error_case(ec);
    }
    else
    {
        log_debug(LOG_PROTOCOL) << "Storing seeded addresses.";
        for (const network_address_type& net_address: packet.addresses)
            hosts_.store(net_address,
                strand_.wrap(std::bind(&protocol::seeds::handle_store,
                    shared_from_this(), _1)));

        if (!finished_)
        {
            ++ended_paths_;
            finished_ = true;
            handle_complete_(std::error_code());
        }
    }
}
void protocol::seeds::handle_store(const std::error_code& ec)
{
    if (ec)
        log_error(LOG_PROTOCOL)
            << "Failed to store addresses from seed nodes: "
            << ec.message();
}

void protocol::run()
{
    strand_.dispatch(std::bind(&protocol::try_connect, this));
    network_.listen(8333,
        strand_.wrap(std::bind(&protocol::handle_listen,
            this, _1, _2)));
}
void protocol::try_connect()
{
    if (connections_.size() >= max_outbound_)
        return;
    for (size_t i = connections_.size(); i < max_outbound_; ++i)
        hosts_.fetch_address(
            strand_.wrap(std::bind(&protocol::attempt_connect,
                this, _1, _2)));
}
void protocol::attempt_connect(const std::error_code& ec,
    const network_address_type& address)
{
    if (ec)
    {
        log_error(LOG_PROTOCOL)
            << "Problem fetching random address: " << ec.message();
        return;
    }
    // Are we already connected to shared_from_this() address?
    for (const connection_info& connection: connections_)
    {
        if (connection.address.ip == address.ip &&
            connection.address.port == address.port)
        {
            log_debug(LOG_PROTOCOL)
                << "Already connected to " << encode_hex(address.ip);
            // Retry another connection
            strand_.post(
                std::bind(&protocol::try_connect, this));
            return;
        }
    }
    log_debug(LOG_PROTOCOL) << "Trying "
        << pretty(address.ip) << ":" << address.port;
    connect(handshake_, network_, pretty(address.ip), address.port,
        strand_.wrap(std::bind(&protocol::handle_connect,
            this, _1, _2, address)));
}
void protocol::handle_connect(const std::error_code& ec, channel_ptr node,
    const network_address_type& address)
{
    if (ec)
    {
        log_warning(LOG_PROTOCOL) << "Unable to connect to "
            << pretty(address.ip) << ":" << address.port
            << " - " << ec.message();
        strand_.post(std::bind(&protocol::try_connect, this));
    }
    else
    {
        connections_.push_back({address, node});
        log_info(LOG_PROTOCOL) << "Connected to "
            << pretty(address.ip) << ":" << address.port
            << " (" << connections_.size() << " connections)";
        setup_new_channel(node);
    }
}

void protocol::handle_listen(const std::error_code& ec, acceptor_ptr accept)
{
    if (ec)
    {
        log_error(LOG_PROTOCOL)
            << "Error while listening: " << ec.message();
    }
    else
    {
        accept->accept(
            strand_.wrap(std::bind(&protocol::handle_accept,
                this, _1, _2, accept)));
    }
}
void protocol::handle_accept(const std::error_code& ec, channel_ptr node,
    acceptor_ptr accept)
{
    if (ec)
    {
        log_error(LOG_PROTOCOL)
            << "Problem accepting connection: " << ec.message();
    }
    else
    {
        accepted_channels_.push_back(node);
        log_info(LOG_PROTOCOL) << "Accepted connection: "
            << accepted_channels_.size();
        setup_new_channel(node);
    }
}

void handle_send(const std::error_code& ec)
{
    if (ec)
        log_error(LOG_PROTOCOL)
            << "Sending error: " << ec.message();
}
void protocol::setup_new_channel(channel_ptr node)
{
    // Remove channel from list of connections
    node->subscribe_stop(
        strand_.wrap(std::bind(&protocol::channel_stopped,
            this, _1, node)));
    subscribe_address(node);
    node->send(get_address_type(), handle_send);
    // Notify subscribers
    channel_subscribe_->relay(std::error_code(), node);
}
void protocol::channel_stopped(const std::error_code& ec,
    channel_ptr which_node)
{
    auto it = connections_.begin();
    for (; it != connections_.end(); ++it)
        if (it->node == which_node)
            break;
    if (it != connections_.end())
    {
        connections_.erase(it);
        // Recreate connections if need be
        try_connect();
    }
    auto acc_it = std::find(
        accepted_channels_.begin(),
        accepted_channels_.end(), which_node);
    if (acc_it != accepted_channels_.end())
        accepted_channels_.erase(acc_it);
}

void protocol::subscribe_address(channel_ptr node)
{
    node->subscribe_address(
        strand_.wrap(std::bind(&protocol::receive_address_message,
            this, _1, _2, node)));
}
void protocol::receive_address_message(const std::error_code& ec,
    const address_type& packet, channel_ptr node)
{
    if (ec)
    {
        log_error(LOG_PROTOCOL)
            << "Problem receiving addresses: " << ec.message();
    }
    else
    {
        log_debug(LOG_PROTOCOL) << "Storing addresses.";
        for (const network_address_type& net_address: packet.addresses)
            hosts_.store(net_address,
                strand_.wrap(std::bind(&protocol::handle_store_address,
                    this, _1)));
    }
}
void protocol::handle_store_address(const std::error_code& ec)
{
    if (ec)
        log_error(LOG_PROTOCOL) << "Failed to store address: " << ec.message();
}

void protocol::fetch_connection_count(
    fetch_connection_count_handler handle_fetch)
{
    strand_.post(
        std::bind(&protocol::do_fetch_connection_count,
            this, handle_fetch));
}
void protocol::do_fetch_connection_count(
    fetch_connection_count_handler handle_fetch)
{
    handle_fetch(std::error_code(), connections_.size());
}

void protocol::subscribe_channel(channel_handler handle_channel)
{
    channel_subscribe_->subscribe(handle_channel);
}

} // namespace libbitcoin

