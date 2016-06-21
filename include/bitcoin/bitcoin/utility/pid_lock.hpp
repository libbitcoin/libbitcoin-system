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
#ifndef LIBBITCOIN_APPLICATION_LOCK_HPP
#define LIBBITCOIN_APPLICATION_LOCK_HPP

#include <string>
#include <boost/filesystem.hpp>

namespace libbitcoin {

typedef boost::filesystem::path file_path;

/**
 * An application level process id lock mechanism.
 * Creates a pid file with the process id inside.
 */
class pid_lock
{
public:
    pid_lock(const std::string& process_name);

    bool lock();
    bool unlock();

    /**
     * Checks whether this process was already locked before.
     */
    bool is_locked();

    file_path path();

private:
    const std::string process_name_;
};

} // namespace libbitcoin

#endif


