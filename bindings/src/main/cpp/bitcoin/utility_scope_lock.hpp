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
 * GNU Affero General Public LicensM for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN__UTILITY_SCOPE_LOCK_HPP
#define LIBBITCOIN__UTILITY_SCOPE_LOCK_HPP

//#include <memory>
//#include <bitcoin/bitcoin/define.hpp>
//#include <bitcoin/bitcoin/utility/thread.hpp>
#include <bitcoin/bitcoin/utility/scope_lock.hpp>
#include <bitcoin/utility_shared_mutex.hpp>

namespace libbitcoin {
namespace api {

/// This class is thread safe.
/// Reserve exclusive access to a resource while this object is in scope.
class BC_API utility_scope_lock
{
public:
//    typedef std::shared_ptr<scope_lock> ptr;

    /// Lock using the specified mutex reference.
    utility_scope_lock(utility_shared_mutex& mutex);

    /// Unlock.
    virtual ~utility_scope_lock();

    scope_lock* getValue() {
		return value_;
	}

	void setValue(scope_lock* value) {
		value_ = value;
	}
private:
	scope_lock* value_;

//private:
//    shared_mutex& mutex_;
};

} // namespace api
} // namespace libbitcoin

#endif
