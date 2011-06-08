#include <bitcoin/kernel.hpp>

#include <bitcoin/util/logger.hpp>
#include <bitcoin/net/network.hpp>
#include <bitcoin/storage/storage.hpp>

// Debug - should go soon
#include <iomanip>

namespace libbitcoin {

void kernel::register_network(net::network_ptr net_comp)
{
    network_component_ = net_comp;
}

net::network_ptr kernel::get_network()
{
    return network_component_;
}

void kernel::send_failed(net::channel_handle chandle, 
        net::message::version message)
{
}

void kernel::send_failed(net::channel_handle chandle, 
        net::message::verack message)
{
}

void kernel::send_failed(net::channel_handle chandle, 
        net::message::getaddr message)
{
}

void kernel::send_failed(net::channel_handle chandle, 
        net::message::inv message)
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

bool kernel::recv_message(net::channel_handle chandle, 
        net::message::verack message)
{
    return true;
}

bool kernel::recv_message(net::channel_handle chandle,
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

bool kernel::recv_message(net::channel_handle chandle,
        net::message::inv message)
{
    for (auto it = message.inv_list.cbegin(); 
            it != message.inv_list.cend(); ++it)
    {
        if (it->type == net::message::inv_type::none)
            return false;
        //logger(LOG_DEBUG) << std::hex << static_cast<unsigned int>(it->hash[0]);
        if (it->type == net::message::inv_type::error)
            logger(LOG_DEBUG) << "ERROR";
        else if (it->type == net::message::inv_type::transaction)
            logger(LOG_DEBUG) << "MSG_TX";
        else if (it->type == net::message::inv_type::block)
            logger(LOG_DEBUG) << "MSG_BLOCK";
        display_byte_array(it->hash);
    }
    storage_component_->push(message);
    return true;
}

void kernel::register_storage(storage::storage_ptr stor_comp)
{
    storage_component_ = stor_comp;
}

storage::storage_ptr kernel::get_storage()
{
    return storage_component_;
}

} // libbitcoin

