/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CONCURRENCY_INTERPROCESS_LOCK_HPP
#define LIBBITCOIN_SYSTEM_CONCURRENCY_INTERPROCESS_LOCK_HPP

#include <string>
#include <boost/filesystem.hpp>
#include <boost/interprocess/detail/os_file_functions.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/type_constraints.hpp>

namespace libbitcoin {
namespace system {

/// This class is not thread safe, and does not throw.
/// TODO: create critical sections around lock/unlock.
class BC_API interprocess_lock
  : noncopyable
{
public:
    typedef boost::filesystem::path path;

    // TODO: may need to implement move construct/assign.

    /// Construction does not touch the file.
    interprocess_lock(const path& file);

    /// Destruction calls unlock.
    virtual ~interprocess_lock();

    /// Creates the file and acquires exclusive access.
    /// Returns false if failed to acquire lock or lock already held.
    bool lock();

    /// Releases access (if locked) and deletes the file.
    /// Returns true if lock not held or succesfully unlocked and deleted.
    bool unlock();

private:
    const std::string filename_;
    boost::interprocess::file_handle_t handle_;
};

} // namespace system
} // namespace libbitcoin

#endif
