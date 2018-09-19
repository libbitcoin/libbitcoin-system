/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_INTERPROCESS_LOCK_HPP
#define LIBBITCOIN_INTERPROCESS_LOCK_HPP

#include <memory>
#include <string>
#include <boost/filesystem.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/unicode/file_lock.hpp>

namespace libbitcoin {

/// This class is not thread safe.
/// Guard a resource againt concurrent use by another instance of this app.
class BC_API interprocess_lock
{
public:
    typedef boost::filesystem::path path;

    interprocess_lock(const path& file);
    virtual ~interprocess_lock();

    bool lock();
    bool unlock();

private:
    typedef interprocess::file_lock lock_file;
    typedef std::shared_ptr<lock_file> lock_ptr;

    static bool create(const std::string& file);
    static bool destroy(const std::string& file);

    lock_ptr lock_;
    const std::string file_;
};

} // namespace libbitcoin

#endif
