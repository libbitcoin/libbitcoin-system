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
#include <bitcoin/bitcoin/utility/crash_lock.hpp>

#include <memory>
#include <boost/filesystem.hpp>
#include <bitcoin/bitcoin/unicode/file_lock.hpp>
#include <bitcoin/bitcoin/unicode/ifstream.hpp>
#include <bitcoin/bitcoin/unicode/ofstream.hpp>

namespace libbitcoin {
    
// static
bool crash_lock::create(const std::string& file)
{
    bc::ofstream stream(file);
    return stream.good();
}

// static
bool crash_lock::exists(const std::string& file)
{
    bc::ifstream stream(file);
    return stream.good();
    ////return boost::filesystem::exists(file);
}

// static
bool crash_lock::destroy(const std::string& file)
{
    return boost::filesystem::remove(file);
    ////std::remove(file.c_str());
}

crash_lock::crash_lock(const path& file)
  : file_(file.string()), locked_(false)
{
}

bool crash_lock::try_lock()
{
    return !exists(file_);
}

// Lock is idempotent, returns true if locked on return.
bool crash_lock::lock_shared()
{
    if (locked_)
        return true;

    locked_ = create(file_);
    return locked_;
}

// Unlock is idempotent, returns true if unlocked on return.
bool crash_lock::unlock_shared()
{
    if (!locked_)
        return true;

    locked_ = !destroy(file_);
    return !locked_;
}

} // namespace libbitcoin
