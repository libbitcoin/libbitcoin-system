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
#ifndef LIBBITCOIN_RESOURCE_LOCK_HPP
#define LIBBITCOIN_RESOURCE_LOCK_HPP

#include <memory>
#include <boost/filesystem.hpp>
#include <boost/interprocess/sync/file_lock.hpp>

namespace libbitcoin {

/**
 * A resource lock device that ensures exclusive access to a resource.
 * It takes a path for creating a lock file object that can be used
 * for telling the usage of some resource between processes.
 * Example: opening/closing blockchain database.
 */
class resource_lock
{
public:
    typedef boost::filesystem::path file_path;

    // Take an explicit path.
    resource_lock(const file_path& lock_path);

    bool lock();
    bool unlock();

private:
    typedef boost::interprocess::file_lock boost_file_lock;
    typedef std::shared_ptr<boost_file_lock> lock_ptr;

    const file_path& lock_path_;
    lock_ptr lock_;
};

} // namespace libbitcoin

#endif

