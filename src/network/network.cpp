#include <bitcoin/network/network.hpp>

#include <boost/lexical_cast.hpp>
#include <functional>
#include <algorithm>
#include <iostream>

#include <bitcoin/util/logger.hpp>
#include <bitcoin/dialect.hpp>

#include "channel.hpp"

using std::placeholders::_1;
using std::placeholders::_2;

namespace libbitcoin {

using boost::asio::socket_base;

network_impl::network_impl()
{
    default_dialect_.reset(new original_dialect);
    our_ip_address_ = message::ip_address{
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0};
}

network_impl::~network_impl()
{
    if (acceptor_)
        acceptor_->close();
}

channel_handle network_impl::create_channel(socket_ptr socket)
{
    channel_pimpl* channel_obj = new channel_pimpl(
        shared_from_this(), default_dialect_, service(), socket);
    channels_.push_back(channel_obj);
    log_debug() << channels_.size() << " peers connected.";
    return channel_obj->get_id();
}

void network_impl::handle_connect(const boost::system::error_code& ec, 
    socket_ptr socket, std::string hostname, connect_handler handle_connect)
{
    if (ec)
    {
        log_error() << "Connecting to peer " << hostname
            << ": " << ec.message();
        handle_connect(error::system_network_error, 0);
        return;
    }
    channel_handle chanid = create_channel(socket);
    handle_connect(std::error_code(), chanid);
}

void network_impl::connect(const std::string& hostname, unsigned short port,
        connect_handler handle_connect)
{
    socket_ptr socket(new tcp::socket(*service()));
    tcp::resolver resolver(*service());
    tcp::resolver::query query(hostname,
        boost::lexical_cast<std::string>(port));
    tcp::endpoint endpoint = *resolver.resolve(query);
    socket->async_connect(endpoint, std::bind(
        &network_impl::handle_connect, shared_from_this(), 
            _1, socket, hostname, handle_connect));
}

void network_impl::listen(connect_handler handle_connect)
{
    strand()->post(
        [&listeners_, handle_connect]
        {
            listeners_.push_back(handle_connect);
        });
}

void remove_matching_channels(channel_list* channels,
    channel_handle chandle)
{
    channels->erase_if(
        [chandle](channel_pimpl& channel_obj)
        {
            return channel_obj.get_id() == chandle;
        });
    log_debug() << channels->size() << " peers remaining.";
}
void network_impl::disconnect(channel_handle chandle)
{
    strand()->post(
        std::bind(remove_matching_channels, &channels_, chandle));
}

void network_impl::subscribe_version(channel_handle chandle, 
    receive_version_handler handle_receive)
{
    generic_subscribe<receive_version_handler, version_registry_map>(
        chandle, handle_receive, version_registry_);
}

void network_impl::subscribe_verack(channel_handle chandle,
    receive_verack_handler handle_receive)
{
    generic_subscribe<receive_verack_handler, verack_registry_map>(
        chandle, handle_receive, verack_registry_);
}

void network_impl::subscribe_addr(channel_handle chandle,
    receive_addr_handler handle_receive)
{
    generic_subscribe<receive_addr_handler, addr_registry_map>(
        chandle, handle_receive, addr_registry_);
}

void network_impl::subscribe_inv(channel_handle chandle,
    receive_inv_handler handle_receive)
{
    generic_subscribe<receive_inv_handler, inv_registry_map>(
        chandle, handle_receive, inv_registry_);
}

void network_impl::subscribe_block(channel_handle chandle,
    receive_block_handler handle_receive)
{
    generic_subscribe<receive_block_handler, block_registry_map>(
        chandle, handle_receive, block_registry_);
}

template<typename Message, typename Callback>
void perform_send(channel_handle chandle,
    const Message packet, channel_list& channels, Callback handle_send)
{
    auto it = std::find_if(channels.begin(), channels.end(),
        [chandle](const channel_pimpl& channel_obj)
        {
            return channel_obj.get_id() == chandle;
        });
    if (it == channels.end())
    {
        log_error() << "Non existant channel " << chandle << " for send.";
        handle_send(error::network_channel_not_found);
        return;
    }
    it->send(packet, handle_send);
}

template<typename Message, typename Callback>
void generic_send(strand_ptr strand, channel_handle chandle,
    const Message packet, channel_list& channels, Callback handle_send)
{
    strand->post(std::bind(&perform_send<Message, Callback>, 
        chandle, packet, std::ref(channels), handle_send));
}

void network_impl::send(channel_handle chandle,
     const message::version& packet, send_handler handle_send)
{
    generic_send(strand(), chandle, packet, std::ref(channels_), handle_send);
}

void network_impl::send(channel_handle chandle,
     const message::verack& packet, send_handler handle_send)
{
    generic_send(strand(), chandle, packet, std::ref(channels_), handle_send);
}

void network_impl::send(channel_handle chandle,
     const message::getaddr& packet, send_handler handle_send)
{
    generic_send(strand(), chandle, packet, std::ref(channels_), handle_send);
}

void network_impl::send(channel_handle chandle,
     const message::getdata& packet, send_handler handle_send)
{
    generic_send(strand(), chandle, packet, std::ref(channels_), handle_send);
}

void network_impl::send(channel_handle chandle, 
        const message::getblocks& packet, send_handler handle_send)
{
    generic_send(strand(), chandle, packet, std::ref(channels_), handle_send);
}

// Relay functions

void network_impl::relay(channel_handle chandle, 
    const message::version& packet)
{
    generic_relay(chandle, packet, version_registry_);
}

void network_impl::relay(channel_handle chandle,
     const message::verack& packet)
{
    generic_relay(chandle, packet, verack_registry_);
}

void network_impl::relay(channel_handle chandle,
     const message::addr& packet)
{
    generic_relay(chandle, packet, addr_registry_);
}

void network_impl::relay(channel_handle chandle,
     const message::inv& packet)
{
    generic_relay(chandle, packet, inv_registry_);
}

void network_impl::relay(channel_handle chandle,
     const message::block& packet)
{
    generic_relay(chandle, packet, block_registry_);
}

size_t network_impl::connection_count() const
{
    return channels_.size();
}

bool network_impl::start_accept()
{
    acceptor_.reset(new tcp::acceptor(*service()));
    socket_ptr socket(new tcp::socket(*service()));
    try
    {
        tcp::endpoint endpoint(tcp::v4(), 8333);
        acceptor_->open(endpoint.protocol());
        acceptor_->set_option(tcp::acceptor::reuse_address(true));
        acceptor_->bind(endpoint);
        acceptor_->listen(socket_base::max_connections);
        acceptor_->async_accept(*socket,
            std::bind(&network_impl::handle_accept, shared_from_this(), 
                socket));
    }
    catch (std::exception& ex)
    {
        log_error() << "Accepting connections: " << ex.what();
        return false;
    }
    return true;
}

void network_impl::handle_accept(socket_ptr socket)
{
    tcp::endpoint remote_endpoint = socket->remote_endpoint();
    log_debug() << "New incoming connection from "
            << remote_endpoint.address().to_string();
    channel_handle chanid = create_channel(socket);
    strand()->post(
        [&listeners_, chanid]
        {
            for (connect_handler handle_connect: listeners_)
                handle_connect(std::error_code(), chanid);
            listeners_.clear();
        });
    socket.reset(new tcp::socket(*service()));
    acceptor_->async_accept(*socket,
            std::bind(&network_impl::handle_accept, shared_from_this(), 
                socket));
}

void network_impl::set_ip_address(std::string ip_addr)
{
    //our_ip_address_ = ip_addr;
}
message::ip_address network_impl::get_ip_address() const
{
    return our_ip_address_;
}

} // libbitcoin

