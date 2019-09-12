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
#include <bitcoin/system/utility/sequencer.hpp>

#include <utility>
#include <bitcoin/system/utility/asio.hpp>
#include <bitcoin/system/utility/assert.hpp>
#include <bitcoin/system/utility/thread.hpp>

namespace libbitcoin {
namespace system {

sequencer::sequencer(asio::service& service)
  : service_(service)
{
}

sequencer::~sequencer()
{
	/* First-handler would be a dummy one, even not popped from queue-- it's ok,
	 * and, if not actions_.size() <= 1, then we have valid handlers pending for
	 * execution; Assert it.
	 */
    BITCOIN_ASSERT_MSG(actions_.size() <= 1, "sequencer not cleared");
}

void sequencer::lock(action&& handler)
{

	/*
	 * Lock the mutex, check whether the queue is empty, if yes, then move the handler to io_service::post.
	 * Finally, move the handler to action_ queue as well-- moving the handler twice is fine in our case,
	 * because, if the handler is already posted to io_service, then what we are going to push is Just a
	 * dummy object which perhaps lost its essence and will be popped on this->unlock() call. If not
	 * posted to io_service, then what we have Just pushed is a valid work that is "pending" and will be executed
	 * on this->unlock() call.
	 */

	unique_lock lock{mutex_};
	if(actions_.empty())
		service_.post(std::move(handler));
	actions_.push(std::move(handler));

}

void sequencer::unlock()
{

    /*
     * Lock the mutex, get the current queue size, assert if the queue size is 0 else
     * pop the first handler which would be a dummy one, then check, do we have more handlers?
     * if yes, move the now-first-handler to io_service queue.
     */

    unique_lock lock{mutex_};
    auto current_size = actions_.size();

    BITCOIN_ASSERT_MSG(current_size != 0,"called unlock but sequence not locked");

    actions_.pop();
    if(current_size > 1)
    	service_.post(std::move(actions_.front()));

}

} // namespace system
} // namespace libbitcoin
