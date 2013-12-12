/**
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/network/handshake.hpp>

#include <functional>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <curl/curl.h>

#include <bitcoin/network/network.hpp>
#include <bitcoin/version.hpp>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;

handshake::handshake(threadpool& pool)
  : strand_(pool.service())
{
    // Setup template version packet with defaults
    template_version_.version = protocol_version;
    template_version_.services = 1;
    // non-constant field
    //template_version_.timestamp = time(NULL);
    template_version_.address_me.services = template_version_.services;
    template_version_.address_me.ip = localhost_ip();
    template_version_.address_me.port = protocol_port;
    template_version_.address_you.services = template_version_.services;
    template_version_.address_you.ip = 
        ip_address_type{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                        0x00, 0x00, 0xff, 0xff, 0x0a, 0x00, 0x00, 0x01};
    template_version_.address_you.port = protocol_port;
    template_version_.user_agent = "/libbitcoin:" LIBBITCOIN_LIB_VERSION "/";
    template_version_.start_height = 0;
    template_version_.nonce = rand();
}

void handshake::start(start_handler handle_start)
{
    discover_external_ip(std::bind(handle_start, _1));
}

void handshake::ready(channel_ptr node,
    handshake::handshake_handler handle_handshake)
{
    auto completion_callback = async_parallel(handle_handshake, 3);

    version_type session_version = template_version_;
    session_version.timestamp = time(NULL);
    node->send(session_version,
        strand_.wrap(std::bind(&handshake::handle_message_sent,
            this, _1, completion_callback)));

    node->subscribe_version(
        strand_.wrap(std::bind(&handshake::receive_version,
            this, _1, _2, node, completion_callback)));
    node->subscribe_verack(
        strand_.wrap(std::bind(&handshake::receive_verack,
            this, _1, _2, completion_callback)));
}

void handshake::handle_message_sent(const std::error_code& ec,
    handshake::handshake_handler completion_callback)
{
    completion_callback(ec);
}

void handshake::receive_version(
    const std::error_code& ec, const version_type&,
    channel_ptr node, handshake::handshake_handler completion_callback)
{
    if (ec)
        completion_callback(ec);
    else
        node->send(verack_type(),
            strand_.wrap(std::bind(&handshake::handle_message_sent,
                this, _1, completion_callback)));
}

void handshake::receive_verack(
    const std::error_code& ec, const verack_type&,
    handshake::handshake_handler completion_callback)
{
    completion_callback(ec);
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
    strand_.post(
        std::bind(&handshake::do_discover_external_ip,
            this, handle_discover));
}

bool handshake::lookup_external(const std::string& website,
    ip_address_type& ip)
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
    curl_easy_cleanup(curl);
    if (result != CURLE_OK)
        return false;
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

ip_address_type handshake::localhost_ip()
{
    return ip_address_type{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0xff, 0xff, 0x0a, 0x00, 0x00, 0x01};
}

void handshake::do_discover_external_ip(discover_ip_handler handle_discover)
{
    template_version_.address_me.ip = localhost_ip();
    std::vector<ip_address_type> corroborate_ips;
    // Lookup our IP address from a bunch of hosts
    ip_address_type lookup_ip;
    if (lookup_external("checkip.dyndns.org", lookup_ip))
        corroborate_ips.push_back(lookup_ip);
    if (lookup_external("whatismyip.org", lookup_ip))
        corroborate_ips.push_back(lookup_ip);
    if (corroborate_ips.empty())
    {
        handle_discover(error::bad_stream, ip_address_type());
        return;
    }
    // Make sure that the IPs are the same
    template_version_.address_me.ip = corroborate_ips[0];
    for (const ip_address_type& match_ip: corroborate_ips)
    {
        if (match_ip != template_version_.address_me.ip)
        {
            template_version_.address_me.ip = localhost_ip();
            handle_discover(error::bad_stream, ip_address_type());
            return;
        }
    }
    handle_discover(std::error_code(), template_version_.address_me.ip);
}

void handshake::fetch_network_address(
    fetch_network_address_handler handle_fetch)
{
    strand_.post(
        std::bind(&handshake::do_fetch_network_address,
            this, handle_fetch));
}
void handshake::do_fetch_network_address(
    fetch_network_address_handler handle_fetch)
{
    handle_fetch(std::error_code(), template_version_.address_me);
}

void handshake::set_port(uint16_t port, setter_handler handle_set)
{
    strand_.post(
        std::bind(&handshake::do_set_port,
            this, port, handle_set));
}
void handshake::do_set_port(uint16_t port, setter_handler handle_set)
{
    template_version_.address_me.port = port;
    handle_set(std::error_code());
}

void handshake::set_user_agent(const std::string& user_agent,
    setter_handler handle_set)
{
    strand_.post(
        std::bind(&handshake::do_set_user_agent,
            this, user_agent, handle_set));
}
void handshake::do_set_user_agent(const std::string& user_agent,
    setter_handler handle_set)
{
    template_version_.user_agent = user_agent;
    handle_set(std::error_code());
}

void handshake::set_start_height(uint32_t height, setter_handler handle_set)
{
    strand_.post(
        std::bind(&handshake::do_set_start_height,
            this, height, handle_set));
}
void handshake::do_set_start_height(uint32_t height, setter_handler handle_set)
{
    template_version_.start_height = height;
    handle_set(std::error_code());
}

void finish_connect(const std::error_code& ec,
    channel_ptr node, handshake& shake,
    network::connect_handler handle_connect)
{
    if (ec)
        handle_connect(ec, node);
    else
        shake.ready(node, std::bind(handle_connect, _1, node));
}

void connect(handshake& shake, network& net,
    const std::string& hostname, uint16_t port,
    network::connect_handler handle_connect)
{
    net.connect(hostname, port, 
        std::bind(finish_connect, _1, _2, std::ref(shake), handle_connect));
}

} // namespace libbitcoin

