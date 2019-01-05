/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CRASH_LOCK_HPP
#define LIBBITCOIN_SYSTEM_CRASH_LOCK_HPP

#include <memory>
#include <boost/filesystem.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/unicode/file_lock.hpp>

namespace libbitcoin {
namespace system {

/// This class is not thread safe.
/// Guard a resource that may be corrupted due to an interrupted write.
class BC_API flush_lock
{
public:
    typedef boost::filesystem::path path;

    flush_lock(const path& file);

    bool try_lock();
    bool lock_shared();
    bool unlock_shared();

private:
    static bool create(const std::string& file);
    static bool exists(const std::string& file);
    static bool destroy(const std::string& file);

    bool locked_;
    const std::string file_;
};

} // namespace system
} // namespace libbitcoin

#endif
