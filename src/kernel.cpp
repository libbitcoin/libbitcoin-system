#include <bitcoin/kernel.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>
#include <algorithm>

#include <bitcoin/util/assert.hpp>
#include <bitcoin/util/logger.hpp>
#include <bitcoin/net/network.hpp>
#include <bitcoin/storage/storage.hpp>

using boost::posix_time::seconds;
using boost::posix_time::minutes;
using boost::posix_time::time_duration;
using boost::asio::buffer;

// Debug - should go soon
#include <iomanip>

namespace libbitcoin {

const time_duration poll_inv_timeout = seconds(10);

static void run_service(shared_ptr<io_service> service)
{
    service->run();
}

kernel::kernel()
{
    service_.reset(new io_service);
    work_.reset(new io_service::work(*service_));
    runner_ = std::thread(run_service, service_);
}

void kernel::register_network(net::network_ptr net_comp)
{
    network_component_ = net_comp;
}

net::network_ptr kernel::get_network()
{
    return network_component_;
}

void kernel::send_failed(net::channel_handle,
        net::message::version)
{
}

void kernel::send_failed(net::channel_handle,
        net::message::verack)
{
}

void kernel::send_failed(net::channel_handle,
        net::message::getaddr)
{
}

void kernel::send_failed(net::channel_handle,
        net::message::inv)
{
}

void kernel::send_failed(net::channel_handle,
        net::message::getdata)
{
}

void kernel::send_failed(net::channel_handle,
        net::message::getblocks)
{
}

template<typename T>
void display_byte_array(T arr)
{
    auto logobj = logger(LOG_DEBUG);
    logobj << std::hex;
    for (auto it = arr.cbegin(); it != arr.cend(); ++it)
    {
        logobj << std::setfill('0') << std::setw(2)
                << static_cast<unsigned int>(*it) << " ";
    }
}

bool kernel::recv_message(net::channel_handle chandle,
        net::message::version message)
{
    logger(LOG_DEBUG) << "nonce is " << message.nonce;
    logger(LOG_DEBUG) << "last block is " << message.start_height;
    display_byte_array(message.addr_you.ip_addr);
    network_component_->send(chandle, net::message::verack());
    return true;
}

bool kernel::recv_message(net::channel_handle,
        net::message::verack)
{
    return true;
}

bool kernel::recv_message(net::channel_handle,
        net::message::addr message)
{
    for (auto it = message.addr_list.cbegin();
            it != message.addr_list.cend(); ++it)
    {
        logger(LOG_DEBUG) << it->port;
        display_byte_array(it->ip_addr);
    }
    return true;
}

bool kernel::recv_message(net::channel_handle,
        net::message::inv message)
{
    net::message::inv request_invs;

    for (auto it = message.invs.cbegin();
            it != message.invs.cend(); ++it)
    {
        if (it->type == net::message::inv_type::none)
            return false;

        if (it->type == net::message::inv_type::error)
            logger(LOG_DEBUG) << "ERROR";
        else if (it->type == net::message::inv_type::transaction)
            logger(LOG_DEBUG) << "MSG_TX";
        else if (it->type == net::message::inv_type::block)
            logger(LOG_DEBUG) << "MSG_BLOCK";
        display_byte_array(it->hash);

        // Push only block invs to the request queue
        if (it->type == net::message::inv_type::block)
            request_invs.invs.push_back(*it);
    }
    storage_component_->push(request_invs);
    accept_inventories(request_invs.invs);
    return true;
}

void kernel::handle_connect(net::channel_handle)
{
}

void kernel::register_storage(storage::storage_ptr stor_comp)
{
    storage_component_ = stor_comp;
    poll_invs_timeout_.reset(new deadline_timer(*service_));
    reset_inventory_poll();
}

void kernel::reset_inventory_poll()
{
    poll_invs_timeout_->cancel();
    poll_invs_timeout_->expires_from_now(poll_inv_timeout);
    poll_invs_timeout_->async_wait(std::bind(
            &kernel::request_inventories, shared_from_this(),
                std::placeholders::_1));
}

void kernel::request_inventories(const boost::system::error_code& ec)
{
    if (ec)
        return;
    storage_component_->request_inventories(
            std::bind(&kernel::accept_inventories,
                shared_from_this(), std::placeholders::_1));
    reset_inventory_poll();
}

storage::storage_ptr kernel::get_storage()
{
    return storage_component_;
}

void kernel::send_to_random(net::channel_handle chandle,
        net::message::getdata request_message)
{
    network_component_->send(chandle, request_message);
}

void kernel::accept_inventories(net::message::inv_list invs)
{
    net::message::getdata request_message;
    request_message.invs = invs;
    network_component_->get_random_handle(std::bind(
            &kernel::send_to_random, shared_from_this(),
                std::placeholders::_1, request_message));
}

} // libbitcoin

