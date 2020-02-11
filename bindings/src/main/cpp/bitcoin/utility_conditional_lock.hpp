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
#ifndef LIBBITCOIN__UTILITY_CONDITIONAL_LOCK_HPP
#define LIBBITCOIN__UTILITY_CONDITIONAL_LOCK_HPP

//#include <memory>
//#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/conditional_lock.hpp>
#include <bitcoin/utility_mutex_ptr.hpp>

namespace libbitcoin {
namespace api {

class BC_API utility_conditional_lock
{
public:
    /// Conditional lock using internally-managed mutex pointer.
    utility_conditional_lock(bool lock);

    /// Conditional lock using parameterized mutex pointer (may be null).
    utility_conditional_lock(utility_mutex_ptr mutex_ptr);

    /// Unlock.
    virtual ~utility_conditional_lock();

    libbitcoin::conditional_lock* getValue() {
        return value_;
    }

    void setValue(libbitcoin::conditional_lock* value) {
        value_ = value;
    }
private:
    libbitcoin::conditional_lock* value_;
//    const std::shared_ptr<shared_mutex> mutex_ptr_;
};

}
} // namespace libbitcoin

#endif
