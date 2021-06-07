/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/concurrency/file_lock.hpp>

#include <string>
#include <boost/filesystem.hpp>
#include <bitcoin/system/unicode/utf8_everywhere/utf8_environment.hpp>
#include <bitcoin/system/unicode/utf8_everywhere/utf8_ifstream.hpp>
#include <bitcoin/system/unicode/utf8_everywhere/utf8_ofstream.hpp>

namespace libbitcoin {
namespace system {

file_lock::file_lock(const boost::filesystem::path& file) noexcept
  : file_(file)
{
}

bool file_lock::exists() const noexcept
{
    ifstream stream(file_);
    return stream.good();
}

// This is non-const as it alters state (externally but may become internal).
bool file_lock::create() noexcept
{
    ofstream stream(file_);
    return stream.good();
}

// This is non-const as it alters state (externally but may become internal).
bool file_lock::destroy() noexcept
{
    // remove returns false if file did not exist though error_code is false if
    // file did not exist. use of error_code overload also precludes exception.
    boost::system::error_code ec;
    boost::filesystem::remove(to_extended_path(file_), ec);
    return !ec;
}

std::string file_lock::file() const noexcept
{
    return file_.string();
}

} // namespace system
} // namespace libbitcoin
