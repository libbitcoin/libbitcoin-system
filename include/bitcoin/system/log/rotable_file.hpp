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
#ifndef LIBBITCOIN_SYSTEM_LOG_ROTABLE_FILE_HPP
#define LIBBITCOIN_SYSTEM_LOG_ROTABLE_FILE_HPP

#include <boost/filesystem.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace log {

typedef boost::shared_ptr<std::ostream> stream;
typedef boost::log::formatting_ostream::ostream_type formatter;

struct rotable_file
{
    boost::filesystem::path original_log;
    boost::filesystem::path archive_directory;
    size_t rotation_size;
    size_t minimum_free_space;
    size_t maximum_archive_size;
    size_t maximum_archive_files;
};

} // namespace log
} // namespace system
} // namespace libbitcoin

#endif
