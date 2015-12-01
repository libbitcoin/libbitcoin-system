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
#include <utility>
#include <string>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/messages.hpp>
#include <bitcoin/bitcoin/math/checksum.hpp>
#include <bitcoin/bitcoin/network/proxy.hpp>
#include <bitcoin/bitcoin/utility/asio.hpp>
#include <bitcoin/bitcoin/network/network_settings.hpp>
#include <bitcoin/bitcoin/network/message_subscriber.hpp>
#include <bitcoin/bitcoin/network/shared_const_buffer.hpp>
#include <bitcoin/bitcoin/utility/atomic.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>
#include <bitcoin/bitcoin/utility/subscriber.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

/// A concrete proxy with timers and state, mostly thread safe.
class BC_API channel
  : public proxy, track<channel>
{
public:
    typedef std::shared_ptr<channel> ptr;
    typedef subscriber<const code&, ptr> channel_subscriber;

    /// Construct an instance.
    channel(threadpool& pool, asio::socket_ptr socket,
        const settings& settings);

    /// This class is not copyable.
    channel(const channel&) = delete;
    void operator=(const channel&) = delete;

    void start(result_handler handler) override;

    uint64_t nonce() const;
    virtual void set_nonce(uint64_t value);

    virtual const message::version& version() const;
    virtual void set_version(const message::version& value);

    virtual void reset_revival();
    virtual void set_revival_handler(result_handler handler);

    virtual bool located(const hash_digest& start,
        const hash_digest& stop) const;
    virtual void set_located(const hash_digest& start,
        const hash_digest& stop);

protected:
    virtual void handle_activity();
    virtual void handle_stopping();

private:
    void do_start(const code& ec, result_handler handler);

    void start_expiration();
    void handle_expiration(const code& ec);

    void start_inactivity();
    void handle_inactivity(const code& ec);

    void start_revival();
    void handle_revival(const code& ec);

    uint64_t nonce_;
    hash_digest located_start_;
    hash_digest located_stop_;
    message::version version_;
    deadline::ptr expiration_;
    deadline::ptr inactivity_;
    deadline::ptr revival_;

    // Deperecated.
    bc::atomic<result_handler> revival_handler_;
};

} // namespace network
} // namespace libbitcoin

#endif
