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
#include <mutex>
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

/// Pool of active connections, thread and lock safe.
class BC_API connections
  : public std::enable_shared_from_this<connections>
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

    /// Completion handler returns operation_failed if any channel send failed.
    template <typename Message>
    void broadcast(const Message& message, channel_handler handle_channel,
        result_handler handle_complete)
    {
        const auto method = &connections::do_broadcast<Message>;
        dispatch_.concurrent(method, shared_from_this(), message,
            handle_channel, handle_complete);
    }

    void stop(const code& ec);
    void count(count_handler handler);
    void store(channel::ptr channel, result_handler handler);
    void remove(channel::ptr channel, result_handler handler);
    void exists(const authority& authority, truth_handler handler);

private:
    typedef std::vector<channel::ptr> list;

    template <typename Message>
    void do_broadcast(const Message& message, channel_handler handle_channel,
        result_handler handle_complete)
    {
        // The list is copied, which protects the iteration without a lock.
        auto channels = safe_copy();
        const auto size = channels.size();
        const auto counter = std::make_shared<std::atomic<size_t>>(size);
        const auto result = std::make_shared<std::atomic<error::error_code_t>>(
            error::success);

        for (const auto channel: channels)
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

    list safe_copy();
    size_t safe_count();
    bool safe_store(channel::ptr channel);
    bool safe_remove(channel::ptr channel);
    bool safe_exists(const authority& address);

    list channels_;
    std::mutex mutex_;
    dispatcher dispatch_;
};

} // namespace network
} // namespace libbitcoin

#endif
