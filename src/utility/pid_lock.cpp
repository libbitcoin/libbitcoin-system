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
#include <bitcoin/bitcoin/utility/pid_lock.hpp>

#include <fstream>

#ifdef _WIN32
#else
    #include <signal.h>
#endif

namespace libbitcoin {

const file_path PID_FOLDER = "/var/tmp/";

file_path pid_path(const std::string& process_name)
{
    const std::string file_name = process_name + ".pid";
    return PID_FOLDER / file_name;
}

pid_lock::pid_lock(const std::string& process_name)
  : process_name_(process_name)
{
}

bool pid_lock::lock()
{
    if (is_locked())
        return false;
    std::ofstream file(path().string());
#ifdef _WIN32
    // Unimplemented!
#else
    int pid = getpid();
    file << pid << "\n";
#endif
    file.close();
    return true;
}
bool pid_lock::unlock()
{
    boost::system::error_code ec;
    boost::filesystem::remove(path(), ec);
    if (ec)
        return false;
    return true;
}

// Opens the pid lock file and fetches the process ID.
int fetch_pid(const file_path& path)
{
    std::ifstream file(path.string());
    int pid;
    file >> pid;
    file.close();
    return pid;
}
// Checks whether the given process ID is running.
bool check_pid_running(int pid)
{
#ifdef _WIN32
    // Unimplemented!
#else
    if (kill(pid, 0) == 0)
        return true;
    return false;
#endif
}

bool pid_lock::is_locked()
{
    if (!boost::filesystem::exists(path()))
        return false;
    int pid = fetch_pid(path());
    return check_pid_running(pid);
}

file_path pid_lock::path()
{
    return pid_path(process_name_);
}

} // namespace libbitcoin


