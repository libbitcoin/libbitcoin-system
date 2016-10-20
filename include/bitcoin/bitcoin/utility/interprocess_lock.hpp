/**
 * Copyright (c) 2011-2016 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_INTERPROCESS_LOCK_HPP
#define LIBBITCOIN_INTERPROCESS_LOCK_HPP

#include <memory>
#include <boost/filesystem.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/unicode/file_lock.hpp>

namespace libbitcoin {

class BC_API interprocess_lock
{
public:
    typedef boost::filesystem::path path;

    interprocess_lock(const path& file, bool exclusive);
    ~interprocess_lock();

    bool try_lock();
    bool unlock();

private:
    typedef interprocess::file_lock lock;
    typedef std::shared_ptr<lock> lock_ptr;

    static bool create(const path& file);
    static bool destroy(const path& file);

    const path file_;
    bool exclusive_;
    lock_ptr lock_;
};

} // namespace libbitcoin

#endif
