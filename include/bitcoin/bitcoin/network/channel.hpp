/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_CHANNEL_HPP
#define LIBBITCOIN_CHANNEL_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/network/authority.hpp>
#include <bitcoin/bitcoin/network/channel_proxy.hpp>
#include <bitcoin/bitcoin/utility/logger.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace network {
    
class channel;

// TODO: move channel_ptr into channel as public type (interface break).
typedef std::shared_ptr<channel> channel_ptr;

class BC_API channel
{
public:
    // TODO: move into channel_proxy (interface break).
    typedef std::shared_ptr<channel_proxy> channel_proxy_ptr;

    channel(channel_proxy_ptr proxy);
    ~channel();

    /// This class is not copyable.
    channel(const channel&) = delete;
    void operator=(const channel&) = delete;

    void stop() const;
    bool stopped() const;
    authority address() const;
    void reset_revival();
    void set_revival_handler(channel_proxy::revivial_handler handler);

    template <typename Message>
    void send(const Message& packet, channel_proxy::send_handler handle_send)
    {
        const auto proxy = weak_proxy_.lock();
        if (proxy)
            proxy->send(packet, handle_send);
        else
            handle_send(error::service_stopped);
    }

    void send_raw(const header_type& packet_header,
        const data_chunk& payload, channel_proxy::send_handler handle_send);
    void subscribe_version(
        channel_proxy::receive_version_handler handle_receive);
    void subscribe_verack(
        channel_proxy::receive_verack_handler handle_receive);
    void subscribe_address(
        channel_proxy::receive_address_handler handle_receive);
    void subscribe_get_address(
        channel_proxy::receive_get_address_handler handle_receive);
    void subscribe_inventory(
        channel_proxy::receive_inventory_handler handle_receive);
    void subscribe_get_data(
        channel_proxy::receive_get_data_handler handle_receive);
    void subscribe_get_blocks(
        channel_proxy::receive_get_blocks_handler handle_receive);
    void subscribe_transaction(
        channel_proxy::receive_transaction_handler handle_receive);
    void subscribe_block(
        channel_proxy::receive_block_handler handle_receive);
    void subscribe_raw(
        channel_proxy::receive_raw_handler handle_receive);
    void subscribe_stop(
        channel_proxy::stop_handler handle_stop);

private:
    std::weak_ptr<channel_proxy> weak_proxy_;
};

} // namespace network
} // namespace libbitcoin

#endif

