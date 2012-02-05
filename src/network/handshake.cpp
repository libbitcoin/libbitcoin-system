#include <bitcoin/network/handshake.hpp>
#include <bitcoin/network/network.hpp>

#include <functional>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <curl/curl.h>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;

const size_t clearance_count = 3;

handshake::handshake()
{
    threaded_ = std::make_shared<thread_core>();
    strand_ = threaded_->create_strand();
    // Setup template version packet with defaults
    template_version_.version = protocol_version;
    template_version_.services = 1;
    // non-constant field
    //template_version_.timestamp = time(NULL);
    template_version_.address_me.services = template_version_.services;
    template_version_.address_me.ip = localhost_ip();
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
    discover_external_ip(
        std::bind(&handshake::handle_discover_ip, shared_from_this(),
            _1, net, hostname, port, handle_connect));
}

void handshake::handle_discover_ip(const std::error_code& ec,
    network_ptr net, const std::string& hostname,
    uint16_t port, network::connect_handler handle_connect)
{
    if (ec)
        handle_connect(ec, nullptr);
    else
        net->connect(hostname, port, 
            strand_->wrap(std::bind(&handshake::handle_connect,
                shared_from_this(), _1, _2, handle_connect)));
}

void handshake::handle_connect(const std::error_code& ec,
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

int writer(char* data, size_t size, size_t count, std::string* buffer)
{
    int result = 0;
    if (buffer != NULL)
    {
        result = count * size;
        *buffer = std::string(data, result);
    }
    return result;
}

void handshake::discover_external_ip(discover_ip_handler handle_discover)
{
    strand_->post(
        std::bind(&handshake::do_discover_external_ip, shared_from_this(),
            handle_discover));
}

bool handshake::lookup_external(const std::string& website,
    message::ip_address& ip)
{
    // Initialise CURL with our various options.
    CURL* curl = curl_easy_init();
    // This goes first in case of any problems below. We get an error message.
    char error_buffer[CURL_ERROR_SIZE];
    error_buffer[0] = '\0';
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_buffer);
    // fail when server sends >= 404
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1);
    curl_easy_setopt(curl, CURLOPT_HEADER, 0);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 0);
    curl_easy_setopt(curl, CURLOPT_POSTREDIR, CURL_REDIR_POST_302);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
    curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_TRY);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1);
    // server response goes in response_buffer
    std::string response_buffer;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_buffer);
    curl_easy_setopt(curl, CURLOPT_URL, website.c_str());
    // Everything fine. Do fetch
    CURLcode result = curl_easy_perform(curl);
    if (result != CURLE_OK)
        return false;
    curl_easy_cleanup(curl);
    // TODO use std::regex instead ... when it work >_>
    boost::cmatch results;
    boost::regex rx("(\\d+)[.](\\d+)[.](\\d+)[.](\\d+)");
    if (!boost::regex_search(response_buffer.c_str(), results, rx))
    {
        return false;
    }
    ip = localhost_ip();
    for (size_t i = 0; i < 4; ++i)
        ip[i + 12] = boost::lexical_cast<unsigned>(results[i + 1]);
    return true;
}

message::ip_address handshake::localhost_ip()
{
    return message::ip_address{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                               0x00, 0x00, 0xff, 0xff, 0x0a, 0x00, 0x00, 0x01};
}

void handshake::do_discover_external_ip(discover_ip_handler handle_discover)
{
    template_version_.address_me.ip = localhost_ip();
    std::vector<message::ip_address> corroborate_ips;
    // Lookup our IP address from a bunch of hosts
    message::ip_address lookup_ip;
    if (lookup_external("checkip.dyndns.org", lookup_ip))
        corroborate_ips.push_back(lookup_ip);
    if (lookup_external("whatismyip.org", lookup_ip))
        corroborate_ips.push_back(lookup_ip);
    if (corroborate_ips.empty())
    {
        handle_discover(error::bad_stream, message::ip_address());
        return;
    }
    // Make sure that the IPs are the same
    template_version_.address_me.ip = corroborate_ips[0];
    for (const message::ip_address& match_ip: corroborate_ips)
    {
        if (match_ip != template_version_.address_me.ip)
        {
            template_version_.address_me.ip = localhost_ip();
            handle_discover(error::bad_stream, message::ip_address());
            return;
        }
    }
    handle_discover(std::error_code(), template_version_.address_me.ip);
}

void handshake::fetch_network_address(
    fetch_network_address_handler handle_fetch)
{
    strand_->post(
        std::bind(&handshake::do_fetch_network_address,
            shared_from_this(), handle_fetch));
}
void handshake::do_fetch_network_address(
    fetch_network_address_handler handle_fetch)
{
    handle_fetch(std::error_code(), template_version_.address_me);
}

void handshake::set_port(uint16_t port, setter_handler handle_set)
{
    strand_->post(
        std::bind(&handshake::do_set_port, shared_from_this(),
            port, handle_set));
}
void handshake::do_set_port(uint16_t port, setter_handler handle_set)
{
    template_version_.address_me.port = port;
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

