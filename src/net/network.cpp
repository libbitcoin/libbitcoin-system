#include <bitcoin/net/network.hpp>

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
namespace net {

using boost::asio::socket_base;

network_impl::network_impl(kernel_ptr kern)
 : kernel_(kern)
{
    strand_.reset(new io_service::strand(*service()));
    default_dialect_.reset(new original_dialect);
}

network_impl::~network_impl()
{
    if (acceptor_)
        acceptor_->close();
}

kernel_ptr network_impl::kernel() const
{
    return kernel_;
}

channel_handle network_impl::create_channel(socket_ptr socket)
{
    channel_pimpl::init_data init_data = {
            shared_from_this(), default_dialect_, service(), socket };

    channel_pimpl* channel_obj = new channel_pimpl(init_data);
    channels_.push_back(channel_obj);
    log_debug() << channels_.size() << " peers connected.";
    return channel_obj->get_id();
}

void network_impl::handle_connect(const boost::system::error_code& ec, 
        socket_ptr socket, std::string ip_addr, connect_handler handle_connect)
{
    if (ec)
    {
        log_error() << "Connecting to peer " << ip_addr << ": " << ec.message();
        handle_connect(network_error::system_network_error, 0);
        return;
    }
    channel_handle chanid = create_channel(socket);
    handle_connect(std::error_code(), chanid);
}

void network_impl::connect(std::string ip_addr, unsigned short port,
        connect_handler handle_connect)
{
    socket_ptr socket(new tcp::socket(*service()));
    tcp::resolver resolver(*service());
    tcp::resolver::query query(ip_addr,
            boost::lexical_cast<std::string>(port));
    tcp::endpoint endpoint = *resolver.resolve(query);
    socket->async_connect(endpoint, std::bind(
            &network_impl::handle_connect, shared_from_this(), 
                _1, socket, ip_addr, handle_connect));
}

static void remove_matching_channels(channel_list* channels,
        channel_handle chandle)
{
    auto is_matching =
            [chandle](channel_pimpl& channel_obj)
            {
                return channel_obj.get_id() == chandle;
            };
    channels->erase_if(is_matching);
    log_debug() << channels->size() << " peers remaining.";
}
void network_impl::disconnect(channel_handle chandle)
{
    strand_->post(
            std::bind(remove_matching_channels, &channels_, chandle));
}

template<typename T>
void perform_send(channel_list* channels, kernel_ptr kern,
        channel_handle chandle, T message_packet)
{
    auto is_matching =
            [chandle](const channel_pimpl& channel_obj)
            {
                return channel_obj.get_id() == chandle;
            };
    auto it = std::find_if(channels->begin(), channels->end(), is_matching);
    if (it == channels->end())
    {
        log_error() << "Non existant channel " << chandle << " for send.";
        kern->send_failed(chandle, message_packet);
        return;
    }
    it->send(message_packet);
}

template<typename T>
void generic_send(T message_packet, channel_handle chandle,
        shared_ptr<io_service::strand> strand, channel_list* channels,
        kernel_ptr kernel)
{
    strand->post(std::bind(
            &perform_send<T>, channels, kernel, chandle, message_packet));
}

void network_impl::send(channel_handle chandle, message::version version)
{
    generic_send(version, chandle, strand_, &channels_, kernel_);
}

void network_impl::send(channel_handle chandle, message::verack verack)
{
    generic_send(verack, chandle, strand_, &channels_, kernel_);
}

void network_impl::send(channel_handle chandle, message::getaddr getaddr)
{
    generic_send(getaddr, chandle, strand_, &channels_, kernel_);
}

void network_impl::send(channel_handle chandle, message::getdata getdata)
{
    generic_send(getdata, chandle, strand_, &channels_, kernel_);
}

void network_impl::send(channel_handle chandle, message::getblocks getblocks)
{
    generic_send(getblocks, chandle, strand_, &channels_, kernel_);
}

size_t network_impl::connection_count() const
{
    return channels_.size();
}

void network_impl::do_get_random_handle(accept_random_handle accept_handler)
{
    size_t rand_index = rand() % channels_.size();
    channel_handle rand_chan = channels_[rand_index].get_id();
    accept_handler(rand_chan);
}
void network_impl::get_random_handle(accept_random_handle accept_handler)
{
    strand_->post(std::bind(
            &network_impl::do_get_random_handle, shared_from_this(), 
                accept_handler));
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
    kernel_->handle_connect(chanid);
    socket.reset(new tcp::socket(*service()));
    acceptor_->async_accept(*socket,
            std::bind(&network_impl::handle_accept, shared_from_this(), 
                socket));
}

} // net
} // libbitcoin

