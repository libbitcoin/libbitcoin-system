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
#ifndef LIBBITCOIN_NETWORK_HOSTS_HPP
#define LIBBITCOIN_NETWORK_HOSTS_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <functional>
#include <vector>
#include <boost/circular_buffer.hpp>
#include <boost/filesystem.hpp>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/config/endpoint.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/message/network_address.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

// TODO: rename to host_pool for symmetry with mempool and txpool (break).
class BC_API hosts
{
public:
    typedef std::function<void(const code&)> load_handler;
    typedef std::function<void(const code&)> save_handler;
    typedef std::function<void(const code&)> store_handler;
    typedef std::function<void(const code&)> remove_handler;
    typedef std::function<void(const code&, size_t)> fetch_count_handler;
    typedef std::function<void(const code&, const message::network_address&)>
        fetch_address_handler;

    hosts(threadpool& pool,
        const boost::filesystem::path& file_path="hosts.cache",
        size_t capacity=1000);
    ~hosts();

    /// This class is not copyable.
    hosts(const hosts&) = delete;
    void operator=(const hosts&) = delete;

    size_t capacity();
    size_t size();
    void load(load_handler handle_load);
    void save(save_handler handle_save);
    void store(const message::network_address& address,
        store_handler handle_store);
    void store(const message::network_address::list& addresses,
        store_handler handle_store);
    void remove(const message::network_address& address,
        remove_handler handle_remove);
    void fetch_address(fetch_address_handler handle_fetch);
    void fetch_count(fetch_count_handler handle_fetch);

private:
    typedef boost::circular_buffer<message::network_address> list;
    typedef list::iterator iterator;

    bool exists(const message::network_address& address);
    iterator find(const message::network_address& address);

    void do_load(const boost::filesystem::path& path,
        load_handler handle_load);
    void do_save(const boost::filesystem::path& path,
        save_handler handle_save);
    void do_remove(const message::network_address& address,
        remove_handler handle_remove);
    void do_store(const message::network_address& address,
        store_handler handle_store);
    void do_fetch_address(fetch_address_handler handle_fetch_address);
    void do_fetch_count(fetch_count_handler handle_fetch);
    size_t select_random_host();

    list buffer_;
    dispatcher dispatch_;
    boost::filesystem::path file_path_;
};

} // namespace network
} // namespace libbitcoin

#endif

