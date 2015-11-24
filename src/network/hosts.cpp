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
#include <string>
#include <vector>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/network_settings.hpp>
#include <bitcoin/bitcoin/unicode/ifstream.hpp>
#include <bitcoin/bitcoin/unicode/ofstream.hpp>
#include <bitcoin/bitcoin/utility/log.hpp>
#include <bitcoin/bitcoin/utility/random.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

#define NAME "hosts"

hosts::hosts(threadpool& pool, const settings& settings)
  : buffer_(std::max(settings.host_pool_capacity, 1u)),
    dispatch_(pool, NAME),
    file_path_(settings.hosts_file),
    disabled_(settings.host_pool_capacity == 0)
{
}

hosts::iterator hosts::find(const address& host)
{
    const auto found = [&host](const address& entry)
    {
        return entry.port == host.port && entry.ip == host.ip;
    };

    return std::find_if(buffer_.begin(), buffer_.end(), found);
}

void hosts::load(result_handler handler)
{
    dispatch_.ordered(&hosts::do_load,
        this, file_path_.string(), handler);
}

void hosts::do_load(const path& file_path, result_handler handler)
{
    if (disabled_)
    {
        handler(error::success);
        return;
    }

    bc::ifstream file(file_path.string());
    if (file.bad())
    {
        handler(error::file_system);
        return;
    }

    // Formerly each address was randomly-queued for insert here.
    std::string line;
    while (std::getline(file, line))
    {
        config::authority host(line);
        if (host.port() != 0)
            buffer_.push_back(host.to_network_address());
    }

    handler(error::success);
}

void hosts::save(result_handler handler)
{
    dispatch_.ordered(&hosts::do_save,
        this, file_path_.string(), handler);
}

void hosts::do_save(const path& path, result_handler handler)
{
    if (disabled_)
    {
        handler(error::success);
        return;
    }

    bc::ofstream file(path.string());
    if (file.bad())
    {
        handler(error::file_system);
        return;
    }

    for (const auto& entry: buffer_)
        file << config::authority(entry) << std::endl;

    handler(error::success);
}

void hosts::remove(const address& host, result_handler handler)
{
    dispatch_.unordered(&hosts::do_remove,
        this, host, handler);
}

void hosts::do_remove(const address& host, result_handler handler)
{
    auto it = find(host);
    if (it == buffer_.end())
    {
        handler(error::not_found);
        return;
    }

    buffer_.erase(it);
    handler(error::success);
}

void hosts::store(const address& host, result_handler handler)
{
    dispatch_.unordered(&hosts::do_store,
        this, host, handler);
}

void hosts::store(const address::list& hosts, result_handler handler)
{
    // We disperse here to allow other addresses messages to interleave hosts.
    dispatch_.disperse(hosts, "hosts", handler,
        &hosts::do_store, this);
}

void hosts::do_store(const address& host, result_handler handler)
{
    if (!host.is_valid())
        log::debug(LOG_PROTOCOL)
            << "Invalid host address from peer";
    else if (find(host) != buffer_.end())
        log::debug(LOG_PROTOCOL)
            << "Redundant host address from peer";
    else
        buffer_.push_back(host);

    // We don't treat invalid address as an error, just log it.
    handler(error::success);
}

void hosts::fetch(fetch_handler handler)
{
    dispatch_.unordered(&hosts::do_fetch,
        this, handler);
}

void hosts::do_fetch(fetch_handler handler)
{
    if (buffer_.empty())
    {
        handler(error::not_found, address());
        return;
    }

    // Randomly select an address from the buffer.
    const auto index = static_cast<size_t>(pseudo_random() % buffer_.size());
    handler(error::success, buffer_[index]);
}

void hosts::count(count_handler handler)
{
    dispatch_.ordered(&hosts::do_count,
        this, handler);
}

void hosts::do_count(count_handler handler)
{
    handler(buffer_.size());
}

} // namespace network
} // namespace libbitcoin
