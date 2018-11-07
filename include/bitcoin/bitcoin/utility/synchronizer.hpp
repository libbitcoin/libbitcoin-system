/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_SYNCHRONIZER_HPP
#define LIBBITCOIN_SYNCHRONIZER_HPP

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/thread.hpp>

namespace libbitcoin {

enum class synchronizer_terminate
{
    /// Terminate on first error or count.
    /// Return code on first error, otherwise success.
    on_error,

    /// Terminate first success (priority) or count.
    /// Return success on first success, otherwise operation_failed.
    on_success,

    /// Terminate on count only.
    /// Return success once count is reached (always).
    on_count
};

template <typename Handler>
class synchronizer
{
public:
    synchronizer(Handler&& handler, size_t clearance_count,
        const std::string& name, synchronizer_terminate mode)
      : handler_(std::forward<Handler>(handler)),
        name_(name),
        clearance_count_(clearance_count),
        counter_(std::make_shared<size_t>(0)),
        mutex_(std::make_shared<upgrade_mutex>()),
        terminate_(mode)
    {
    }

    // Determine if the code is cause for termination.
    bool complete(const code& ec)
    {
        switch (terminate_)
        {
            case synchronizer_terminate::on_error:
                return !!ec;
            case synchronizer_terminate::on_success:
                return !ec;
            case synchronizer_terminate::on_count:
                return false;
            default:
                throw std::invalid_argument("mode");
        }
    }

    // Assuming we are terminating, generate the proper result code.
    code result(const code& ec)
    {
        switch (terminate_)
        {
            case synchronizer_terminate::on_error:
                return ec ? ec : error::success;
            case synchronizer_terminate::on_success:
                return !ec ? error::success : ec;
            case synchronizer_terminate::on_count:
                return error::success;
            default:
                throw std::invalid_argument("mode");
        }
    }

    template <typename... Args>
    void operator()(const code& ec, Args&&... args)
    {
        // Critical Section
        ///////////////////////////////////////////////////////////////////////
        mutex_->lock_upgrade();

        const auto initial_count = *counter_;
        BITCOIN_ASSERT(initial_count <= clearance_count_);

        // Another handler cleared this and shortcircuited the count, ignore.
        if (initial_count == clearance_count_)
        {
            mutex_->unlock_upgrade();
            //-----------------------------------------------------------------
            return;
        }

        const auto count = complete(ec) ? clearance_count_ : initial_count + 1;
        const auto cleared = count == clearance_count_;

        mutex_->unlock_upgrade_and_lock();
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        (*counter_) = count;

        mutex_->unlock();
        ///////////////////////////////////////////////////////////////////////

        if (cleared)
            handler_(result(ec), std::forward<Args>(args)...);
    }

private:
    typedef typename std::decay<Handler>::type decay_handler;

    decay_handler handler_;
    const std::string name_;
    const size_t clearance_count_;
    const synchronizer_terminate terminate_;

    // We use pointer to reference the same value/mutex across instance copies.
    std::shared_ptr<size_t> counter_;
    upgrade_mutex_ptr mutex_;
};

template <typename Handler>
synchronizer<Handler> synchronize(Handler&& handler, size_t clearance_count,
    const std::string& name, synchronizer_terminate mode=
        synchronizer_terminate::on_error)
{
    return synchronizer<Handler>(std::forward<Handler>(handler),
        clearance_count, name, mode);
}

} // namespace libbitcoin

#endif
