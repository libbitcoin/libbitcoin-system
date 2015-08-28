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
#ifndef LIBBITCOIN_NETWORK_CHANNEL_HPP
#define LIBBITCOIN_NETWORK_CHANNEL_HPP

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/chain/block.hpp>
#include <bitcoin/bitcoin/chain/transaction.hpp>
#include <bitcoin/bitcoin/messages.hpp>
#include <bitcoin/bitcoin/math/checksum.hpp>
#include <bitcoin/bitcoin/network/proxy.hpp>
//#include <bitcoin/bitcoin/network/network.hpp>
#include <bitcoin/bitcoin/network/asio.hpp>
#include <bitcoin/bitcoin/network/shared_const_buffer.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/logger.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {


class BC_API channel
  : public track<channel>
{
public:
    typedef std::shared_ptr<channel> ptr;

    channel(proxy::ptr proxy);
    channel(threadpool& pool, asio::socket_ptr socket, const timeout& timeouts);
    ~channel();

    /// This class is not copyable.
    channel(const channel&) = delete;
    void operator=(const channel&) = delete;

    void start();
    void stop(const code& ec);
    uint64_t nonce() const;
    void set_nonce(uint64_t nonce);
    config::authority address() const;
    void reset_revival();
    void set_revival_handler(proxy::revival_handler handler);

    DECLARE_CHANNEL_MESSAGE_SUBSCRIBERS();
    //void subscribe_transaction(proxy::receive_transaction_handler handler);
    //void subscribe_block(proxy::receive_block_handler handler);
    void subscribe_raw(proxy::receive_raw_handler handle);
    void subscribe_stop(proxy::stop_handler handle);

    template <typename Message>
    void send(const Message& packet, proxy::send_handler handler)
    {
        proxy_->send(packet, handler);
    }

    void send_raw(const message::header& packet_header,
        const data_chunk& payload, proxy::send_handler handler);

private:

    proxy::ptr proxy_;
    uint64_t nonce_;
};

} // namespace network
} // namespace libbitcoin

#endif
