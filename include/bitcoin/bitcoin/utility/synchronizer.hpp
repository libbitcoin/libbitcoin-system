/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see COPYING.
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
#include <system_error>
#include <bitcoin/bitcoin/utility/assert.hpp>

namespace libbitcoin {

template <typename Handler>
class synchronizer
{
public:
    synchronizer(Handler handler, size_t clearance_count,
        const std::string& name)
      : handler_(handler),
        clearance_count_(clearance_count),
        name_(name),
        counter_(std::make_shared<std::size_t>(0)),
        counter_mutex_(std::make_shared<std::mutex>())
    {
    }

    template <typename... Args>
    void operator()(const std::error_code& code, Args&&... args)
    {
        auto handle = false;

        // This requires a critical section, not just an atomic counter.
        {
            std::lock_guard<std::mutex> lock(*counter_mutex_);

            BITCOIN_ASSERT(*counter_ <= clearance_count_);
            if (*counter_ == clearance_count_)
            {
                ////log_debug(LOG_PROTOCOL)
                ////    << "Handled [" << name_ << "] > "
                ////    << clearance_count_ << " (ignored)";
                return;
            }

            ++(*counter_);

            if (code)
            {
                ////log_debug(LOG_PROTOCOL)
                ////    << "Handled [" << name_ << "] " << *counter_
                ////    << "/" << clearance_count_ << " " << code.message();

                // Stop because of failure.
                *counter_ = clearance_count_;
                handle = true;
            }
            else if (*counter_ == clearance_count_)
            {
                // Finished executing multiple async paths.
                handle = true;
            }

            if (!code)
            {
                ////log_debug(LOG_PROTOCOL)
                ////    << "Handled [" << name_ << "] " << *counter_ << "/"
                ////    << clearance_count_;
            }
        }

        // Keep this long-running call out of critical section.
        if (handle)
            handler_(code, std::forward<Args>(args)...);
    }

private:
    Handler handler_;
    size_t clearance_count_;
    const std::string name_;

    // We use pointer to reference the same value/mutex across instance copies.
    std::shared_ptr<size_t> counter_;
    std::shared_ptr<std::mutex> counter_mutex_;
};

} // libbitcoin

#endif
