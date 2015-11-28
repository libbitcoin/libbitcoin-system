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
#ifndef LIBBITCOIN_NETWORK_CONNECTIONS_HPP
#define LIBBITCOIN_NETWORK_CONNECTIONS_HPP

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/synchronizer.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

/// Pool of active connections, thread safe.
class BC_API connections
{
public:
    typedef config::authority authority;
    typedef std::function<void(bool)> truth_handler;
    typedef std::function<void(size_t)> count_handler;
    typedef std::function<void(const code&)> result_handler;
    typedef std::function<void(const code&, channel::ptr)> channel_handler;

    /// Construct an instance.
    connections(threadpool& pool);

    /// Validate connections stopped.
    ~connections();

    /// This class is not copyable.
    connections(const connections&) = delete;
    void operator=(const connections&) = delete;

    /// handle_complete returns operation_failed if send to any channel failed.
    template <typename Message>
    void broadcast(const Message& message, channel_handler handle_channel,
        result_handler handle_complete) const
    {
        const auto size = buffer_.size();
        const auto counter = std::make_shared<std::atomic<size_t>>(size);
        const auto result = std::make_shared<std::atomic<error::error_code_t>>(
            error::success);

        for (const auto channel: buffer_)
        {
            const auto handle_send = [=](const code ec)
            {
                handle_channel(ec, channel);

                if (ec)
                    result->store(error::operation_failed);

                if (counter->fetch_sub(1) == 1)
                    handle_complete(result->load());
            };

            channel->send(message, handle_send);
        }
    }

    void stop(const code& ec);
    void count(count_handler handler);
    void store(const channel::ptr& channel, result_handler handler);
    void remove(const channel::ptr& channel, result_handler handler);
    void exists(const authority& authority, truth_handler handler);

private:
    typedef std::vector<channel::ptr> list;
    typedef list::const_iterator iterator;

    iterator find(const uint64_t nonce) const;
    iterator find(const channel::ptr& channel) const;
    iterator find(const authority& authority) const;

    void do_stop(const code& ec);
    void do_count(count_handler handler) const;
    void do_store(const channel::ptr& channel, result_handler handler);
    void do_remove(const channel::ptr& channel, result_handler handler);
    void do_exists(const authority& authority, truth_handler handler) const;

    list buffer_;
    dispatcher dispatch_;
};

} // namespace network
} // namespace libbitcoin

#endif
