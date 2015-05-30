/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/network/hosts.hpp>

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <system_error>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/formats/base16.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/unicode/ifstream.hpp>
#include <bitcoin/bitcoin/unicode/ofstream.hpp>
#include <bitcoin/bitcoin/unicode/unicode.hpp>
#include <bitcoin/bitcoin/utility/async_strand.hpp>
#include <bitcoin/bitcoin/utility/string.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {
    
using boost::format;
using boost::filesystem::path;

hosts::hosts(threadpool& pool, const path& file_path, size_t capacity)
  : strand_(pool), file_path_(file_path), buffer_(capacity)
{
    srand(static_cast<uint32_t>(time(nullptr)));
}

hosts::hosts(threadpool& pool, size_t capacity)
  : hosts(pool, "hosts.p2p", capacity)
{
}

hosts::~hosts()
{
    // This was reportedly required for use with circular_buffer.
}

bool hosts::empty() const
{
    return buffer_.empty();
}

size_t hosts::size() const
{
    return buffer_.size();
}

void hosts::load(load_handler handle_load)
{
    strand_.randomly_queue(&hosts::do_load,
        this, file_path_.string(), handle_load);
}

void hosts::load(const std::string& path, load_handler handle_load)
{
    strand_.randomly_queue(&hosts::do_load,
        this, path, handle_load);
}

void hosts::do_load(const path& path, load_handler handle_load)
{
    bc::ifstream file(path.string());
    if (file.bad())
    {
        const auto error = std::make_error_code(std::errc::io_error);
        handle_load(error);
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        ip_address address(line);
        const auto enqueue = [this, address]()
        {
            buffer_.push_back(address);
        };

        if (address.port != 0)
            strand_.randomly_queue(enqueue);
    }

    handle_load(std::error_code());
}

void hosts::save(save_handler handle_save)
{
    strand_.randomly_queue(
        std::bind(&hosts::do_save,
            this, file_path_.string(), handle_save));
}

void hosts::save(const std::string& path, save_handler handle_save)
{
    strand_.randomly_queue(
        std::bind(&hosts::do_save,
            this, path, handle_save));
}

void hosts::do_save(const path& path, save_handler handle_save)
{
    bc::ofstream file(path.string());
    if (file.bad())
    {
        const auto error = std::make_error_code(std::errc::io_error);
        handle_save(error);
        return;
    }

    for (const auto& entry: buffer_)
        file << encode_base16(entry.ip) << " " << entry.port << std::endl;

    handle_save(std::error_code());
}

void hosts::remove(const network_address_type& address,
    remove_handler handle_remove)
{
    strand_.randomly_queue(
        std::bind(&hosts::do_remove,
            this, address, handle_remove));
}

void hosts::do_remove(const network_address_type& address,
    remove_handler handle_remove)
{
    const ip_address host(address);
    const auto it = std::find(buffer_.begin(), buffer_.end(), host);
    if (it == buffer_.end())
    {
        handle_remove(error::not_found);
        return;
    }

    buffer_.erase(it);
    //log_info("hosts") << "Remove host ("
    //    << buffer_.size() << ") [" << authority(address).to_string() << "]";
    handle_remove(std::error_code());
}

void hosts::store(const network_address_type& address,
    store_handler handle_store)
{
    strand_.randomly_queue(
        std::bind(&hosts::do_store,
            this, address, handle_store));
}

void hosts::do_store(const network_address_type& address,
    store_handler handle_store)
{
    buffer_.push_back(ip_address(address));
    //log_info("hosts") << "Add host ("
    //    << buffer_.size() << ") [" << authority(address).to_string() << "]";
    handle_store(std::error_code());
}

void hosts::fetch_address(fetch_address_handler handle_fetch)
{
    strand_.randomly_queue(
        std::bind(&hosts::do_fetch_address,
            this, handle_fetch));
}

void hosts::do_fetch_address(fetch_address_handler handle_fetch)
{
    if (buffer_.empty())
    {
        handle_fetch(error::not_found, network_address_type());
        return;
    }

    const auto index = rand() % buffer_.size();
    network_address_type address;
    address.timestamp = 0;
    address.services = 0;
    address.ip = buffer_[index].ip;
    address.port = buffer_[index].port;
    handle_fetch(std::error_code(), address);
}

void hosts::fetch_count(fetch_count_handler handle_fetch)
{
    strand_.randomly_queue(
        std::bind(&hosts::do_fetch_count,
            this, handle_fetch));
}

void hosts::do_fetch_count(fetch_count_handler handle_fetch)
{
    handle_fetch(std::error_code(), buffer_.size());
}

// private struct ip_address

hosts::ip_address::ip_address()
  : port(0)
{
}

hosts::ip_address::ip_address(const network_address_type& host)
  : ip(host.ip), port(host.port)
{
    // TODO: We are getting unspecified ports over the wire.
    // We may want to explicitly write in the default port at that time.
    // BITCOIN_ASSERT(port != 0);
}

// This is the format utilized by the hosts file.
hosts::ip_address::ip_address(const std::string& line)
  : ip_address()
{
    const auto parts = split(boost::trim_copy(line), " ");
    if (parts.size() != 2)
        return;

    data_chunk data;
    if (!decode_base16(data, parts[0]) || (data.size() != ip.size()))
        return;

    // TODO: verify lexical_cast ndebug exception safety.
    std::copy(data.begin(), data.end(), ip.begin());
    port = boost::lexical_cast<uint16_t>(parts[1]);

    // The port was invalid or unspecified.
    // BITCOIN_ASSERT(port != 0);
}

bool hosts::ip_address::operator==(const hosts::ip_address& other) const
{
    return ip == other.ip && port == other.port;
}

} // namespace network
} // namespace libbitcoin
