#include <bitcoin/kernel.hpp>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/bind.hpp>
#include <algorithm>

#include <bitcoin/util/assert.hpp>
#include <bitcoin/util/logger.hpp>
#include <bitcoin/network/network.hpp>
#include <bitcoin/storage/storage.hpp>

using boost::posix_time::seconds;
using boost::posix_time::minutes;
using boost::posix_time::time_duration;
using boost::asio::buffer;

// Debug - should go soon
#include <iomanip>

namespace libbitcoin {

const time_duration poll_inv_timeout = seconds(10);

void null(std::error_code)
{
}

kernel::kernel()
{
}

void kernel::register_network(network_ptr net_comp)
{
    network_component_ = net_comp;
}

network_ptr kernel::get_network()
{
    return network_component_;
}

void kernel::send_failed(channel_handle, const message::version&)
{
}

void kernel::send_failed(channel_handle, const message::verack&)
{
}

void kernel::send_failed(channel_handle, const message::getaddr&)
{
}

void kernel::send_failed(channel_handle, const message::inv&)
{
}

void kernel::send_failed(channel_handle, const message::getdata&)
{
}

void kernel::send_failed(channel_handle, const message::getblocks&)
{
}

bool kernel::recv_message(channel_handle chandle,
        const message::version& message)
{
    log_debug() << "nonce is " << message.nonce;
    log_debug() << "last block is " << message.start_height;
    log_debug() << pretty_hex(message.addr_you.ip_addr);
    network_component_->send(chandle, message::verack());
    return true;
}

bool kernel::recv_message(channel_handle, const message::verack&)
{
    // When you receive this, then you know other side is accepting your sends
    return true;
}

bool kernel::recv_message(channel_handle, const message::addr& message)
{
    //for (const message::net_addr addr: message.addr_list)
    //    log_debug() << pretty_hex(addr.ip_addr) << ' ' << addr.port;
    return true;
}

bool kernel::recv_message(channel_handle chandle, const message::inv& message)
{
    message::inv request_invs;
    for (const message::inv_vect curr_inv: message.invs)
    {
        if (curr_inv.type == message::inv_type::none)
            return false;

        // Push only block invs to the request queue
        if (curr_inv.type == message::inv_type::block)
            request_invs.invs.push_back(curr_inv);
    }
    if (request_invs.invs.size() > 0)
        accept_inventories(request_invs.invs);
    return true;
}

bool kernel::recv_message(channel_handle, const message::block& message)
{
    storage_component_->store(message, null);
    return true;
}

void kernel::handle_connect(channel_handle)
{
}

void kernel::register_storage(storage_ptr stor_comp)
{
    storage_component_ = stor_comp;
}

storage_ptr kernel::get_storage()
{
    return storage_component_;
}

void kernel::accept_inventories(const message::inv_list& invs)
{
    message::getdata request_message;
    request_message.invs = invs;
    network_component_->get_random_handle(std::bind(
        &kernel::send_to_random, shared_from_this(),
            std::placeholders::_1, request_message));
}

void kernel::send_to_random(channel_handle chandle,
    const message::getdata& request_message)
{
    network_component_->send(chandle, request_message);
}

} // libbitcoin

