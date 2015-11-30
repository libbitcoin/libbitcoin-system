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
#ifndef LIBBITCOIN_SYNCHRONIZER_HPP
#define LIBBITCOIN_SYNCHRONIZER_HPP

#include <cstddef>
#include <memory>
#include <mutex>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>

namespace libbitcoin {

// It is not possible for this class to produce a deadlock.

template <typename Handler>
class synchronizer
{
public:
    synchronizer(Handler handler, size_t clearance_count,
        const std::string& name, bool suppress_errors=false)
      : handler_(handler),
        clearance_count_(clearance_count),
        name_(name),
        counter_(std::make_shared<std::size_t>(0)),
        counter_mutex_(std::make_shared<std::mutex>()),
        suppress_errors_(suppress_errors)
    {
    }

    template <typename... Args>
    void operator()(const code& ec, Args... args)
    {
        auto cleared = false;

        // Critical Section
        ///////////////////////////////////////////////////////////////////////
        if (true)
        {
            std::lock_guard<std::mutex> lock(*counter_mutex_);

            BITCOIN_ASSERT(*counter_ <= clearance_count_);
            if (*counter_ == clearance_count_)
            {
                //log::debug(LOG_NETWORK)
                //    << "Synchronizing [" << name_ << "] > "
                //    << clearance_count_ << " (ignored)";
                return;
            }

            ++(*counter_);

            if (ec)
            {
                //log::debug(LOG_NETWORK)
                //    << "Synchronizing [" << name_ << "] " << *counter_
                //    << "/" << clearance_count_ << " " << ec.message()
                //    << (suppress_errors_ ? " (suppressed)" : "");

                if (!suppress_errors_)
                    *counter_ = clearance_count_;
            }
            else
            {
                //log::debug(LOG_NETWORK)
                //    << "Synchronizing [" << name_ << "] " << *counter_ << "/"
                //    << clearance_count_;
            }

            cleared = (*counter_ == clearance_count_);
        }
        ///////////////////////////////////////////////////////////////////////

        // Use execute flag to keep this log task out of the critical section.
        if (cleared)
        {
            const auto result = suppress_errors_ ? error::success : ec;
            handler_(result, std::forward<Args>(args)...);
        }
    }

private:
    Handler handler_;
    size_t clearance_count_;
    const std::string name_;
    const bool suppress_errors_;

    // We use pointer to reference the same value/mutex across instance copies.
    std::shared_ptr<size_t> counter_;
    std::shared_ptr<std::mutex> counter_mutex_;
};

template <typename Handler>
synchronizer<Handler> synchronize(Handler handler, size_t clearance_count,
    const std::string& name, bool suppress_errors=false)
{
    return synchronizer<Handler>(handler, clearance_count, name,
        suppress_errors);
}

} // libbitcoin

#endif
