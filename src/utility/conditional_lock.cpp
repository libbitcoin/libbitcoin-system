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
#include <bitcoin/system/utility/conditional_lock.hpp>

#include <memory>
#include <bitcoin/system/utility/thread.hpp>

namespace libbitcoin {
namespace system {

conditional_lock::conditional_lock(bool lock)
  : conditional_lock(lock ? std::make_shared<shared_mutex>() : nullptr)
{
}

conditional_lock::conditional_lock(std::shared_ptr<shared_mutex> mutex_ptr)
  : mutex_ptr_(mutex_ptr)
{
    if (mutex_ptr_)
        mutex_ptr->lock();
}

conditional_lock::~conditional_lock()
{
    if (mutex_ptr_)
        mutex_ptr_->unlock();
}

} // namespace system
} // namespace libbitcoin
