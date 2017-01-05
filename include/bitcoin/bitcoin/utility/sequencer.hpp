/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_SEQUENCER_HPP
#define LIBBITCOIN_SEQUENCER_HPP

#include <functional>
#include <memory>
#include <queue>
#include <utility>
#include <bitcoin/bitcoin/utility/asio.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/enable_shared_from_base.hpp>
#include <bitcoin/bitcoin/utility/thread.hpp>
////#include <bitcoin/bitcoin/utility/track.hpp>

namespace libbitcoin {

class sequencer
  : public enable_shared_from_base<sequencer>
    /*, track<sequencer>*/
{
public:
    typedef std::shared_ptr<sequencer> ptr;
    typedef std::function<void()> action;

    sequencer(asio::service& service)
      : service_(service), executing_(false)
    {
    }

    ~sequencer()
    {
        BITCOIN_ASSERT_MSG(actions_.empty(), "sequencer not cleared");
    }

    void lock(action&& handler)
    {
        auto post = false;

        // Critical Section
        ///////////////////////////////////////////////////////////////////////
        mutex_.lock();

        if (executing_)
        {
            actions_.emplace(std::move(handler));
        }
        else
        {
            post = true;
            executing_ = true;
        }

        mutex_.unlock();
        ///////////////////////////////////////////////////////////////////////

        if (post)
            service_.post(std::move(handler));
    }

    void unlock()
    {
        action handler;

        // Critical Section
        ///////////////////////////////////////////////////////////////////////
        mutex_.lock();

        DEBUG_ONLY(const auto executing = executing_;)

        if (actions_.empty())
        {
            executing_ = false;
        }
        else
        {
            executing_ = true;
            std::swap(handler, actions_.front());
            actions_.pop();
        }

        mutex_.unlock();
        ///////////////////////////////////////////////////////////////////////

        BITCOIN_ASSERT_MSG(executing, "called unlock but sequence not locked");

        if (handler)
            service_.post(std::move(handler));
    }

private:
    // This is thread safe.
    asio::service& service_;

    // These are protected by mutex.
    bool executing_;
    std::queue<action> actions_;
    mutable upgrade_mutex mutex_;
};

} // namespace libbitcoin

////#include <bitcoin/bitcoin/impl/utility/sequencer.ipp>

#endif
