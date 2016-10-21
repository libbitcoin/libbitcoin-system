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

typedef boost::shared_ptr<bc::ofstream> file;
typedef boost::shared_ptr<std::ostream> stream;
typedef boost::log::formatting_ostream::ostream_type formatter;

struct file_sink_configuration
{
    std::string name_pattern;
    std::string rotation_path;
    uintmax_t rotation_size;
    uintmax_t rotation_max_files;
    uintmax_t rotation_max_files_size;
    uintmax_t rotation_min_free_space;
};

/// Initializes default libbitcoin logging sinks and formats.
void initialize(log::file& debug_file, log::file& error_file,
    log::stream& output_stream, log::stream& error_stream);

void initialize(const file_sink_configuration& debug_file,
    const file_sink_configuration& error_file,
    log::stream& output_stream, log::stream& error_stream);

/// Log stream operator.
formatter& operator<<(formatter& stream, severity level);

} // namespace log
} // namespace libbitcoin

#endif
