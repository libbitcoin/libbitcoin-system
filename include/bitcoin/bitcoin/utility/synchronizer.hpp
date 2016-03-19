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
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/thread.hpp>

namespace libbitcoin {

// It is not possible for this class to produce a deadlock.

template <typename Handler>
class synchronizer
{
public:
    synchronizer(Handler handler, size_t clearance_count,
        const std::string& name, bool suppress_errors)
      : handler_(handler),
        clearance_count_(clearance_count),
        name_(name),
        counter_(std::make_shared<std::size_t>(0)),
        mutex_(std::make_shared<shared_mutex>()),
        suppress_errors_(suppress_errors)
    {
    }

    template <typename... Args>
    void operator()(const code& ec, Args&&... args)
    {
        // Critical Section
        ///////////////////////////////////////////////////////////////////////
        mutex_->lock_upgrade();

        const auto initial_count = *counter_;
        BITCOIN_ASSERT(initial_count <= clearance_count_);

        if (initial_count == clearance_count_)
        {
            mutex_->unlock_upgrade();
            //-----------------------------------------------------------------
            return;
        }

        const auto complete = ec && !suppress_errors_;
        const auto count = complete ? clearance_count_ : initial_count + 1;
        const auto cleared = count == clearance_count_;

        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        mutex_->unlock_upgrade_and_lock();
        (*counter_) = count;
        mutex_->unlock();
        ///////////////////////////////////////////////////////////////////////

        if (cleared)
        {
            // If not suppressing errors pass the last result code.
            // This is useful when clearance count is one.
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
    std::shared_ptr<upgrade_mutex> mutex_;
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
