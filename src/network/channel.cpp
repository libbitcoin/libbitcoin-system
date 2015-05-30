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
#include <bitcoin/bitcoin/network/channel.hpp>

#include <bitcoin/bitcoin/network/authority.hpp>
#include <bitcoin/bitcoin/network/channel_proxy.hpp>
#include <bitcoin/bitcoin/primitives.hpp>

namespace libbitcoin {
namespace network {

channel::channel(channel_proxy_ptr proxy)
  : weak_proxy_(proxy)
{
}

channel::~channel()
{
    stop();
}

void channel::stop() const
{
    const auto proxy = weak_proxy_.lock();
    if (proxy)
        proxy->stop();
}

bool channel::stopped() const
{
    const auto proxy = weak_proxy_.lock();
    if (proxy)
        return proxy->stopped();

    return true;
}

authority channel::address() const
{
    const auto proxy = weak_proxy_.lock();
    if (proxy)
        return proxy->address();

    return address();
}

void channel::send_raw(const header_type& packet_header,
    const data_chunk& payload, channel_proxy::send_handler handle_send)
{
    const auto proxy = weak_proxy_.lock();
    if (proxy)
        proxy->send_raw(packet_header, payload, handle_send);
    else
        handle_send(error::service_stopped);
}

void channel::subscribe_version(
    channel_proxy::receive_version_handler handle_receive)
{
    const auto proxy = weak_proxy_.lock();
    if (proxy)
        proxy->subscribe_version(handle_receive);
    else
        handle_receive(error::service_stopped, version_type());
}

void channel::subscribe_verack(
    channel_proxy::receive_verack_handler handle_receive)
{
    const auto proxy = weak_proxy_.lock();
    if (proxy)
        proxy->subscribe_verack(handle_receive);
    else
        handle_receive(error::service_stopped, verack_type());
}

void channel::subscribe_address(
    channel_proxy::receive_address_handler handle_receive)
{
    const auto proxy = weak_proxy_.lock();
    if (proxy)
        proxy->subscribe_address(handle_receive);
    else
        handle_receive(error::service_stopped, address_type());
}

void channel::subscribe_get_address(
    channel_proxy::receive_get_address_handler handle_receive)
{
    const auto proxy = weak_proxy_.lock();
    if (proxy)
        proxy->subscribe_get_address(handle_receive);
    else
        handle_receive(error::service_stopped, get_address_type());
}

void channel::subscribe_inventory(
    channel_proxy::receive_inventory_handler handle_receive)
{
    const auto proxy = weak_proxy_.lock();
    if (proxy)
        proxy->subscribe_inventory(handle_receive);
    else
        handle_receive(error::service_stopped, inventory_type());
}

void channel::subscribe_get_data(
    channel_proxy::receive_get_data_handler handle_receive)
{
    const auto proxy = weak_proxy_.lock();
    if (proxy)
        proxy->subscribe_get_data(handle_receive);
    else
        handle_receive(error::service_stopped, get_data_type());
}

void channel::subscribe_get_blocks(
    channel_proxy::receive_get_blocks_handler handle_receive)
{
    const auto proxy = weak_proxy_.lock();
    if (proxy)
        proxy->subscribe_get_blocks(handle_receive);
    else
        handle_receive(error::service_stopped, get_blocks_type());
}

void channel::subscribe_transaction(
    channel_proxy::receive_transaction_handler handle_receive)
{
    const auto proxy = weak_proxy_.lock();
    if (proxy)
        proxy->subscribe_transaction(handle_receive);
    else
        handle_receive(error::service_stopped, transaction_type());
}

void channel::subscribe_block(
    channel_proxy::receive_block_handler handle_receive)
{
    const auto proxy = weak_proxy_.lock();
    if (proxy)
        proxy->subscribe_block(handle_receive);
    else
        handle_receive(error::service_stopped, block_type());
}

void channel::subscribe_raw(
    channel_proxy::receive_raw_handler handle_receive)
{
    const auto proxy = weak_proxy_.lock();
    if (proxy)
        proxy->subscribe_raw(handle_receive);
    else
        handle_receive(error::service_stopped, header_type(), data_chunk());
}

void channel::subscribe_stop(
    channel_proxy::stop_handler handle_stop)
{
    const auto proxy = weak_proxy_.lock();
    if (proxy)
        proxy->subscribe_stop(handle_stop);
    else
        handle_stop(error::service_stopped);
}

} // namespace network
} // namespace libbitcoin
