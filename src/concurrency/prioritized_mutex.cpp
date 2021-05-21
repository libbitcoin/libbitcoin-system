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
#include <bitcoin/system/concurrency/prioritized_mutex.hpp>

namespace libbitcoin {
namespace system {

prioritized_mutex::prioritized_mutex(bool prioritize)
  : prioritize_(prioritize)
{
}

void prioritized_mutex::lock_low_priority()
{
    if (prioritize_)
    {
        wait_mutex_.lock();
        next_mutex_.lock();
    }

    data_mutex_.lock();
}

void prioritized_mutex::unlock_low_priority()
{
    if (prioritize_)
        next_mutex_.unlock();

    data_mutex_.unlock();

    if (prioritize_)
        wait_mutex_.unlock();
}

void prioritized_mutex::lock_high_priority()
{
    if (prioritize_)
        next_mutex_.lock();

    data_mutex_.lock();

    if (prioritize_)
        next_mutex_.unlock();
}

void prioritized_mutex::unlock_high_priority()
{
    data_mutex_.unlock();
}

} // namespace system
} // namespace libbitcoin
