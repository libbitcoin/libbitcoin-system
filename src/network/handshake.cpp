#include <bitcoin/network/handshake.hpp>
#include <bitcoin/network/network.hpp>

#include <functional>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;

const size_t clearance_count = 3;

handshake::handshake()
{
    threaded_ = std::make_shared<thread_core>();
    strand_ = threaded_->create_strand();
    // Setup template version packet with defaults
    template_version_.version = 60000;
    template_version_.services = 1;
    // non-constant field
    //template_version_.timestamp = time(NULL);
    template_version_.address_me.services = template_version_.services;
    template_version_.address_me.ip = 
        message::ip_address{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                            0x00, 0x00, 0xff, 0xff, 0x0a, 0x00, 0x00, 0x01};
    template_version_.address_me.port = 8333;
    template_version_.address_you.services = template_version_.services;
    template_version_.address_you.ip = 
        message::ip_address{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                            0x00, 0x00, 0xff, 0xff, 0x0a, 0x00, 0x00, 0x01};
    template_version_.address_you.port = 8333;
    template_version_.user_agent = "/libbitcoin:0.1a/";
    template_version_.start_height = 0;
    template_version_.nonce = rand();
}

void handshake::connect(network_ptr net, const std::string& hostname,
    uint16_t port, network::connect_handler handle_connect)
{
    net->connect(hostname, port, 
        strand_->wrap(std::bind(&handshake::initiate_handshake,
            shared_from_this(), _1, _2, handle_connect)));
}

void handshake::initiate_handshake(const std::error_code& ec,
    channel_ptr node, network::connect_handler handle_connect)
{
    if (ec)
        handle_connect(ec, node);
    else
        start(node, std::bind(handle_connect, _1, node));
}

void handshake::start(channel_ptr node,
    handshake::handshake_handler handle_handshake)
{
    atomic_counter_ptr counter = std::make_shared<atomic_counter>(0);

    message::version session_version = template_version_;
    session_version.timestamp = time(NULL);
    node->send(session_version,
        strand_->wrap(std::bind(&handshake::handle_message_sent,
            shared_from_this(), _1, counter, handle_handshake)));

    node->subscribe_version(
        strand_->wrap(std::bind(&handshake::receive_version,
            shared_from_this(), _1, _2, node, counter, handle_handshake)));
    node->subscribe_verack(
        strand_->wrap(std::bind(&handshake::receive_verack,
            shared_from_this(), _1, _2, counter, handle_handshake)));
}

void handshake::handle_message_sent(const std::error_code& ec,
    atomic_counter_ptr counter,
    handshake::handshake_handler completion_callback)
{
    if (ec)
        completion_callback(ec);
    else if (++(*counter) == clearance_count)
        completion_callback(std::error_code());
}

void handshake::receive_version(const std::error_code& ec,
    const message::version&, channel_ptr node, atomic_counter_ptr counter,
    handshake::handshake_handler completion_callback)
{
    if (ec)
        completion_callback(ec);
    else
        node->send(message::verack(),
            strand_->wrap(std::bind(&handshake::handle_message_sent,
                shared_from_this(), _1, counter, completion_callback)));
}

void handshake::receive_verack(const std::error_code& ec,
    const message::verack&, atomic_counter_ptr counter,
    handshake::handshake_handler completion_callback)
{
    if (ec)
        completion_callback(ec);
    else if (++(*counter) == clearance_count)
        completion_callback(std::error_code());
}

void handshake::set_external_ip(const message::ip_address& ip,
    setter_handler handle_set)
{
    strand_->post(
        std::bind(&handshake::do_set_external_ip, shared_from_this(),
            ip, handle_set));
}
void handshake::do_set_external_ip(const message::ip_address& ip,
    setter_handler handle_set)
{
    template_version_.address_me.ip = ip;
    handle_set(std::error_code());
}

void handshake::set_user_agent(const std::string& user_agent,
    setter_handler handle_set)
{
    strand_->post(
        std::bind(&handshake::do_set_user_agent, shared_from_this(),
            user_agent, handle_set));
}
void handshake::do_set_user_agent(const std::string& user_agent,
    setter_handler handle_set)
{
    template_version_.user_agent = user_agent;
    handle_set(std::error_code());
}

} // libbitcoin

