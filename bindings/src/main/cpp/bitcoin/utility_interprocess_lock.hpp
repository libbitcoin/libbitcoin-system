/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN__UTILITY_INTERPROCESS_LOCK_HPP
#define LIBBITCOIN__UTILITY_INTERPROCESS_LOCK_HPP

#include <bitcoin/bitcoin/utility/interprocess_lock.hpp>
#include <bitcoin/utility_path.hpp>

namespace libbitcoin {
namespace api {

/// This class is not thread safe.
/// Guard a resource againt concurrent use by another instance of this app.
class BC_API utility_interprocess_lock : public libbitcoin::interprocess_lock
{
public:

    utility_interprocess_lock(const libbitcoin::api::utility_path& file);
    virtual ~utility_interprocess_lock();

    bool lock();
    bool unlock();

//private:
//    typedef interprocess::file_lock lock_file;
//    typedef std::shared_ptr<lock_file> lock_ptr;

//    static bool create(const std::string& file);
//    static bool destroy(const std::string& file);
//
//    lock_ptr lock_;
//    const std::string file_;
};

} // namespace api
} // namespace libbitcoin

#endif
