#include <bitcoin/net/network.hpp>

#include <boost/lexical_cast.hpp>
#include <functional>
#include <algorithm>
#include <iostream>

#include <bitcoin/util/logger.hpp>

#include "channel.hpp"
#include "dialect.hpp"

namespace libbitcoin {
namespace net {

using boost::asio::socket_base;

static void run_service(shared_ptr<io_service> service)
{
    service->run();
}

network_impl::network_impl(kernel_ptr kern)
 : kernel_(kern)
{
    service_.reset(new io_service);
    work_.reset(new io_service::work(*service_));
    strand_.reset(new io_service::strand(*service_));
    runner_ = std::thread(run_service, service_);
    default_dialect_.reset(new original_dialect);
}

network_impl::~network_impl()
{
    if (acceptor_)
        acceptor_->close();
    service_->stop();
    runner_.join();
}

kernel_ptr network_impl::kernel() const
{
    return kernel_;
}

channel_handle network_impl::create_channel(socket_ptr socket)
{
    channel_pimpl::init_data init_data = { 
            shared_from_this(), default_dialect_, service_, socket };

    channel_pimpl* channel_obj = new channel_pimpl(init_data);
    strand_->post(
            [&channels_, channel_obj]
            {
                channels_.push_back(channel_obj);
                logger(LOG_DEBUG) << channels_.size() << " peers connected.";
            });
    return channel_obj->get_id();
}

channel_handle network_impl::connect(bool& ec, std::string ip_addr, 
        unsigned short port)
{
    ec = false;
    socket_ptr socket(new tcp::socket(*service_));
    try 
    {
        tcp::resolver resolver(*service_);
        tcp::resolver::query query(ip_addr, 
                boost::lexical_cast<std::string>(port));
        tcp::endpoint endpoint = *resolver.resolve(query);
        socket->connect(endpoint);
    }
    catch (std::exception& ex) 
    {
        logger(LOG_ERROR) << "Connecting to peer " << ip_addr 
                << ": " << ex.what();
        ec = true;
        return 0;
    }
    return create_channel(socket);
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
    logger(LOG_DEBUG) << channels->size() << " peers remaining.";
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
        logger(LOG_ERROR) << "Non existant channel " << chandle << " for send.";
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
            &network_impl::do_get_random_handle, this, accept_handler));
}

bool network_impl::start_accept()
{
    acceptor_.reset(new tcp::acceptor(*service_));
    socket_ptr socket(new tcp::socket(*service_));
    try
    {
        tcp::endpoint endpoint(tcp::v4(), 8333);
        acceptor_->open(endpoint.protocol());
        acceptor_->set_option(tcp::acceptor::reuse_address(true));
        acceptor_->bind(endpoint);
        acceptor_->listen(socket_base::max_connections);
        acceptor_->async_accept(*socket, 
                std::bind(&network_impl::handle_accept, this, socket));
    }
    catch (std::exception& ex)
    {
        logger(LOG_ERROR) << "Accepting connections: " << ex.what();
        return false;
    }
    return true;
}

void network_impl::handle_accept(socket_ptr socket)
{
    tcp::endpoint remote_endpoint = socket->remote_endpoint();
    logger(LOG_DEBUG) << "New incoming connection from " 
            << remote_endpoint.address().to_string();
    channel_handle chandle = create_channel(socket);
    kernel_->handle_connect(chandle);
    socket.reset(new tcp::socket(*service_));
    acceptor_->async_accept(*socket, 
            std::bind(&network_impl::handle_accept, this, socket));
}

} // net
} // libbitcoin

