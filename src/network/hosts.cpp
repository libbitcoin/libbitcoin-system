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
#include <bitcoin/bitcoin/utility/sequencer.hpp>
#include <bitcoin/bitcoin/utility/string.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {
    
using boost::format;
using boost::filesystem::path;

hosts::hosts(threadpool& pool, const path& file_path, size_t capacity)
  : strand_(pool), file_path_(file_path), buffer_(capacity)
{
}

hosts::hosts(threadpool& pool, size_t capacity)
  : hosts(pool, "hosts.cache", capacity)
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
    strand_.randomly_queue(
        std::bind(&hosts::do_load,
            this, file_path_.string(), handle_load));
}

void hosts::load(const std::string& path, load_handler handle_load)
{
    strand_.randomly_queue(
        std::bind(&hosts::do_load,
            this, path, handle_load));
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
        config::authority address(line);
        if (address.port() != 0)
        {
            const auto load_address = [this, address]()
            {
                buffer_.push_back(address);
            };

            strand_.randomly_queue(load_address);
        }
    }

    handle_load(error::success);
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
        handle_save(error::file_system);
        return;
    }

    for (const auto& entry: buffer_)
        file << entry << std::endl;

    handle_save(error::success);
}

void hosts::remove(const message::network_address& address,
    remove_handler handle_remove)
{
    strand_.randomly_queue(
        std::bind(&hosts::do_remove,
            this, address, handle_remove));
}

void hosts::do_remove(const message::network_address& address,
    remove_handler handle_remove)
{
    const config::authority host(address);
    const auto it = std::find(buffer_.begin(), buffer_.end(), host);

    if (it == buffer_.end())
    {
        handle_remove(error::not_found);
        return;
    }

    buffer_.erase(it);
    handle_remove(error::success);
}

void hosts::store(const message::network_address& address,
    store_handler handle_store)
{
    if (address.port == 0)
        return;

    strand_.randomly_queue(
        std::bind(&hosts::do_store,
            this, address, handle_store));
}

void hosts::do_store(const message::network_address& address,
    store_handler handle_store)
{
    buffer_.push_back(config::authority(address));
    handle_store(error::success);
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
        handle_fetch(error::not_found, message::network_address());
        return;
    }

    // Randomly select a host from the buffer.
    const auto index = static_cast<size_t>(pseudo_random() % buffer_.size());
    handle_fetch(error::success, buffer_[index].to_network_address());
}

void hosts::fetch_count(fetch_count_handler handle_fetch)
{
    strand_.randomly_queue(
        std::bind(&hosts::do_fetch_count,
            this, handle_fetch));
}

void hosts::do_fetch_count(fetch_count_handler handle_fetch)
{
    handle_fetch(error::success, buffer_.size());
}

} // namespace network
} // namespace libbitcoin
