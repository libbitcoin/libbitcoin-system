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
#ifndef LIBBITCOIN_SYSTEM_CONCURRENCY_PRIORITIZED_MUTEX_HPP
#define LIBBITCOIN_SYSTEM_CONCURRENCY_PRIORITIZED_MUTEX_HPP

#include <memory>
#include <bitcoin/system/concurrency/thread.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// This class is thread safe.
/// Encapsulation of prioritized locking conditions.
/// This is unconcerned with thread priority and is instead explicit.
class BC_API prioritized_mutex
{
public:
    typedef std::shared_ptr<prioritized_mutex> ptr;

    prioritized_mutex(bool prioritize=true);

    void lock_low_priority();
    void unlock_low_priority();

    void lock_high_priority();
    void unlock_high_priority();

private:
    const bool prioritize_;
    shared_mutex data_mutex_;
    shared_mutex next_mutex_;
    shared_mutex wait_mutex_;
};

} // namespace system
} // namespace libbitcoin

#endif
