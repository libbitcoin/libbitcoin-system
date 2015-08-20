/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
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

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <system_error>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <bitcoin/bitcoin/config/endpoint.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/formats/base16.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/unicode/ifstream.hpp>
#include <bitcoin/bitcoin/unicode/ofstream.hpp>
#include <bitcoin/bitcoin/unicode/unicode.hpp>
#include <bitcoin/bitcoin/utility/random.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/string.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {
    
using boost::format;
using boost::filesystem::path;

// TODO: look into whether randomly_queue is achiving its objective.
hosts::hosts(threadpool& pool, const path& file_path, size_t capacity)
  : buffer_(capacity), dispatch_(pool), file_path_(file_path)
{
}

hosts::~hosts()
{
    // This was reportedly required for use with circular_buffer.
}

hosts::iterator hosts::find(const network_address_type& address)
{
    const auto found = [address](const network_address_type& entry)
    {
        return entry.port == address.port && entry.ip == address.ip;
    };

    return std::find_if(buffer_.begin(), buffer_.end(), found);
}

bool hosts::exists(const network_address_type& address)
{
    return find(address) != buffer_.end();
}

size_t hosts::capacity()
{
    return buffer_.capacity();
}

size_t hosts::size()
{
    return buffer_.size();
}

void hosts::load(load_handler handle_load)
{
    dispatch_.randomly_queue(
        std::bind(&hosts::do_load,
            this, file_path_.string(), handle_load));
}

void hosts::do_load(const path& path, load_handler handle_load)
{
    bc::ifstream file(path.string());
    if (file.bad())
    {
        handle_load(error::file_system);
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        config::authority host(line);
        if (host.port() != 0)
        {
            const auto load_address = [this, host]()
            {
                buffer_.push_back(host.to_network_address());
            };

            dispatch_.randomly_queue(load_address);
        }
    }

    handle_load(error::success);
}

void hosts::save(save_handler handle_save)
{
    dispatch_.randomly_queue(
        std::bind(&hosts::do_save,
            this, file_path_.string(), handle_save));
}

void hosts::do_save(const path& path, save_handler handle_save)
{
    bc::ofstream file(path.string());
    if (file.bad())
    {
        handle_save(error::file_system);
        return;
    }

    for (const auto& entry: buffer_)
        file << config::authority(entry) << std::endl;

    handle_save(error::success);
}

void hosts::remove(const network_address_type& address,
    remove_handler handle_remove)
{
    dispatch_.randomly_queue(
        std::bind(&hosts::do_remove,
            this, address, handle_remove));
}

void hosts::do_remove(const network_address_type& address,
    remove_handler handle_remove)
{
    const auto it = find(address);
    if (it == buffer_.end())
    {
        handle_remove(error::not_found);
        return;
    }

    buffer_.erase(it);
    handle_remove(error::success);
}

void hosts::store(const network_address_type& address,
    store_handler handle_store)
{
    if (address.port == 0)
        return;

    dispatch_.randomly_queue(
        std::bind(&hosts::do_store,
            this, address, handle_store));
}

void hosts::do_store(const network_address_type& address,
    store_handler handle_store)
{
    if (!exists(address))
    {
        buffer_.push_back(address);
        handle_store(error::success);
        return;
    }

    handle_store(error::duplicate);
}

void hosts::store(const network_address_list& addresses,
    store_handler handle_store)
{
    // If these are queued concurrently with others then it will lead to 
    // random distribution in the pool, which is why we queue here.
    for (const auto& address: addresses)
        dispatch_.randomly_queue(
            std::bind(&hosts::do_store,
                this, address, handle_store));
}

void hosts::fetch_address(fetch_address_handler handle_fetch)
{
    dispatch_.randomly_queue(
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

    // Randomly select a host from the buffer.
    const auto index = static_cast<size_t>(pseudo_random() % buffer_.size());
    handle_fetch(error::success, buffer_[index]);
}

void hosts::fetch_count(fetch_count_handler handle_fetch)
{
    dispatch_.randomly_queue(
        std::bind(&hosts::do_fetch_count,
            this, handle_fetch));
}

void hosts::do_fetch_count(fetch_count_handler handle_fetch)
{
    handle_fetch(error::success, buffer_.size());
}

} // namespace network
} // namespace libbitcoin
