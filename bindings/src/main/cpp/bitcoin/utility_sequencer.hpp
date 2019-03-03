/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN__UTILITY_SEQUENCER_HPP
#define LIBBITCOIN__UTILITY_SEQUENCER_HPP

//#include <functional>
//#include <memory>
//#include <queue>
//#include <bitcoin/bitcoin/utility/asio.hpp>
//#include <bitcoin/bitcoin/utility/enable_shared_from_base.hpp>
//#include <bitcoin/bitcoin/utility/thread.hpp>
#include <bitcoin/bitcoin/utility/sequencer.hpp>
#include <utility_service.hpp>
#include <utility_action.hpp>


namespace libbitcoin {
namespace api {

class utility_sequencer
{
public:
//    typedef std::shared_ptr<sequencer> ptr;
//    typedef std::function<void()> action;

    utility_sequencer(utility_service& service);
    virtual ~utility_sequencer();

    void lock(utility_action&& handler);
    void unlock();

	sequencer getValue() {
		return value;
	}

	void setValue(sequencer value) {
		this->value = value;
	}
private:
	sequencer value;
//    // This is thread safe.
//    asio::service& service_;
//
//    // These are protected by mutex.
//    bool executing_;
//    std::queue<action> actions_;
//    mutable shared_mutex mutex_;
};

} // namespace api
} // namespace libbitcoin

#endif
