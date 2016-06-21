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
#include <bitcoin/bitcoin/utility/resource_lock.hpp>

#include <boost/filesystem.hpp>
#include <boost/interprocess/exceptions.hpp>
#include <bitcoin/bitcoin/unicode/ifstream.hpp>
#include <bitcoin/bitcoin/unicode/ofstream.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>

namespace libbitcoin {

static void touch_file(const resource_lock::file_path& path)
{
    bc::ofstream file(path.string(), std::ofstream::app);
    file.close();
}

resource_lock::resource_lock(const file_path& lock_path)
  : lock_path_(lock_path)
{
}

bool resource_lock::lock()
{
    // If file exists then lock is already acquired.
    if (boost::filesystem::exists(lock_path_))
    {
        return false;
    }

    // Touch the lock file to ensure its existence.
    touch_file(lock_path_);

    // BOOST:
    // Opens a file lock. Throws interprocess_exception if the file does not
    // exist or there are no operating system resources. The file lock is
    // destroyed on its destruct and does not throw.
    try
    {
        lock_ = std::make_shared<boost_file_lock>(lock_path_.c_str());
        return lock_->try_lock();
    }
    catch (const boost::interprocess::interprocess_exception&)
    {
        lock_ = nullptr;
        return false;
    }

    // Should not arrive here!
    BITCOIN_ASSERT_MSG(false, "Reached unreachable code path.");
    return false;
}
bool resource_lock::unlock()
{
    boost::system::error_code ec;
    boost::filesystem::remove(lock_path_, ec);
    return !ec;
}

} // namespace libbitcoin

