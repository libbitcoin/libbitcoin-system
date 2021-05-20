/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CONCURRENCY_PENDING_IPP
#define LIBBITCOIN_SYSTEM_CONCURRENCY_PENDING_IPP

#include <algorithm>
#include <cstddef>
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/error.hpp>
#include <bitcoin/system/utility/thread.hpp>

namespace libbitcoin {
namespace system {

template <class Element>
pending<Element>::pending(size_t initial_capacity)
  : stopped_(false)
{
    elements_.reserve(initial_capacity);
}

template <class Element>
pending<Element>::~pending()
{
    ////BITCOIN_ASSERT_MSG(elements_.empty(), "Pending collection not cleared.");
}

template <class Element>
typename pending<Element>::elements pending<Element>::collection() const
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    shared_lock lock(mutex_);

    return elements_;
    ///////////////////////////////////////////////////////////////////////////
}

template <class Element>
size_t pending<Element>::size() const
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    shared_lock lock(mutex_);

    return elements_.size();
    ///////////////////////////////////////////////////////////////////////////
}

template <class Element>
bool pending<Element>::exists(finder match) const
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    shared_lock lock(mutex_);

    return std::find_if(elements_.begin(), elements_.end(), match) !=
        elements_.end();
    ///////////////////////////////////////////////////////////////////////////
}

template <class Element>
code pending<Element>::store(element_ptr element)
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    mutex_.lock_upgrade();

    if (stopped_)
    {
        mutex_.unlock_upgrade();
        //---------------------------------------------------------------------
        return error::service_stopped;
    }

    mutex_.unlock_upgrade_and_lock();
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    elements_.push_back(element);

    mutex_.unlock();
    ///////////////////////////////////////////////////////////////////////////

    return error::success;
}

template <class Element>
code pending<Element>::store(element_ptr element, finder match)
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    mutex_.lock_upgrade();

    const auto stopped = stopped_.load();

    if (!stopped)
    {
        if (std::find_if(elements_.begin(), elements_.end(), match) ==
            elements_.end())
        {
            mutex_.unlock_upgrade_and_lock();
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            elements_.push_back(element);
            //-----------------------------------------------------------------
            mutex_.unlock();
            return error::success;
        }
    }

    mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////

    // Stopped and found are the only ways to get here.
    return stopped ? error::service_stopped : error::address_in_use;
}

template <class Element>
void pending<Element>::remove(element_ptr element)
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    mutex_.lock_upgrade();

    auto it = std::find(elements_.begin(), elements_.end(), element);

    if (it != elements_.end())
    {
        mutex_.unlock_upgrade_and_lock();
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        elements_.erase(it);
        //---------------------------------------------------------------------
        mutex_.unlock();
        return;
    }

    mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////
}

// This is idempotent.
template <class Element>
void pending<Element>::stop(const code& ec)
{
    elements copy;

    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    mutex_.lock_upgrade();

    if (!stopped_)
    {
        mutex_.unlock_upgrade_and_lock();
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        stopped_ = true;
        //---------------------------------------------------------------------
        mutex_.unlock_and_lock_upgrade();

        // Once stopped list cannot increase, but copy to escape lock.
        copy = elements_;
    }

    mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////

    for (const auto element: copy)
        element->stop(ec);
}

template <class Element>
void pending<Element>::close()
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    unique_lock lock(mutex_);

    // Close should block until element has freed all resources.
    for (auto element: elements_)
        element->close();

    elements_.clear();
    ///////////////////////////////////////////////////////////////////////////
}

} // namespace system
} // namespace libbitcoin

#endif
