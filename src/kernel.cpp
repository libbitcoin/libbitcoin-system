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

void run_service(shared_ptr<io_service> service)
{
    service->run();
}

void null(std::error_code)
{
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
        message::version)
{
}

void kernel::send_failed(net::channel_handle,
        message::verack)
{
}

void kernel::send_failed(net::channel_handle,
        message::getaddr)
{
}

void kernel::send_failed(net::channel_handle,
        message::inv)
{
}

void kernel::send_failed(net::channel_handle,
        message::getdata)
{
}

void kernel::send_failed(net::channel_handle,
        message::getblocks)
{
}

template<typename T>
void display_byte_array(T arr)
{
    auto logobj = log_debug();
    logobj << std::hex;
    for (unsigned int number: arr)
        logobj << std::setfill('0') << std::setw(2) << number << " ";
}

bool kernel::recv_message(net::channel_handle chandle,
        message::version message)
{
    log_debug() << "nonce is " << message.nonce;
    log_debug() << "last block is " << message.start_height;
    display_byte_array(message.addr_you.ip_addr);
    network_component_->send(chandle, message::verack());
    return true;
}

bool kernel::recv_message(net::channel_handle,
        message::verack)
{
    // When you receive this, then you know other side is accepting your sends
    return true;
}

bool kernel::recv_message(net::channel_handle,
        message::addr message)
{
    for (const message::net_addr addr: message.addr_list)
    {
        log_debug() << addr.port;
        display_byte_array(addr.ip_addr);
    }
    return true;
}

bool kernel::recv_message(net::channel_handle,
        message::inv message)
{
    message::inv request_invs;
    for (const message::inv_vect curr_inv: message.invs)
    {
        if (curr_inv.type == message::inv_type::none)
            return false;

        if (curr_inv.type == message::inv_type::error)
            log_debug() << "ERROR";
        else if (curr_inv.type == message::inv_type::transaction)
            log_debug() << "MSG_TX";
        else if (curr_inv.type == message::inv_type::block)
            log_debug() << "MSG_BLOCK";
        display_byte_array(curr_inv.hash);

        // Push only block invs to the request queue
        if (curr_inv.type == message::inv_type::block)
            request_invs.invs.push_back(curr_inv);
    }
    storage_component_->store(request_invs, null);
    accept_inventories(request_invs.invs, std::error_code());
    return true;
}

bool kernel::recv_message(net::channel_handle,
        message::block message)
{
    storage_component_->store(message, null);
    return true;
}

void kernel::handle_connect(net::channel_handle)
{
}

void kernel::register_storage(storage_ptr stor_comp)
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
    storage_component_->fetch_inventories(
            std::bind(&kernel::accept_inventories,
                shared_from_this(), 
                std::placeholders::_1, std::placeholders::_2));
    reset_inventory_poll();
}

storage_ptr kernel::get_storage()
{
    return storage_component_;
}

void kernel::send_to_random(net::channel_handle chandle,
        message::getdata request_message)
{
    network_component_->send(chandle, request_message);
}

void kernel::accept_inventories(message::inv_list invs, std::error_code ec)
{
    if (ec)
        return;
    message::getdata request_message;
    request_message.invs = invs;
    network_component_->get_random_handle(std::bind(
            &kernel::send_to_random, shared_from_this(),
                std::placeholders::_1, request_message));
}

} // libbitcoin

