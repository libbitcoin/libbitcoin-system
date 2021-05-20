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
#ifndef LIBBITCOIN_SYSTEM_CONCURRENCY_SEQUENTIAL_LOCK_HPP
#define LIBBITCOIN_SYSTEM_CONCURRENCY_SEQUENTIAL_LOCK_HPP

#include <atomic>
#include <cstddef>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// This class is thread safe.
/// Encapsulation of sequential locking conditions.
class BC_API sequential_lock
{
public:
    typedef size_t handle;

    /// Determine if the given handle is a write-locked handle.
    static bool is_write_locked(handle value)
    {
        return (value % 2) == 1;
    }

    sequential_lock();

    handle begin_read() const;
    bool is_read_valid(handle value) const;

    bool begin_write();
    bool end_write();

private:
    std::atomic<size_t> sequence_;
};

} // namespace system
} // namespace libbitcoin

#endif
