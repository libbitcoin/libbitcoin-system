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
#include <bitcoin/bitcoin/network/acceptor.hpp>

#include <algorithm>
#include <cstdint>
#include <functional>
#include <iostream>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/asio.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/proxy.hpp>
#include <bitcoin/bitcoin/network/timeout.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

INITIALIZE_TRACK(bc::network::acceptor);

namespace libbitcoin {
namespace network {

using std::placeholders::_1;

acceptor::acceptor(threadpool& pool, asio::acceptor_ptr accept,
    uint32_t network_magic, const timeout& timeouts)
  : pool_(pool), magic_(network_magic), timeouts_(timeouts), asio_acceptor_(accept),
    CONSTRUCT_TRACK(acceptor, LOG_NETWORK)
{
}

void acceptor::accept(accept_handler handle_accept)
{
    const auto socket = std::make_shared<asio::socket>(pool_.service());

    asio_acceptor_->async_accept(*socket,
        std::bind(&acceptor::create_channel,
            shared_from_this(), _1, socket, handle_accept));
}

void acceptor::create_channel(const boost_code& ec, asio::socket_ptr socket,
    accept_handler handle_accept)
{
    if (ec)
    {
        handle_accept(error::boost_to_error_code(ec), nullptr);
        return;
    }

    const auto peer = std::make_shared<channel>(pool_, socket, magic_,
        timeouts_);

    handle_accept(error::success, peer);
}

} // namespace network
} // namespace libbitcoin
