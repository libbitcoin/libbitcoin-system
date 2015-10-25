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
#ifndef LIBBITCOIN_NETWORK_PROTOCOL_VERSION_HPP
#define LIBBITCOIN_NETWORK_PROTOCOL_VERSION_HPP

#include <cstddef>
#include <memory>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/message/verack.hpp>
#include <bitcoin/bitcoin/message/version.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/p2p.hpp>
#include <bitcoin/bitcoin/network/protocol_base.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

class BC_API protocol_version
  : public protocol_base<protocol_version>, track<protocol_version>
{
public:
    typedef std::shared_ptr<protocol_version> ptr;

    protocol_version(threadpool& pool, p2p&, const settings& settings,
        channel::ptr channel, size_t height, completion_handler handler);

    void start() override;

private:
    static completion_handler synchronizer_factory(completion_handler handler);
    static message::version template_factory(channel::ptr channel,
        const settings& settings, size_t height);

    void handle_receive_version(const code& ec,
        const message::version& version);
    void handle_receive_verack(const code& ec, const message::verack&);
    void handle_version_sent(const code& ec);
    void handle_verack_sent(const code& ec);

    static const message::version template_;

    size_t height_;
    message::version version_;
};

} // namespace network
} // namespace libbitcoin

#endif

