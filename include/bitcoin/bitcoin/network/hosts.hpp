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
#ifndef LIBBITCOIN_HOSTS_HPP
#define LIBBITCOIN_HOSTS_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <functional>
#include <system_error>
#include <boost/circular_buffer.hpp>
#include <boost/filesystem.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/network/authority.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/primitives.hpp>
#include <bitcoin/bitcoin/utility/async_strand.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

// TODO: rename to host_pool for symmetry with mempool and txpool (break).
class BC_API hosts
{
public:
    typedef std::vector<authority> authority_list;
    typedef std::function<void (const std::error_code&)> load_handler;
    typedef std::function<void (const std::error_code&)> save_handler;
    typedef std::function<void (const std::error_code&)> store_handler;
    typedef std::function<void (const std::error_code&)> remove_handler;
    typedef std::function<void (const std::error_code&, size_t)>
        fetch_count_handler;
    typedef std::function<void (const std::error_code&,
        const network_address_type&)> fetch_address_handler;

    hosts(threadpool& pool, const boost::filesystem::path& file_path,
        size_t capacity=1000);
    ~hosts();

    /// This class is not copyable.
    hosts(const hosts&) = delete;
    void operator=(const hosts&) = delete;

    bool empty() const;
    size_t size() const;

    void load(load_handler handle_load);
    void save(save_handler handle_save);
    void store(const network_address_type& address,
        store_handler handle_store);
    void remove(const network_address_type& address,
        remove_handler handle_remove);
    void fetch_address(fetch_address_handler handle_fetch);
    void fetch_count(fetch_count_handler handle_fetch);

    /// Deprecated, require hosts path on construct.
    hosts(threadpool& pool, size_t capacity=1000);

    /// Deprecated, set hosts path on construct.
    void load(const std::string& path, load_handler handle_load);

    /// Deprecated, set hosts path on construct.
    void save(const std::string& path, save_handler handle_save);

private:
    struct ip_address
    {
        ip_address();
        ip_address(const std::string& line);
        ip_address(const network_address_type& host);
        bool operator==(const ip_address& other) const;

        ip_address_type ip;
        uint16_t port;
    };

    void do_load(const boost::filesystem::path& path,
        load_handler handle_load);
    void do_save(const boost::filesystem::path& path,
        save_handler handle_save);
    void do_remove(const network_address_type& address,
        remove_handler handle_remove);
    void do_store(const network_address_type& address,
        store_handler handle_store);
    void do_fetch_address(fetch_address_handler handle_fetch_address);
    void do_fetch_count(fetch_count_handler handle_fetch);
    size_t select_random_host();

    async_strand strand_;
    boost::filesystem::path file_path_;
    boost::circular_buffer<ip_address> buffer_;

    // Deprecated, remove when protocol::set_hosts_filename is removed.
    // This allows (deprecated) protocol::set_hosts_filename to set file_path_.
    friend class protocol;
};

} // namespace network
} // namespace libbitcoin

#endif

