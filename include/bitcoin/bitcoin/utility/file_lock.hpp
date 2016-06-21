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
#ifndef LIBBITCOIN_FILE_LOCK_HPP
#define LIBBITCOIN_FILE_LOCK_HPP

#include <boost/interprocess/sync/file_lock.hpp>

namespace libbitcoin {

using namespace boost::filesystem;

/**
 * A file lock device that ensures exclusive access to a resource.
 */
class file_lock
{
public:
    // Take an explicit path.
    file_lock(const path& lock_file_path);

    bool lock();
    bool unlock();

private:
    typedef boost::interprocess::file_lock boost_file_lock;
    typedef std::shared_ptr<boost_file_lock> lock_ptr;

    const path& lock_file_path_;
    lock_ptr lock_;
};

} // namespace libbitcoin

#endif

