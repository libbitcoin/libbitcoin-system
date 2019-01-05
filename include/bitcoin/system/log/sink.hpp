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
#ifndef LIBBITCOIN_SYSTEM_LOG_SINK_HPP
#define LIBBITCOIN_SYSTEM_LOG_SINK_HPP

#include <iostream>
#include <boost/smart_ptr.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/log/rotable_file.hpp>
#include <bitcoin/system/log/severity.hpp>
#include <bitcoin/system/unicode/ofstream.hpp>

namespace libbitcoin {
namespace system {
namespace log {

typedef boost::shared_ptr<ofstream> file;

/// Initializes null (as opposed to default) logging sinks.
void initialize();

/// Initializes default non-rotable libbitcoin logging sinks and formats.
void initialize(log::file& debug_file, log::file& error_file,
    log::stream& output_stream, log::stream& error_stream, bool verbose);

/// Initializes default rotable libbitcoin logging sinks and formats.
void initialize(const rotable_file& debug_file, const rotable_file& error_file,
    log::stream& output_stream, log::stream& error_stream, bool verbose);

/// Log stream operator.
formatter& operator<<(formatter& stream, severity value);

} // namespace log
} // namespace system
} // namespace libbitcoin

#endif
