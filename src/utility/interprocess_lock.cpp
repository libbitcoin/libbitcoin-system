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
#include <bitcoin/bitcoin/utility/interprocess_lock.hpp>

#include <memory>
#include <boost/filesystem.hpp>
#include <bitcoin/bitcoin/unicode/file_lock.hpp>
#include <bitcoin/bitcoin/unicode/ofstream.hpp>

namespace libbitcoin {

// static
bool interprocess_lock::create(const path& file)
{
    bc::ofstream stream(file.string());
    return !stream.bad();
}

// static
bool interprocess_lock::destroy(const path& file)
{
    return boost::filesystem::remove(file);
}

interprocess_lock::interprocess_lock(const path& file, bool exclusive)
  : file_(file), exclusive_(exclusive)
{
}

interprocess_lock::~interprocess_lock()
{
    unlock();
}

//  This succeeds if no other process has exclusive or sharable ownership.
bool interprocess_lock::try_lock()
{
    if (!create(file_))
        return false;

    if (!exclusive_)
        return true;

    lock_ = std::make_shared<lock>(file_.string());
    return lock_->try_lock();
}

// This may leave the lock file behind, which is not a problem.
bool interprocess_lock::unlock()
{
    if (exclusive_ && !lock_)
        return false;

    lock_.reset();
    return destroy(file_);
}

} // namespace libbitcoin
