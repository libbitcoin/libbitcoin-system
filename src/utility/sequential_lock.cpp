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
#include <bitcoin/system/utility/sequential_lock.hpp>

namespace libbitcoin {
namespace system {

sequential_lock::sequential_lock()
  : sequence_(0)
{
}

sequential_lock::handle sequential_lock::begin_read() const
{
    // Start read lock.
    return sequence_.load();
}

bool sequential_lock::is_read_valid(handle value) const
{
    // Test read lock.
    return value == sequence_.load();
}

// Failure does not prevent a subsequent begin or end resetting the lock state.
bool sequential_lock::begin_write()
{
    // Start write lock.
    return is_write_locked(++sequence_);
}

// Failure does not prevent a subsequent begin or end resetting the lock state.
bool sequential_lock::end_write()
{
    // End write lock.
    return !is_write_locked(++sequence_);
}

} // namespace system
} // namespace libbitcoin
