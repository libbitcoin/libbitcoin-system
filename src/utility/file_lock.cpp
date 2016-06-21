/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/utility/file_lock.hpp>

namespace libbitcoin {

const char* generate_pid_name()
{
}

file_lock::file_lock(const path& lock_file_path)
  : lock_file_path_(lock_file_path)
{
}

bool file_lock::lock()
{
    // Touch the lock file to ensure its existence.
    bc::ofstream file(lock_file_path_, std::ios::app);
    file.close();

    // BOOST:
    // Opens a file lock. Throws interprocess_exception if the file does not
    // exist or there are no operating system resources. The file lock is
    // destroyed on its destruct and does not throw.
    lock_ = std::make_shared<boost_file_lock>(lock_file_path_.c_str());
    return lock_.try_lock();
}
bool file_lock::unlock()
{
    // BUGBUG: Throws if the lock is not held (i.e. in error condition).
    boost::filesystem::remove(lock_);
}

} // namespace libbitcoin

