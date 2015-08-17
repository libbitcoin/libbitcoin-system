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
#ifndef LIBBITCOIN_PROTOCOL_ADDRESS_HPP
#define LIBBITCOIN_PROTOCOL_ADDRESS_HPP

#include <memory>
#include <system_error>
#include <boost/system/error_code.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/hosts.hpp>
#include <bitcoin/bitcoin/primitives.hpp>
#include <bitcoin/bitcoin/utility/sequencer.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

class protocol_address;
typedef std::shared_ptr<protocol_address> protocol_address_ptr;

class BC_API protocol_address
  : public std::enable_shared_from_this<protocol_address>
{
public:
    typedef std::function<void(const std::error_code&)> completion_handler;

    protocol_address(channel_ptr node, threadpool& pool, hosts& hosts_,
        const network_address_type& self);

    /// This class is not copyable.
    protocol_address(const protocol_address&) = delete;
    void operator=(const protocol_address&) = delete;

    void start();
    //void start(completion_handler handle_complete, const timeout& timeouts);

private:
    //bool stopped() const;
    //void stop(const std::error_code& ec);

    //void clear_timer();
    //void reset_timer();
    //void set_timer(const boost::posix_time::time_duration& timeout);
    //void handle_timer(const boost::system::error_code& ec);

    bool is_self_defined() const;
    network_address_type get_address() const;

    void handle_receive_address(const std::error_code& ec,
        const address_type& address);
    void handle_send_address(const std::error_code& ec) const;
    void handle_send_get_address(const std::error_code& ec) const;
    void handle_store_address(const std::error_code& ec) const;

    channel_ptr node_;
    //const timeout& timeouts_;
    //boost::asio::deadline_timer timer_;
    sequencer sequence_;
    //bool stopped_;
    hosts& hosts_;
    const address_type self_;
};

} // namespace network
} // namespace libbitcoin

#endif
