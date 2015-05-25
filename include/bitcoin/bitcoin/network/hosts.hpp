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
#include <bitcoin/bitcoin/primitives.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

class BC_API hosts
{
public:
    typedef std::function<void (const std::error_code&)> load_handler;
    typedef std::function<void (const std::error_code&)> save_handler;
    typedef std::function<void (const std::error_code&)> store_handler;
    typedef std::function<void (const std::error_code&)> remove_handler;

    typedef std::function<
        void (const std::error_code&, const network_address_type&)>
            fetch_address_handler;

    typedef std::function<void (const std::error_code&, size_t)>
        fetch_count_handler;

    hosts(threadpool& pool, const std::string& path, size_t capacity=1000);
    ~hosts();

    hosts(const hosts&) = delete;
    void operator=(const hosts&) = delete;

    void load(load_handler handle_load);
    void save(save_handler handle_save);

    void store(const network_address_type& address,
        store_handler handle_store);
    void remove(const network_address_type& address,
        remove_handler handle_remove);
    void fetch_address(fetch_address_handler handle_fetch);
    void fetch_count(fetch_count_handler handle_fetch);

    /// Deprecated, set hosts path on construct.
    hosts(threadpool& pool, size_t capacity=1000);
    void load(const std::string& path, load_handler handle_load);
    void save(const std::string& path, save_handler handle_save);

private:
    struct hosts_field
    {
        bool operator==(const hosts_field& other);
        ip_address_type ip;
        uint16_t port;
    };

    void do_load(const boost::filesystem::path& path,
        load_handler handle_load);
    void do_save(const boost::filesystem::path& path,
        save_handler handle_save);

    void do_remove(const network_address_type& address,
        remove_handler handle_remove);
    void do_fetch_address(fetch_address_handler handle_fetch_address);
    void do_fetch_count(fetch_count_handler handle_fetch);

    async_strand strand_;
    boost::filesystem::path hosts_path_;
    boost::circular_buffer<hosts_field> buffer_;
};

} // namespace network
} // namespace libbitcoin

#endif

