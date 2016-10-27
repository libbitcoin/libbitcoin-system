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
#ifndef LIBBITCOIN_LOG_INITIALIZE_HPP
#define LIBBITCOIN_LOG_INITIALIZE_HPP

#include <iostream>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/log/attributes/clock.hpp>
#include <boost/log/expressions/keyword.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/unicode/ofstream.hpp>

#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/log/attributes.hpp>
#include <bitcoin/bitcoin/log/severity.hpp>

namespace libbitcoin {
namespace log {

using namespace boost::filesystem;

typedef boost::shared_ptr<bc::ofstream> file;
typedef boost::shared_ptr<std::ostream> stream;
typedef boost::log::formatting_ostream::ostream_type formatter;

struct rotable_file
{
    path original_log;
    path archive_directory;
    size_t rotation_size;
    size_t maximum_files_size;
    size_t minimum_free_space;
    size_t maximum_files;
};

/// Initializes default non-rotable libbitcoin logging sinks and formats.
void initialize(log::file& debug_file, log::file& error_file,
    log::stream& output_stream, log::stream& error_stream);

/// Initializes default rotable libbitcoin logging sinks and formats.
void initialize(const rotable_file& debug_file, const rotable_file& error_file,
    log::stream& output_stream, log::stream& error_stream);

/// Log stream operator.
formatter& operator<<(formatter& stream, severity level);

} // namespace log
} // namespace libbitcoin

#endif
